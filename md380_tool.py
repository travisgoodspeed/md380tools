#!/usr/bin/env python2

# md380-tool by KK4VCZ and Friends

# This is the client for the patched MD380 firmware.  It does all
# sorts of clever things that the official clients can't, but it
# probably has bugs and will do all sorts of unsavory things.  Do not
# expose it to light, do not feed it after midnight, and *NEVER* give
# it water.

from DFU import DFU, State, Request
import time, sys, struct, usb.core

# The tricky thing is that *THREE* different applications all show up
# as this same VID/PID pair.
#
# 1. The Tytera application image.
# 2. The Tytera bootloader at 0x08000000
# 3. The mask-rom bootloader from the STM32F405.
md380_vendor   = 0x0483
md380_product  = 0xdf11

class UsersDB():
    """List of registered DMR-MARC users."""
    users={};
    def __init__(self, filename=None):
        """Loads the database."""
        import csv;
        if filename==None:
            filename=sys.path[0]+'/db/users.csv';
        with open(filename,'rb') as csvfile:
            reader=csv.reader(csvfile);
            for row in reader:
                if len(row)>0:
                    self.users[int(row[0])]=row;
    def getuser(self,id):
        """Returns a user from the ID."""
        try:
            return self.users[id];
        except:
            call="";
            name="";
            nickname="";
            city="";
            state="";
            country="";
            comment="";
            return ["%i"%id,call,name,nickname,city,state,country,comment];
    def getusername(self,id):
        """Returns a formatted username from the ID."""
        user=self.getuser(id);
        return("%s %s (%s)"%(
                user[1],
                user[2],
                user[0]));
    

#Quick to load, so might as well do it early.
users=UsersDB();


class Tool(DFU):
    """Client class for extra features patched into the MD380's firmware.
    None of this will work with the official firmware, of course."""
    
    def drawtext(self,str,a,b):
        """Sends a new MD380 command to draw text on the screen.."""
        cmd=0x80; #Drawtext
        a=a&0xFF;
        b=b&0xFF;
        self._device.ctrl_transfer(0x21, Request.DNLOAD, 1, 0, chr(cmd)+chr(a)+chr(b)+self.widestr(str))
        self.get_status(); #this changes state
        time.sleep(0.1);
        status=self.get_status(); #this gets the status
        if status[2]==State.dfuDNLOAD_IDLE:
            if self.verbose: print "Sent custom %02x %02x." % (a,b);
            self.enter_dfu_mode();
        else:
            print "Failed to send custom %02x %02x." % (a,b);
            return False;
        return True;
    def peek(self,adr,size):
        """Returns so many bytes from an address."""
        self.set_address(adr);
        return self.upload(1,size,0);
    def spiflashgetid(self):
	size=4;
        """Returns SPI Flash ID."""
        cmd=0x05; #SPIFLASHGETID
	cmdstr=(chr(cmd));
        self._device.ctrl_transfer(0x21, Request.DNLOAD, 1, 0,
                                   cmdstr)
        self.get_status(); #this changes state
        status=self.get_status(); #this gets the status
        return self.upload(1,size,0);
    def spiflashpeek(self,adr,size=1024):
        """Returns so many bytes from SPI Flash."""
        cmd=0x01; #SPIFLASHREAD
        cmdstr=(chr(cmd)+
                chr(adr&0xFF)+
                chr((adr>>8)&0xFF)+
                chr((adr>>16)&0xFF)+
                chr((adr>>24)&0xFF)
                );
        self._device.ctrl_transfer(0x21, Request.DNLOAD, 1, 0,
                                   cmdstr)
        self.get_status(); #this changes state
        status=self.get_status(); #this gets the status
        return self.upload(1,size,0);
    def spiflash_erase64kblock(self, adr,size=1024):
        """Clear 64kb block on spi flash."""
        cmd=0x03; #SPIFLASHWRITE
        cmdstr=(chr(cmd)+
                chr(adr&0xFF)+
                chr((adr>>8)&0xFF)+
                chr((adr>>16)&0xFF)+
                chr((adr>>24)&0xFF)
                );
        self._device.ctrl_transfer(0x21, Request.DNLOAD, 1, 0,
                                   cmdstr)
	self.get_status(); #this changes state
        time.sleep(0.1);

        status=self.get_status(); #this gets the status
        return self.upload(1,size,0);

    def spiflashpoke(self,adr,size,data):
        """Returns so many bytes from SPI Flash."""
        cmd=0x04; #SPIFLASHWRITE_NEW
