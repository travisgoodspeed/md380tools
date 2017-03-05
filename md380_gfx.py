#!/usr/bin/env python2
# -*- coding: utf-8 -*-

from __future__ import print_function

import argparse
import binascii
import math
import os
import struct
import sys

FontGFX_CN_START = 0x809c714
FontGFX_CN_END = 0x80d0f80


class Memory(object):
    def __init__(self, data, base_address=0x800c000):
        # expecting a byte-like data object
        # makes for efficient read, but inefficient write
        assert type(data) == str
        self.addr = base_address
        self.mem = bytes(data)

    def rb(self, addr):
        """Read unsigned byte"""
        addr -= self.addr
        f = '<B'
        return struct.unpack(f, self.mem[addr])[0]

    def rbs(self, addr):
        """Read signed byte"""
        addr -= self.addr
        f = '<b'
        return struct.unpack(f, self.mem[addr])[0]

    def rw(self, addr):
        """Read unsigned 16-bit word"""
        addr -= self.addr
        f = '<H'
        return struct.unpack(f, self.mem[addr:addr + 2])[0]

    def rws(self, addr):
        """Read signed 16-bit word"""
        addr -= self.addr
        f = '<h'
        return struct.unpack(f, self.mem[addr:addr + 2])[0]

    def rl(self, addr):
        """Read unsigned 32-bit longword"""
        addr -= self.addr
        f = '<I'
        return struct.unpack(f, self.mem[addr:addr + 4])[0]

    def rls(self, addr):
        """Read signed 32-bit longword"""
        addr -= self.addr
        f = '<i'
        return struct.unpack(f, self.mem[addr:addr + 4])[0]

    def read(self, addr, length):
        """Read a number of bytes, returns list"""
        s = struct.Struct('<' + 'B' * length)
        return s.unpack_from(self.mem, addr - self.addr)

    def readbytes(self, addr, length):
        """Read a number of bytes, returns bytes object"""
        addr -= self.addr
        return self.mem[addr:addr + length]

    def write(self, addr, data):
        """Write a number of bytes"""
        addr -= self.addr
        data = bytes(data)
        l = len(data)
        self.mem = self.mem[:addr] + data + self.mem[addr + l:]

    def readbits(self, addr, bitlength, skip=0):
        """Read a number of bits"""
        addr -= self.addr
        bytelength = int(math.ceil(bitlength / 8.0))
        bytedata = self.mem[addr:addr + bytelength]
        hexdata = binascii.hexlify(b'\x01' + bytedata)
        bitdata = bin(int(hexdata, 16))[3:]  # 1+...[:3] to preserve leading zeros
        cutoff = len(bitdata) - bitlength
        if cutoff > 0:
            bitdata = bitdata[:-cutoff]
        return bitdata[skip:]

    def writebits(self, addr, bitdata):
        """Write a number of bits with zero-padding"""
        # TODO: masking instead of padding
        bitdata += b'0' * ((8 - len(bitdata)) % 8)  # pad to full bytes
        hexdata = hex(int(b'1' + bitdata, 2))[3:]  # 1+...[3:] to preserve leading zeros
        # Throw away python2.7's trailing long int marker
        if hexdata.endswith('L'):
            hexdata = hexdata[:-1]
        bytedata = binascii.unhexlify(hexdata)
        self.write(addr, bytedata)


