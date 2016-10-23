#! python2.7


# This script implements our old methods for merging an MD380 firmware
# image with its patches.  It is presently being rewritten to require
# fewer explicit addresses, so that we can target our patches to more
# than one version of the MD380 firmware.

import sys;

class Symbols():
    addresses={}
    names={}
    def __init__(self,filename):
        print "Loading symbols from %s" % filename
        fsyms=open(filename);
        for l in fsyms:
            try:
                r=l.strip().split('\t');
                if len(r)==2 and r[0].split(' ')[7]=='.text':
                    adr=r[0].split(' ')[0].strip();
                    name=r[1].split(' ')[1]#.strip();
                    #print "%s is at %s"% (name,adr)
                    self.addresses[name]=int(adr,16);
                    self.names[int(adr,16)]=name;
            except IndexError:
                pass;
    def getadr(self,name):
        return self.addresses[name];
    def getname(self,adr):
        return self.names[adr];

class Merger():
    def __init__(self,filename,offset=0x0800C000):
        """Opens the input file."""
        self.offset=offset;
        self.file=open(filename,"rb");
        self.bytes=bytearray(self.file.read());
        self.length=len(self.bytes);

    def setbyte(self, adr, new, old=None):
        """Patches a single byte from the old value to the new value."""
        self.bytes[adr-self.offset]=new;
    def getbyte(self,adr):
        """Reads a byte from the firmware address."""
        b=self.bytes[adr-self.offset];
        return b;
    def export(self,filename):
        """Exports to a binary file."""
        outfile=open(filename,"wb");
        outfile.write(self.bytes);
    def assertbyte(self, adr, val):
        """Asserts that a byte has a given value."""
        assert self.getbyte(adr)==val;
        return;
    def getword(self,adr):
        """Reads a byte from the firmware address."""
        w=(
            self.bytes[adr-self.offset]+
            (self.bytes[adr-self.offset+1]<<8)+
            (self.bytes[adr-self.offset+2]<<16)+
            (self.bytes[adr-self.offset+3]<<24)
            );
            
        return w;
    
    def setword(self, adr, new, old=None):
        """Patches a 32-bit word from the old value to the new value."""
        if old!=None:
            self.assertbyte(adr,old&0xFF);
            self.assertbyte(adr+1,(old>>8)&0xFF);
            self.assertbyte(adr+2,(old>>16)&0xFF);
            self.assertbyte(adr+3,(old>>24)&0xFF);
        
        #print "Patching word at %08x to %08x" % (adr,new)
        self.bytes[adr-self.offset]=new&0xFF;
        self.bytes[adr-self.offset+1]=(new>>8)&0xFF;
        self.bytes[adr-self.offset+2]=(new>>16)&0xFF;
        self.bytes[adr-self.offset+3]=(new>>24)&0xFF;
        self.assertbyte(adr,new&0xFF);
        self.assertbyte(adr+1,(new>>8)&0xFF);
    def sethword(self, adr, new, old=None):
        """Patches a byte pair from the old value to the new value."""
        if old!=None:
            self.assertbyte(adr,old&0xFF);
            self.assertbyte(adr+1,(old>>8)&0xFF);
        #print "Patching hword at %08x to %04x" % (adr,new)
        self.bytes[adr-self.offset]=new&0xFF;
        self.bytes[adr-self.offset+1]=(new>>8)&0xFF;
        self.assertbyte(adr,new&0xFF);
        self.assertbyte(adr+1,(new>>8)&0xFF);
    def hookstub(self, adr, handler):
        """Hooks a function by placing an unconditional branch at adr to
           handler.  The recipient function must have an identical calling
           convention. """
        adr=adr & ~1;          #Address must be even.
        handler = handler | 1; #Destination address must be odd.
        #print "Inserting a stub hook at %08x to %08x." % (adr,handler);
        
        #FIXME This clobbers r0, should use a different register.
        self.sethword(adr,0x4801); # ldr r0, [pc, 4]
        self.sethword(adr+2,0x4700); # bx r0
        self.sethword(adr+4,0x4600); #NOP
        self.sethword(adr+6,0x4600); #NOP, might be overwritten
        if adr&2>0:
            self.setword(adr+6,handler); # bx r0
        else:
            self.setword(adr+8,handler); # bx r0
    def hookstub2(self, adr, handler):
        """Hooks a function by placing an unconditional branch at adr to
           handler.  The recipient function must have an identical calling
           convention. """
        adr=adr & ~1;          #Address must be even.
        handler = handler | 1; #Destination address must be odd.
        print "Inserting a stub hook at %08x to %08x." % (adr,handler);
        
        # insert trampoline
        # rasm2 -a arm -b 16 '<asm code>'
        self.sethword(adr,0xb401);   # push {r0}
        self.sethword(adr+2,0xb401); # push {r0}
        self.sethword(adr+4,0x4802); # ldr r0, [pc, 8]
        self.sethword(adr+6,0x9001); # str r0, [sp, 4] (pc)
        self.sethword(adr+8,0xbc01); # pop {r0}
        self.sethword(adr+10,0xbd00); # pop {pc}
        self.sethword(adr+12,0x4600); #NOP
        self.sethword(adr+14,0x4600); #NOP, might be overwritten
        if adr&2>0:
            self.setword(adr+14,handler); 
        else:
            self.setword(adr+16,handler);
    def calcbl(self,adr,target):
        """Calculates the Thumb code to branch to a target."""
        offset=target-adr;
        #print "offset=%08x" % offset;
        offset=offset-4;    #PC points to the next ins.
        offset=(offset>>1); #LSBit is ignored.
        hi=0xF000 | ((offset&0xfff800)>>11);   #Hi address setter, but at lower adr.
        lo=0xF800 | (offset&0x7ff);            #Low adr setter goes next.
        #print "%04x %04x" % (hi,lo);
        word=((lo<<16) | hi);
        #print "%08x" % word
        return word;

    def hookbl(self,adr,handler,oldhandler=None):
        """Hooks a function by replacing a 32-bit relative BL."""
        
        #print "Redirecting a bl at %08x to %08x." % (adr,handler);
        
        #TODO This is sometimes tricked by old data.
        # Fix it by ensuring no old data.
        #if oldhandler!=None:
        #    #Verify the old handler.
        #    if self.calcbl(adr,oldhandler)!=self.getword(adr):
        #        print "The old handler looks wrong.";
        #        print "Damn, we're in a tight spot!";
        #        sys.exit(1);
        
        self.setword(adr,
                     self.calcbl(adr,handler));