#        print  size
        cmdstr=(chr(cmd)+
                chr(adr&0xFF)+
                chr((adr>>8)&0xFF)+
                chr((adr>>16)&0xFF)+
                chr((adr>>24)&0xFF)+
                chr(size&0xFF)+
                chr((size>>8)&0xFF)+
                chr((size>>16)&0xFF)+
                chr((size>>24)&0xFF)
                );

        for i in range(0,size,1):
            cmdstr=cmdstr+data[i];

#        print len(cmdstr)
        self._device.ctrl_transfer(0x21, Request.DNLOAD, 1, 0,
                                   cmdstr)
        self.get_status(); #this changes state
        status=self.get_status(); #this gets the status
#        print status
        return self.upload(1,size,0);
    def getsms(self,index):
        """Returns an inbound SMS from SPI Flash."""
        buf=self.spiflashpeek(0x41674+index*0x124,
                              0x124);
        srcadr=buf[0]+(buf[1]<<8)+(buf[2]<<16);
        flags=buf[3];
        message="";
        for i in range(4,0x124,2):
            c=chr(buf[i]);
            if c!='\0':
                message=message+c;
            else:
                return [srcadr,flags,message];
        return [None,None,None];
    def getsentsms(self,index):
        """Returns an outbound SMS from SPI Flash."""
        buf=self.spiflashpeek(0x450a4+index*0x124,
                              0x124);
        srcadr=buf[0]+(buf[1]<<8)+(buf[2]<<16);
        flags=buf[3];
        message="";
        for i in range(4,0x124,2):
            c=chr(buf[i]&0x7F);
            if c!='\0':
                message=message+c;
            else:
                return [srcadr,flags,message];
        return [None,None,None];
    def getkey(self,index):
        """Returns an Enhanced Privacy key from SPI Flash.  1-indexed"""
        buf=self.spiflashpeek(0x59c0+16*index-16,
                              16);
        return buf;

    def c5000peek(self,reg):
        """Returns one byte from a C5000 register."""
        cmd=0x11; #C5000 Read Reg
        cmdstr=(chr(cmd)+
                chr(reg&0xFF)
                );
        self._device.ctrl_transfer(0x21, Request.DNLOAD, 1, 0,
                                   cmdstr)
        self.get_status(); #this changes state
        status=self.get_status(); #this gets the status
        buf=self.upload(1,1024,0); #Peek the 1024 byte dmesg buffer.
        return buf[0];
    def c5000poke(self,reg,val):
        """Writes a byte into a C5000 register."""
        cmd=0x10; #C5000 Write Reg
        cmdstr=(chr(cmd)+
                chr(reg&0xFF)+
                chr(val&0xFF)
                );
        self._device.ctrl_transfer(0x21, Request.DNLOAD, 1, 0,
                                   cmdstr)
        self.get_status(); #this changes state
        status=self.get_status(); #this gets the status
    def getdmesg(self):
        """Returns the 1024 byte DMESG buffer."""
        cmd=0x00; #DMESG
        self._device.ctrl_transfer(0x21, Request.DNLOAD, 1, 0, chr(cmd))
        self.get_status(); #this changes state
        #time.sleep(0.1);
        status=self.get_status(); #this gets the status
        buf=self.upload(1,1024,0); #Peek the 1024 byte dmesg buffer.
        
        #Okay, so at this point we have the buffer, but it's a ring
        #buffer that might have already looped, so we need to reorder
        #if that is the case or crop it if it isn't.
        tail="";
        head=None;
        for b in buf:
            if head==None:
                if b>0:
                    tail=tail+chr(b);
                else:
                    head="";
            else:
                if b>0:
                    head=head+chr(b);
                else:
                    break;
        if head==None:
            return tail;
        return head+tail;

