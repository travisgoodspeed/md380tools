#!/usr/bin/env python3
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
        out = bytearray()
        l = max(len(a), len(b))
        for i in range(l):
            val=a[i%len(a)]^b[i%len(b)]
            out.append(val);

        # Length better match.
        assert len(out)==l
        
        return out

class MD2017FW(TYTFW):
    #Many thanks to KG5RKI
    #https://github.com/travisgoodspeed/md380tools/issues/789
    key = bytes.fromhex('00aa89891f4beccf424514540065eb66417d4c88495a210df2f5c8e638edbcb9'
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
          'd57e11e04a36e7a666dc44e2f79afa30fc00a9c2adf9e0f8bbfe8431d88976e2')

    def __init__(self, base_address=0x800c000):
        self.magic = b'OutSecurityBin'
        self.jst = b'MD-9600\x00\x00'
        self.foo = bytes.fromhex('3002003000400047')
        self.bar = bytes.fromhex('02190C030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20')
        self.foob = bytes.fromhex('0200000000000600')
        self.start = base_address
        self.app = None
        self.footer = b'OutputBinDataEnd'
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
    key = bytes.fromhex(
        '2edf40b5bdda91352142e3e26da90b90'
        '31303afa4f0574640a29447e6077ad8c'
        '9ae263c421fe3cf793c2e174168cc92a'
        'ed65680c4986a3ba611c885dc4493cd2'
        'ee6b340c1aa0a8b3588a4511df4f232f'
        'a4e4f63b2c8c882d9e9b67ab1c80da29'
        '53021a5451cabfb19722798170fc00e9'
        '81364e4fa01c0b07ea2f492f0f2571d7'
        'f1307d666e8368387913e38c709a4a9e'
        'a9e2d6104f40148e6c5e96b2463ee825'
        'ef7cc50818d48b9226e3edfa8832e897'
        '4770f846deff8b0c4db3b6fc69d6275b'
        '766f5b03f7c31105c51dfe925fcbc21c'
        '81691bb8f86258c7b4b311d51ff216c1'
        'ad8fa51eb45be0da7f467d1d9e6dc074'
        '7f54a62f436f6408cae80f05109c9d9f'
        'bd670c23f7a1e1597be8d464ec20cae9'
        '6ab9037367309516b6d91953e5dba43c'
        'cd7cf9d8679ffcc9e28a6a2cf2edc8c1'
        '6a20994c0dadd43ba10e958846b813e1'
        '0658d207ad5c1a74dbb5a74057dba245'
        'a612d082dded0abdb310ed6cda39d2d6'
        '9082007671e021a08ff0f367c4f340bd'
        '471610dc7ef81de5136687c74a69c963'
        '9282ecee5a34fb9625c3b668e13c8a71'
        '74b5c12399d6f7fbea98cd613d4de1d0'
        '34e1fd36105f8e9ec6b6580c55be69a8'
        '56764b1fd5907e475f2f25025cef0064'
        'a0269a183c69c4ff9a52411bc981c3ac'
        '15e11798db2c9c109bb2f9714f560f68'
        'fbd92d5a865b8303c81eda5de48e82c3'
        'd87e8b5652b538a0c6a9b077bd8af724'
        '70821dc5953cb5f079a389994fec8c36'
        'c7d61020e330393d079cb2dc4f949ee0'
        '24aad2211214410fd467b799b1a3cb4d'
        '0c700fc036a7893086146768ac7beee4'
        '42d8b436a4eb0fa802f4cd23b3bc254f'
        'ccd4eefcf2210fc16c9937e27c47ce77'
        'f0952bcbf4ca07032ad23100fd3e8486'
        '328b179dbfa7b337e1b18a14690025e3'
        '56689faaa9b8116775874df83631cf38'
        '631cf06b47405ddc0ce6c8c419afdd6e'
        '9ed978996cbe151e0b9d88d2069deeae'
        '8a0fe32d2ff4f5f616bf59bb345cdd61'
        'ed701e61e5e3fb6e139c4958178bc830'
        'cded56ad22cb63ce26c4a5c1630d0d04'
        '6eb6f9cabb2faba0b50afa500e024705'
        '543db3b1c6ce8fac657e159e4ecc559e'
        '4632719b97aa0dfb1b710283960b5277'
        '48876102c30462d7fb740f199ca09d79'
        'a06def9e205d0ac96a58c9b955add1cc'
        'd154c868c276c2990f2efcfbf592cddb'
        'a2edd999ff4f8850cd48b7b9f3f0ad4d'
        '162a50aa6b2a9838c935450c03a8cd0d'
        '743c9955db8870da6ac8344d19dccc42'
        '40946192652acdfd521050146bec8557'
        '3fe2959a5d11abad6960a83b6f7a17f3'
        '761763e6597e4730d24787dbd866de00'
        '2b65372f2df12011f3987b4c9cd176a7'
        'e13dbe6fee2cf01970635128f01dbe52'
        '5f4fe6def230b65030f9154849e9d2a8'
        'a98ddaf5cd3eaf0055eb15c55b190f93'
        '0427096d54d757b1470adef71dcb113c'
        'f58f20409dbb6b2ca9673d78c262b70c')
    def __init__(self, base_address=0x800c000):
        self.magic = b'OutSecurityBin'
        self.jst = b'JST51'
        self.foo = bytes.fromhex('3002003000400047')
        self.bar = bytes.fromhex('010d02030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20')
        self.start = base_address
        self.app = None
        self.footer = b'OutputBinDataEnd'
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
    for k in radios.keys():
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
            #for i in range(0, 256, 16):
            #    hl = binascii.hexlify(input[i:i + 16])
            #    hl = ' '.join(hl[i:i + 2] for i in range(0, 32, 2))
            #    sys.stderr.write(hl + '\n')
            sys.stderr.write('Trying anyway.  This works on S013.020.bin.\n')
        output = md.app
        #print('INFO: base address 0x{0:x}'.format(md.start))
        print('INFO: length 0x{0:x}'.format(len(md.app)))

    print('DEBUG: writing "%s"' % args.output[0])
    with open(args.output[0], 'wb') as f:
        f.write(output)


if __name__ == "__main__":
    main()
