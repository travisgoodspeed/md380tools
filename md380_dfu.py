#!/usr/bin/env python2
import time
import struct
from DFU import DFU, State
import usb.core


# The tricky thing is that *THREE* different applications all show up
# as this same VID/PID pair.
#
# 1. The Tytera application image.
# 2. The Tytera bootloader at 0x08000000
# 3. The mask-rom bootloader from the STM32F405.
md380_vendor   = 0x0483
md380_product  = 0xdf11
#application_offset = 0x08000000
#ram_offset = 0x20000000
#application_size   = 0x00040000

def download(dfu, data, flash_address):
    block_size = 1 << 8
    sector_size = 1 << 12
    if flash_address & (sector_size - 1) != 0:
        raise Exception('Download must start at flash sector boundary')

    block_number = flash_address / block_size
    assert block_number * block_size == flash_address

    try:
        while len(data) > 0:
            packet, data = data[:block_size], data[block_size:]
            if len(packet) < block_size:
                packet += '\xFF' * (block_size - len(packet))
            dfu.download(block_number, packet)
            status, timeout, state, discarded = dfu.get_status()
            sys.stdout.write('.')
            sys.stdout.flush()
            block_number += 1
    finally:
        print

def download_codeplug(dfu, data):
    """Downloads a codeplug to the MD380."""
    block_size = 1024
    
    dfu.md380_custom(0x91,0x01); #Programming Mode
    dfu.md380_custom(0x91,0x01); #Programming Mode
    #dfu.md380_custom(0xa2,0x01); #Returns "DR780...", seems to crash client.
    #hexdump(dfu.get_command());  #Gets a string.
    dfu.md380_custom(0xa2,0x02);
    hexdump(dfu.get_command());  #Gets a string.
    time.sleep(2);
    dfu.md380_custom(0xa2,0x02);
    dfu.md380_custom(0xa2,0x03);
    dfu.md380_custom(0xa2,0x04);
    dfu.md380_custom(0xa2,0x07);
    

    dfu.erase_block(0x00000000);
    dfu.erase_block(0x00010000);
    dfu.erase_block(0x00020000);
    dfu.erase_block(0x00030000);

    dfu.set_address(0x00000000); # Zero address, used by configuration tool.
    
    #sys.exit();
    
    status, timeout, state, discarded = dfu.get_status()
    #print status, timeout, state, discarded
    
    block_number = 2
    
    try:
        while len(data) > 0:
            packet, data = data[:block_size], data[block_size:]
            if len(packet) < block_size:
                packet += '\xFF' * (block_size - len(packet))
            dfu.download(block_number, packet)
            state=11
            while state!=State.dfuDNLOAD_IDLE:
                status, timeout, state, discarded = dfu.get_status()
                #print status, timeout, state, discarded
            sys.stdout.write('.')
            sys.stdout.flush()
            block_number += 1
    finally:
        print

def hexdump(string):
    """God awful hex dump function for testing."""
    buf="";
    i=0;
    for c in string:
        buf=buf+("%02x"%c);
        i=i+1;
        if i&3==0:
            buf=buf+" "
        if i&0xf==0:
            buf=buf+"   "
        if i&0x1f==0:
            buf=buf+"\n"
        
    print buf;


def upload_bootloader(dfu,filename):
    """Dumps the bootloader, but only on Mac."""
    #dfu.set_address(0x00000000); # Address is ignored, so it doesn't really matter.
    
    # Bootloader stretches from 0x08000000 to 0x0800C000, but our
    # address and block number are ignored, so we set the block size
    # ot 0xC000 to yank the entire thing in one go.  The application
    # comes later, I think.
    block_size=0xC000; #0xC000;
    
    f=None;
    if filename!=None:
        f=open(filename,'wb');
    
    print "Dumping bootloader.  This only works in radio mode, not programming mode."
    try:
        data = dfu.upload(2, block_size)
        status, timeout, state, discarded = dfu.get_status()
        if len(data) == block_size:
            print "Got it all!";
        else:
            print "Only got %i bytes.  Older versions would give it all." % len(data);
            #raise Exception('Upload failed to read full block.  Got %i bytes.' % len(data))
        if f!=None:
            f.write(data)
        else:
            hexdump(data);
        
    finally:
        print "Done."


def upload_codeplug(dfu,filename):
    """Uploads a codeplug from the radio to the host."""
    dfu.md380_custom(0x91,0x01); #Programming Mode
    #dfu.md380_custom(0xa2,0x01); #Returns "DR780...", seems to crash client.
    #hexdump(dfu.get_command());  #Gets a string.
    dfu.md380_custom(0xa2,0x02);
    dfu.md380_custom(0xa2,0x02);
    dfu.md380_custom(0xa2,0x03);
    dfu.md380_custom(0xa2,0x04);
    dfu.md380_custom(0xa2,0x07);
    
    dfu.set_address(0x00000000); # Zero address, used by configuration tool.
    
    f = open(filename, 'wb')
    block_size=1024
    try:
        # Codeplug region is 0 to 3ffffff, but only the first 256k are used.
        for block_number in range(2,0x102):
            data = dfu.upload(block_number, block_size)
            status, timeout, state, discarded = dfu.get_status()
            #print "Status is: %x %x %x %x" % (status, timeout, state, discarded);
            sys.stdout.write('.')
            sys.stdout.flush()
            if len(data) == block_size:
                f.write(data)
                #hexdump(data);
            else:
                raise Exception('Upload failed to read full block.  Got %i bytes.' % len(data))
        #dfu.md380_reboot()
    finally:
        print "Done."