class MD380Graphics(Memory):
    def __init__(self, data, base_address=0x800c000):
        super(MD380Graphics, self).__init__(data, base_address)
        self.gfxscancache = None

    def gfxparse(self, addr):
        """Parse sprite structure and return image"""
        s = struct.Struct('<hhhhLL')
        width, height, bytesperline, bitsperpixel, pixptr, palptr = s.unpack_from(self.mem, addr - self.addr)

        s = struct.Struct('<llL')
        ncol, someb, colptr = s.unpack_from(self.mem, palptr - self.addr)

        # bitplanes = len(bin(ncol-1))-2
        img = {'address': addr, 'width': width, 'height': height, 'palette': None, 'pixels': []}

        img['palette'] = []
        for i in range(colptr, colptr + ncol * 4, 4):
            r, g, b, a = self.read(i, 4)
            img['palette'].append([r, g, b, a])

        for y in range(height):
            linebits = self.readbits(pixptr + y * bytesperline, width * bitsperpixel)
            line = [int(linebits[i:i + bitsperpixel], 2) for i in range(0, len(linebits), bitsperpixel)]
            img['pixels'].append(line)

        img['checksum'] = self.gfxchecksum(img)
        return img

    def glyphparse(self, addr):
        """Parse a font glyph structure and return image"""
        s = struct.Struct('<bbbbL')
        width, height, bytesperline, somea, pixptr = s.unpack_from(self.mem, addr - self.addr)
        # somea seems to be padding; always 0 in v2.32

        # sys.stdout.write('glyphparse: address 0x%x wid=%d height=%d  bpl=%d  somea=%d  pixptr=0x%x\n' % (addr, width, height, bytesperline, somea, pixptr) )
        if height <= 8:
            height *= 2  # CN characters are right height, latin are reported 1/2 height

        img = {'address': addr, 'width': width, 'height': height, 'palette': None, 'pixels': []}

        for y in range(height):
            linebits = self.readbits(pixptr + y * bytesperline, width)
            line = [int(color) for color in linebits]
            img['pixels'].append(line)

        img['checksum'] = self.gfxchecksum(img)
        return img

    @staticmethod
    def gfxchecksum(gfx):
        data = '%d:%d:%s:%s' % (gfx['width'], gfx['height'], repr(gfx['pixels']), repr(gfx['palette']))
        return binascii.crc32(data)

    def glyphreplace(self, gfx, addr):
        """Overwrite existing glyph structure - must be same byte width."""
        s = struct.Struct('<bbbbL')
        old_width, old_height, old_bytesperline, somea, pixptr = s.unpack_from(self.mem, addr - self.addr)
        height_in_table = old_height  # funky value stored in table
        if old_height <= 8:
            old_height *= 2  # CN characters are right height, latin are reported 1/2 height

        new_bytesperline = int(math.ceil(gfx['width'] / 8.0))

        assert new_bytesperline <= old_bytesperline  # technically new can be <= old
        # assert gfx['width'] == old_width  # doesn't matter as long as bytesperline same.
        assert gfx['height'] == old_height

        # ok, now update.  Height and bytesperline must stay the same, but width can change.
        s = struct.Struct('<bbb')
        self.write(addr, s.pack(gfx['width'], height_in_table, new_bytesperline))

        pixbytes = b''
        for line in gfx['pixels']:
            padding = new_bytesperline * 8 - len(line)
            line += [0] * padding
            linebits = [bin(c + 0x10000)[-1:] for c in line]
            linebits = ''.join(linebits)
            for i in range(0, len(linebits), 8):
                pixbytes += chr(int(linebits[i:i + 8], 2))
        self.write(pixptr, pixbytes)

    def gfxreplace(self, gfx, addr):
        """Overwrite existing sprite structure"""
        s = struct.Struct('<hhhhLL')
        width, height, bytesperline, bitsperpixel, pixptr, palptr = s.unpack_from(self.mem, addr - self.addr)
        assert gfx['width'] == width
        assert gfx['height'] == height

        s = struct.Struct('<llL')
        ncol, someb, colptr = s.unpack_from(self.mem, palptr - self.addr)
        assert len(gfx['palette']) <= ncol

        # TODO: check bitplanes match
        # TODO: rewrite to use Memory bitbanging methods

        pixbytes = b''
        for line in gfx['pixels']:
            padding = bytesperline * (8 / bitsperpixel) - len(line)
            line += [0] * padding
            linebits = [bin(c + 0x10000)[-bitsperpixel:] for c in line]
            linebits = ''.join(linebits)
            for i in range(0, len(linebits), 8):
                pixbytes += chr(int(linebits[i:i + 8], 2))
        self.write(pixptr, pixbytes)

        palbytes = b''
        for color in gfx['palette']:
            r, g, b, a = color
            palbytes += chr(r)
            palbytes += chr(g)
            palbytes += chr(b)
            palbytes += chr(a)
        self.write(colptr, palbytes)

    def gfxrelocate(self, gfx, addr, location):
        """Overwrite existing sprite structure with data in a new location"""
        s = struct.Struct('<hhhhLL')
        width, height, bytesperline, bitsperpixel, pixptr, palptr = s.unpack_from(self.mem, addr - self.addr)
        if gfx['width'] != width or gfx['height'] != height:
            sys.stderr.write('WARNING: Graphics dimensions have changed. Expect glitches.\n')

        new_width = gfx['width']
        new_height = gfx['height']

        s = struct.Struct('<llL')
        ncol, someb, colptr = s.unpack_from(self.mem, palptr - self.addr)

        # TODO: check bitplanes match
        # TODO: rewrite to use Memory bitbanging methods

        new_bitsperpixel = int(math.ceil(math.log(len(gfx['palette'])) / math.log(2)))
        new_bytesperline = int(math.ceil(new_bitsperpixel * new_width / 8.0))
        new_pixptr = location
        pixbytes = b''
        for line in gfx['pixels']:
            padding = new_width - len(line)
            line += [0] * padding
            linebits = [bin(c + 0x10000)[-new_bitsperpixel:] for c in line]
            linebits = ''.join(linebits)
            for i in range(0, len(linebits), 8):
                pixbytes += chr(int(linebits[i:i + 8], 2))
        self.write(new_pixptr, pixbytes)

        new_colptr = location + len(pixbytes) + (16 - (len(pixbytes) % 16))
        palbytes = b''
        for color in gfx['palette']:
            r, g, b, a = color
            palbytes += chr(r)
            palbytes += chr(g)
            palbytes += chr(b)
            palbytes += chr(a)
        self.write(new_colptr, palbytes)

        new_palptr = new_colptr + len(palbytes) + (16 - (len(palbytes) % 16))
        new_ncol = len(gfx['palette'])
        s = struct.Struct('<llL')
        self.write(new_palptr, s.pack(new_ncol, someb, new_colptr))

        s = struct.Struct('<hhhhLL')
        self.write(addr, s.pack(new_width, new_height, new_bytesperline,
                                new_bitsperpixel, new_pixptr, new_palptr))

    @staticmethod
    def bashcolor(r=None, g=None, b=None):
        if b is not None:
            # return b'\x1b[38;2;%d;%d;%dm' % (r, g, b)
            return b'\x1b[48;2;%d;%d;%dm' % (r, g, b)
        else:
            return '\x1b[0m'

    @staticmethod
    def gfxshow(gfx):
        for line in gfx['pixels']:
            for color in line:
                r, g, b, a = gfx['palette'][color]
                sys.stdout.write(MD380Graphics.bashcolor(r, g, b) + ' ')
            sys.stdout.write(MD380Graphics.bashcolor() + '\n')

    @staticmethod
    def glyphshow(gfx):
        for line in gfx['pixels']:
            sys.stdout.write(''.join(line) + '\n')

    @staticmethod
    def gfxprint(gfx):
        print("%s %d×%d" % (hex(gfx['address']), gfx['width'], gfx['height']))
        print("%s" % repr(gfx['palette']))
        for line in gfx['pixels']:
            for color in line:
                if color == 0:
                    px = '·'  # CAVE: using visually less intrusive Unicode middle dot
                else:
                    px = hex(color)[-1]  # ensure single char for color
                sys.stdout.write(px)
            sys.stdout.write('\n')

    @staticmethod
    def ppm(gfx):
        """Convert sprite object to PPM(P6) image"""
        assert gfx['palette'] is not None
        out = 'P6\n'
        out += '# MD380 address: %s\n' % hex(gfx['address'])
        out += '# MD380 checksum: %d\n' % gfx['checksum']
        out += '# MD380 palette: %s\n' % repr(gfx['palette'])
        out += '%d %d\n' % (gfx['width'], gfx['height'])
        out += '255\n'
        for line in gfx['pixels']:
            for color in line:
                r, g, b, a = gfx['palette'][color]
                out += chr(r) + chr(g) + chr(b)
        return out

    @staticmethod
    def ppmparse(ppm):
        """Convert PPM(P6) image to sprite object"""
        ppml = ppm.splitlines()
        assert ppml[0] == 'P6'
        i = 1
        addr = 0
        oldpalette = None
        oldchecksum = None
        while ppml[i].startswith('#'):
            if ppml[i].startswith('# MD380 address: '):
                addr = int(ppml[i][17:], 16)
            if ppml[i].startswith('# MD380 checksum: '):
                oldchecksum = int(ppml[i][18:])
            if ppml[i].startswith('# MD380 palette: '):
                # CAVE: arbitrary command execution there
                oldpalette = eval(ppml[i][17:])
            i += 1
        width, height = ppml[i].split()
        width = int(width)
        height = int(height)
        maxc = int(ppml[i + 1])
        assert maxc == 255
        data = '\n'.join(ppml[i + 2:])
        paletteidx = {}
        pixels = []
        palette = []
        for y in range(height):
            line = []
            for x in range(width):
                r = ord(data[y * width * 3 + x * 3])
                g = ord(data[y * width * 3 + x * 3 + 1])
                b = ord(data[y * width * 3 + x * 3 + 2])
                a = 0
                key = '%d,%d,%d,%d' % (r, g, b, a)
                if key in paletteidx:
                    color = paletteidx[key]
                else:
                    color = len(palette)
                    palette.append([r, g, b, a])
                    paletteidx[key] = color
                line.append(color)
            pixels.append(line)
        img = {'address': addr, 'width': width, 'height': height,
               'palette': palette, 'pixels': pixels,
               'oldchecksum': oldchecksum, 'oldpalette': oldpalette}
        img['checksum'] = MD380Graphics.gfxchecksum(img)
        return img

    @staticmethod
    def pbm(gfx):
        """Convert glyph object to PBM(P1) ascii bitmap image"""
        assert gfx['palette'] is None
        out = 'P1\n'
        out += '# MD380 address: %s\n' % hex(gfx['address'])
        out += '# MD380 checksum: %d\n' % gfx['checksum']
        out += '%d %d\n' % (gfx['width'], gfx['height'])
        for line in gfx['pixels']:
            bitline = ''.join([str(pixel) for pixel in line])

            # ASCII output (P1)
            sys.stdout.write('bit line  %s\n' % bitline)
            out += bitline + '\n'

            # Hex ouput (P4) - OSX doesn't render non-byte widths correctly (like all latin fonts)
            # bitline += '0' * ((8 - len(bitline)) % 8)  # pad to full byte
            # hexline = hex(int('1'+bitline, 2))[3:]
            # # Throw away python2.7's trailing long int marker
            # if hexline.endswith('L'):
            #    hexline = hexline[:-1]
            # byteline = binascii.unhexlify(hexline)
            # out += byteline
        return out

    @staticmethod
    def pbmparse(pbml):
        """Convert PBM(P1) ascii bitmap image to glyph object"""
        while (pbml != []) and (pbml[0] != 'P1'):
            pbml = pbml[1:]
        if (pbml == []) or (pbml[0] != 'P1'):
            return None, None
        i = 1
        addr = 0
        while pbml[i].startswith('#'):
            if pbml[i].startswith('# MD380 address: '):
                addr = int(pbml[i][17:], 16)
            if pbml[i].startswith('# MD380 checksum: '):
                oldchecksum = int(pbml[i][18:])
            i += 1
        width, height = pbml[i].split()
        width = int(width)
        height = int(height)

        # bytewidth = int(math.ceil(width/8.0))
        # data = b''.join(pbml[i+1:])
        # pixels = []
        # for y in range(height):
        #    bytesline = data[y*bytewidth:(y+1)*bytewidth]
        #    hexline = binascii.hexlify(b'\x01'+bytesline)
        #    binline = bin(int(hexline, 16))[3:]
        #    line = [int(pixel) for pixel in binline]
        #    pixels.append(line[:width])

        i += 1
        pixels = []
        for y in range(height):
            hexline = '1' + pbml[i]
            binline = bin(int(hexline, 2))[3:]
            line = [int(pixel) for pixel in binline]
            pixels.append(line[:width])
            i += 1

        img = {'address': addr, 'width': width, 'height': height,
               'palette': None, 'pixels': pixels,
               'oldchecksum': oldchecksum}
        img['checksum'] = MD380Graphics.gfxchecksum(img)
        return img, pbml[i:]

    def isSpriteStruct(self, p):
        mstart = self.addr
        mend = mstart + len(self.mem)
        if 0 < self.rw(p) < 0x200 and 0 < self.rw(p + 2) < 0x200:
            if 0 < self.rw(p + 4) < 0x0200 and 0 < self.rw(p + 6) < 0x200:
                if mstart < self.rl(p + 8) < mend and mstart < self.rl(p + 12) < mend:
                    cptr = self.rl(p + 12)
                    if self.rl(cptr) < 0x20 and self.rl(cptr + 4) < 0x20:
                        if mstart < self.rl(cptr + 8) < mend:
                            return True
        return False

    def isGlyphStruct(self, p):
        mstart = self.addr
        mend = mstart + len(self.mem)
        if 0 < self.rb(p) < 0x20 and 0 < self.rb(p + 1) < 0x20:
            if 0 < self.rb(p + 2) < 0x10 and 0 == self.rb(p + 3):
                if mstart < self.rl(p + 4) < mend:
                    return True
        return False

    def gfxscan(self):
        """Returns list of graphics structures found in memory"""
        if self.gfxscancache is None:
            self.gfxscancache = []
            mstart = self.addr
            mend = mstart + len(self.mem)
            for p in range(mstart, mend, 4):
                if self.isSpriteStruct(p):
                    parsed = self.gfxparse(p)
                    self.gfxscancache.append(parsed)
                    yield parsed
                    continue
                if self.isGlyphStruct(p):
                    parsed = self.glyphparse(p)
                    self.gfxscancache.append(parsed)
                    yield parsed
        else:
            for g in self.gfxscancache:
                yield g

    def gfxfind(self, checksum):
        """Searches memory for a specific graphics checksum"""
        candidates = []
        for candidate in self.gfxscan():
            if candidate['checksum'] == checksum:
                candidates.append(candidate)
        return candidates


