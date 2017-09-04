# Copyright (c) 2015-2017 Vector 35 LLC
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.


# MD380 firmware loader for Binary Ninja, forked from Vector35's NDS
# example by Travis Goodspeed.

# from binaryninja import *
from binaryninja.binaryview import BinaryView
from binaryninja.architecture import Architecture
from binaryninja.enums import SegmentFlag
from binaryninja.log import log_error
from binaryninja.types import Symbol
from binaryninja.enums import SymbolType, SegmentFlag
from binaryninja import PluginCommand
from binaryninja.interaction import get_open_filename_input

import struct
import traceback


class MD380View(BinaryView):
    """This class implements a view of the loaded firmware, for any image
    that might be a firmware image for the MD380 or related radios loaded
    to 0x0800C000.
    """
    
    def __init__(self, data):
        BinaryView.__init__(self, file_metadata = data.file, parent_view = data)
        self.raw = data

    @classmethod
    def is_valid_for_data(self, data):
        hdr = data.read(0, 0x160)
        if len(hdr) < 0x160 or len(hdr)>0x100000:
            return False
        if ord(hdr[0x3]) != 0x20:
            # First word is the initial stack pointer, must be in SRAM around 0x20000000.
            return False
        if ord(hdr[0x7]) != 0x08:
            # Second word is the reset vector, must be in Flash around 0x08000000.
            return False
        #if struct.unpack("<H", hdr[0x15c:0x15e])[0] != crc16(hdr[0xc0:0x15c]):
        #    return False
        return True

    def init_common(self):
        self.platform = Architecture["thumb2"].standalone_platform
        #self.hdr = self.raw.read(0, 0x200)
        self.hdr = self.raw.read(0, 0x100001)
        print "Loaded %d bytes." % len(self.hdr)



    def init_thumb2(self, adr=0x08000000):
        try:
            self.init_common()
            self.thumb2_offset = 0
            self.arm_entry_addr = struct.unpack("<L", self.hdr[0x4:0x8])[0]
            self.thumb2_load_addr = adr #struct.unpack("<L", self.hdr[0x38:0x3C])[0]
            self.thumb2_size = len(self.hdr);

            # Add segment for SRAM, not backed by file contents
	    self.add_auto_segment(0x20000000, 0x20000, #128K at address 0x20000000.
                                  0, 0,
                                  SegmentFlag.SegmentReadable | SegmentFlag.SegmentWritable | SegmentFlag.SegmentExecutable)
            # Add segment for TCRAM, not backed by file contents
	    self.add_auto_segment(0x10000000, 0x10000, #64K at address 0x10000000.
                                  0, 0,
                                  SegmentFlag.SegmentReadable | SegmentFlag.SegmentWritable)
            
            #Add a segment for this Flash application.
            self.add_auto_segment(self.thumb2_load_addr, self.thumb2_size,
                                  self.thumb2_offset, self.thumb2_size,
            SegmentFlag.SegmentReadable | SegmentFlag.SegmentExecutable)

            #Define the RESET vector entry point.
            self.define_auto_symbol(Symbol(SymbolType.FunctionSymbol,
                                           self.arm_entry_addr&~1, "RESET"))
            self.add_entry_point(self.arm_entry_addr&~1)

            #Define other entries of the Interrupt Vector Table (IVT)
            for ivtindex in range(8,0x184+4,4):
                ivector=struct.unpack("<L", self.hdr[ivtindex:ivtindex+4])[0]
                if ivector>0:
                    #Create the symbol, then the entry point.
                    self.define_auto_symbol(Symbol(SymbolType.FunctionSymbol,
                                                   ivector&~1, "vec_%x"%ivector))
                    self.add_function(ivector&~1);
            return True
        except:
            log_error(traceback.format_exc())
            return False
    def perform_is_executable(self):
        return True

    def perform_get_entry_point(self):
        return self.arm_entry_addr


class MD380AppView(MD380View):
    """MD380 Application loaded to 0x0800C000."""
    name = "MD380"
    long_name = "MD380 Flash Application"

    def init(self):
        return self.init_thumb2(0x0800c000)


