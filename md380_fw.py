#!/usr/bin/env python2
# -*- coding: utf-8 -*-

from __future__ import print_function

import argparse
import binascii
import struct
import sys

class TYTFW(object):
    def pad(self, align=512, byte=b'\xff'):
        pad_length = (align - len(self.app) % align) % align
        self.app += byte * pad_length

    def unwrap(self, img):
        header = struct.Struct(self.header_fmt)
        header = header.unpack(img[:256])

        self.start = header[6]
        app_len = header[7]
        self.app = self.crypt(img[256:256 + app_len])

        assert header[0].startswith(self.magic)
        assert header[1].startswith(self.jst)
        assert header[3].startswith(self.foo)
        assert header[4] == self.bar
        assert 0x8000000 <= header[6] < 0x8200000
        assert header[7] == len(img) - 512

    def crypt(self, data):
        return self.xor(data, self.key)

    @staticmethod
    def xor(a, b):
        # FIXME: optimized version
        out = b''
        l = max(len(a), len(b))
        for i in range(l):
            out += chr(ord(a[i % len(a)]) ^ ord(b[i % len(b)]))
        return out

class MD2017FW(TYTFW):
    #Many thanks to KG5RKI
    #https://github.com/travisgoodspeed/md380tools/issues/789
    key = ('00aa89891f4beccf424514540065eb66417d4c88495a210df2f5c8e638edbcb9'
          'fb357133010a7f9e3b2903b6493e42b83f9f90bdaa3a7146cecdfd183255894a'
          '5fc8839ce4069e0a9d0d2fa1356dd792eafd638590cbf02fd9595327d306b8f5'
          'b2ca886cd026913bf25b61becddbf21ac9fd8d8804f4e8f19a0292bc24e990e4'
          '7ea3494dce529f58c17a5fb5186edb786408d56f0d9d9fb39930817e2be65b3c'
          '4aba8be5e472118993d2f12d1e0fd9d5438704e2b4ae5e9e5f4ce916f2e65f28'
          '9f7919dc1d6f2ff8efcbe1cee8a73659efe0e28800106dda73bc922b81cfe6ce'
          '0447badb7e2f41ca5dcdf6417d1c382bef7d370af9a9148e5dea4466de8b3656'
          '018c358a119b8f2a6540f72ee6582874cbc4cb0fa7629be3a63cc76f130097e9'
          '1eb15390dd9b613e908cad3c29414e5b0c1f664013244900d51ce2ed281853af'
          'e41cdc96ea18fe2e6519e01450c1f10939f5cf4544d5680d72f15f8823b9b1cf'
          'da36984341f8af236d50575e62bf5aa5daadcfc5425e3e34062304e90ecdf871'
          '89674e40e925bc452e97dcc16822d25877b12e6916a8149b181a9ab8f03b71bf'
          '7718c834ea856dbb325735e569d49f4a9968b4d8c79a316a303de89cd2eb64de'
          '2eafccc84d0209ae01f92b736dbc09a2c73a28ba5d1bdfcad6f6b83ebbc518f9'
          '369623a41983da4521e386137dc25a898a8f54b9e11564e393add046b3b1d736'
          '1533956f56ef26a91c7f0e6c9fced82669cffe7b5a6f09dceec8f95bc397e7bd'
          '55f0e9d10c3036017a348b27ddc8cda2ec62efa8d01116dd70b0fb25f15f91b7'
          '7d34e974442d5276c169c4eb3f987f249bb1efe94be3d3109fcd9e4e47f11d4c'
          '16665bfd06cec2307b888261cc2737d5ff22c6e6d4cc879b0687aa7bcd35d3a3'
          'a7f0081758fbcd562ff88d318c5b3cdc9f1e3b4672b77ca62a47e6568a14fbe5'
          'b839b868449cbc106621ad02871dd862030e17b12e89f85a95731b878674dc39'
          'd2a93298d199d788a76baa7cc656518fb45822d10f2b44dece7511b6c93fbfc8'
          '7ca8405007da66e37a3e4b4850ecf08a3966244b1d85a85b5db3908a5c5becba'
          '3e9ea838ef48b14c6702590e2dc9fd7c1a9ee5ca607f6bf9cb9760ab46b2ab36'
          'a0f333f790c900e9f71f9d7566d3c08ce06a2cf4e102d7df9e8748c28f2a4464'
          '2b0fa936f3469ae2b1fddc2602f480e31231c371a7f4323661ed127740adfe6d'
          '665bd29c1ea8c8601e04e1c9091387a8385a70eaba3fc525993084715f222379'
          'd93d76d21bd5d28bc49d730584171b04db4ffc0723c9d8d5d0b86759f770f9af'
          '0d1e5c7ff2b7008a2d2e59827aea851f82772f6fe97cb36e8ded82d60d81c938'
          '89674d4ca9359986e1215ce9f3730d20b53ad0cb143e9d1759379f91ab3cda3c'
          'd57e11e04a36e7a666dc44e2f79afa30fc00a9c2adf9e0f8bbfe8431d88976e2').decode('hex')

    def __init__(self, base_address=0x800c000):
        self.magic = b'OutSecurityBin'
        self.jst = b'MD-9600\x00\x00'
        self.foo = '\x30\x02\x00\x30\x00\x40\x00\x47'
        self.bar = ('\x02\x19\x0C\x03\x04\x05\x06\x07'
                    '\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f'
                    '\x10\x11\x12\x13\x14\x15\x16\x17'
                    '\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f'
                    '\x20')
        self.foob = ('\x02\x00\x00\x00\x00\x00\x06\x00')
        self.start = base_address
        self.app = None
        self.footer = 'OutputBinDataEnd'
        self.header_fmt = '<16s9s7s16s33s43s8sLLL112s'
        self.footer_fmt = '<240s16s'
        self.rsrcSize = 0x5D400

    def unwrap(self, img):
        header = struct.Struct(self.header_fmt)
        header = header.unpack(img[:256])

        self.start = header[6]
        app_len = header[7]
        self.app = self.crypt(img[256:256 + app_len])

    def wrap(self):
        bin = b''
        header = struct.Struct(self.header_fmt)
        footer = struct.Struct(self.footer_fmt)
        self.pad()
        app = self.crypt(self.app)
        bin += header.pack(
            self.magic, self.jst, b'\xff' * 7, self.foo,
            self.bar, b'\xff' * 43, self.foob, self.rsrcSize, self.start, len(app)-self.rsrcSize,
                                  b'\xff' * 112)
        bin += self.crypt(self.app)
        bin += footer.pack(b'\xff' * 240, self.footer)
        return bin

