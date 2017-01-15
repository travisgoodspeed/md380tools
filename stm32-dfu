#!/usr/bin/env python2
# -*- coding: utf-8 -*-
#
# Copyright 2010-2012 Michael Ossmann, Travis Goodspeed
#
# This file is forked from Project Ubertooth in order to support the
# STM32F2xx.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

# The STM32 series of ARM chips implement a weird variant of DFU, so
# I've written this client in order to understand it.  Sometime in the
# future, I expect to build a general-purpose DFU tool that works for
# all chips.  For now, though, this will certainly not be expected to
# run on anything but the STM32 series.

# http://pyusb.sourceforge.net/docs/1.0/tutorial.html

from __future__ import print_function

import struct
import sys
import time

stm32_vendor = 0x0483
stm32_product = 0xdf11

ram_offset = 0x20002000  # RAM
ram_size = 0x1E000
application_offset = 0x08000000  # Flash
application_size = 0x100000  # 1MB
rom_offset = 0x1fff0000  # ROM
rom_size = 0x8000  # 32K
otp_offset = 0x1fff7800  # OTP ROM
otp_size = 512

# Smaller block sizes cause problems. Not sure why.
block_size = 2048


class Enumeration(object):
    def __init__(self, id, name):
        self._id = id
        self._name = name
        setattr(self.__class__, name, self)
        self.map[id] = self

    def __int__(self):
        return self.id

    def __repr__(self):
        return self.name

    @property
    def id(self):
        return self._id

    @property
    def name(self):
        return self._name

    @classmethod
    def create_from_map(cls):
        for id, name in cls.map.items():
            cls(id, name)


class Request(Enumeration):
    map = {
        0: 'DETACH',
        1: 'DNLOAD',
        2: 'UPLOAD',
        3: 'GETSTATUS',
        4: 'CLRSTATUS',
        5: 'GETSTATE',
        6: 'ABORT',
    }


Request.create_from_map()


class State(Enumeration):
    map = {
        0: 'appIDLE',
        1: 'appDETACH',
        2: 'dfuIDLE',
        3: 'dfuDNLOAD_SYNC',
        4: 'dfuDNBUSY',
        5: 'dfuDNLOAD_IDLE',
        6: 'dfuMANIFEST_SYNC',
        7: 'dfuMANIFEST',
        8: 'dfuMANIFEST_WAIT_RESET',
        9: 'dfuUPLOAD_IDLE',
        10: 'dfuERROR',
    }


State.create_from_map()


class Status(Enumeration):
    map = {
        0x00: 'OK',
        0x01: 'errTARGET',
        0x02: 'errFILE',
        0x03: 'errWRITE',
        0x04: 'errERASE',
        0x05: 'errCHECK_ERASED',
        0x06: 'errPROG',
        0x07: 'errVERIFY',
        0x08: 'errADDRESS',
        0x09: 'errNOTDONE',
        0x0A: 'errFIRMWARE',
        0x0B: 'errVENDOR',
        0x0C: 'errUSBR',
        0x0D: 'errPOR',
        0x0E: 'errUNKNOWN',
        0x0F: 'errSTALLEDPKT',
    }


Status.create_from_map()