def calllog(dfu):
    """Prints a call log to stdout, fetched from the MD380's memory."""
    dfu.drawtext("Hooking calls!",160,50);
    
    #Set the target address to the list of DMR addresses.
    dfu.set_address(0x2001d098);
    old1=0;
    old2=0;
    while 1:
        data=dfu.upload(1,16,0);#Peek sixteen bytes.
        llid0=(data[0]+
               (data[1]<<8)+
               (data[2]<<16)+
               (data[3]<<24));
        llid1=(data[4]+
               (data[5]<<8)+
               (data[6]<<16)+
               (data[7]<<24));
        llid2=(data[8]+
               (data[9]<<8)+
               (data[10]<<16)+
               (data[11]<<24));
        if old1!=llid1 or old2!=llid2:
            old1=llid1;
            old2=llid2;
            print "DMR call from %s to %s." % (
                users.getusername(llid1),users.getusername(llid2));
            #get actual canel name
            dfu.set_address(0x2001c9d4);
            data=dfu.upload(1,32,0);
            message=""
            for i in range(0,32,2):
               c=chr(data[i]&0x7F);
               if c!='\0':
                  message=message+c;
            print message
             #get actual zone name
            dfu.set_address(0x2001b958);
            data=dfu.upload(1,32,0);
            message=""
            for i in range(0,32,2):
               c=chr(data[i]&0x7F);
               if c!='\0':
                 message=message+c;
            print message
            sys.stdout.flush();


def dmesg(dfu):
    """Prints the dmesg log from main memory."""
    #dfu.drawtext("Dumping dmesg",160,50);
    print dfu.getdmesg();
def coredump(dfu,filename):
    """Dumps a corefile of RAM."""
    with open(filename,'wb') as f:
        for adr in range(0x20000000,
                         0x20000000+(128*1024),
                         1024):
            #print "Fetching %08x"%adr
            buf=dfu.peek(adr,1024);
            f.write(buf);
        f.close();
def hexdump(dfu,address,length=512):
    """Dumps from memory to the screen"""
    adr=int(address,16);
    buf=dfu.peek(adr,length);
    i=0;
    cbuf="";
    for b in buf:
        sys.stdout.write("%02x "%b);
        i=i+1;
        if(b > 32 and b < 127 ):
            cbuf = cbuf + chr(b);
        else:
            cbuf = cbuf + ".";
        if i%16==0:
            sys.stdout.write(" " + cbuf);
            sys.stdout.write("\n");
            cbuf="";
        elif i%8==0:
            sys.stdout.write(" ");
def hexwatch(dfu,address):
    """Dumps from memory to the screen"""
    while True:
        hexdump(dfu,address,16);
        time.sleep(0.05);
def dump(dfu,filename,address):
    """1k Binary dumps"""
    adr=int(address,16);
    with open(filename,'wb') as f:
        buf=dfu.peek(adr,1024);
        f.write(buf);
        f.close();
def flashgetid(dfu):
    size=0;
    buf=dfu.spiflashgetid();
    print "SPI Flash ID: %x %x %x"%(buf[0],buf[1],buf[2])
    if (buf[0] == 0xef and buf[1] == 0x40):
        if (buf[2] == 0x18):
            sys.stdout.write("W25Q128FV 16MByte\n");
            size=16*1024*1024;
        elif (buf[2] == 0x14):
            sys.stdout.write("W25Q80BL 1MByte\n");
            size=1*1024*1024;
    elif (buf[0] == 0x10 and buf[1] == 0xdc):
        if (buf[2] == 0x01):
            sys.stdout.write("W25Q128FV 16MByte maybe\n");
            size=16*1024*1024;          
    else:
            sys.stdout.write("Unkown SPI Flash - please report\n");
    return size;

