#!/usr/bin/env python2

# md380-tool by KK4VCZ and Friends

# This is the client for the patched MD380 firmware.  It does all
# sorts of clever things that the official clients can't, but it
# probably has bugs and will do all sorts of unsavory things.  Do not
# expose it to light, do not feed it after midnight, and *NEVER* give
# it water.

# 2017-01-10, DL4YHF : Added some new "potentially lethal" functions 
#    to poke around in the C5000, that shouldn't be in md380_tool.py .
#    Thus using a different name (tool2.py) for this evil twin.

from DFU import DFU, State, Request
import time # the PEP8 dictator said only one module per import. Bleah.
import sys
import struct
import usb.core
from collections import namedtuple
import json
# The tricky thing is that *THREE* different applications all show up
# as this same VID/PID pair.
#
# 1. The Tytera application image.
# 2. The Tytera bootloader at 0x08000000
# 3. The mask-rom bootloader from the STM32F405.
md380_vendor   = 0x0483
md380_product  = 0xdf11
    # the PEP8 dictator disagreed with multiple spaces before operators. Oh, shut up !

sfr_addresses = { # tiny subset of special function registers in an STM32F4
    0xE000ED08L: "VTOR",  # Vector Table Offset Register, part of the SCB, PM0214 Rev5 page 220
    0xE000ED00L: "SCB",   # System Control Block, with CPUID[0], ICSR[4], VTOR[8], AIRCR[12], .... PM0214 pg 220.
    0x40026000L: "DMA1",  # first DMA unit, register offsets in RM0090 Rev7 pages 332 .. 335
    0x40026010L: "DMA1_S0CR",
    0x40026028L: "DMA1_S1CR",
    0x40026040L: "DMA1_S2CR", # one of the USED streams in D013.020
    0x40026044L: "DMA1_S2NDTR", # RM0090 Rev7 page 333 ...
    0x40026048L: "DMA1_S2PAR",
    0x4002604CL: "DMA1_S2M0AR",
    0x40026050L: "DMA1_S2M1AR",
    0x40026054L: "DMA1_S2FCR",
    0x40026058L: "DMA1_S3CR",
    0x40026078L: "DMA1_S4CR",
    0x40026088L: "DMA1_S5CR", # another of the USED streams in D013.020
    0x4002608CL: "DMA1_S5NDTR", # RM0090 Rev7 page 334
    0x40026090L: "DMA1_S5PAR",
    0x40026094L: "DMA1_S5M0AR",
    0x40026098L: "DMA1_S5M1AR",
    0x4002609CL: "DMA1_S5FCR",
    0x400260A0L: "DMA1_S6CR",
    0x400260B8L: "DMA1_S7CR",
    0x40026400L: "DMA2",  # second DMA unit (each DMA has registers at offsets 0x00..0xCC)
    0x40026410L: "DMA2_S0CR",
    0x40026428L: "DMA2_S1CR",
    0x40026440L: "DMA2_S2CR",
    0x40026458L: "DMA2_S3CR", # another of the USED streams in D013.020
    0x4002645CL: "DMA2_S3NDTR", # RM0090 Rev7 page 334
    0x40026460L: "DMA2_S3PAR",
    0x40026464L: "DMA2_S3M0AR",
    0x40026468L: "DMA2_S3M1AR",
    0x4002646CL: "DMA2_S3FCR",
    0x40026478L: "DMA2_S4CR",
    0x40026488L: "DMA2_S5CR", # another of the USED streams in D013.020
    0x400264A0L: "DMA2_S6CR",
    0x400264B8L: "DMA2_S7CR",
    # GPIO register offsets: 0x00="MODER", 0x10="IDR", 0x14="ODR", 0x20="AFRL", etc
    0x40020000L: "GPIOA", # .. with PA8="Save", PA1="Batt", PA0="TX LED", PA3="VOX", PA7="POW_C", ..
    0x40020004L:  "PA_OTYPE",0x40020008L:"PA_OSPEED", 0x4002000CL:"PA_PUPD",
    0x40020010L:  "PA_IDR",  0x40020014L:"PA_ODR",    0x40020018L:"PA_BSRR", 
    0x4002001CL:  "PA_LCKR", 0x40020020L:"PA_AFRL",   0x40020024L:"PC_AFRH",
    0x40020400L: "GPIOB", # .. with PB8='anti-pop' switch for speaker, PB9=audio PA power switch, ..
    0x40020404L:  "PB_OTYPE",0x40020408L:"PB_OSPEED", 0x4002040CL:"PB_PUPD",
    0x40020410L:  "PB_IDR",  0x40020414L:"PB_ODR",    0x40020418L:"PB_BSRR", 
    0x4002041CL:  "PB_LCKR", 0x40020420L:"PB_AFRL",   0x40020424L:"PB_AFRH",
    0x40020800L: "GPIOC", # .. with PC8="Beep", offset0 = "MODER", ..
    0x40020804L:  "PC_OTYPE",0x40020808L:"PC_OSPEED", 0x4002080CL:"PC_PUPD",
    0x40020810L:  "PC_IDR",  0x40020814L:"PC_ODR",    0x40020818L:"PC_BSRR", 
    0x4002081CL:  "PC_LCKR", 0x40020820L:"PC_AFRL",   0x40020824L:"PC_AFRH",
    0x40020C00L: "GPIOD", # .. with PD3="K3" ("Key 3" .. what's that ? )
    0x40020C04L:  "PD_OTYPE",0x40020C08L:"PD_OSPEED", 0x40020C0CL:"PD_PUPD",
    0x40020C10L:  "PD_IDR",  0x40020C14L:"PD_ODR",    0x40020C18L:"PD_BSRR", 
    0x4002081CL:  "PD_LCKR", 0x40020C20L:"PD_AFRL",   0x40020C24L:"PD_AFRH",
    0x40021000L: "GPIOE", # .. with RX_LED, TX_LED, FSMC, PTT, etc..
    0x40021004L:  "PE_OTYPE",0x40021008L:"PE_OSPEED", 0x4002100CL:"PE_PUPD",
    0x40021010L:  "PE_IDR",  0x40021014L:"PE_ODR",    0x40021018L:"PE_BSRR", 
    0x4002101CL:  "PE_LCKR", 0x40021020L:"PE_AFRL",   0x40021024L:"PE_AFRH",

    0x40011400L: "USART6",# abused by DL4YHF to generate PWM(!) on PC6 = USART6_TX. offsets in RM0090 Rev7 page 1002 .
    0x40023800L: "RCC",   # RCC = Reset and Clock Control, RM0090 Rev7 page 363(!) for STM32F405
    0x40023824L: "RCC_APB2RSTR", # APB2 peripheral ReSeT Register. Bit 5 controls USART6. RM0090 Rev7 page 236 .
    0x40023844L: "RCC_APB2ENR",  # APB2 peripheral clock ENable Register. Bit 5 controls USART6. RM0090 Rev7 page 246 .
    0x40001800L: "TIM12", # register offsets in RM0090 Rev13 page 672 .
    0x40010400L: "TIM8",  # added 2017-02-19 to develop the Morse generator . RM0090 Rev13 page 588 . 
    0x40010404L:"T8CR2",  0x40010408L:"T8SMCR", 0x4001040CL:"T8DIER", 
    0x40010410L:"T8SR",   0x40010414L:"T8EGR",  0x40010418L:"T8CCMR1",
    0x4001041CL:"T8CCMR2",0x40010420L:"T8CCER", 0x40010424L:"T8CNT",
    0x40010428L:"T8PSC",  0x4001042CL:"T8ARR",  0x40010430L:"T8RCR",
    0x40010434L:"T8CCR1", 0x40010438L:"T8CCR2", 0x4001043CL:"T8CCR3",
    0x40010440L:"T8CCR4", 0x40010444L:"T8BDTR", 0x40010448L:"T8DCR",
    0x4001044CL:"T8DMAR"

}

