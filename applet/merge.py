#! python2

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
        print "Inserting a stub hook at %08x to %08x." % (adr,handler);
        
        #FIXME This clobbers r0, should use a different register.
        self.sethword(adr,0x4801); # ldr r0, [pc, 4]
        self.sethword(adr+2,0x4700); # bx r0
        self.sethword(adr+4,0x4600); #NOP
        self.sethword(adr+6,0x4600); #NOP, might be overwritten
        if adr&2>0:
            self.setword(adr+6,handler); # bx r0
        else:
            self.setword(adr+8,handler); # bx r0
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
        
        print "Redirecting a bl at %08x to %08x." % (adr,handler);
        
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
    
    #Patch some symbols
    merger.hookstub(0x080969de,    #USB manufacturer string handler function.
                    sapplet.getadr("getmfgstr"));
    merger.hookstub(0x08021894, #startup_botline
                    sapplet.getadr("demo"));
    merger.hookstub(0x080154de,
                    sapplet.getadr("loadfirmwareversion"));
    merger.hookbl(0x0808cc36, #Call to usb_dfu_upload().
                  sapplet.getadr("usb_upld_hook"),
                  0x0808d3d8); #Old handler adr.
    merger.hookbl(0x0803eb64, #Call to dmr_call_end()
                  sapplet.getadr("dmr_call_end_hook"),
                  0x0803f33c); #Old handler adr.