def flashdump(dfu,filename):
    """Dumps flash."""
    with open(filename,'wb') as f:
        for adr in range(0x08000000,
                         0x08000000+(1024*1024),
                         1024):
            #print "Fetching %08x"%adr
            buf=dfu.peek(adr,1024);
            f.write(buf);
        f.close();
def spiflashdump(dfu,filename):
    """Dumps SPI Flash."""
    with open(filename,'wb') as f:
        for adr in range(0x00000000,
                         0x00000000+(16*1024*1024),
                         1024):
            #print "Fetching %08x"%adr
            buf=dfu.spiflashpeek(adr,1024);
            f.write(buf);
        f.close();
def spiflashwrite(dfu,filename,adr):
     """Programm SPI Flash."""
     if (flashgetid(dfu) == 16*1024*1024):
        with open(filename,'rb') as f:
            data = f.read()
            size=len(data)
            dfu.md380_custom(0x91,0x01);  # disable any radio and UI events
                                          # while on spi flash 
            print "erase %d bytes @ 0x%x" % (size, adr);
            for n in range(adr,adr+size+1,0x1000):
#                print "erase %x " % n
                dfu.spiflash_erase64kblock(n)
            fullparts = int(size/1024)
            print "flashing %d bytes @ 0x%x" % (size, adr);
            if fullparts > 0:
                for n in range(0,fullparts,1):
#                    print "%d %d %x %d " % (fullparts, n, adr+n*1024, 1024)
                    dfu.spiflashpoke( adr+n*1024,1024,data[n*1024:(n+1)*1024]);
            lastpartsize= size - fullparts * 1024

            if ( lastpartsize  > 0 ):
#                print "%d  %x %d " % (fullparts,  adr+fullparts*1024, lastpartsize)
                dfu.spiflashpoke(adr+fullparts*1024,lastpartsize,data[(fullparts)*1024:(fullparts)*1024+lastpartsize]);
            sys.stdout.write("reboot radio now\n"); 
            dfu.md380_reboot();
            f.close();
     else:
         sys.stdout.write("can't programm spi flash wrong flash type\n");
def dmesgtail(dfu):
    """Keeps printing the dmesg buffer."""
    while True:
        sys.stdout.write(dfu.getdmesg());
        #time.sleep(0.01);
        sys.stdout.flush();
def c5000(dfu):
    """Prints some DMR registers."""
    for r in range(0,0x87):
        sys.stdout.write("[0x%02x]=0x%02x\t" % (r,dfu.c5000peek(r)));
        if r%4==3:
            sys.stdout.write("\n");
            sys.stdout.flush();
    sys.stdout.write("\n");
def rssi(dfu):
    """Graphs the RSSI value.  Kinda useless."""
    while True:
        rssih=dfu.c5000peek(0x43);
        rssil=dfu.c5000peek(0x44);
        rssi=(rssih<<8)|rssil;
        print "%04x" % rssi;
        time.sleep(0.25);
def messages(dfu):
    """Prints all the SMS messages."""
    for i in range(1,0xff):
        [src,flags,message]=dfu.getsms(i);
        if src!=None and flags&0xF==0x2:
            print "From %s: %s"%(users.getusername(src),message);
    for i in range(1,0xff):
        [src,flags,message]=dfu.getsentsms(i);
        if src!=None and flags&0xF==0x2:
            print "To   %s: %s"%(users.getusername(src),message);

def keys(dfu):
    """Prints all the Enhanced Privacy keys."""
    for i in range(1,9):
        buf=dfu.getkey(i);
        keystr="";    #Keys are displayed as big-endian, stored as little.
        for b in buf:
            keystr="%02x %s" % (b,keystr);
        print "%02i: %s"%(i,keystr);

def findcc(dfu):
    """Hunts for the color code of a transmitter."""
    cc=0; #Guess at the color code.
    while True:
        #Try a new color.
        cc=cc+1;
        print "Trying color %d." % (cc&0xf);
        sys.stdout.flush();
        dfu.c5000poke(0x1f,(cc&0xF)<<4);
        time.sleep(1.0);
        if dfu.c5000peek(0x0d)&0x10:
            print "Got a match on color %i"%(cc&0xf);
            while dfu.c5000peek(0x0d)&0x10:
                time.sleep(0.5);
                sys.stdout.write(".");
                sys.stdout.flush();
            sys.stdout.write("\n");