class UsersDB():
    """List of registered DMR-MARC users."""
    users = {} # the PEP8 dictator wants exactly whitespace around operators. Heavens, no.
               # He also wanted one blank line before a 'def'. Agreed.

    def __init__(self, filename=None):
        """Loads the database."""
        import csv
        try:
            if filename is None:
                filename = sys.path[0]+'/user.bin'
            with open(filename,'rb') as csvfile:
                reader = csv.reader(csvfile)
                for row in reader:
                    if len(row)>0:
                        self.users[int(row[0])]=row
        except:
            print "WARNING: Unable to load user.bin."
    def getuser(self,id):
        """Returns a user from the ID."""
        try:
            return self.users[id]
        except:
            call = ""
            name = ""
            nickname = ""
            city = ""
            state = ""
            country = ""
            comment = ""
            return ["%i"%id,call,name,nickname,city,state,country,comment]
    def getusername(self,id):
        """Returns a formatted username from the ID."""
        user = self.getuser(id)
        return("%s %s (%s)"%( user[1], user[2], user[0]))
    

# ex: Quick to load, so might as well do it early.
# users=UsersDB();


class Tool(DFU):
    """Client class for extra features patched into the MD380's firmware.
    None of this will work with the official firmware, of course."""
    
    def drawtext(self,str,a,b):
        """Sends a new MD380 command to draw text on the screen.."""
        cmd=0x80 #Drawtext
        a=a&0xFF
        b=b&0xFF
        self._device.ctrl_transfer(0x21, Request.DNLOAD, 1, 0, chr(cmd)+chr(a)+chr(b)+self.widestr(str))
        self.get_status() #this changes state
        time.sleep(0.1)
        status=self.get_status() #this gets the status
        if status[2]==State.dfuDNLOAD_IDLE:
            if self.verbose: print "Sent custom %02x %02x." % (a,b)
            self.enter_dfu_mode()
        else:
            print "Failed to send custom %02x %02x." % (a,b)
            return False
        return True
    def peek(self,adr,size):
        """Returns so many bytes from an address."""
        self.set_address(adr)
        return self.upload(1,size,0)
    def spiflashgetid(self):
        size=4
        """Returns SPI Flash ID."""
        cmd=0x05; #SPIFLASHGETID
        cmdstr=(chr(cmd))
        self._device.ctrl_transfer(0x21, Request.DNLOAD, 1, 0, cmdstr)
        self.get_status(); #this changes state
        status=self.get_status(); #this gets the status
        return self.upload(1,size,0)
    def spiflashpeek(self,adr,size=1024):
        """Returns so many bytes from SPI Flash."""
        cmd=0x01 #SPIFLASHREAD
        cmdstr=(chr(cmd)+
                chr(adr&0xFF)+
                chr((adr>>8)&0xFF)+
                chr((adr>>16)&0xFF)+
                chr((adr>>24)&0xFF)
                )
        self._device.ctrl_transfer(0x21, Request.DNLOAD, 1, 0, cmdstr)
        self.get_status(); #this changes state
        status=self.get_status(); #this gets the status
        return self.upload(1,size,0)
    def spiflash_erase64kblock(self, adr,size=1024):
        """Clear 64kb block on spi flash."""
        cmd=0x03; #SPIFLASHWRITE
        cmdstr=(chr(cmd)+
                chr(adr&0xFF)+
                chr((adr>>8)&0xFF)+
                chr((adr>>16)&0xFF)+
                chr((adr>>24)&0xFF)
                )
        self._device.ctrl_transfer(0x21, Request.DNLOAD, 1, 0, cmdstr)
        self.get_status() #this changes state
        time.sleep(0.1)

        status=self.get_status() #this gets the status
        return self.upload(1,size,0)

    def spiflashpoke(self,adr,size,data):
        """Returns so many bytes from SPI Flash."""
        cmd=0x04 #SPIFLASHWRITE_NEW
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
                )

        for i in range(0,size,1):
            cmdstr=cmdstr+data[i]

