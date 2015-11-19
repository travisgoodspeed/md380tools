
class Patcher():
    """MD380 Firmware Patching Tool"""
    
    # #Includes Bootloader
    # offset=0x08000000;
    
    #Just the application.
    offset=0x0800C000;
    
    
    def getbyte(self,adr):
        """Reads a byte from the firmware address."""
        b=self.bytes[adr-self.offset];
        return b;
    
    def assertbyte(self, adr, val):
        """Asserts that a byte has a given value."""
        assert self.getbyte(adr)==val;
        return;

    def setbyte(self, adr, new, old=None):
        """Patches a single byte from the old value to the new value."""
        if old!=None:
            self.assertbyte(adr,old);
        print "Patching byte at %08x to %02x" % (adr,new)
        self.bytes[adr-self.offset]=new;
        self.assertbyte(adr,new);
    def ffrange(self, start,end):
        """Patches a range to FF."""
        print "Patching range from %08x to %08x to FF." % (start,end)
        for adr in range(start,end):
            self.bytes[adr-self.offset]=0xFF;
    def sethword(self, adr, new, old=None):
        """Patches a byte pair from the old value to the new value."""
        if old!=None:
            self.assertbyte(adr,old&0xFF);
            self.assertbyte(adr+1,(old>>8)&0xFF);
        print "Patching hword at %08x to %04x" % (adr,new)
        self.bytes[adr-self.offset]=new&0xFF;
        self.bytes[adr-self.offset+1]=(new>>8)&0xFF;
        self.assertbyte(adr,new&0xFF);
        self.assertbyte(adr+1,(new>>8)&0xFF);
    def nopout(self, adr, old=None):
        """Nops out an instruction with 0xd11f."""
        self.sethword(adr,0x46c0,old);

    def __init__(self,filename,offset=0x08000000):
        """Opens the input file."""
        self.file=open(filename,"rb");
        self.bytes=bytearray(self.file.read());
        self.length=len(self.bytes);

    def export(self,filename):
        """Exports to a binary file."""
        outfile=open(filename,"wb");
        outfile.write(self.bytes);
        outfile.close();

        