class DFU(object):
    def __init__(self, device):
        self._device = device

    def detach(self):
        self._device.ctrl_transfer(0x21, Request.DETACH, 0, 0, None)

    def download(self, block_number, data):
        """Download a block to RAM or Flash."""
        self._device.ctrl_transfer(0x21, Request.DNLOAD, block_number, 0, data)

    def masserase(self):
        """Mass erase all of Flash memory.  OTP left intact."""
        self._device.ctrl_transfer(bmRequestType=0x21,
                                   bRequest=Request.DNLOAD,
                                   wValue=0,
                                   wIndex=0,
                                   data_or_wLength=[0x41])
        # Erasure doesn't occur until we check the status.
        status = self.get_status()

        print("The device is now erasing itself.")
        print("This will result in a USB disconnect.")
        return True

    def go(self, address=0x08000000):
        """Branch to the address at addrptr+4."""

        # Branches to the value stored at the word *AFTER* this address.
        self.setaddresspointer(address)

        # Use DNLOAD, not DETACH.
        self._device.ctrl_transfer(bmRequestType=0x21,
                                   bRequest=Request.DNLOAD,
                                   # bRequest=Request.DETACH,
                                   wValue=0,
                                   wIndex=0,
                                   data_or_wLength=[])  # Zero length.
        # Execution.
        print(self.get_status())

        print("The device is now executing its application.")
        print("This will result in a USB disconnect.")
        return True

    def readprotect(self):
        """Over the control word to protect the chip."""
        # Option byte address.
        adr = 0x1fffc000
        self.setaddresspointer(adr)

        # #Grab the old block.
        # data=self.upload(2,2);
        # #Second read verifies the state.
        # status, timeout, state, discarded = dfu.get_status()
        # print state;
        # assert state==State.dfuUPLOAD_IDLE


        # print "Grabbing old option bytes.";
        # for foo in data:
        #     print "%02x" % foo;

        # Write the new block.
        # self._device.ctrl_transfer(0x21, Request.DNLOAD, 2, 0, [0xFF, 0xAA, 0x00, 0x55])
        self.download(2, [0xFF, 0xFF])

        # status check causes the write
        status, timeout, state, discarded = dfu.get_status()
        assert state == State.dfuDNBUSY;

    def readunprotect(self):
        """Mass erase all of Flash memory.  OTP left intact."""
        self._device.ctrl_transfer(bmRequestType=0x21,
                                   bRequest=Request.DNLOAD,
                                   wValue=0,
                                   wIndex=0,
                                   data_or_wLength=[0x92])

        print("Unprotecting the device.")
        print("This will cause a USB disconnection.")
        status = self.get_status()

        return True

    def upload(self, block_number, length):
        # 2 to 2048 byte size for Flash, RAM, and System memory.
        # option bytes should be equal to option byte block size.
        # Other locations defined in Important Considerations in AN2606

        # Address_Pointer is expected to have been already set by
        # Address = ((wBlockNum - 2) * wTransferSize) + Address_Pointer,

        # print "Requesting block 0x%08x size %06x" % (block_number,length);

        if block_number > 0xFFFF:
            print("WARNING: Block 0x%04x will be returned instead. (16-bit addr.)" % (block_number & 0xFFFF))

        # data = self._device.ctrl_transfer(0xA1, Request.UPLOAD, block_number, 0, length)
        data = self._device.ctrl_transfer(bmRequestType=0xA1,
                                          bRequest=Request.UPLOAD,
                                          wValue=block_number,
                                          wIndex=0,
                                          data_or_wLength=length)
        return data

    def setaddresspointer(self, address=application_offset):
        """Sets the address pointer in the STM32."""

        byte0 = address & 0xFF
        byte1 = (address >> 8) & 0xFF
        byte2 = (address >> 16) & 0xFF
        byte3 = (address >> 24) & 0xFF

        data = [0x21,  # Set pointer op-code
                byte0, byte1, byte2, byte3  # Address, little-endian.
                ]
        # print self.get_status();
        toret = self._device.ctrl_transfer(bmRequestType=0x21,
                                           bRequest=Request.DNLOAD,
                                           wValue=0,
                                           wIndex=0,
                                           data_or_wLength=data)
        # Address isn't set until first GETSTATUS query.
        status = self.get_status()

        # Second query is needed to check if correctly set.
        # Failures result in dfuERROR or errTARGET.
        status = self.get_status()

        if status[2] == State.dfuDNLOAD_IDLE:
            print("Setting address pointer to 0x%08x." % address)
            # This will get us back to the entry point.
            self.enter_dfu_mode()
        else:
            print("Failed to set address pointer.")
            return False
        return True

    def get_status(self):
        status_packed = self._device.ctrl_transfer(0xA1, Request.GETSTATUS, 0, 0, 6)
        status = struct.unpack('<BBBBBB', status_packed)
        return (Status.map[status[0]], (((status[1] << 8) | status[2]) << 8) | status[3],
                State.map[status[4]], status[5])

    def clear_status(self):
        self._device.ctrl_transfer(0x21, Request.CLRSTATUS, 0, 0, None)

    def get_state(self):
        state_packed = self._device.ctrl_transfer(0xA1, Request.GETSTATE, 0, 0, 1)
        return State.map[struct.unpack('<B', state_packed)[0]]

    def abort(self):
        self._device.ctrl_transfer(0x21, Request.ABORT, 0, 0, None)

    def enter_dfu_mode(self):
        action_map = {
            State.dfuDNLOAD_SYNC: self.abort,
            State.dfuDNLOAD_IDLE: self.abort,
            State.dfuMANIFEST_SYNC: self.abort,
            State.dfuUPLOAD_IDLE: self.abort,
            State.dfuERROR: self.clear_status,
            State.appIDLE: self.detach,
            State.appDETACH: self._wait,
            State.dfuDNBUSY: self._wait,
            State.dfuMANIFEST: self.abort,
            State.dfuMANIFEST_WAIT_RESET: self._wait,
            State.dfuIDLE: self._wait
        }

        while True:
            state = self.get_state()
            if state == State.dfuIDLE:
                break
            action = action_map[state]
            action()

    def _wait(self):
        time.sleep(0.1)