#        print len(cmdstr)
        self._device.ctrl_transfer(0x21, Request.DNLOAD, 1, 0, cmdstr)
        self.get_status() #this changes state
        status=self.get_status() #this gets the status
#        print status
        return self.upload(1,size,0)
    
    def getinbox(self,address):
        """return non-deleted messages from inbox"""
        buf = self.spiflashpeek(address, 50 * 4 )
        messages = []
        for i in range(0,50*4,4):
            message = {}
            header = buf[i:i+4]
            message["deleted"] = (header[0]) != 0x01
            if header[1] == 0x1 :
                message["read"] = True
            elif header[1] == 0x2:
                message["read"] = False
            else:
                message["read"] = "N/A"   
            message["order"] = (header[2])
            message["index"] = (header[3])
            if not message["deleted"]:
                messages.append(message)
        for message in messages:
            message_bytes = self.spiflashpeek(address+50*4+message["index"]*0x124,50*4+message["index"]*0x124+0x124)
            message["srcaddr"] = message_bytes[0]+(message_bytes[1]<<8)+(message_bytes[2]<<16)
            message["flags"] = message_bytes[4]
            message_text = ""
            for i in range(4,0x124,2):
                c = chr(message_bytes[i])
                if c!= '\0':
                    message_text = message_text + c
                else:
                    break
            message["text"] = message_text
        return messages

    def getkey(self,index):
        """Returns an Enhanced Privacy key from SPI Flash.  1-indexed"""
        buf=self.spiflashpeek(0x59c0+16*index-16, 16)
        return buf

    def c5000peek(self,reg):
        """Returns one byte from a C5000 register."""
        cmd=0x11 #C5000 Read Reg
        cmdstr=(chr(cmd) + chr(reg&0xFF) )
        self._device.ctrl_transfer(0x21, Request.DNLOAD, 1, 0, cmdstr)
        self.get_status() #this changes state
        status=self.get_status() #this gets the status
        buf=self.upload(1,1024,0) #Peek the 1024 byte dmesg buffer.
        return buf[0]
    def c5000poke(self,reg,val):
        """Writes a byte into a C5000 register."""
        cmd=0x10 #C5000 Write Reg
        cmdstr=(chr(cmd)+
                chr(reg&0xFF)+
                chr(val&0xFF)
                )
        self._device.ctrl_transfer(0x21, Request.DNLOAD, 1, 0, cmdstr)
        self.get_status() #this changes state
        status=self.get_status() #this gets the status
    def custom( self, cmd):
        """Returns the 1024 byte DMESG buffer."""

        self._device.ctrl_transfer(0x21, Request.DNLOAD, 1, 0, chr(cmd))
        self.get_status() #this changes state
        #time.sleep(0.1)
        status=self.get_status() #this gets the status

    def getdmesg(self):
        """Returns the 1024 byte DMESG buffer."""
        cmd=0x00 #DMESG
        self._device.ctrl_transfer(0x21, Request.DNLOAD, 1, 0, chr(cmd))
        self.get_status() #this changes state
        #time.sleep(0.1)
        status=self.get_status() #this gets the status
        buf=self.upload(1,1024,0) #Peek the 1024 byte dmesg buffer.
        
        #Okay, so at this point we have the buffer, but it's a ring
        #buffer that might have already looped, so we need to reorder
        #if that is the case or crop it if it isn't.
        tail=""
        head=None
        for b in buf:
            if head is None:
                if b>0:
                    tail=tail+chr(b)
                else:
                    head=""
            else:
                if b>0:
                    head=head+chr(b)
                else:
                    break
        if head is None:
            return tail
        return head+tail


        
    def parse_calibration_data(self,data):

        freqs_bcd = data[432:] #last 80 bytes represent 9*2 BCD frequencies,for example 00 35 10 40 == 401.03500  
        freqs = [] #parse into a list of frequency settings

        def bcd2freq(bcd):
            freq_whole = "%02x"%ord(bcd[3]) + ("%02x"%ord(bcd[2]))[0]
            freq_decimal = ("%02x"%ord(bcd[2]))[1] + "%02x"%ord(bcd[1]) + "%02x"%ord(bcd[0])
            return "%s.%s"%(freq_whole,freq_decimal)

        Frequency = namedtuple("Frequency","rx_freq tx_freq vox1 vox10 rx_low_voltage rx_full_voltage RSSI1    \
                                RSSI4 analog_mic digital_mic freq_adjust_high freq_adjust_mid freq_adjust_low1 \
                                tx_high_power tx_low_power rx_sensitivity open_sql_9 close_sql_9 open_sql_1    \
                                close_sql_1 max_volume ctcss_67hz ctcss_151_4hz ctcss_254_1hz dcs_mod2 dcs_mod1\
                                mod1_partial analog_voice_adjust lock_voltage_partial send_i_partial           \
                                send_q_partial send_i_range send_q_range rx_i_partial rx_q_partial             \
                                analog_send_i_range analog_send_q_range")
        rest = data[16:432] # first 16 bytes are settings which seem to be equal for all frequencies
        rest_i = ""
        for k in range(0,9): # invert the 2d array so it's easier to map 
            for j in range(0,24):
                rest_i += rest[j*16+k]
        codes_per_freq  = []
        for i in range(0,9): # 9 frequencies
            rx_freq = bcd2freq(freqs_bcd[i*8:i*8+4])
            tx_freq = bcd2freq(freqs_bcd[i*8+4:i*8+8])
            codes = data[:11] # from vox1 till freq_adjust_low , the mutual ones
            codes += rest_i[i*24:i*24+24] # the rest of info , each is a single 8 bit integer
            freqs.append(Frequency._asdict(Frequency._make((rx_freq,tx_freq ) + struct.unpack("B"*35,codes))))
        return freqs