def bcd(b):
    return int("%02x"%b);

def calldate(dfu):
    """Print Time and Date  to stdout, fetched from the MD380's RTC."""
    dfu.set_address(0x40002800);  # 2.032
    data=dfu.upload(1,8,0);

    print "%02d.%02d.%02d %02d:%02d:%02d" % (
                                bcd(data[4] & (0x0f | 0x30)),
                                bcd(data[5] & (0x0f )),
                                bcd(data[6] ),
                                bcd(data[2] ),
                                bcd(data[1] & (0x0f |  0x70)),
                                bcd(data[0] & (0x0f | 0x70)) )

def calladc1(dfu):
    """Print ADC1 Voltage (Battery), fetched from the MD380's Memory (Update with DMA)."""
    dfu.set_address(0x2001cfcc);   #  2.032
    data=dfu.upload(1,4,0);
    # 7.2V ~ 2.4V PA1 (BATT) ... 2715 ~ 6.5V ... 3.3V 12BIT
    print "%f Volt"  % ( 3.3 / 0xfff * ((data[3] << 8 )+ data[2]) * 3 )
def getchannel(dfu):
    """Print actual Channel, fetched from the MD380's Memory."""
    dfu.set_address(0x2001d376);  # 2.032
    data=dfu.upload(1,4,0);
    print "%02d %02d %02d %02d" % ( data[3], data[2], data[1], data[0])
def readword(dfu, address):
    print "%x"%(int(address, 0))
    dfu.set_address(int(address,0));  # 2.032
    data=dfu.upload(1,4*4,0);
    print "%x %02x%02x%02x%02x" % (int(address, 0), data[3], data[2], data[1], data[0])
    print "%x %02x %02x %02x %02x" % (int(address, 0), data[3], data[2], data[1], data[0])
    print "%x %02x %02x %02x %02x" % (int(address, 0)+4, data[7], data[6], data[5], data[4])
    print "%x %02x %02x %02x %02x" % (int(address, 0)+8, data[11], data[10], data[9], data[8])
    print "%x %02x %02x %02x %02x" % (int(address, 0)+12, data[15], data[14], data[13], data[12])
    print "%d" % ( data[3] << 24|  data[2]<<16 | data[1]<<8 | data[0])
def init_dfu(alt=0):
    dev = usb.core.find(idVendor=md380_vendor,
                        idProduct=md380_product)
    
    if dev is None:
        raise RuntimeError('Device not found')

    dfu = Tool(dev, alt)
    dev.default_timeout = 3000
    
    try:
        dfu.enter_dfu_mode()
        pass;
    except usb.core.USBError, e:
        if len(e.args) > 0 and e.args[0] == 'Pipe error':
            raise RuntimeError('Failed to enter DFU mode. Is bootloader running?')
        else:
            raise e
    
    return dfu

def usage():
    print("""
Usage: md380-tool <command> <arguments>

Print a log of incoming DMR calls to stdout.
    md380-tool calllog

Looks up the name by an ID number.
    md380-tool lookup 12345

Prints the dmesg buffer.
    md380-tool dmesg
Follow the dmesg buffer.
    md380-tool dmesgtail

Prints the C5000 baseband registers.
    md380-tool c5000
Scans for DMR traffic on all color codes.
    md380-tool findcc
Dumps all the inbound and outbound text messages.
    md380-tool messages
Dumps all the keys.
    md380-tool keys

Prints the SPI Flash Type.
    md380-tool spiflashid
Dump all of flash memory.
    md380-tool flashdump <filename.bin>
Dump the complete SPI Flash image (16MByte).
    md380-tool spiflashdump <filename.bin>
Dump a core file of RAM.
    md380-tool coredump <filename.bin>
Dumps memory in hex.
    md380-tool hexdump <0xcafebabe>
Watches a hex address.
    md380-tool hexwatch <0xcafebabe>
Dump one word.
    md380-tool readword <0xcafebabe>
Dump 1kB from arbitrary address
    md380-tool dump <filename.bin> <address>

Copy File to SPI flash.
    md380-tool spiflashwrite <filename> <address>"

Copy users.csv to SPI flash:
    wc -c < db/users.csv > data ; cat db/users.csv >> data
    md380-tool spiflashwrite data 0x100000


""")

