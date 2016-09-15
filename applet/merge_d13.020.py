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
    
    merger.hookstub(0x0809661e,    #USB manufacturer string handler function.
                    sapplet.getadr("getmfgstr"));
    merger.hookstub(0x080226d2, #startup_botline
                    sapplet.getadr("demo"));
    merger.hookstub(0x08016a96,
                    sapplet.getadr("loadfirmwareversion_hook"));
    merger.hookbl(0x0808eb66, #Call to usb_dfu_upload().
                  sapplet.getadr("usb_upld_hook"),
                  0x0808f308); #Old handler adr.

    merger.hookbl(0x80408e0, #Call to dmr_call_end()
                 sapplet.getadr("dmr_call_end_hook"));

    merger.hookstub2(0x800c72e, sapplet.getadr("create_menu_entry_rev"));

    dmr_call_start_hook_list=[0x804076a,0x80408ca];
    for adr in dmr_call_start_hook_list:
        merger.hookbl(adr,sapplet.getadr("dmr_call_start_hook"));  
     
    dmr_handle_data_hook_list=[0x804093e,0x8040956,0x804099c];
    for adr in dmr_handle_data_hook_list:
        merger.hookbl(adr, sapplet.getadr("dmr_handle_data_hook"));
            
    merger.hookbl(0x8040926, sapplet.getadr("dmr_sms_arrive_hook"));

    
    # os semaphore hook .. now we can crate own semaphores
    merger.hookbl(0x804647a,sapplet.getadr("OSSemCreate_hook"),0);

    # gfx hooks
    merger.hookbl(0x08025d54,sapplet.getadr("rx_screen_blue_hook"),0);
    merger.hookbl(0x08025e26,sapplet.getadr("rx_screen_blue_hook"),0);
    merger.hookbl(0x08020428,sapplet.getadr("rx_screen_gray_hook"),0);

    # date format  hook, this hook can modify the date format on the status line
    merger.hookbl(0x0800df92, sapplet.getadr("print_date_hook"),0);