if __name__== '__main__':
    print "Merging an applet."
    if len(sys.argv) != 4:
        print "Usage: python merge.py firmware.img patch.img offset"
        sys.exit(1);
    
    #Open the firmware image.
    merger=Merger(sys.argv[1]);
    
    #Open the applet.
    fapplet=open(sys.argv[2],"rb");
    bapplet=bytearray(fapplet.read());
    index=int(sys.argv[3],16);
    
    #Open the applet symbols
    sapplet=Symbols("%s.sym"%sys.argv[2]);
    #############  All address comments from D13.020
    merger.hookstub(0x080974de,   #0x0809661e,    #USB manufacturer string handler function.
                    sapplet.getadr("getmfgstr"));
    merger.hookstub(0x080229ae,  #0x080226d2, #startup_botline
                    sapplet.getadr("splash_hook_handler"));
    merger.hookstub(0x08016b7e,  #0x08016a96,
                    sapplet.getadr("loadfirmwareversion_hook"));
    merger.hookbl(  0x0808f9a6,   #0x0808eb66, #Call to usb_dfu_upload().
                  sapplet.getadr("usb_upld_hook"));
                                                                                                                
    merger.hookbl(0x080413f8, # 0x80408e0, #Call to dmr_call_end()
                 sapplet.getadr("dmr_call_end_hook"));
                         
    dmr_call_start_hook_list=[0x08041282, 0x080413e2 ]; #0x804076a,0x80408ca];
    for adr in dmr_call_start_hook_list:
        merger.hookbl(adr,sapplet.getadr("dmr_call_start_hook"));  
                                              
    dmr_handle_data_hook_list=[0x08041456, 0x0804146e, 0x080414b4 ]; #0x804093e,0x8040956,0x804099c];
    for adr in dmr_handle_data_hook_list:
        merger.hookbl(adr, sapplet.getadr("dmr_handle_data_hook"));
                                                                          
    merger.hookbl( 0x0804143e, #0x8040926,
                 sapplet.getadr("dmr_sms_arrive_hook"));
                                                                              
                                                                                  
    # os semaphore hook .. now we can crate own semaphores
    merger.hookbl(0x0804717a ,sapplet.getadr("OSSemCreate_hook"),0); #0x804647a
        
    # gfx hooks
    merger.hookbl(0x08026024,sapplet.getadr("rx_screen_blue_hook"),0); #0x08025d54
    merger.hookbl(0x08026102,sapplet.getadr("rx_screen_blue_hook"),0); #0x08025e26
    merger.hookbl(0x0802058a,sapplet.getadr("rx_screen_gray_hook"),0);  #0x08020428
                       
    # date format  hook, this hook can modify the date format on the status line
    merger.hookbl(0x0800df92,sapplet.getadr("print_date_hook"),0);    # 0x0800df92

    # Hook the startup AES check.
    merger.hookbl(0x08048474,sapplet.getadr("aes_startup_check_hook"),0); # 0x0804764c

    # Patch a single call in the wrapper function so catch all
    # aes_loadkey() calls.
    merger.hookbl(0x0803774a,sapplet.getadr("aes_loadkey_hook"),0); # 0x08036c32

    #Function that calls aes_cipher() twice.  When are these called?
    #there a 3 calls on d02.32 
    aes_cipher_hook_list=[ 0x08022936,0x0804070a];     #0x802265a,0x803fbf2];
    for adr in aes_cipher_hook_list:
        merger.hookbl(adr, sapplet.getadr("aes_cipher_hook"));

    #Hook lots of AMBE2+ encoder code and hope our places are correct.
    ambelist=[
              0x804b036, 0x804b16e, 0x804b1c6, 0x804b314, 0x804b38a, 0x804b504,
              0x804b584, 0x804b6f0, 0x804b726
    ];