def calllog(dfu):
    """Prints a call log to stdout, fetched from the MD380's memory."""
    dfu.drawtext("Hooking calls!",160,50)
    
    #Set the target address to the list of DMR addresses.
    dfu.set_address(0x2001d098)
    old1=0
    old2=0
    while 1:
        data=dfu.upload(1,16,0) # Peek sixteen bytes.
        llid0=(data[0]+
               (data[1]<<8)+
               (data[2]<<16)+
               (data[3]<<24))
        llid1=(data[4]+
               (data[5]<<8)+
               (data[6]<<16)+
               (data[7]<<24))
        llid2=(data[8]+
               (data[9]<<8)+
               (data[10]<<16)+
               (data[11]<<24))
        if old1!=llid1 or old2!=llid2:
            old1=llid1
            old2=llid2
            print "DMR call from %s to %s." % (
                users.getusername(llid1),users.getusername(llid2))
            #get actual canel name
            dfu.set_address(0x2001c9d4)
            data=dfu.upload(1,32,0);
            message=""
            for i in range(0,32,2):
               c=chr(data[i]&0x7F);
               if c!='\0':
                  message=message+c
            print message
             #get actual zone name
            dfu.set_address(0x2001b958)
            data=dfu.upload(1,32,0)
            message=""
            for i in range(0,32,2):
               c=chr(data[i]&0x7F)
               if c!='\0':
                 message=message+c
            print message
            sys.stdout.flush()