#    merger.hookstub2(0x0800df1a, sapplet.getadr("draw_datetime_row_hook"));

    merger.hookbl(0x0800c8f2, sapplet.getadr("gfx_chars_to_display_hook"),0);   
    merger.hookbl(0x0800cad2, sapplet.getadr("gfx_chars_to_display_hook"),0);   
    merger.hookbl(0x0800cd04, sapplet.getadr("gfx_chars_to_display_hook"),0);   
    merger.hookbl(0x0800cd20, sapplet.getadr("gfx_chars_to_display_hook"),0);
    merger.hookbl(0x0800cd68, sapplet.getadr("gfx_chars_to_display_hook"),0);
    merger.hookbl(0x0800cdfe, sapplet.getadr("gfx_chars_to_display_hook"),0);
    merger.hookbl(0x0800ce1a, sapplet.getadr("gfx_chars_to_display_hook"),0);
    merger.hookbl(0x0800ce62, sapplet.getadr("gfx_chars_to_display_hook"),0);
    merger.hookbl(0x0800ceca, sapplet.getadr("gfx_chars_to_display_hook"),0);
    merger.hookbl(0x0800cee6, sapplet.getadr("gfx_chars_to_display_hook"),0);
    merger.hookbl(0x0800cf32, sapplet.getadr("gfx_chars_to_display_hook"),0);
    merger.hookbl(0x0800d06e, sapplet.getadr("gfx_chars_to_display_hook"),0);
    merger.hookbl(0x0800d080, sapplet.getadr("gfx_chars_to_display_hook"),0);
    merger.hookbl(0x0800d0ea, sapplet.getadr("gfx_chars_to_display_hook"),0);
    merger.hookbl(0x0800d150, sapplet.getadr("gfx_chars_to_display_hook"),0);
    merger.hookbl(0x0800d19e, sapplet.getadr("gfx_chars_to_display_hook"),0);
    merger.hookbl(0x0800d1d6, sapplet.getadr("gfx_chars_to_display_hook"),0);
    merger.hookbl(0x0800d1f2, sapplet.getadr("gfx_chars_to_display_hook"),0);
    merger.hookbl(0x0800d242, sapplet.getadr("gfx_chars_to_display_hook"),0);
    merger.hookbl(0x0800d708, sapplet.getadr("gfx_chars_to_display_hook"),0);
    merger.hookbl(0x0800d7b6, sapplet.getadr("gfx_chars_to_display_hook"),0);
    merger.hookbl(0x0800d906, sapplet.getadr("gfx_chars_to_display_hook"),0);
    merger.hookbl(0x0800d958, sapplet.getadr("gfx_chars_to_display_hook"),0);
    merger.hookbl(0x0800d974, sapplet.getadr("gfx_chars_to_display_hook"),0);
    merger.hookbl(0x0800df92, sapplet.getadr("gfx_chars_to_display_hook"),0);
    merger.hookbl(0x0800dff6, sapplet.getadr("gfx_chars_to_display_hook"),0);
    merger.hookbl(0x0800e174, sapplet.getadr("gfx_chars_to_display_hook"),0);
    merger.hookbl(0x0800e18e, sapplet.getadr("gfx_chars_to_display_hook"),0);
    merger.hookbl(0x0800e1b4, sapplet.getadr("gfx_chars_to_display_hook"),0);
    merger.hookbl(0x0800e1da, sapplet.getadr("gfx_chars_to_display_hook"),0);
    merger.hookbl(0x0800ea82, sapplet.getadr("gfx_chars_to_display_hook"),0);
    merger.hookbl(0x0801ea2e, sapplet.getadr("gfx_chars_to_display_hook"),0);
    merger.hookbl(0x0801f02c, sapplet.getadr("gfx_chars_to_display_hook"),0);
    merger.hookbl(0x0801f044, sapplet.getadr("gfx_chars_to_display_hook"),0);
    merger.hookbl(0x0801f07a, sapplet.getadr("gfx_chars_to_display_hook"),0);
    merger.hookbl(0x0801f092, sapplet.getadr("gfx_chars_to_display_hook"),0);
    merger.hookbl(0x0802d660, sapplet.getadr("gfx_chars_to_display_hook"),0);
    merger.hookbl(0x0802d70e, sapplet.getadr("gfx_chars_to_display_hook"),0);
    merger.hookbl(0x0802d8e2, sapplet.getadr("gfx_chars_to_display_hook"),0);
    merger.hookbl(0x0802d9e4, sapplet.getadr("gfx_chars_to_display_hook"),0);

    merger.hookbl(0x08046804, sapplet.getadr("gfx_drawtext_hook"),0);
    merger.hookbl(0x0804681a, sapplet.getadr("gfx_drawtext_hook"),0);

    merger.hookbl(0x0800e5ba, sapplet.getadr("gfx_drawtext4_hook"),0);
    merger.hookbl(0x0800e604, sapplet.getadr("gfx_drawtext4_hook"),0);
    merger.hookbl(0x0800e618, sapplet.getadr("gfx_drawtext4_hook"),0);
    merger.hookbl(0x0800e634, sapplet.getadr("gfx_drawtext4_hook"),0);
    merger.hookbl(0x0800e696, sapplet.getadr("gfx_drawtext4_hook"),0);
    merger.hookbl(0x0802d6f2, sapplet.getadr("gfx_drawtext4_hook"),0);
    merger.hookbl(0x0802d9c8, sapplet.getadr("gfx_drawtext4_hook"),0);

#    merger.hookstub2(0x0802b142, sapplet.getadr("dummy"));

#    merger.hookstub2(0x0801dcc0, sapplet.getadr("dummy"));
#    merger.hookstub2(0x08027728, sapplet.getadr("dummy"));