MD380AppView.register()


def importldsymbols(bv,filename):
    """Janky parser to import a GNU LD symbols file to Binary Ninja."""
    f=open(filename,"r");
    for l in f:
        words=l.strip().split();

        try:
            name=words[0];
            adrstr=words[2];
            adr=int(adrstr.strip(";"),16);

            #Function symbols are odd address in Flash.
            if adr&0xF8000001==0x08000001:
                bv.define_auto_symbol(Symbol(SymbolType.FunctionSymbol, adr&~1, name));
                bv.add_function(adr&~1);
                print("Imported function symbol %s at 0x%x"%(name,adr));

            #Data symbols are in SRAM or TCRAM with unpredictable alignment.
            elif adr&0xC0000000==0:
                bv.define_auto_symbol(Symbol(SymbolType.DataSymbol, adr, name));
                print("Imported data symbol %s at 0x%x"%(name,adr));
            else:
                print "Uncategorized adr=0x%08x."%adr;
        except:
            # Print warnings when our janky parser goes awry.
            if len(words)>0 and words[0]!="/*" and words[0]!="*/":
                print("#Warning in: %s\n"%words);
                log_error(traceback.format_exc())

def md380ldsymbols(view):
    """This loads an MD380Tools symbols file in GNU LD format."""
    filename=get_open_filename_input("Select GNU LD symbols file from MD380Tools.")
    if filename:
        print("Opening: %s"%filename);
        importldsymbols(view,filename);
    else:
        print("Aborting.");

PluginCommand.register("Load MD380 LD Symbols",
                       "Load GNU LD symbols from MD380Tools",
                       md380ldsymbols);


def importr2symbols(bv,filename):
    """Janky shotgun parser to import Radare2 symbols file to Binary Ninja."""
    f=open(filename,"r");
    for l in f:
        words=l.strip().split();

        try:
            if words[0][0]=="#":
                pass;
            elif words[0]=="f" and words[2]=="@":
                #f name @ 0xDEADBEEF
                name=words[1];
                adrstr=words[3];
                adr=int(adrstr.strip(";"),16);

                #Functions are in Flash
                if adr&0xF8000000==0x08000000:
                    bv.define_auto_symbol(Symbol(SymbolType.FunctionSymbol, adr&~1, name));
                    bv.add_function(adr&~1);
                    print("Imported function symbol %s at 0x%x"%(name,adr));
                #Data in SRAM or DRAM
                elif adr&0xFE000000==0x02000000:
                    bv.define_auto_symbol(Symbol(SymbolType.DataSymbol, adr&~1, name));
                    print("Imported data symbol %s at 0x%x"%(name,adr));
                    
            elif (words[0]=="af+" or words[0]=="f") and int(words[2],16)>0:
                name=words[3];
                adrstr=words[1];
                adr=int(adrstr.strip(";"),16);

                #Functions are in Flash
                if adr&0xF8000000==0x08000000:
                    bv.define_auto_symbol(Symbol(SymbolType.FunctionSymbol, adr&~1, name));
                    bv.add_function(adr&~1);
                    print("Imported function symbol %s at 0x%x"%(name,adr));
                #Data in SRAM or DRAM
                elif adr&0xFE000000==0x02000000:
                    bv.define_auto_symbol(Symbol(SymbolType.DataSymbol, adr&~1, name));
                    print("Imported data symbol %s at 0x%x"%(name,adr));
                
            else:
                print "Ignoring: ",words;
        except:
            if len(words)>3:
                print("Ignoring: %s\n"%words);
                #log_error(traceback.format_exc())


def md380r2symbols(view):
    """This loads an MD380Tools symbols file in Radare2 format."""
    filename=get_open_filename_input("Select GNU LD symbols file from MD380Tools.")
    if filename:
        print("Opening: %s"%filename);
        importr2symbols(view,filename);
    else:
        print("Aborting.");



PluginCommand.register("Load MD380 R2 Symbols",
                       "Load Radare2 symbols from MD380Tools",
                       md380r2symbols);