def download(dfu, data, flash_address):
    # block_size = 1 << 8
    sector_size = 1 << 12

    print("Flashing to 0x%08x" % flash_address)

    base_address = flash_address

    # Rebase the address pointer.
    if not dfu.setaddresspointer(base_address):
        print("Failed to set address.")
        sys.exit(1)

    flash_address = flash_address + block_size * 2 - base_address  # Correct offset.

    if flash_address & (sector_size - 1) != 0:
        raise Exception('Download must start at flash sector boundary')

    block_number = flash_address / block_size
    assert block_number * block_size == flash_address

    print("Based from  0x%08x" % base_address)

    try:
        while len(data) > 0:
            packet, data = data[:block_size], data[block_size:]
            if len(packet) < block_size:
                print("Padding a short packet.")
                packet += '\xFF' * (block_size - len(packet))
            # print "Downloading block %i." % block_number;
            dfu.download(block_number, packet)

            # status check causes the write
            status, timeout, state, discarded = dfu.get_status()
            assert state == State.dfuDNBUSY;

            # Second read verifies the state.
            status, timeout, state, discarded = dfu.get_status()
            assert state == State.dfuDNLOAD_IDLE

            sys.stdout.write('.')
            sys.stdout.flush()
            block_number += 1
    finally:
        print()
    dfu.enter_dfu_mode()


def upload(dfu, flash_address, length, path):
    """Uploads a region from the chip to a file on the workstation."""

    # Set the base address, then make it zero.
    base_address = flash_address
    # flash_address=base_address;

    # Rebase the address pointer.
    if not dfu.setaddresspointer(base_address):
        print("Failed to set address.")
        sys.exit(1)

    flash_address = flash_address + block_size * 2 - base_address  # Correct offset.

    if flash_address & (block_size - 1) != 0:
        raise Exception('Upload must start at block boundary')

    block_number = flash_address / block_size
    # assert block_number * block_size == flash_address   #Ubertooth, not STM32
    # address_pointer=0;
    # assert flash_address==((block_number-2)*block_size)+address_pointer;

    print("flash_address = %08x" % flash_address)
    print("block_number  = %08x" % block_number)
    print("block_size    = %08x" % block_size)

    f = open(path, 'wb')

    try:
        while length > 0:
            data = dfu.upload(block_number, block_size)
            status, timeout, state, discarded = dfu.get_status()
            sys.stdout.write('.')
            sys.stdout.flush()
            if len(data) == block_size:
                f.write(data)
                block_number += 1
                length -= len(data)
            else:
                # raise Exception('Upload failed to read full block')
                print("Failed to return full block number 0x%x" % block_number)
                print("Got 0x%i bytes." % len(data))
    finally:
        f.close()
        print()