class MD380Fonts(Memory):
    def table(self):
        t = []
        start = 0x080fbbb4
        end = 0x080fc6dc
        for p in range(start, end, 12):
            a = self.rw(p)
            b = self.rw(p + 2)
            addra = self.rl(p + 4)
            addrb = self.rl(p + 8)
            t.append({'x': a, 'y': b, 'addra': addra, 'addrb': addrb})
        return t


# with open('prom-private.img', 'rb') as f:
#    mdgfx = MD380Graphics(f.read(), 0x800c000)
# with open('patched.bin', 'rb') as f:
#     mdgfx = MD380Graphics(f.read(), 0x8000000)
#
# gfx = mdgfx.gfxparse(0x80f9cf8)
# mdgfx.gfxprint(gfx)
# glyph = mdgfx.glyphparse(0x807bfc8)
# mdgfx.gfxprint(glyph)
# glyph = mdgfx.glyphparse(0x8063918)
# mdgfx.gfxprint(glyph)
#
# if not os.path.exists('rawimg'):
#   os.mkdir('rawimg')
# for gfx in mdgfx.gfxscan():
#   # mdgfx.gfxprint(gfx)
#   if gfx['palette'] is None:
#       img = mdgfx.pbm(gfx)
#       name = '%s.pbm' % hex(gfx['address'])
#   else:
#       img = mdgfx.ppm(gfx)
#       name = '%s.ppm' % hex(gfx['address'])
#   with open('rawimg/%s' % name, 'wb') as f:
#       f.write(img)
#
# gfx = mdgfx.gfxparse(0x080f9ca8)
# img = mdgfx.ppm(gfx)
# with open('0x080f9ca8.ppm', 'wb') as f:
#     f.write(img)
#
# with open(sys.argv[1], 'rb') as f:
#     img = f.read()
#
# gfx = mdgfx.ppmparse(img)
# print(gfx)
# mdgfx.gfxprint(gfx)


