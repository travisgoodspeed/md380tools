# -*- coding: utf-8 -*-
from __future__ import print_function

import argparse
import math
import re
import sys


class MD380Graphics(object):
    @staticmethod
    def gfxprintpal(gfx, name):
        byte_cnt = 0
        palstring = ''
        for color in gfx['palette']:
            r, g, b, a = color
            if byte_cnt == 0:
                palstring = "  0x" + str(hex(r)[2:].zfill(2))
            else:
                palstring += ', '
                if byte_cnt % 4 == 0:
                    palstring += '\n  '
                palstring += "0x" + str(hex(r)[2:].zfill(2))
            palstring += ', ' + "0x" + str(hex(g)[2:].zfill(2))
            palstring += ', ' + "0x" + str(hex(b)[2:].zfill(2))
            palstring += ', ' + "0x" + str(hex(a)[2:].zfill(2))
            byte_cnt += 1

        print("char %s_paltab[] = {" % name)
        print(palstring)
        print("  };")

    @staticmethod
    def gfxprintpix(gfx, name):
        bitsperpixel = int(math.ceil(math.log(len(gfx['palette'])) / math.log(2)))
        byte_cnt = 0
        pixstring = ''
        for line in gfx['pixels']:
            padding = gfx['width'] - len(line)
            line += [0] * padding
            linebits = [bin(c + 0x10000)[-bitsperpixel:] for c in line]
            linebits = ''.join(linebits)

            for i in range(0, len(linebits), 8):
                if byte_cnt == 0:
                    pixstring = "  0x" + str(hex(int(linebits[i:i + 8], 2))[2:].zfill(2))
                else:
                    pixstring += ','
                    if byte_cnt % 20 == 0:
                        pixstring += '\n  '
                    pixstring += "0x" + str(hex(int(linebits[i:i + 8], 2))[2:].zfill(2))
                byte_cnt += 1
        print("char %s_pix[] = {" % name)
        print(pixstring)
        print("  };")

    @staticmethod
    def gfxprintstruct(gfx, name):
        print("const gfx_pal    %s_pal\t= { %d, 0, %s_paltab};" % (name, len(gfx['palette']), name))
        print("const gfx_bitmap bmp_%s\t= { %d, %d, %d, %d, %s_pix, &%s_pal, 0};" % (
            name,
            gfx['width'],
            gfx['height'],
            int(math.ceil(math.log(len(gfx['palette'])) / math.log(2)) * gfx['width'] / 8.0),
            int(math.ceil(math.log(len(gfx['palette'])) / math.log(2))),
            name,
            name))

    @staticmethod
    def ppmparse(ppm):  # stolen from md380-gfx
        """Convert PPM(P6) image to sprite object"""
        ppml = ppm.split('\n')
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
        return img


def main():
    parser = argparse.ArgumentParser(description='addad graphics objects')
    parser.add_argument('--gfx', '-g', dest='gfx', type=str,
                        help='single graphics image file to work on')

    args = parser.parse_args()

    md = MD380Graphics()

    if args.gfx is None:
        sys.stderr.write('ERROR: Can\'t work without --gfx to write.\n')
        sys.exit(5)
    with open(args.gfx, 'rb') as f:
        gfx = f.read()
    if args.gfx.lower().endswith('.ppm'):
        gfx = md.ppmparse(gfx)
    name = re.sub(r'\.ppm$', '', args.gfx)
    md.gfxprintpal(gfx, name)
    md.gfxprintpix(gfx, name)
    md.gfxprintstruct(gfx, name)


if __name__ == "__main__":
    main()