class MD380FW(TYTFW):
    # The stream cipher of MD-380 OEM firmware updates boils down
    # to a cyclic, static XOR key block, and here it is:
    key = (
        '\x2e\xdf\x40\xb5\xbd\xda\x91\x35\x21\x42\xe3\xe2\x6d\xa9\x0b\x90'
        '\x31\x30\x3a\xfa\x4f\x05\x74\x64\x0a\x29\x44\x7e\x60\x77\xad\x8c'
        '\x9a\xe2\x63\xc4\x21\xfe\x3c\xf7\x93\xc2\xe1\x74\x16\x8c\xc9\x2a'
        '\xed\x65\x68\x0c\x49\x86\xa3\xba\x61\x1c\x88\x5d\xc4\x49\x3c\xd2'
        '\xee\x6b\x34\x0c\x1a\xa0\xa8\xb3\x58\x8a\x45\x11\xdf\x4f\x23\x2f'
        '\xa4\xe4\xf6\x3b\x2c\x8c\x88\x2d\x9e\x9b\x67\xab\x1c\x80\xda\x29'
        '\x53\x02\x1a\x54\x51\xca\xbf\xb1\x97\x22\x79\x81\x70\xfc\x00\xe9'
        '\x81\x36\x4e\x4f\xa0\x1c\x0b\x07\xea\x2f\x49\x2f\x0f\x25\x71\xd7'
        '\xf1\x30\x7d\x66\x6e\x83\x68\x38\x79\x13\xe3\x8c\x70\x9a\x4a\x9e'
        '\xa9\xe2\xd6\x10\x4f\x40\x14\x8e\x6c\x5e\x96\xb2\x46\x3e\xe8\x25'
        '\xef\x7c\xc5\x08\x18\xd4\x8b\x92\x26\xe3\xed\xfa\x88\x32\xe8\x97'
        '\x47\x70\xf8\x46\xde\xff\x8b\x0c\x4d\xb3\xb6\xfc\x69\xd6\x27\x5b'
        '\x76\x6f\x5b\x03\xf7\xc3\x11\x05\xc5\x1d\xfe\x92\x5f\xcb\xc2\x1c'
        '\x81\x69\x1b\xb8\xf8\x62\x58\xc7\xb4\xb3\x11\xd5\x1f\xf2\x16\xc1'
        '\xad\x8f\xa5\x1e\xb4\x5b\xe0\xda\x7f\x46\x7d\x1d\x9e\x6d\xc0\x74'
        '\x7f\x54\xa6\x2f\x43\x6f\x64\x08\xca\xe8\x0f\x05\x10\x9c\x9d\x9f'
        '\xbd\x67\x0c\x23\xf7\xa1\xe1\x59\x7b\xe8\xd4\x64\xec\x20\xca\xe9'
        '\x6a\xb9\x03\x73\x67\x30\x95\x16\xb6\xd9\x19\x53\xe5\xdb\xa4\x3c'
        '\xcd\x7c\xf9\xd8\x67\x9f\xfc\xc9\xe2\x8a\x6a\x2c\xf2\xed\xc8\xc1'
        '\x6a\x20\x99\x4c\x0d\xad\xd4\x3b\xa1\x0e\x95\x88\x46\xb8\x13\xe1'
        '\x06\x58\xd2\x07\xad\x5c\x1a\x74\xdb\xb5\xa7\x40\x57\xdb\xa2\x45'
        '\xa6\x12\xd0\x82\xdd\xed\x0a\xbd\xb3\x10\xed\x6c\xda\x39\xd2\xd6'
        '\x90\x82\x00\x76\x71\xe0\x21\xa0\x8f\xf0\xf3\x67\xc4\xf3\x40\xbd'
        '\x47\x16\x10\xdc\x7e\xf8\x1d\xe5\x13\x66\x87\xc7\x4a\x69\xc9\x63'
        '\x92\x82\xec\xee\x5a\x34\xfb\x96\x25\xc3\xb6\x68\xe1\x3c\x8a\x71'
        '\x74\xb5\xc1\x23\x99\xd6\xf7\xfb\xea\x98\xcd\x61\x3d\x4d\xe1\xd0'
        '\x34\xe1\xfd\x36\x10\x5f\x8e\x9e\xc6\xb6\x58\x0c\x55\xbe\x69\xa8'
        '\x56\x76\x4b\x1f\xd5\x90\x7e\x47\x5f\x2f\x25\x02\x5c\xef\x00\x64'
        '\xa0\x26\x9a\x18\x3c\x69\xc4\xff\x9a\x52\x41\x1b\xc9\x81\xc3\xac'
        '\x15\xe1\x17\x98\xdb\x2c\x9c\x10\x9b\xb2\xf9\x71\x4f\x56\x0f\x68'
        '\xfb\xd9\x2d\x5a\x86\x5b\x83\x03\xc8\x1e\xda\x5d\xe4\x8e\x82\xc3'
        '\xd8\x7e\x8b\x56\x52\xb5\x38\xa0\xc6\xa9\xb0\x77\xbd\x8a\xf7\x24'
        '\x70\x82\x1d\xc5\x95\x3c\xb5\xf0\x79\xa3\x89\x99\x4f\xec\x8c\x36'
        '\xc7\xd6\x10\x20\xe3\x30\x39\x3d\x07\x9c\xb2\xdc\x4f\x94\x9e\xe0'
        '\x24\xaa\xd2\x21\x12\x14\x41\x0f\xd4\x67\xb7\x99\xb1\xa3\xcb\x4d'
        '\x0c\x70\x0f\xc0\x36\xa7\x89\x30\x86\x14\x67\x68\xac\x7b\xee\xe4'
        '\x42\xd8\xb4\x36\xa4\xeb\x0f\xa8\x02\xf4\xcd\x23\xb3\xbc\x25\x4f'
        '\xcc\xd4\xee\xfc\xf2\x21\x0f\xc1\x6c\x99\x37\xe2\x7c\x47\xce\x77'
        '\xf0\x95\x2b\xcb\xf4\xca\x07\x03\x2a\xd2\x31\x00\xfd\x3e\x84\x86'
        '\x32\x8b\x17\x9d\xbf\xa7\xb3\x37\xe1\xb1\x8a\x14\x69\x00\x25\xe3'
        '\x56\x68\x9f\xaa\xa9\xb8\x11\x67\x75\x87\x4d\xf8\x36\x31\xcf\x38'
        '\x63\x1c\xf0\x6b\x47\x40\x5d\xdc\x0c\xe6\xc8\xc4\x19\xaf\xdd\x6e'
        '\x9e\xd9\x78\x99\x6c\xbe\x15\x1e\x0b\x9d\x88\xd2\x06\x9d\xee\xae'
        '\x8a\x0f\xe3\x2d\x2f\xf4\xf5\xf6\x16\xbf\x59\xbb\x34\x5c\xdd\x61'
        '\xed\x70\x1e\x61\xe5\xe3\xfb\x6e\x13\x9c\x49\x58\x17\x8b\xc8\x30'
        '\xcd\xed\x56\xad\x22\xcb\x63\xce\x26\xc4\xa5\xc1\x63\x0d\x0d\x04'
        '\x6e\xb6\xf9\xca\xbb\x2f\xab\xa0\xb5\x0a\xfa\x50\x0e\x02\x47\x05'
        '\x54\x3d\xb3\xb1\xc6\xce\x8f\xac\x65\x7e\x15\x9e\x4e\xcc\x55\x9e'
        '\x46\x32\x71\x9b\x97\xaa\x0d\xfb\x1b\x71\x02\x83\x96\x0b\x52\x77'
        '\x48\x87\x61\x02\xc3\x04\x62\xd7\xfb\x74\x0f\x19\x9c\xa0\x9d\x79'
        '\xa0\x6d\xef\x9e\x20\x5d\x0a\xc9\x6a\x58\xc9\xb9\x55\xad\xd1\xcc'
        '\xd1\x54\xc8\x68\xc2\x76\xc2\x99\x0f\x2e\xfc\xfb\xf5\x92\xcd\xdb'
        '\xa2\xed\xd9\x99\xff\x4f\x88\x50\xcd\x48\xb7\xb9\xf3\xf0\xad\x4d'
        '\x16\x2a\x50\xaa\x6b\x2a\x98\x38\xc9\x35\x45\x0c\x03\xa8\xcd\x0d'
        '\x74\x3c\x99\x55\xdb\x88\x70\xda\x6a\xc8\x34\x4d\x19\xdc\xcc\x42'
        '\x40\x94\x61\x92\x65\x2a\xcd\xfd\x52\x10\x50\x14\x6b\xec\x85\x57'
        '\x3f\xe2\x95\x9a\x5d\x11\xab\xad\x69\x60\xa8\x3b\x6f\x7a\x17\xf3'
        '\x76\x17\x63\xe6\x59\x7e\x47\x30\xd2\x47\x87\xdb\xd8\x66\xde\x00'
        '\x2b\x65\x37\x2f\x2d\xf1\x20\x11\xf3\x98\x7b\x4c\x9c\xd1\x76\xa7'
        '\xe1\x3d\xbe\x6f\xee\x2c\xf0\x19\x70\x63\x51\x28\xf0\x1d\xbe\x52'
        '\x5f\x4f\xe6\xde\xf2\x30\xb6\x50\x30\xf9\x15\x48\x49\xe9\xd2\xa8'
        '\xa9\x8d\xda\xf5\xcd\x3e\xaf\x00\x55\xeb\x15\xc5\x5b\x19\x0f\x93'
        '\x04\x27\x09\x6d\x54\xd7\x57\xb1\x47\x0a\xde\xf7\x1d\xcb\x11\x3c'
        '\xf5\x8f\x20\x40\x9d\xbb\x6b\x2c\xa9\x67\x3d\x78\xc2\x62\xb7\x0c')

    def __init__(self, base_address=0x800c000):
        self.magic = b'OutSecurityBin'
        self.jst = b'JST51'
        self.foo = '\x30\x02\x00\x30\x00\x40\x00\x47'
        self.bar = ('\x01\x0d\x02\x03\x04\x05\x06\x07'
                    '\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f'
                    '\x10\x11\x12\x13\x14\x15\x16\x17'
                    '\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f'
                    '\x20')
        self.start = base_address
        self.app = None
        self.footer = 'OutputBinDataEnd'
        self.header_fmt = '<16s7s9s16s33s47sLL120s'
        self.footer_fmt = '<240s16s'

    def wrap(self):
        bin = b''
        header = struct.Struct(self.header_fmt)
        footer = struct.Struct(self.footer_fmt)
        self.pad()
        app = self.crypt(self.app)
        bin += header.pack(
            self.magic, self.jst, b'\xff' * 9, self.foo,
            self.bar, b'\xff' * 47, self.start, len(app),
                                  b'\xff' * 120)
        bin += self.crypt(self.app)
        bin += footer.pack(b'\xff' * 240, self.footer)
        return bin