def detach(dfu):
    if dfu.get_state() == State.dfuIDLE:
        dfu.detach()
        print('Detached')
    else:
        print('In unexpected state: %s' % dfu.get_state())


def init_dfu(idVendor=stm32_vendor, idProduct=stm32_product):
    dev = usb.core.find(idVendor=idVendor, idProduct=idProduct)
    if dev is None:
        raise RuntimeError('Device not found')

    dfu = DFU(dev)
    dev.default_timeout = 3000

    try:
        dfu.enter_dfu_mode()
    except usb.core.USBError as e:
        if len(e.args) > 0 and e.args[0] == 'Pipe error':
            raise RuntimeError('Failed to enter DFU mode. Is bootloader running?')
        else:
            raise e

    return dfu


def usage():
    print("""
Usage: stm32-dfu <command> <arguments>

Write a file to application flash region:
    stm32-dfu writeflash $file
Write a file to RAM at 0x20002000, after DFU region.
    stm32-dfu writeram $file
Write a file to an arbitrary address.
    stm32-dfu write $file $adr

Read data from application flash region and write to a file:
    stm32-dfu read <filename>
Read data from SRAM region and write to a file:
    stm32-dfu readram <filename>
Read data from ROM region and write to a file:
    stm32-dfu readrom <filename>
Read data from OTP region and write to a file:
    stm32-dfu readotp <filename>

Mass erase STM32 in preparation for reflashing.
    stm32-dfu erase
Unprotect the STM32's RDP.
    stm32-dfu unprotect
Protect the STM32's RDP.
    stm32-dfu protect

Detach the bootloader and execute the flash application.
    stm32-dfu go [0x08000000]
Detach the bootloader and execute from RAM at 0x20002000.
    stm32-dfu goram

""")


if __name__ == '__main__':
    if len(sys.argv) > 1:
        if sys.argv[1] == 'read':
            import usb.core

            dfu = init_dfu()
            upload(dfu, application_offset, application_size, sys.argv[2])
            print('Read complete')
        elif sys.argv[1] == 'readram':
            import usb.core

            dfu = init_dfu()
            upload(dfu, ram_offset, ram_size, sys.argv[2])
            print('Read complete')
        elif sys.argv[1] == 'readrom':
            import usb.core

            dfu = init_dfu()
            upload(dfu, rom_offset, rom_size, sys.argv[2])
            print('Read complete')
        elif sys.argv[1] == 'readotp':
            import usb.core

            dfu = init_dfu()
            upload(dfu, otp_offset, otp_size, sys.argv[2])
            print('Read complete')
        elif sys.argv[1] == 'write':
            import usb.core

            f = open(sys.argv[2], 'rb')
            data = f.read()
            f.close()

            dfu = init_dfu()
            firmware = data
            application_offset = int(sys.argv[3], 16)

            download(dfu, firmware, application_offset)
            print('Write complete')
        elif sys.argv[1] == 'writeflash':
            import usb.core

            f = open(sys.argv[2], 'rb')
            data = f.read()
            f.close()

            dfu = init_dfu()
            firmware = data

            download(dfu, firmware, application_offset)
            print('Write complete')
        elif sys.argv[1] == 'writeram':
            import usb.core

            f = open(sys.argv[2], 'rb')
            data = f.read()
            f.close()

            dfu = init_dfu()
            firmware = data

            download(dfu, firmware, 0x20002000)
            print('Write complete')

        elif sys.argv[1] == 'detach':
            import usb.core

            dfu = init_dfu()
            detach(dfu)
        elif sys.argv[1] == 'erase':
            import usb.core

            dfu = init_dfu()
            dfu.masserase()
        elif sys.argv[1] == 'go':
            import usb.core

            dfu = init_dfu()
            if len(sys.argv) == 2:
                dfu.go()
            else:
                dfu.go(int(sys.argv[2], 16))
        elif sys.argv[1] == 'unprotect':
            import usb.core

            dfu = init_dfu()
            dfu.readunprotect()
        elif sys.argv[1] == 'protect':
            import usb.core

            dfu = init_dfu()
            dfu.readprotect()
        else:
            usage()
    else:
        usage()