def dmesg(dfu):
    """Prints the dmesg log from main memory."""
    #dfu.drawtext("Dumping dmesg",160,50);
    print dfu.getdmesg()

def parse_calibration(dfu):
    dfu.md380_custom(0xA2,0x05)
    data = str(bytearray(dfu.upload(0,512)))
    freqs = dfu.parse_calibration_data(data)
    print(json.dumps(freqs,indent=4))

def coredump(dfu,filename):
    """Dumps a corefile of RAM."""
    with open(filename,'wb') as f:
        for adr in range(0x20000000,
                         0x20000000+(128*1024),
                         1024):
            #print "Fetching %08x"%adr
            buf=dfu.peek(adr,1024)
            f.write(buf)
        f.close()


def hexdump(dfu,address,length=512):
    """Dumps from memory to the screen"""
    adr=address
    buf=dfu.peek(adr,length)
    i=0
    cbuf=""
    for b in buf:
        sys.stdout.write("%02x "%b)
        i=i+1
        if(b > 32 and b < 127 ):
            cbuf = cbuf + chr(b)
        else:
            cbuf = cbuf + "."
        if i%16==0:
            sys.stdout.write(" " + cbuf)
            sys.stdout.write("\n")
            cbuf=""
        elif i%8==0:
            sys.stdout.write(" ")


