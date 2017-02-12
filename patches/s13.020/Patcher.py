# -*- coding: utf-8 -*-


class Patcher(object):
    """MD380 Firmware Patching Tool"""

    # #Includes Bootloader
    # offset=0x08000000;

    # Just the application.
    offset = 0x0800C000

    def getbyte(self, adr):
        """Reads a byte from the firmware address."""
        b = self.bytes[adr - self.offset]
        return b

    def getword(self, adr):
        """Reads a byte from the firmware address."""
        w = (
            self.bytes[adr - self.offset] +
            (self.bytes[adr - self.offset + 1] << 8) +
            (self.bytes[adr - self.offset + 2] << 16) +
            (self.bytes[adr - self.offset + 3] << 24)
        )

        return w

    def assertbyte(self, adr, val):
        """Asserts that a byte has a given value."""
        assert self.getbyte(adr) == val
        return

    def setbyte(self, adr, new, old=None):
        """Patches a single byte from the old value to the new value."""
        if old is not None:
            self.assertbyte(adr, old)
        print("Patching byte at %08x to %02x" % (adr, new))
        self.bytes[adr - self.offset] = new
        self.assertbyte(adr, new)

    def ffrange(self, start, end):
        """Patches a range to FF."""
        print("Patching range from %08x to %08x to FF." % (start, end))
        for adr in range(start, end):
            self.bytes[adr - self.offset] = 0xFF

    def setstring(self, adr, newstring):
        """Patches a string"""
        print("Patching string at %08x to '%s'." % (adr, newstring))
        for c in newstring:
            self.bytes[adr - self.offset] = ord(c)
            adr += 1
        # Null terminate.
        self.bytes[adr - self.offset] = 0

    def setwstring(self, adr, newstring):
        """Patches a wide string"""
        print("Patching wide string at %08x to '%s'." % (adr, newstring))
        for c in newstring:
            self.bytes[adr - self.offset] = ord(c)
            self.bytes[adr - self.offset + 1] = 0
            adr += 2
        # Null terminate.
        self.bytes[adr - self.offset] = 0
        self.bytes[adr - self.offset + 1] = 0

    def sethword(self, adr, new, old=None):
        """Patches a byte pair from the old value to the new value."""
        if old is not None:
            self.assertbyte(adr, old & 0xFF)
            self.assertbyte(adr + 1, (old >> 8) & 0xFF)
        print("Patching hword at %08x to %04x" % (adr, new))
        self.bytes[adr - self.offset] = new & 0xFF
        self.bytes[adr - self.offset + 1] = (new >> 8) & 0xFF
        self.assertbyte(adr, new & 0xFF)
        self.assertbyte(adr + 1, (new >> 8) & 0xFF)

    def setword(self, adr, new, old=None):
        """Patches a 32-bit word from the old value to the new value."""
        if old is not None:
            self.assertbyte(adr, old & 0xFF)
            self.assertbyte(adr + 1, (old >> 8) & 0xFF)
            self.assertbyte(adr + 2, (old >> 16) & 0xFF)
            self.assertbyte(adr + 3, (old >> 24) & 0xFF)

        print("Patching word at %08x to %08x" % (adr, new))
        self.bytes[adr - self.offset] = new & 0xFF
        self.bytes[adr - self.offset + 1] = (new >> 8) & 0xFF
        self.bytes[adr - self.offset + 2] = (new >> 16) & 0xFF
        self.bytes[adr - self.offset + 3] = (new >> 24) & 0xFF
        self.assertbyte(adr, new & 0xFF)
        self.assertbyte(adr + 1, (new >> 8) & 0xFF)

    def nopout(self, adr, old=None):
        """Nops out an instruction with 0xd11f."""
        self.sethword(adr, 0x46c0, old)

    def haltat(self, adr, old=None):
        """Sets an infinite while loop with bytes "fe e7"."""
        self.sethword(adr, 0xe7fe, old)

    def __init__(self, filename, offset=0x08000000):
        """Opens the input file."""
        self.file = open(filename, "rb")
        self.bytes = bytearray(self.file.read())
        self.length = len(self.bytes)

    def export(self, filename):
        """Exports to a binary file."""
        outfile = open(filename, "wb")
        outfile.write(self.bytes)
        outfile.close()

    def sprite2str(self, adr, width, size):
        """Extracts a sprite as ASCII art."""
        sprite = ""
        for i in range(0, size):
            b = self.getbyte(adr + i)
            if i % width == 0:
                sprite += "\n"
            for j in range(0, 8):
                c = '.'
                if b & (128 >> j) > 0:  # Most significant bit is on the left.
                    c = 'X'
                sprite += c
        return sprite

    def str2sprite(self, adr, sprite):
        """Writes ASCII art into memory as a sprite."""
        b = 0  # Current byte.
        i = 0  # Pixel index.
        j = 0  # Byte index.
        for c in sprite:
            if c == 'X':
                b |= 1
            elif c == '.':
                pass
            else:
                continue
            i += 1
            if i % 8 == 0:
                self.setbyte(adr + j, b & 0xFF)
                b = 0
                j += 1
            b = (b << 1)

        return