def main():
    try:
        if len(sys.argv) == 2:
            if sys.argv[1] == 'dmesg':
                dfu=init_dfu();
                dmesg(dfu);
            elif sys.argv[1] == 'dmesgtail':
                dfu=init_dfu();
                dmesgtail(dfu);
            elif sys.argv[1] == 'calllog':
                dfu=init_dfu();
                calllog(dfu);
            elif sys.argv[1] == 'date':
                dfu=init_dfu();
                calldate(dfu);
            elif sys.argv[1] == 'adc1':
                dfu=init_dfu();
                calladc1(dfu);         	    	
            elif sys.argv[1] == 'channel':
                dfu=init_dfu();
                getchannel(dfu);
            elif sys.argv[1] == 'c5000':
                dfu=init_dfu();
                c5000(dfu);
            elif sys.argv[1] == 'rssi':
                dfu=init_dfu();
                rssi(dfu);
            elif sys.argv[1] == 'findcc':
                dfu=init_dfu();
                findcc(dfu);
            elif sys.argv[1] == 'messages':
                dfu=init_dfu();
                messages(dfu);
            elif sys.argv[1] == 'keys':
                dfu=init_dfu();
                keys(dfu);
            elif sys.argv[1] == 'spiflashid':
                dfu=init_dfu();
                flashgetid(dfu);
            
        elif len(sys.argv) == 3:
            if sys.argv[1] == 'flashdump':
                print "Dumping flash from 0x08000000 to '%s'." % sys.argv[2];
                dfu=init_dfu();
                flashdump(dfu,sys.argv[2]);
            elif sys.argv[1] == 'spiflashdump':
                print "Dumping SPI Flash to '%s'." % sys.argv[2];
                dfu=init_dfu();
                spiflashdump(dfu,sys.argv[2]);
            elif sys.argv[1] == 'coredump':
                print "Dumping ram from 0x20000000 to '%s'." % sys.argv[2];
                dfu=init_dfu();
                coredump(dfu,sys.argv[2]);
            elif sys.argv[1] == 'hexdump':
                print "Dumping memory from %s." % sys.argv[2];
                dfu=init_dfu();
                hexdump(dfu,sys.argv[2]);
            elif sys.argv[1] == 'ramdump':
                print "Dumping memory from %s." % sys.argv[3];
                dfu=init_dfu();
                ramdump(dfu,sys.argv[2],sys.argv[3]);
            elif sys.argv[1] == 'hexwatch':
                print "Dumping memory from %s." % sys.argv[2];
                dfu=init_dfu();
                hexwatch(dfu,sys.argv[2]);
            elif sys.argv[1] == 'lookup':
                print users.getusername(int(sys.argv[2]));
            elif sys.argv[1] == 'readword':
                dfu=init_dfu();
                readword(dfu, sys.argv[2]);

        elif len(sys.argv) == 4:
            if sys.argv[1] == 'spiflashwrite':
                filename=sys.argv[2];
                adr=int(sys.argv[3],16);
                if ( adr >= int("0x100000",16) ):
		   dfu=init_dfu();
                   spiflashwrite(dfu,sys.argv[2],adr);
                else:
                   print "address to low"
            if sys.argv[1] == 'dump':
                print "Dumping memory from %s." % sys.argv[3];
                dfu=init_dfu();
                dump(dfu,sys.argv[2],sys.argv[3]);

        else:
            usage();

    except RuntimeError, e:
        print(e.args[0])
        exit(1)
    except Exception, e:
        print e
        #print dfu.get_status()
        exit(1)

if __name__ == '__main__':
    main()