def download_firmware(dfu, data):
    """ Download new firmware binary to the radio. """
    addresses = [
            0x0800c000,
            0x08010000,
            0x08020000,
            0x08040000,
            0x08060000,
            0x08080000,
            0x080a0000,
            0x080c0000,
            0x080e0000]
    sizes = [0x4000, #0c
            0x10000, #1
            0x20000, #2
            0x20000, #4
            0x20000, #6
            0x20000, #8
            0x20000, #a
            0x20000, #c
            0x20000] #e
    block_ends  = [0x11, 0x41, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81]
    try:
        print("Beginning firmware upgrade.")
        status, timeout, state, discarded = dfu.get_status()
        assert state == State.dfuIDLE

        dfu.md380_custom(0x91,0x01)
        dfu.md380_custom(0x91,0x31)

        for address in addresses:
            if dfu.verbose: print("Erasing address@ 0x%x" % address)
            dfu.erase_block( address )

        block_size = 1024
        block_start = 2
        address_idx = 0

        if data[0:14] == "OutSecurityBin": #skip header if present
            if dfu.verbose: print("Skipping 0x100 byte header in data file")
            header, data = data[:0x100], data[0x100:]

        print("Writing firmware:")

        assert len(addresses) == len(sizes)
        numaddresses = len(addresses)

        while address_idx < numaddresses: #for each section
            print("%0d%% complete" % (address_idx*100/numaddresses) )
            address = addresses[ address_idx ]
            size = sizes[ address_idx ]
            dfu.set_address( address )

            if address_idx != len(addresses) -1:
                assert address + size == addresses[ address_idx + 1]

            datawritten = 0
            block_number = block_start

            while len(data) > 0 and size > datawritten: #for each block
                assert block_number <= block_ends[ address_idx ]
                packet, data = data[:block_size], data[block_size:]

                if len(packet) < block_size:
                    packet += '\xFF' * (block_size - len(packet))

                dfu.download(block_number, packet)
                dfu.wait_till_ready()

                datawritten += len(packet)
                block_number += 1
                #if dfu.verbose: sys.stdout.write('.'); sys.stdout.flush()
            #if dfu.verbose: sys.stdout.write('_\n'); sys.stdout.flush()
            address_idx += 1
        print("100% complete, now safe to disconnect and/or reboot radio")
    except Exception as e:
        print(e)



def upload(dfu, flash_address, length, path):
    #block_size = 1 << 8
    block_size = 1 << 14
    
    print "Address: 0x%08x"%flash_address
    print "Block Size:    0x%04x"%block_size
    
    if flash_address & (block_size - 1) != 0:
        raise Exception('Upload must start at block boundary')

    block_number = flash_address / block_size
    assert block_number * block_size == flash_address
    #block_number=0x8000;
    print "Block Number:    0x%04x"%block_number
    
    
    cmds=dfu.get_command();
    print "%i supported commands." % len(cmds)
    for cmd in cmds:
        print "Command %02x is supported by UPLOAD."%cmd;
    
    dfu.set_address(0x08001000); #RAM
    block_number=2;
    
    f = open(path, 'wb')
   
    try:
        while length > 0:
            data = dfu.upload(block_number, block_size)
            status, timeout, state, discarded = dfu.get_status()
            print "Status is: %x %x %x %x" % (status, timeout, state, discarded);
            sys.stdout.write('.')
            sys.stdout.flush()
            if len(data) == block_size:
                f.write(data)
            else:
                raise Exception('Upload failed to read full block.  Got %i bytes.' % len(data))
            block_number += 1
            length -= len(data)
    finally:
        f.close()
        print("done")

def detach(dfu):
    if dfu.get_state() == State.dfuIDLE:
        dfu.detach()
        print('Detached')
    else:
        print 'In unexpected state: %s' % dfu.get_state()

def init_dfu(alt=0):
    dev = usb.core.find(idVendor=md380_vendor,
                        idProduct=md380_product)
    
    if dev is None:
        raise RuntimeError('Device not found')

    dfu = DFU(dev, alt)
    dev.default_timeout = 3000

    try:
        dfu.enter_dfu_mode()
    except usb.core.USBError, e:
        if len(e.args) > 0 and e.args[0] == 'Pipe error':
            raise RuntimeError('Failed to enter DFU mode. Is bootloader running?')
        else:
            raise e

    return dfu