# 0x807bfc8


def main():
    def hex_int(x):
        return int(x, 0)

    parser = argparse.ArgumentParser(description='Modifies graphics objects \
                                     in MD-380 firmware')
    parser.add_argument('--firmware', '-f', dest='firmware', type=str,
                        help='firmware image file to work on')
    parser.add_argument('--addr', '-a', dest='addr', type=hex_int,
                        default=0x800c000,
                        help='base address of raw flash image (default 0x800c000)')
    location_default = FontGFX_CN_START + 0x30000 - 0xc714
    parser.add_argument('--location', '-l', dest='location', type=hex_int,
                        default=location_default,
                        help='Location of free memory to write to (default 0x%x)'
                             % location_default)
    parser.add_argument('--dir', '-d', dest='dir', type=str,
                        help='directory for graphics files')
    parser.add_argument('--gfx', '-g', dest='gfx', type=str,
                        help='single graphics image file to work on')
    parser.add_argument('--checksum', '-c', dest='checksum', type=int,
                        help='checksum of graphics to be replaced')
    parser.add_argument('command', nargs=1, help='function to perform')
    args = parser.parse_args()

    # print("DEBUG: args: %s" % repr(args))

    sys.stdout.write('DEBUG: reading "%s"\n' % args.firmware)
    with open(args.firmware, 'rb') as f:
        input = f.read()

    md = MD380Graphics(input, base_address=args.addr)
    modified = False

    cmd = args.command[0]
    if cmd == 'extract':
        if args.dir is None:
            sys.stderr.write('ERROR: You must specify --dir to extract to.\n')
            sys.exit(5)
        if not os.path.isdir(args.dir):
            os.makedirs(args.dir)
        for gfx in md.gfxscan():
            if gfx['palette'] is None:
                img = md.pbm(gfx)
                name = '%s.pbm' % hex(gfx['address'])
                sys.stdout.write('DEBUG: Writing font glyph "%s".\n' % name)
            else:
                img = md.ppm(gfx)
                name = '%s.ppm' % hex(gfx['address'])
                sys.stdout.write('DEBUG: Writing sprite "%s".\n' % name)
            with open('%s/%s' % (args.dir, name), 'wb') as f:
                f.write(img)
    elif cmd == 'restore':
        sys.stderr.write('ERROR: Not implemented\n')
        sys.exit(5)
        if args.dir is None:
            sys.stderr.write('ERROR: You must specify --dir to restore from.\n')
            sys.exit(5)
        if not os.path.isdir(args.dir):
            sys.stderr.write('ERROR: --dir must be an existing directory.\n')
            sys.exit(5)
    elif cmd == 'write':
        if args.gfx is None:
            sys.stderr.write('ERROR: Can\'t work without --gfx to write.\n')
            sys.exit(5)
        with open(args.gfx, 'rb') as f:
            gfx = f.read()
        if args.gfx.lower().endswith('.ppm'):
            gfx = md.ppmparse(gfx)
        elif args.gfx.lower().endswith('.pbm'):
            sys.stderr.write('ERROR: PBM glyphs are not supported, yet.\n')
            sys.exit(5)
        else:
            sys.stderr.write('ERROR: Unsupported file format.\n')
            sys.exit(5)
        md.gfxprint(gfx)
        if args.checksum is not None:
            target_checksum = args.checksum
        elif 'oldchecksum' in gfx and gfx['oldchecksum'] is not None:
            target_checksum = gfx['oldchecksum']
        else:
            sys.stderr.write('ERROR: Checksum required in --checksum or PPM header.\n')
            sys.exit(5)
        sys.stderr.write("DEBUG: Looking for graphics checksum %d...\n" % target_checksum)
        candidates = md.gfxfind(target_checksum)
        if len(candidates) > 0:
            sys.stderr.write("DEBUG: Overwriting matching structure at %s.\n" % hex(candidates[0]['address']))
            md.gfxreplace(gfx, candidates[0]['address'])
            modified = True
        else:
            sys.stderr.write('WARNING: Checksum not found. Trying address fallback.\n')
            if gfx['address'] is not None:
                addr = gfx['address']
                sys.stderr.write('DEBUG: Overwriting address %s from PPM header.\n' % hex(addr))
                md.gfxreplace(gfx, addr)
                modified = True
            else:
                sys.stderr.write('ERROR: Can not determine fallback address from PPM header.\n')
                sys.exit(5)

    elif cmd == 'fontreplace':
        if args.gfx is None:
            sys.stderr.write('ERROR: Can\'t work without --gfx to read.\n')
            sys.exit(5)
        if not args.gfx.lower().endswith('.pbm'):
            sys.stderr.write('ERROR: only ascii PBM glyphs supported (multiple in one file ok)\n')
            sys.exit(5)
        with open(args.gfx, 'rb') as f:
            pbmfonts = f.read()
        pbmfontsl = pbmfonts.splitlines()

        gfx, pbmfontsl = md.pbmparse(pbmfontsl)
        while gfx is not None:
            # print('\n---------------------\nNew glyph:')
            # md.gfxprint(gfx)
            target_checksum = gfx['oldchecksum']
            # print("DEBUG: Looking for graphics checksum %d...\n" % target_checksum)
            candidates = md.gfxfind(target_checksum)
            if len(candidates) > 0:
                # print("DEBUG: Overwriting matching structure at %s.\n" % hex(candidates[0]['address']))
                # print('replaces old glyph:')
                md.glyphreplace(gfx, candidates[0]['address'])
                modified = True
            else:
                print('WARNING: Checksum not found. Trying address fallback.\n')
                if gfx['address'] is not None:
                    addr = gfx['address']
                    print('DEBUG: Overwriting address %s from PPM header.\n' % hex(addr))
                    md.glyphreplace(gfx, addr)
                    modified = True
                else:
                    print('ERROR: Can not determine fallback address from PPM header.\n')
                    sys.exit(5)
            gfx, pbmfontsl = md.pbmparse(pbmfontsl)

    elif cmd == 'relocate':
        if args.gfx is None:
            sys.stderr.write('ERROR: Can\'t work without --gfx to write.\n')
            sys.exit(5)
        if args.location is None:
            sys.stderr.write('ERROR: Don\'t know the --location to write to.\n')
            sys.exit(5)

        with open(args.gfx, 'rb') as f:
            gfx = f.read()
        if args.gfx.lower().endswith('.ppm'):
            gfx = md.ppmparse(gfx)
        elif args.gfx.lower().endswith('.pbm'):
            sys.stderr.write('ERROR: PBM glyphs are not supported, yet.\n')
            sys.exit(5)
        else:
            sys.stderr.write('ERROR: Unsupported file format.\n')
            sys.exit(5)
        md.gfxprint(gfx)

        if gfx['address'] is not None:
            addr = gfx['address']
            sys.stderr.write('DEBUG: Overwriting address %s from PPM header.\n' % hex(addr))
            md.gfxrelocate(gfx, addr, args.location)
            modified = True
        else:
            sys.stderr.write('ERROR: Can not determine gfx struct address from PPM header.\n')
            sys.exit(5)

    elif cmd == 'ipython':
        font = MD380Fonts(input, args.addr)
        no = '不'
        yes = '是'
        from IPython import embed
        embed()
    else:
        sys.stderr.write('ERROR: Use extract, restore, write, fontreplace or relocate as command. %s\n' % cmd)
        sys.exit(5)
    if modified:
        # args.firmware += '.modified'
        sys.stderr.write('DEBUG: writing "%s"\n' % args.firmware)
        with open(args.firmware, 'wb') as f:
            f.write(md.mem)


if __name__ == "__main__":
    main()