#    merger.hookbl(0x0803eb7a, #Call to dmr_call_thing();
#                  sapplet.getadr("dmr_call_thing_hook"),
#                  0x0803f6d8); #Old handler adr.
    merger.hookbl(0x0803e9ee, #Call to dmr_call_start();
                  sapplet.getadr("dmr_call_start_hook"),
                  0x0803ec86); #Old handler adr.
    merger.hookbl(0x0803eb4e, #Call to dmr_call_start();
                  sapplet.getadr("dmr_call_start_hook"),
                  0x0803ec86); #Old handler adr.
    #Three calls to dmr_handle_data, likely for diff reasons.
    merger.hookbl(0x0803ebc2, #First call to dmr_handle_data
                  sapplet.getadr("dmr_handle_data_hook"),
                  0x0804b66c);
    merger.hookbl(0x0803ebda, #Second call to dmr_handle_data
                  sapplet.getadr("dmr_handle_data_hook"),
                  0x0804b66c);
    merger.hookbl(0x0803ec20, #Third call to dmr_handle_data
                  sapplet.getadr("dmr_handle_data_hook"),
                  0x0804b66c);
    #SMS handler function.
    merger.hookbl(0x0803ebaa, #sms_arrive_hook().
                  sapplet.getadr("dmr_sms_arrive_hook"),
                  0x0803f03c);

    merger.hookbl(0x8042368,
                  sapplet.getadr("OSTaskCreateExt_hook"),
                  0x0804bbf4);
    merger.hookbl(0x8044028,
                  sapplet.getadr("OSTaskCreateExt_hook"),
                  0x0804bbf4);
    merger.hookbl(0x80442c4,
                  sapplet.getadr("OSTaskCreateExt_hook"),
                  0x0804bbf4);
    merger.hookbl(0x80442f8,
                  sapplet.getadr("OSTaskCreateExt_hook"),
                  0x0804bbf4);
    merger.hookbl(0x804432c,
                  sapplet.getadr("OSTaskCreateExt_hook"),
                  0x0804bbf4);
    merger.hookbl(0x8044360,
                  sapplet.getadr("OSTaskCreateExt_hook"),
                  0x0804bbf4);
    merger.hookbl(0x8044394,
                  sapplet.getadr("OSTaskCreateExt_hook"),
                  0x0804bbf4);
    merger.hookbl(0x80443c8,
                  sapplet.getadr("OSTaskCreateExt_hook"),
                  0x0804bbf4);
    merger.hookbl(0x80443fc,
                  sapplet.getadr("OSTaskCreateExt_hook"),
                  0x0804bbf4);
    merger.hookbl(0x8044430,
                  sapplet.getadr("OSTaskCreateExt_hook"),
                  0x0804bbf4);
    merger.hookbl(0x8044464,
                  sapplet.getadr("OSTaskCreateExt_hook"),
                  0x0804bbf4);
    merger.hookbl(0x8044498,
                  sapplet.getadr("OSTaskCreateExt_hook"),
                  0x0804bbf4);
    merger.hookbl(0x80444cc,
                  sapplet.getadr("OSTaskCreateExt_hook"),
                  0x0804bbf4);
    merger.hookbl(0x8044500,
                  sapplet.getadr("OSTaskCreateExt_hook"),
                  0x0804bbf4);
    merger.hookbl(0x8044534,
                  sapplet.getadr("OSTaskCreateExt_hook"),
                  0x0804bbf4);
    merger.hookbl(0x8049150,
                  sapplet.getadr("OSTaskCreateExt_hook"),
                  0x0804bbf4);
    merger.hookbl(0x804ae5c,
                  sapplet.getadr("OSTaskCreateExt_hook"),
                  0x0804bbf4);


    merger.hookbl(0x8042374,
                  sapplet.getadr("OSTaskNameSet_hook"),
                  0x804bcc0);
    merger.hookbl(0x8044034,
                  sapplet.getadr("OSTaskNameSet_hook"),
                  0x804bcc0);
    merger.hookbl(0x80442d0,
                  sapplet.getadr("OSTaskNameSet_hook"),
                  0x804bcc0);
    merger.hookbl(0x8044304,
                  sapplet.getadr("OSTaskNameSet_hook"),
                  0x804bcc0);
    merger.hookbl(0x8044338,
                  sapplet.getadr("OSTaskNameSet_hook"),
                  0x804bcc0);
    merger.hookbl(0x804436c,
                  sapplet.getadr("OSTaskNameSet_hook"),
                  0x804bcc0);
    merger.hookbl(0x80443a0,
                  sapplet.getadr("OSTaskNameSet_hook"),
                  0x804bcc0);
    merger.hookbl(0x80443d4,
                  sapplet.getadr("OSTaskNameSet_hook"),
                  0x804bcc0);
    merger.hookbl(0x8044408,
                  sapplet.getadr("OSTaskNameSet_hook"),
                  0x804bcc0);
    merger.hookbl(0x804443c,
                  sapplet.getadr("OSTaskNameSet_hook"),
                  0x804bcc0);
    merger.hookbl(0x8044470,
                  sapplet.getadr("OSTaskNameSet_hook"),
                  0x804bcc0);
    merger.hookbl(0x80444a4,
                  sapplet.getadr("OSTaskNameSet_hook"),
                  0x804bcc0);
    merger.hookbl(0x80444d8,
                  sapplet.getadr("OSTaskNameSet_hook"),
                  0x804bcc0);
    merger.hookbl(0x804450c,
                  sapplet.getadr("OSTaskNameSet_hook"),
                  0x804bcc0);
    merger.hookbl(0x8044540,
                  sapplet.getadr("OSTaskNameSet_hook"),
                  0x804bcc0);
    merger.hookbl(0x804915c,
                  sapplet.getadr("OSTaskNameSet_hook"),
                  0x804bcc0);
    merger.hookbl(0x804ae68,
                  sapplet.getadr("OSTaskNameSet_hook"),
                  0x804bcc0);


    
    #Throwaway hook to see if adr is called.
    #merger.hookstub(0x0803f03c,
    #                sapplet.getadr("demo"));

    print "Merging %s into %s at %08x" % (
          sys.argv[2],
          sys.argv[1],
          index);
    
    i=0;
    for b in bapplet:
        merger.setbyte(index+i,bapplet[i]);
        i=i+1;
    
    merger.export(sys.argv[1]);