# keyboard polling
#    merger.hookstub2(0x0804eb64, sapplet.getadr("dummy"));

    # Hook the startup AES check.
    merger.hookbl(0x0804764c, sapplet.getadr("aes_startup_check_hook"),0);

    # Patch a single call in the wrapper function so catch all
    # aes_loadkey() calls.
    merger.hookbl(0x08036c32,sapplet.getadr("aes_loadkey_hook"),0);

    #Function that calls aes_cipher() twice.  When are these called?
    #there a 3 calls on d02.32 
    aes_cipher_hook_list=[0x802265a,0x803fbf2];
    for adr in aes_cipher_hook_list:
        merger.hookbl(adr, sapplet.getadr("aes_cipher_hook"));

    #Hook lots of AMBE2+ encoder code and hope our places are correct.
    ambelist=[0x804a20a,0x804a342,0x804a39a,0x804a4e8,0x804a55e,
              0x804a6d8,0x804a758,0x804a8c4,0x804a8fa 
    ];
    for adr in ambelist:
        merger.hookbl(adr,sapplet.getadr("ambe_encode_thing_hook"));

    #Hook calls within the AMBE2+ decoder.
    unpacklist=[
        0x8034106, 0x8034112, 0x803412a, 0x8034136, 0x804aebe, 0x804b3ea, 0x804b43e,
        0x804b494
    ];
    for adr in unpacklist:
        merger.hookbl(adr,sapplet.getadr("ambe_unpack_hook"));

    #Hook calls that produce WAV audio.  (Maybe.)
    wavdeclist=[
        0x8049f98, 0x804ac0c, 0x804ad62, 0x804b074, 0x804b0b0, 0x804b170, 0x804b1ac
    ];
    for adr in wavdeclist:
        merger.hookbl(adr,sapplet.getadr("ambe_decode_wav_hook"));

    #Hooks the squelch routines, so we can do monitor mode in C.
    merger.hookbl(0x08040ce0, sapplet.getadr("dmr_apply_privsquelch_hook"),0); #Private calls.
    #########  this function has been changed 
    merger.hookbl(0x08040c1c, sapplet.getadr("dmr_apply_squelch_hook"),0);     #Public calls.

    # additional menu hook
    merger.hookbl(0x080135a8, sapplet.getadr("create_menu_utilies_hook"),0);

    # print_ant_sym_hook (shows eye on status line when promiscus mode is active)
    print_ant_sym_hook_list=[
        0x0802136a, 0x08021378, 0x0802136a, 0x08021378, 0x8033e1e, 0x0802136a, 0x080217a8
        ]; # bad hooks, not work well
    for adr in print_ant_sym_hook_list:
        merger.hookbl(adr,sapplet.getadr("print_ant_sym_hook"));

    # init the addl global config struct from spi flash
    merger.hookbl(0x08046326,sapplet.getadr("init_global_addl_config_hook"),0);

    # no menu exit on RX hook 
    merger.hookbl(0x0801fe7c,sapplet.getadr("f_4225_internel_hook"),0);

    # OSMboxPend Hook to diag Beep_Process
    merger.hookbl(0x0802fa00, sapplet.getadr("OSMboxPend_hook"));

    # hooks regarding the beep_process
    beep_process_list=[
      0x0802fc2e, 0x0802fc40, # beep "9"
      0x0802fbf4, 0x0802fc06, # roger beep
      0x0802fd6c, 0x0802fd7e, 0x0802fd8c, 0x0802fd9a #dmr sync
      ];
    for adr in beep_process_list:
      merger.hookbl(adr,sapplet.getadr("F_294_replacement"),0);


    merger.hookbl(0x080468e6, sapplet.getadr("f_4225_hook"),0);
    merger.hookbl(0x0802db42, sapplet.getadr("f_4225_hook"),0);
    