def radioFW(name):
    radios = {
            "MD2017":MD2017FW,
            "MD380":MD380FW
            }
    for k in radios.iterkeys():
        if name.upper() in k:
            return radios[k]
    raise KeyError



def main():
    def hex_int(x):
        return int(x, 0)

    parser = argparse.ArgumentParser(description='Wrap and unwrap MD-380 and MD2017 firmware')
    parser.add_argument('--radio', '-r', dest='radioname', default=None, help='Radio model (MD380 or MD2017, default MD380 if not provided and can\'t be guessed from input filename)') #default gets set below
    parser.add_argument('--wrap', '-w', dest='wrap', action='store_true',
                        default=False,
                        help='wrap app into firmware image')
    parser.add_argument('--unwrap', '-u', dest='unwrap', action='store_true',
                        default=False,
                        help='unwrap app from firmware image')
    parser.add_argument('--addr', '-a', dest='addr', type=hex_int,
                        default=0x800c000,
                        help='base address in flash')
    parser.add_argument('--offset', '-o', dest='offset', type=hex_int,
                        default=0,
                        help='offset to skip in app binary')
    parser.add_argument('input', nargs=1, help='input file')
    parser.add_argument('output', nargs=1, help='output file')
    args = parser.parse_args()

    if not (args.wrap ^ args.unwrap):
        sys.stderr.write('ERROR: --wrap or --unwrap?')
        sys.exit(5)

    print('DEBUG: reading "%s"' % args.input[0])
    with open(args.input[0], 'rb') as f:
        input = f.read()

    if args.radioname is None: #if not explicitly set, try and guess from filename
        radioname = "MD2017" if "TYT2017" in args.input[0] else "MD380"
        print("Guessing %s for radio model"%(radioname))
    else:
        radioname = args.radioname

    if args.wrap:
        if args.offset > 0:
            print('INFO: skipping 0x%x bytes in input file' % args.offset)
        
        md = radioFW(radioname)(args.addr)
        md.app = input[args.offset:]
        if len(md.app) == 0:
            sys.stderr.write('ERROR: seeking beyond end of input file\n')
            sys.exit(5)
        output = md.wrap()
        print('INFO: base address 0x{0:x}'.format(md.start))
        print('INFO: length 0x{0:x}'.format(len(md.app)))

    elif args.unwrap:
        md = radioFW(radioname)(args.addr)
        try:
            md.unwrap(input)
        except AssertionError:
            sys.stderr.write('WARNING: Funky header:\n')
            for i in range(0, 256, 16):
                hl = binascii.hexlify(input[i:i + 16])
                hl = ' '.join(hl[i:i + 2] for i in range(0, 32, 2))
                sys.stderr.write(hl + '\n')
            sys.stderr.write('Trying anyway.\n')
        output = md.app
        #print('INFO: base address 0x{0:x}'.format(md.start))
        print('INFO: length 0x{0:x}'.format(len(md.app)))

    print('DEBUG: writing "%s"' % args.output[0])
    with open(args.output[0], 'wb') as f:
        f.write(output)


if __name__ == "__main__":
    main()