#              0x804a20a,0x804a342,0x804a39a,0x804a4e8,0x804a55e,
#              0x804a6d8,0x804a758,0x804a8c4,0x804a8fa 
    for adr in ambelist:
        merger.hookbl(adr,sapplet.getadr("ambe_encode_thing_hook"));

    #Hook calls within the AMBE2+ decoder.
    unpacklist=[
                0x8034c1e, 0x8034c2a, 0x8034c42, 0x8034c4e, 0x804bcea, 0x804c216, 
                0x804c26a, 0x804c2c0
#        0x8034106, 0x8034112, 0x803412a, 0x8034136, 0x804aebe, 0x804b3ea, 0x804b43e,
#        0x804b494
    ];
    for adr in unpacklist:
        merger.hookbl(adr,sapplet.getadr("ambe_unpack_hook"));

    #Hook calls that produce WAV audio.  (Maybe.)
    wavdeclist=[
         0x804adc4, 0x804ba38, 0x804bb8e, 0x804bea0, 0x804bedc, 0x804bf9c, 0x804bfd8
#        0x8049f98, 0x804ac0c, 0x804ad62, 0x804b074, 0x804b0b0, 0x804b170, 0x804b1ac
    ];  
    for adr in wavdeclist:
        merger.hookbl(adr,sapplet.getadr("ambe_decode_wav_hook"));

    #Hooks the squelch routines, so we can do monitor mode in C.
    merger.hookbl(0x080417f8, sapplet.getadr("dmr_apply_privsquelch_hook"),0); #Private calls. # 0x08040ce0
    #########  this function has been changed 
    merger.hookbl(0x08041734, sapplet.getadr("dmr_apply_squelch_hook"),0);     #Public calls. # 0x08040c1c

                                
    # additional menu hook
    merger.hookbl(0x08013642, sapplet.getadr("create_menu_utilies_hook"),0); # 0x080135a8

    # print_ant_sym_hook (shows eye on status line when promiscus mode is active)
    print_ant_sym_hook_list=[
         0x0802161a, 0x08021628, 0x0802161a, 0x08034936, 0x08021a84 
#        0x0802136a, 0x08021378, 0x0802136a, 0x8033e1e, 0x080217a8
        ]; # bad hooks, not work well
    for adr in print_ant_sym_hook_list:
        merger.hookbl(adr,sapplet.getadr("print_ant_sym_hook"));

    # init the addl global config struct from spi flash
    merger.hookbl(0x08047026 ,sapplet.getadr("init_global_addl_config_hook"),0); # 0x08046326

    # no menu exit on RX hook 
    merger.hookbl(0x0801ffb0,sapplet.getadr("f_4225_internel_hook"),0);#0x0801fe7c

    # OSMboxPend Hook to diag Beep_Process
    merger.hookbl(0x0802fe54, sapplet.getadr("OSMboxPend_hook"));#0x0802fa00

    # hooks regarding the beep_process
    beep_process_list=[
       0x08030082, 0x08030094, # beep "9"
#      0x0802fc2e, 0x0802fc40, # beep "9"
       0x08030048, 0x0803005a, # roger beep
#      0x0802fbf4, 0x0802fc06, # roger beep
       0x080301c0, 0x080301d2, 0x080301e0, 0x080301ee
#      0x0802fd6c, 0x0802fd7e, 0x0802fd8c, 0x0802fd9a #dmr sync
      ];
    for adr in beep_process_list:
      merger.hookbl(adr,sapplet.getadr("F_294_replacement"),0);


    merger.hookbl(0x802ded6, sapplet.getadr("f_4225_hook"),0); # 0x080468e6
    merger.hookbl(0x8047640, sapplet.getadr("f_4225_hook"),0); # 0x0802db42

#    merger.hookstub2(0x0800c72e, sapplet.getadr("create_menu_entry_rev"));

    # keyboard
    merger.hookbl(0x0804fa12, sapplet.getadr("kb_handler_hook"));

    print "Merging %s into %s at %08x" % (
          sys.argv[2],
          sys.argv[1],
          index);
    
    i=0;
    for b in bapplet:
        merger.setbyte(index+i,bapplet[i]);
        i=i+1;
    
    merger.export(sys.argv[1]);