############ i2c hooks
###    I2C_GenerateSTART_hook_list=[
###    0x8046a3a, 0x8046ac6, 0x8046b6c];
###    for adr in I2C_GenerateSTART_hook_list:
###        merger.hookbl(adr,sapplet.getadr("I2C_GenerateSTART_hook"),0);
###        
###    I2C_GenerateSTOP_hook_list=[
###    0x8046b50, 0x8046c3c];
###    for adr in I2C_GenerateSTOP_hook_list:
###        merger.hookbl(adr,sapplet.getadr("I2C_GenerateSTOP_hook"),0);
###    
###    I2C_ReceiveData_hook_list=[
###    0x8046b1c];
###    for adr in I2C_ReceiveData_hook_list:
###        merger.hookbl(adr,sapplet.getadr("I2C_ReceiveData_hook"),0);
###        
###    I2C_Send7bitAddress_hook_list=[
###    0x8046a66, 0x8046af2, 0x8046b98];
###    for adr in I2C_Send7bitAddress_hook_list:    
###        merger.hookbl(adr,sapplet.getadr("I2C_Send7bitAddress_hook"),0);
###        
###    I2C_SendData_hook_list=[
###    0x8046a90, 0x8046bc2, 0x8046c06];
###    for adr in I2C_SendData_hook_list:
###        merger.hookbl(adr,sapplet.getadr("I2C_SendData_hook"),0);
###
#############  Debug and training hooks
###    OSTaskCreateExt_hook_list=[
###        0x8042368, 0x8044028, 0x80442c4, 0x80442f8, 0x804432c, 0x8044360, 0x8044394, 0x80443c8,
###        0x80443fc, 0x8044430, 0x8044464, 0x8044498, 0x80444cc, 0x8044500, 0x8044534, 0x8049150,
###        0x804ae5c];
###    ### only for debug and information addiction
####    for adr in OSTaskCreateExt_hook_list:
####        merger.hookbl(adr, sapplet.getadr("OSTaskCreateExt_hook"),0);
###
###    OSTaskNameSet_hook_list=[
###        0x8042374, 0x8044034, 0x80442d0, 0x8044304, 0x8044338, 0x804436c, 0x80443a0, 0x80443d4,
###        0x8044408, 0x804443c, 0x8044470, 0x80444a4, 0x80444d8, 0x804450c, 0x8044540, 0x804915c,
###        0x804ae68];
###    ### only for debug and information addiction
### #   for adr in OSTaskNameSet_hook_list:
### #       merger.hookbl(adr, sapplet.getadr("OSTaskNameSet_hook"),0);
###
###    Create_MenuEntrylist=[
###        0x0800c278, 0x0800c2c0, 0x0800c2f4, 0x0800c326, 0x0800c358, 0x0800c38a, 0x0800c3bc, 0x0800c468,
###        0x0800c4a8, 0x0800c4d2, 0x0800c4fa, 0x0800c522, 0x0800c54a, 0x0800c572, 0x0800c5ec, 0x0800c614,
###        0x0800c64a, 0x0800c674, 0x0800c69c, 0x0800c6c4, 0x0800c6ec, 0x080191f2, 0x08019226, 0x0801925a,
###        0x0801928a, 0x0802d264, 0x0802d2ca, 0x0802d326, 0x0802d356, 0x080197d8, 0x0801980a, 0x08019a08,
###        0x08019a3a, 0x08019a6c, 0x08019a9e, 0x08019e78, 0x08019ea6, 0x0801a070, 0x0801a0a2, 0x0802c060,
###        0x0802c096, 0x0802c0cc, 0x0802c102, 0x0802c138, 0x08018f70, 0x08018fa2, 0x08012872, 0x080128a4,
###        0x080128d6, 0x08012908, 0x08012948, 0x0801297c, 0x080129bc, 0x080129ee, 0x08012a22, 0x08012a54,
###        0x08012b3a, 0x0802c1d4, 0x0801535a, 0x0801538c, 0x0801674e, 0x08016782, 0x080167ca, 0x0801681c,
###        0x08016866, 0x080168ba, 0x08016904, 0x0801694e, 0x08016986, 0x080169d0, 0x08016a20, 0x08016a4a,
###        0x08016a90, 0x08016ac4, 0x08016afc, 0x08016b32, 0x08016b7c, 0x08038c66, 0x08038cd4, 0x08038d38,
###        0x08038d6c, 0x080194e0, 0x0801950e, 0x080179d6, 0x08017a08, 0x08017ff2, 0x08018022, 0x08018052,
###        0x08018080, 0x080180ae, 0x080180dc, 0x08016e2a, 0x0802c558, 0x0801a158, 0x0801a18a, 0x0801a1bc,
###        0x0801a1ee, 0x0801270e, 0x08012782, 0x0802c610, 0x0802c63c, 0x0801273c, 0x0802d1ce, 0x0802ce84,
###        0x0802c8aa, 0x0802c8d6, 0x0802cc28, 0x0802c952, 0x0802ca62, 0x0802c6ca, 0x0802c6f6, 0x0802c76a,
###        0x0802c798, 0x0802c7c6, 0x0802fa46, 0x0801b130, 0x0800f50c, 0x0800f53e, 0x0800f588, 0x08019d54,
###        0x0801b240, 0x0800e01e, 0x0800e040, 0x080205cc, 0x0802dfe0, 0x0802e014, 0x0802e048, 0x080346da,
###        0x08034704, 0x08012c44, 0x08012c76, 0x08012ca8, 0x08012cda, 0x08012d1a, 0x08012d4e, 0x08012d8e,
###        0x08012dc0, 0x08012df4, 0x08012e26, 0x08012ea4, 0x080154bc, 0x08016c4a, 0x08016c7e, 0x08016d22,
###        0x08016dac, 0x08016fe4, 0x08017b30, 0x08017b60, 0x08017df0, 0x08017e22, 0x08018186, 0x080181b2,
###        0x0801829c, 0x080182cc, 0x08018320, 0x08018350, 0x08018402, 0x08018434, 0x08018466, 0x08018498,
###        0x08018562, 0x08018594, 0x0801865a, 0x0801868c, 0x0801871a, 0x080187de, 0x08018966, 0x08018b94,
###        0x08018c24, 0x08018ce0, 0x08018d10, 0x08018da4, 0x08018e68, 0x08019038, 0x080190d8, 0x08019318,
###        0x080193a2, 0x0801942a, 0x080195ba, 0x08019688, 0x08019714, 0x08019b30, 0x08019bc4, 0x08019c4c,
###        0x08019cce, 0x08019f2a, 0x08019fae, 0x0801a2d0, 0x0801a360, 0x0801a3e0, 0x0801a458, 0x0801a4cc,
###        0x0801a624, 0x0801a652, 0x0801a682, 0x0801a6fe, 0x0801a7b8, 0x0801a85e, 0x0801a91c, 0x0801a9f2,
###        0x0801aa22, 0x0801aa54, 0x0801abb2, 0x0801ac1c, 0x0801ac4e, 0x0801af14, 0x0801b07a, 0x0801b1c0,
###        0x0801b2ca, 0x0801b2f2, 0x0801b364, 0x0801ad50, 0x0801ad82, 0x0801adec, 0x0801ae1e, 0x0801245c,
###        0x080198a2, 0x0801992c];
###    ### only for debug and information addiction
###    #for adr in Create_MenuEntrylist:
###    #    merger.hookbl(adr,sapplet.getadr("create_menu_entry_hook"),0);
###
###    #All _maybe_ hooks on OSMboxPost
###    OSMboxPost_hook_list=[
###        0x0801f11e, 0x0801f16e, 0x08020540, 0x08027bec, 0x080284f8, 0x0802859c, 0x08028628, 0x0802873a,
###        0x08028c6a, 0x08028d9e, 0x08028de8, 0x08028eca, 0x08028f04, 0x0802900c, 0x0802915e, 0x080293c4,
###        0x08029508, 0x08029552, 0x080295c0, 0x080295f6, 0x0802979c, 0x0802983c, 0x08029868, 0x080298fe,
###        0x08029970, 0x0802998a, 0x080299f2, 0x08029a28, 0x08029a74, 0x08029aac, 0x08029b04, 0x08029b3c,
###        0x08029bd8, 0x08029c64, 0x08029cd8, 0x08029cfa, 0x08029d1c, 0x08029d34, 0x08029daa, 0x08029dfa,
###        0x08029e5c, 0x08029e8c, 0x08029ef6, 0x08029f28, 0x08029fa4, 0x08029fc4, 0x08029ffe, 0x0802a054,
###        0x0802a1ac, 0x0802ce18, 0x0802df4e, 0x08030aee, 0x08030c2e, 0x08030cc4, 0x08030da2, 0x08031006,
###        0x08031060, 0x0803b786, 0x0803b7a6, 0x0803b7f2, 0x0803b86e, 0x0803b970, 0x0803ba02, 0x0803bb50,
###        0x0803bc04, 0x0803bcc6, 0x0803bd2c, 0x0803bdac, 0x0803bdc4, 0x0803be7e, 0x0803bf28, 0x0803bf64,
###        0x0803bf82, 0x0803bf96, 0x0803bfbc, 0x0803c090, 0x0803c0c2, 0x0803c158, 0x0803c18a, 0x0803c1a0,
###        0x0803c1fe, 0x0803c278, 0x0803c3ce, 0x0803c474, 0x0803c50a, 0x0803c542, 0x0803c71c, 0x0803c75e,
###        0x0803c7f6, 0x0803c8cc, 0x0803c936, 0x0803c956, 0x0803cad4, 0x0803cbb6, 0x0803cbde, 0x0803cc78,
###        0x0803cca0, 0x0803cd36, 0x0803cd54, 0x0803cd82, 0x0803cda0, 0x0803cdfa, 0x0803ce62, 0x0803ce96,
###        0x0803cece, 0x0803cf02, 0x0803cf52, 0x0803cf90, 0x0803d5ac, 0x0803d8be, 0x0803dc7c, 0x0803dd94,
###        0x0803e252, 0x0803eaa8, 0x0803eb90, 0x0803ebf2, 0x0803ec08, 0x0803eea0, 0x0803ef64, 0x0803f01c,
###        0x0803f1b6, 0x0803f2ae, 0x0803f2fc, 0x0803f30e, 0x0803f3f4, 0x0803f4b0, 0x0803f524, 0x0803f668,
###        0x0803f7b8, 0x0803f7dc, 0x0803f81e, 0x0803f846, 0x0803f872, 0x0803f8aa, 0x0803f8ca, 0x0803f8f6,
###        0x0803fd1a, 0x0803fd76, 0x08040340, 0x080406f4, 0x08040924, 0x08040ca8, 0x08040cc8, 0x08040d04,
###        0x080411c2, 0x080411da, 0x08041226, 0x0804358c, 0x080440ea, 0x080446b8, 0x080447da, 0x0804b724,
###        0x0808ce7e, 0x0808ce94, 0x0808ceda, 0x0808cf0c, 0x0808cfba];
####    for adr in OSMboxPost_hook_list:
####        merger.hookbl(adr,sapplet.getadr("OSMboxPost_hook"),0);
###
###    #Throwaway hook to see if adr is called.
###    #merger.hookstub(0x0803f03c,
###    #                sapplet.getadr("demo"));
###
###    f_4137_hook_list=[
###        0x8027fe2, 0x8028288, 0x8028298, 0x80282f0];
###
####    for adr in f_4137_hook_list:
####        merger.hookbl(adr,sapplet.getadr("f_4137_hook"),0);
####    merger.hookbl(0x804464a,sapplet.getadr("f_4520_hook"),0);
####    merger.hookbl(0x8044642,sapplet.getadr("f_4098_hook"),0);
####    merger.hookbl(0x804c1e8,sapplet.getadr("f_4102_hook"),0);
###
###    merger.hookbl(0x8044662, sapplet.getadr("f_4225_hook"),0);
###    
###    # display hooks is in d13.020 included
###    ## display flip workaround see issue #178 not necessary on 0X3.020
###    merger.hookbl(0x08031fde,sapplet.getadr("display_init_hook_1"),0);
###    merger.hookbl(0x0803200e,sapplet.getadr("display_init_hook_2"),0);


    print "Merging %s into %s at %08x" % (
          sys.argv[2],
          sys.argv[1],
          index);
    
    i=0;
    for b in bapplet:
        merger.setbyte(index+i,bapplet[i]);
        i=i+1;
    
    merger.export(sys.argv[1]);