def hexwatch(dfu,address):
    """Dumps from memory to the screen"""
    adr=ParseHexOrRegName(address)
    while True:
        hexdump(dfu,adr,16)
        time.sleep(0.05)


def ParseHexOrRegName(address):
    if address in sfr_addresses.values(): # there must be a more elegant way than this..
        return sfr_addresses.keys()[sfr_addresses.values().index(address)]
    else:
        return int(address,16)


def ShowRegNameIfKnown(address):
    if address in sfr_addresses:
        sys.stdout.write(' ; '+ sfr_addresses[address] )


def hexdump32(dfu,address,length=512):
    """Dumps 32-bit hex values to the screen"""
    adr=ParseHexOrRegName(address)
    buf=dfu.peek(adr,length+3)
    i=0
    know_name=0;
    cbuf=""
    names=""
    while i<length:
        if i%16==0:
            if know_name: # if at least one address is a known SFR, show them
               sys.stdout.write("("+names+")")
            sys.stdout.write("\n%08X: "%(adr+i))
            know_name=0
            names=""
        dw = buf[i] | (buf[i+1]<<8) | (buf[i+2]<<16) | (buf[i+3]<<24)
        sys.stdout.write("%08X "%dw)
        if (adr+i) in sfr_addresses:
            know_name=1
            names = names+sfr_addresses[adr+i]
        else:
            names = names+"?"
        if i%16 < 12:
            names = names+","
        i=i+4
    if know_name: # if known, show SFR names for the last line
        sys.stdout.write("("+names+")")


def bindump32(dfu,address,length=256):
    """Dumps 32-bit binary values to the screen"""
    adr=ParseHexOrRegName(address)
    buf=dfu.peek(adr,length+3)
    i=0
    cbuf=""
    sys.stdout.write('\n Bit Nr : 3 2         1         0' )
    sys.stdout.write('\n          10987654321098765432109876543210' )
    while i<length:
        sys.stdout.write("\n%08X: "%(adr+i))
        dw = buf[i] | (buf[i+1]<<8) | (buf[i+2]<<16) | (buf[i+3]<<24)
        sys.stdout.write( '{0:032b}'.format(dw) )
        ShowRegNameIfKnown( adr+i )
        i=i+4


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
def dmesgfasttail(dfu):
    """Keeps printing the dmesg buffer."""
    while True:
        sys.stdout.write(dfu.getdmesg());
        #time.sleep(0.1);
        sys.stdout.flush();
def dmesgtail(dfu):
    """Keeps printing the dmesg buffer."""
    while True:
        sys.stdout.write(dfu.getdmesg());
        time.sleep(0.1);
        sys.stdout.flush();
def c5000(dfu):
    """Prints some DMR registers."""
    for r in range(0,0x87):
        sys.stdout.write("[0x%02x]=0x%02x\t" % (r,dfu.c5000peek(r)));
        if r%4==3:
            sys.stdout.write("\n");
            sys.stdout.flush();
    sys.stdout.write("\n");
def c5read(dfu, regaddr ):         # DL4YHF 2016-12
    sys.stdout.write("C5000 reg %02X contains ...\n" % (regaddr) );
    for i in range(0,256):
       sys.stdout.write("0x%02X " % (dfu.c5000peek(regaddr)) );
       time.sleep(0.02);
       if i%16==15:
          sys.stdout.write("\n");
          sys.stdout.flush();
    sys.stdout.write("\n");
def c5write(dfu, regaddr, value ): # DL4YHF 2016-12
    dfu.c5000poke(regaddr, value ); # beware ! potentially dangerous !
    sys.stdout.write("Tried to write C5000 reg %02X := %02X\n" % (regaddr, value) );
    c5read(dfu, regaddr );
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
    print "Inbox:"
    messages = dfu.getinbox(0x416d0)[::-1]
    for msg in messages:
        print "From: %s Text: %s"%(msg["srcaddr"],msg["text"])
    print "Sent:"
    messages = dfu.getinbox(0x45100)[::-1]
    for msg in messages:
        print "To  : %s Text: %s"%(msg["srcaddr"],msg["text"])

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

    dfu = Tool(dev, alt)  # YHF: 'dfu' = "device firmware update" .. but THIS does much more
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
Dumps 32-bit values from memory in hex.
    md380-tool hexdump32 <address or regname like VTOR> [<count>]
Watches a hex address.
    md380-tool hexwatch <0xcafebabe>
Dump one word.
    md380-tool readword <0xcafebabe>
Dump 1kB from arbitrary address
    md380-tool dump <filename.bin> <address>
Dump calibration data 
    md380-tool calibration

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
            elif sys.argv[1] == "calibration":
                dfu=init_dfu();
                parse_calibration(dfu)
            
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
                adr=ParseHexOrRegName(sys.argv[2])
                dfu=init_dfu();
                hexdump(dfu,adr);
            elif sys.argv[1] == 'hexdump32':
                dfu=init_dfu();
                hexdump32(dfu,sys.argv[2]);
            elif sys.argv[1] == 'bindump32':
                dfu = init_dfu()
                bindump32(dfu, sys.argv[2])
            elif sys.argv[1] == 'ramdump':
                print "Dumping memory from %s." % sys.argv[3]
                dfu=init_dfu()
                ramdump(dfu,sys.argv[2],sys.argv[3])
            elif sys.argv[1] == 'hexwatch':
                print "Watching memory at %s." % sys.argv[2]
                dfu=init_dfu()
                hexwatch(dfu,sys.argv[2])
            elif sys.argv[1] == 'lookup':
                print users.getusername(int(sys.argv[2]))
            elif sys.argv[1] == 'readword':
                dfu=init_dfu()
                readword(dfu, sys.argv[2])
            elif sys.argv[1] == 'custom':
                dfu=init_dfu()
                dfu.custom(int(sys.argv[2], 16))
                dmesg(dfu)
            elif sys.argv[1] == 'c5read': # DL4YHF 2016-12
                dfu = init_dfu()
                c5read(dfu, int(sys.argv[2],16) )

        elif len(sys.argv) == 4:
            if sys.argv[1] == 'spiflashwrite':
                filename=sys.argv[2]
                adr=int(sys.argv[3],16)
                if ( adr >= int("0x100000",16) ):
                   dfu=init_dfu()
                   spiflashwrite(dfu,sys.argv[2],adr)
                else:
                   print "address to low"
            if sys.argv[1] == 'dump':
                print "Dumping memory from %s." % sys.argv[3]
                dfu=init_dfu()
                dump(dfu,sys.argv[2],sys.argv[3])
            elif sys.argv[1] == 'c5write':   # DL4YHF 2016-12
                dfu = init_dfu()
                c5write(dfu, int(sys.argv[2],16), int(sys.argv[3],16))
            elif sys.argv[1] == 'hexdump32': # DL4YHF 2017-01
                dfu=init_dfu()
                hexdump32(dfu,sys.argv[2], int(sys.argv[3]) )
            elif sys.argv[1] == 'bindump32': # DL4YHF 2017-01
                dfu=init_dfu()
                bindump32(dfu,sys.argv[2], int(sys.argv[3]) )

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
