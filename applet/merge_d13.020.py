#! python2.7
# -*- coding: utf-8 -*-


# This script implements our old methods for merging an MD380 firmware
# image with its patches.  It is presently being rewritten to require
# fewer explicit addresses, so that we can target our patches to more
# than one version of the MD380 firmware.

from __future__ import print_function

import sys


class Symbols(object):
    addresses = {}
    names = {}

    def __init__(self, filename):
        print("Loading symbols from %s" % filename)
        fsyms = open(filename)
        for l in fsyms:
            try:
                r = l.strip().split('\t')
                if len(r) == 2 and r[0].split(' ')[7] == '.text':
                    adr = r[0].split(' ')[0].strip()
                    name = r[1].split(' ')[1]  # .strip();
                    # print("%s is at %s" % (name, adr))
                    self.addresses[name] = int(adr, 16)
                    self.names[int(adr, 16)] = name
            except IndexError:
                pass;
    def getadr(self,name):
        return self.addresses[name];
    def try_getadr(self,name): # DL4YHF 2017-01, used to CHECK if a symbol exists
        try:                   # to perform patches for 'optional' C functions 
            return self.addresses[name];
        except KeyError:
            return None;
    def getname(self,adr):
        return self.names[adr];

class Merger(object):
    def __init__(self, filename, offset=0x0800C000):
        """Opens the input file."""
        self.offset = offset
        self.file = open(filename, "rb")
        self.bytes = bytearray(self.file.read())
        self.length = len(self.bytes)

    def setbyte(self, adr, new, old=None):
        """Patches a single byte from the old value to the new value."""
        self.bytes[adr - self.offset] = new

    def getbyte(self, adr):
        """Reads a byte from the firmware address."""
        b = self.bytes[adr - self.offset]
        return b

    def export(self, filename):
        """Exports to a binary file."""
        outfile = open(filename, "wb")
        outfile.write(self.bytes)

    def assertbyte(self, adr, val):
        """Asserts that a byte has a given value."""
        assert self.getbyte(adr) == val
        return

    def getword(self, adr):
        """Reads a byte from the firmware address."""
        w = (
            self.bytes[adr - self.offset] +
            (self.bytes[adr - self.offset + 1] << 8) +
            (self.bytes[adr - self.offset + 2] << 16) +
            (self.bytes[adr - self.offset + 3] << 24)
        )

        return w

    def setword(self, adr, new, old=None):
        """Patches a 32-bit word from the old value to the new value."""
        if old is not None:
            self.assertbyte(adr, old & 0xFF)
            self.assertbyte(adr + 1, (old >> 8) & 0xFF)
            self.assertbyte(adr + 2, (old >> 16) & 0xFF)
            self.assertbyte(adr + 3, (old >> 24) & 0xFF)

        # print("Patching word at %08x to %08x" % (adr, new))
        self.bytes[adr - self.offset] = new & 0xFF
        self.bytes[adr - self.offset + 1] = (new >> 8) & 0xFF
        self.bytes[adr - self.offset + 2] = (new >> 16) & 0xFF
        self.bytes[adr - self.offset + 3] = (new >> 24) & 0xFF
        self.assertbyte(adr, new & 0xFF)
        self.assertbyte(adr + 1, (new >> 8) & 0xFF)

    def sethword(self, adr, new, old=None):
        """Patches a byte pair from the old value to the new value."""
        if old is not None:
            self.assertbyte(adr, old & 0xFF)
            self.assertbyte(adr + 1, (old >> 8) & 0xFF)
        # print("Patching hword at %08x to %04x" % (adr, new))
        self.bytes[adr - self.offset] = new & 0xFF
        self.bytes[adr - self.offset + 1] = (new >> 8) & 0xFF
        self.assertbyte(adr, new & 0xFF)
        self.assertbyte(adr + 1, (new >> 8) & 0xFF)

    def hookstub(self, adr, handler):
        """Hooks a function by placing an unconditional branch at adr to
           handler.  The recipient function must have an identical calling
           convention. """
        adr &= ~1  # Address must be even.
        handler |= 1  # Destination address must be odd.
        # print("Inserting a stub hook at %08x to %08x." % (adr, handler))

        # FIXME This clobbers r0, should use a different register.
        self.sethword(adr, 0x4801)  # ldr r0, [pc, 4]
        self.sethword(adr + 2, 0x4700)  # bx r0
        self.sethword(adr + 4, 0x4600)  # NOP
        self.sethword(adr + 6, 0x4600)  # NOP, might be overwritten
        if adr & 2 > 0:
            self.setword(adr + 6, handler)  # bx r0
        else:
            self.setword(adr + 8, handler)  # bx r0

    def hookstub2(self, adr, handler):
        """Hooks a function by placing an unconditional branch at adr to
           handler.  The recipient function must have an identical calling
           convention. """
        adr &= ~1  # Address must be even.
        handler |= 1  # Destination address must be odd.
        print("Inserting a stub hook at %08x to %08x." % (adr, handler))

        # insert trampoline
        # rasm2 -a arm -b 16 '<asm code>'
        self.sethword(adr, 0xb401)  # push {r0}
        self.sethword(adr + 2, 0xb401)  # push {r0}
        self.sethword(adr + 4, 0x4801)  # ldr r0, [pc, 4]
        self.sethword(adr + 6, 0x9001)  # str r0, [sp, 4] (pc)
        self.sethword(adr + 8, 0xbd01)  # pop {r0,pc}
        self.sethword(adr + 10, 0x4600)  # NOP, might be overwritten
        if adr & 2 > 0:
            self.setword(adr + 10, handler)
        else:
            self.setword(adr + 12, handler)

    def calcbl(self, adr, target):
        """Calculates the Thumb code to branch to a target."""
        offset = target - adr
        # print("offset=%08x" % offset)
        offset -= 4  # PC points to the next ins.
        offset = (offset >> 1)  # LSBit is ignored.
        hi = 0xF000 | ((offset & 0xfff800) >> 11)  # Hi address setter, but at lower adr.
        lo = 0xF800 | (offset & 0x7ff)  # Low adr setter goes next.
        # print("%04x %04x" % (hi, lo))
        word = ((lo << 16) | hi)
        # print("%08x" % word)
        return word

    def hookbl(self, adr, handler, oldhandler=None):
        """Hooks a function by replacing a 32-bit relative BL."""

        # print("Redirecting a bl at %08x to %08x." % (adr, handler))

        # TODO This is sometimes tricked by old data.
        # Fix it by ensuring no old data.
        # if oldhandler!=None:
        #    #Verify the old handler.
        #    if self.calcbl(adr,oldhandler)!=self.getword(adr):
        #        print("The old handler looks wrong.")
        #        print("Damn, we're in a tight spot!")
        #        sys.exit(1);

        self.setword(adr,
                     self.calcbl(adr, handler))


if __name__ == '__main__':
    print("Merging an applet.")
    if len(sys.argv) != 4:
        print("Usage: python merge.py firmware.img patch.img offset")
        sys.exit(1)

    # Open the firmware image.
    merger = Merger(sys.argv[1])

    # Open the applet.
    fapplet = open(sys.argv[2], "rb")
    bapplet = bytearray(fapplet.read())
    index = int(sys.argv[3], 16)

    # Open the applet symbols
    sapplet = Symbols("%s.sym" % sys.argv[2])

    merger.hookstub(0x0809661e,  # USB manufacturer string handler function.
                    sapplet.getadr("getmfgstr"))
    merger.hookstub(0x080226d2,  # startup_botline
                    sapplet.getadr("splash_hook_handler"))
    
    merger.hookstub(0x08016a96,
                    sapplet.getadr("loadfirmwareversion_hook"))
    merger.hookbl(0x0808eb66,  # Call to usb_dfu_upload().
                  sapplet.getadr("usb_upld_hook"),
                  0x0808f308)  # Old handler adr.

    merger.hookbl(0x80408e0,  # Call to dmr_call_end()
                  sapplet.getadr("dmr_call_end_hook"))

    merger.hookstub2(0x800c72e, sapplet.getadr("create_menu_entry_rev"))

    dmr_call_start_hook_list = [0x804076a, 0x80408ca]
    for adr in dmr_call_start_hook_list:
        merger.hookbl(adr, sapplet.getadr("dmr_call_start_hook"))

    dmr_handle_data_hook_list = [0x804093e, 0x8040956, 0x804099c]
    for adr in dmr_handle_data_hook_list:
        merger.hookbl(adr, sapplet.getadr("dmr_handle_data_hook"))

    merger.hookbl(0x08040926, sapplet.getadr("dmr_sms_arrive_hook"))

    merger.hookbl(0x080408f6, sapplet.getadr("dmr_CSBK_handler_hook"))

    # os semaphore hook .. now we can crate own semaphores
    merger.hookbl(0x804647a, sapplet.getadr("OSSemCreate_hook"), 0)

    merger.hookbl(0x0801d956, sapplet.getadr("display_init"))

    #########
    # gfx_ primitives hooks, to be overriden later in this file.

    gfxdrawcharpos = [
        0x0800cd36,
        0x0800cd7e,
        0x0800ce30,
        0x0800ce78,
        0x0800cefc,
        0x0800cf48,
        0x0800d03e,
        0x0800d0bc,
        0x0800d120,
        0x0800d8a6,
        0x0802d72a,
        0x0802d736,
        0x0802d748,
        0x0802da00,
        0x0802da0c,
        0x0802da1e,
    ]
    for adr in gfxdrawcharpos:
        merger.hookbl(adr, sapplet.getadr("gfx_drawchar_pos_hook"))

    drwbmplist = [
        0x0800cdd0,
        0x08020428,
        0x0802136a,  # antenna symbol signal strength
        0x08021378,  # antenna symbol signal strength
        0x080217a8,  # antenna symbol signal strength
        0x080217b4,
        0x08025cfa,
        0x08025d48,
        0x08025d54,
        0x08025dbe,
        0x08025e0e,
        0x08025e26,
        0x08028782,
        0x080287ac,
        0x080287d6,
        0x080287f4,
        0x080288be,
        0x08028900,
        0x08028924,
        0x08028970,
        0x08028984,
        0x080289e4,
        0x080289f8,
        0x08028a30,
        0x08028a6e,
        0x08028abe,
        0x08028afe,
        0x08028b58,
        0x0802b900,
        0x0802b99e,
        0x0802ba82,
        0x0802bbe6,
        0x0802bc7a,
        0x0802bd92,
        0x0802be74,
        0x0802befc,
        0x0802c050,
        0x0802c05c,
        0x0802c17e,
        0x0802c20a,
        0x0802c2fa,
        0x0802c35e,
        0x0802c3da,
        0x0802cb08,
        0x0802d67a,
        0x0802d938,
        0x0802d954,
        0x0802da68,
        0x08031a78,
        0x08031aaa,
        0x08033e1e,  # antenna symbol signal strength
        0x08033e6e,
        0x080467e4,
        0x0804bf1a,
    ]
    for adr in drwbmplist:
        merger.hookbl(adr, sapplet.getadr("gfx_drawbmp_hook"))

    gfxblockfill = [
        0x0800c85e,
        0x0800c86a,
        0x0800c876,
        0x0800c882,
        0x0800c896,
        0x0800c8a2,
        0x0800c8ae,
        0x0800c8ba,
        0x0800c8c6,
        0x0800c8e0,
        0x0800c906,
        0x0800c912,
        0x0800c926,
        0x0800ca0a,
        0x0800ca16,
        0x0800ca28,
        0x0800ca46,
        0x0800ca52,
        0x0800ca66,
        0x0800cafa,
        0x0800cb06,
        0x0800cb12,
        0x0800cb84,
        0x0800cb90,
        0x0800cb9c,
        0x0800cba8,
        0x0800cbb4,
        0x0800cbc8,
        0x0800cbd4,
        0x0800cbe8,
        0x0800cbf4,
        0x0800cc08,
        0x0800cc1c,
        0x0800cc28,
        0x0800cc9a,
        0x0800d028,
        0x0800d056,
        0x0800d166,
        0x0800d172,
        0x0800d1b2,
        0x0800d2da,
        0x0800d2e6,
        0x0800d2f2,
        0x0800d2fe,
        0x0800d3fc,
        0x0800d408,
        0x0800d414,
        0x0800d420,
        0x0800d4cc,
        0x0800d54c,
        0x0800d558,
        0x0800d564,
        0x0800d570,
        0x0800d762,
        0x0800d77c,
        0x0800d788,
        0x0800d832,
        0x0800d91c,
        0x0800d928,
        0x0800d934,
        0x0800dc06,
        0x0800de8c,
        0x0800dea0,
        0x0800deac,
        0x0800deb8,
        0x0800dec4,
        0x0800e064,
        0x0800e072,
        0x0800e07e,
        0x0800e08a,
        0x0800e0a0,
        0x0800e19a,
        0x0800e1c0,
        0x0800e338,
        0x0800e34c,
        0x0800e358,
        0x0800e364,
        0x0800e370,
        0x0800e6de,
        0x0800e91a,
        0x0800e934,
        0x0800e976,
        0x0800e982,
        0x0800e98e,
        0x0800e9a0,
        0x0800e9b4,
        0x0800e9c0,
        0x0800ea96,
        0x0800eaba,
        0x0800ead4,
        0x0800eaf6,
        0x0800eb10,
        0x0800ec18,
        0x0801e754,
        0x0801e762,
        0x0801e772,
        0x0801e808,
        0x0801e818,
        0x080200ce,
        0x08020578,
        0x08020752,
        0x08020c7e,
        0x08020c8a,
        0x08020c96,
        0x08020ca2,
        0x08020cae,
        0x08020cba,
        0x0802104c,
        0x0802113e,
        0x080211a6,
        0x080211b2,
        0x080211be,
        0x080211ca,
        0x080211d6,
        0x080211e2,
        0x0802124e,
        0x08025b48,
        0x08025b6e,
        0x08025c10,
        0x08025ca4,
        0x08025cbe,
        0x08025cca,
        0x08025cd6,
        0x08025df2,
        0x08025e42,
        0x08025e4e,
        0x0802bd0c,
        0x0802be5e,
        0x0802c194,
        0x0802c266,
        0x0802c352,
        0x0802c6bc,
        0x0802c738,
        0x0802cfda,
        0x0802cfec,
        0x0802cff8,
        0x0802d00c,
        0x0802d018,
        0x0802d5fc,
        0x0802d61c,
        0x0802d628,
        0x0802d634,
        0x0802d68e,
        0x0802d69a,
        0x0802d6a6,
        0x0802d6b2,
        0x0802d6be,
        0x0802d6ca,
        0x0802d75c,
        0x0802d768,
        0x0802d780,
        0x0802d794,
        0x0802d7a0,
        0x0802d7ac,
        0x0802d8a6,
        0x0802d8b2,
        0x0802d8be,
        0x0802d904,
        0x0802d96a,
        0x0802d992,
        0x0802d99e,
        0x0802da32,
        0x0802da3e,
        0x0802da56,
        0x0802da84,
        0x0802da90,
        0x0802da9c,
        0x0802daa8,
        0x0802dab4,
        0x0802dac0,
        0x0802e184,
        0x0802e190,
        0x0802e26e,
        0x0802e27a,
        0x08033e92,
        0x080370b2,
        0x080370ce,
    ]
    for adr in gfxblockfill:
        merger.hookbl(adr, sapplet.getadr("gfx_blockfill_hook"))

    dt2list = [
        0x0800c8f2,
        0x0800cad2,
        0x0800cd04,
        0x0800cd20,
        0x0800cd68,
        0x0800cdfe,
        0x0800ce1a,
        0x0800ce62,
        0x0800ceca,
        0x0800cee6,
        0x0800cf32,
        0x0800d06e,
        0x0800d080,
        0x0800d0ea,
        0x0800d150,
        0x0800d19e,
        0x0800d1d6,
        0x0800d1f2,
        0x0800d242,
        0x0800d708,
        0x0800d7b6,
        0x0800d906,
        0x0800d958,
        0x0800d974,
        #        0x0800df92,
        0x0800dff6,
        0x0800e174,
        0x0800e18e,
        0x0800e1b4,
        0x0800e1da,
        0x0800ea82,
        0x0801ea2e,
        0x0801f02c,
        0x0801f044,
        0x0801f07a,
        0x0801f092,
        0x0802d660,
        0x0802d70e,
        0x0802d8e2,
        0x0802d9e4,
    ]
    for adr in dt2list:
        merger.hookbl(adr, sapplet.getadr("gfx_drawtext2_hook"))

    dt4list = [
        0x0800e5ba,
        0x0800e604,
        0x0800e618,
        0x0800e634,
        0x0800e696,
        0x0802d6f2,
        0x0802d9c8,
    ]
    for adr in dt4list:
        merger.hookbl(adr, sapplet.getadr("gfx_drawtext4_hook"))

    # gfx_ primitives hooks, to be overriden later in this file.
    #########

    #########
    # status line

    # date format  hook, this hook can modify the date format on the status line
    merger.hookbl(0x0800df92, sapplet.getadr("print_date_hook"), 0)

    merger.hookbl(0x08021782, sapplet.getadr("draw_statusline_hook"))

    draw_datetime_row_list = [
        0x08020c26,
        0x08020c6e,
        0x08021196,
        0x080212c6,
        0x0802d76c,
        0x0802da42,
    ]
    for adr in draw_datetime_row_list:
        merger.hookbl(adr, sapplet.getadr("draw_datetime_row_hook"))

    # rx popup overrides of gfx_drawbmp
    merger.hookbl(0x08025d54, sapplet.getadr("rx_screen_blue_hook"), 0)
    merger.hookbl(0x08025e26, sapplet.getadr("rx_screen_blue_hook"), 0)
    merger.hookbl(0x08020428, sapplet.getadr("rx_screen_gray_hook"), 0)

    # drawtext hooks

    merger.hookbl(0x08046804, sapplet.getadr("gfx_drawtext_hook"), 0)
    merger.hookbl(0x0804681a, sapplet.getadr("gfx_drawtext_hook"), 0)

    # f_4315
    merger.hookbl(0x080202d0, sapplet.getadr("f_4315_hook"))
    merger.hookbl(0x080202fc, sapplet.getadr("f_4315_hook"))

    # 0x800def7 gfx_drawtext
    #    merger.hookstub(0x800def6, sapplet.getadr("dummy"));

    # 0x0801dd2c gfx_drawtext5
    #    merger.hookstub2(0x0801dd2c, sapplet.getadr("dummy"));
    #    merger.hookstub2(0x0801dd2c, sapplet.getadr("gfx_drawtext5_hook"));

    # 0x08027728 gfx_drawtext6
    #    merger.hookstub2(0x08027728, sapplet.getadr("dummy"));

    # gfx_drawtext7
    #    merger.hookstub2(0x080277c2, sapplet.getadr("dummy"));

    # gfx_drawtext8 (used to print menu entries, main display)
    #    merger.hookstub2(0x08036fc0, sapplet.getadr("dummy"));
    #    merger.hookstub2(0x08036fc0, sapplet.getadr("gfx_drawtext8_hook"));

    # gfc_drawtext3
    #    merger.hookstub2(0x0802b142, sapplet.getadr("dummy"));

    # gfx_clear3 (clear to eol)
    #    merger.hookstub2(0x0801dcc0, sapplet.getadr("dummy"));

    # gfx_drawtext9 (used to print call popups, poweroff)
    #    merger.hookstub2(0x0802b0d4, sapplet.getadr("dummy"));

    gfxdt10 = [
        0x0800c956,
        0x0800c976,
        0x0800ca96,
        0x0800cc58,
        0x0800cc78,
        0x0800cd4c,
        0x0800cd94,
        0x0800ce46,
        0x0800ce8e,
        0x0800cf16,
        0x0800cf62,
        0x0800cffa,
        0x0800d48e,
        0x0800e254,
        0x0800e272,
        0x0800e962,
        0x0800e9f0,
        0x08020010,
        0x080200f4,
        0x0802077c,
        0x0802079a,
        0x080209c8,
        0x08020a1e,
        0x0802154a,
        0x08021588,
        0x080215b4,
        0x080215ee,
        0x08021628,
        0x08021638,
        0x08021672,
        0x08025c90,
        0x08025d28,
        0x08025d82,
        0x08025dde,
        0x0802c908,
        0x0802d046,
        0x0802d064,
        0x080461d2,
    ]
    for adr in gfxdt10:
        merger.hookbl(adr, sapplet.getadr("gfx_drawtext10_hook"))

        # keyboard polling
    #    merger.hookstub2(0x0804eb64, sapplet.getadr("dummy"));

    # intercept disp_something
    #    merger.hookstub2(0x0800d69c, sapplet.getadr("dummy"));

    # call to Create_MainMenyEntry
    #    merger.hookbl(0x080202cc, sapplet.getadr("dummy"),0);

    #    merger.hookstub(0x08025ae4, sapplet.getadr("mode17_hook"));

    #    merger.hookbl(0x08020314, sapplet.getadr("mode17_hook"),0);
    #    merger.hookbl(0x0802032a, sapplet.getadr("mode17_hook"),0);

    # Hook the startup AES check.
    merger.hookbl(0x0804764c, sapplet.getadr("aes_startup_check_hook"), 0)

    # Patch a single call in the wrapper function so catch all
    # aes_loadkey() calls.
    merger.hookbl(0x08036c32, sapplet.getadr("aes_loadkey_hook"), 0)

    # Function that calls aes_cipher() twice.  When are these called?
    # there a 3 calls on d02.32
    aes_cipher_hook_list = [0x802265a, 0x803fbf2]
    for adr in aes_cipher_hook_list:
        merger.hookbl(adr, sapplet.getadr("aes_cipher_hook"))

    # Hook lots of AMBE2+ encoder code and hope our places are correct.
    ambelist = [0x804a20a, 0x804a342, 0x804a39a, 0x804a4e8, 0x804a55e,
                0x804a6d8, 0x804a758, 0x804a8c4, 0x804a8fa
                ]
    for adr in ambelist:
        merger.hookbl(adr, sapplet.getadr("ambe_encode_thing_hook"))

    # Hook calls within the AMBE2+ decoder.
    unpacklist = [
        0x8034106, 0x8034112, 0x803412a, 0x8034136, 0x804aebe, 0x804b3ea, 0x804b43e,
        0x804b494
    ]
    for adr in unpacklist:
        merger.hookbl(adr, sapplet.getadr("ambe_unpack_hook"))

    # Hook calls that produce WAV audio.  (Maybe.)
    wavdeclist = [
        0x8049f98, 0x804ac0c, 0x804ad62, 0x804b074, 0x804b0b0, 0x804b170, 0x804b1ac
    ]
    for adr in wavdeclist:
        merger.hookbl(adr, sapplet.getadr("ambe_decode_wav_hook"))

    # Hooks the squelch routines, so we can do monitor mode in C.
    merger.hookbl(0x08040ce0, sapplet.getadr("dmr_apply_privsquelch_hook"), 0)  # Private calls.
    # ########  this function has been changed
    merger.hookbl(0x08040c1c, sapplet.getadr("dmr_apply_squelch_hook"), 0)  # Public calls.

    # additional menu hook
    merger.hookbl(0x080135a8, sapplet.getadr("create_menu_utilies_hook"), 0)

    # init the addl global config struct from spi flash
    merger.hookbl(0x08046326, sapplet.getadr("init_global_addl_config_hook"), 0)

    spiflashrd = [
        0x08011640,
        0x08013358,
        0x080226ba,
        0x080226cc,
        0x080226de,
        0x080226f0,
        0x08022720,
        0x0802295c,
        0x08022976,
        0x0802298c,
        0x08022a32,
        0x08022a54,
        0x08022a86,
        0x08022aa0,
        0x08022abc,
        0x08022b56,
        0x08022ce2,
        0x08022d88,
        0x08022daa,
        0x08022e2c,
        0x08022e44,
        0x08022e5e,
        0x08022e70,
        0x08022e9c,
        0x08022ec8,
        0x08022eec,
        0x08022f1e,
        0x08022f66,
        0x08022fb0,
        0x08022fe2,
        0x0802302e,
        0x080230aa,
        0x080230c4,
        0x080230dc,
        0x080230fc,
        0x0802312c,
        0x0802319a,
        0x080231d4,
        0x08023234,
        0x080232a6,
        0x080232f8,
        0x08023364,
        0x080233a8,
        0x08023426,
        0x08023478,
        0x080234e0,
        0x08023520,
        0x08023570,
        0x080235bc,
        0x0802363e,
        0x08023650,
        0x08023678,
        0x080236a0,
        0x08023a9a,
        0x08026048,
        0x0802607e,
        0x08026108,
        0x0802613e,
        0x0802f5e0,
        0x0802f69e,
        0x080315ca,
        0x08031628,
        0x08031678,
        0x080316c0,
        0x080316fc,
        0x08031780,
        0x080317f2,
        0x08039780,
        0x080398e0,
        0x08097aba,
    ]
    for adr in spiflashrd:
        merger.hookbl(adr, sapplet.getadr("spiflash_read_hook"))

        # no menu exit on RX hook
    #    merger.hookbl(0x0801fe7c,sapplet.getadr("f_4225_internel_hook"),0);

    # OSMboxPend Hook to intercept Beep_Process
    merger.hookbl(0x0802fa00, sapplet.getadr("beep_OSMboxPend_hook"))

    # other OSMboxPend hooks
    mbx_pend_list = [
        #       0x0802fa00,
        0x0803b8fa,
        0x0803c398,
        0x0803c806,
        0x08046be2,
        0x08046bfa,
    ]
    for adr in mbx_pend_list:
        merger.hookbl(adr, sapplet.getadr("OSMboxPend_hook"))

    # hooks regarding the beep_process
    beep_process_list = [
        0x0802fc2e, 0x0802fc40,  # beep "9"
        0x0802fbf4, 0x0802fc06,  # roger beep
        0x0802fd6c, 0x0802fd7e, 0x0802fd8c, 0x0802fd9a  # dmr sync
    ]
    for adr in beep_process_list:
        merger.hookbl(adr, sapplet.getadr("F_294_replacement"), 0)

    merger.hookbl(0x080468e6, sapplet.getadr("f_4225_hook"), 0)
    merger.hookbl(0x0802db42, sapplet.getadr("f_4225_hook"), 0)

    # keyboard
    merger.hookbl(0x0804ebd2, sapplet.getadr("kb_handler_hook"));

#    for adr in drwbmplist:
#        merger.hookbl(adr, sapplet.getadr("dummy"));

############ i2c hooks
###    I2C_GenerateSTART_hook_list=[
###    0x8046a3a, 0x8046ac6, 0x8046b6c];
###    for adr in I2C_GenerateSTART_hook_list:
###        merger.hookbl(adr,sapplet.getadr("I2C_GenerateSTART_hook"),0);
###        
###    I2C_GenerateSTOP_hook_list=[
###    0x8046b50, 0x8046c3c];
###    for adr in I2C_GenerateSTOP_hook_list:
###        merger.hookbl(adr,sapplet.getadr("I2C_GenerateSTOP_hook"),0);
###    
###    I2C_ReceiveData_hook_list=[
###    0x8046b1c];
###    for adr in I2C_ReceiveData_hook_list:
###        merger.hookbl(adr,sapplet.getadr("I2C_ReceiveData_hook"),0);
###        
###    I2C_Send7bitAddress_hook_list=[
###    0x8046a66, 0x8046af2, 0x8046b98];
###    for adr in I2C_Send7bitAddress_hook_list:    
###        merger.hookbl(adr,sapplet.getadr("I2C_Send7bitAddress_hook"),0);
###        
###    I2C_SendData_hook_list=[
###    0x8046a90, 0x8046bc2, 0x8046c06];
###    for adr in I2C_SendData_hook_list:
###        merger.hookbl(adr,sapplet.getadr("I2C_SendData_hook"),0);
###
#############  Debug and training hooks
###    OSTaskCreateExt_hook_list=[
###        0x8042368, 0x8044028, 0x80442c4, 0x80442f8, 0x804432c, 0x8044360, 0x8044394, 0x80443c8,
###        0x80443fc, 0x8044430, 0x8044464, 0x8044498, 0x80444cc, 0x8044500, 0x8044534, 0x8049150,
###        0x804ae5c];
###    ### only for debug and information addiction
####    for adr in OSTaskCreateExt_hook_list:
####        merger.hookbl(adr, sapplet.getadr("OSTaskCreateExt_hook"),0);
###
###    OSTaskNameSet_hook_list=[
###        0x8042374, 0x8044034, 0x80442d0, 0x8044304, 0x8044338, 0x804436c, 0x80443a0, 0x80443d4,
###        0x8044408, 0x804443c, 0x8044470, 0x80444a4, 0x80444d8, 0x804450c, 0x8044540, 0x804915c,
###        0x804ae68];
###    ### only for debug and information addiction
### #   for adr in OSTaskNameSet_hook_list:
### #       merger.hookbl(adr, sapplet.getadr("OSTaskNameSet_hook"),0);
###
###    Create_MenuEntrylist=[
###        0x0800c278, 0x0800c2c0, 0x0800c2f4, 0x0800c326, 0x0800c358, 0x0800c38a, 0x0800c3bc, 0x0800c468,
###        0x0800c4a8, 0x0800c4d2, 0x0800c4fa, 0x0800c522, 0x0800c54a, 0x0800c572, 0x0800c5ec, 0x0800c614,
###        0x0800c64a, 0x0800c674, 0x0800c69c, 0x0800c6c4, 0x0800c6ec, 0x080191f2, 0x08019226, 0x0801925a,
###        0x0801928a, 0x0802d264, 0x0802d2ca, 0x0802d326, 0x0802d356, 0x080197d8, 0x0801980a, 0x08019a08,
###        0x08019a3a, 0x08019a6c, 0x08019a9e, 0x08019e78, 0x08019ea6, 0x0801a070, 0x0801a0a2, 0x0802c060,
###        0x0802c096, 0x0802c0cc, 0x0802c102, 0x0802c138, 0x08018f70, 0x08018fa2, 0x08012872, 0x080128a4,
###        0x080128d6, 0x08012908, 0x08012948, 0x0801297c, 0x080129bc, 0x080129ee, 0x08012a22, 0x08012a54,
###        0x08012b3a, 0x0802c1d4, 0x0801535a, 0x0801538c, 0x0801674e, 0x08016782, 0x080167ca, 0x0801681c,
###        0x08016866, 0x080168ba, 0x08016904, 0x0801694e, 0x08016986, 0x080169d0, 0x08016a20, 0x08016a4a,
###        0x08016a90, 0x08016ac4, 0x08016afc, 0x08016b32, 0x08016b7c, 0x08038c66, 0x08038cd4, 0x08038d38,
###        0x08038d6c, 0x080194e0, 0x0801950e, 0x080179d6, 0x08017a08, 0x08017ff2, 0x08018022, 0x08018052,
###        0x08018080, 0x080180ae, 0x080180dc, 0x08016e2a, 0x0802c558, 0x0801a158, 0x0801a18a, 0x0801a1bc,
###        0x0801a1ee, 0x0801270e, 0x08012782, 0x0802c610, 0x0802c63c, 0x0801273c, 0x0802d1ce, 0x0802ce84,
###        0x0802c8aa, 0x0802c8d6, 0x0802cc28, 0x0802c952, 0x0802ca62, 0x0802c6ca, 0x0802c6f6, 0x0802c76a,
###        0x0802c798, 0x0802c7c6, 0x0802fa46, 0x0801b130, 0x0800f50c, 0x0800f53e, 0x0800f588, 0x08019d54,
###        0x0801b240, 0x0800e01e, 0x0800e040, 0x080205cc, 0x0802dfe0, 0x0802e014, 0x0802e048, 0x080346da,
###        0x08034704, 0x08012c44, 0x08012c76, 0x08012ca8, 0x08012cda, 0x08012d1a, 0x08012d4e, 0x08012d8e,
###        0x08012dc0, 0x08012df4, 0x08012e26, 0x08012ea4, 0x080154bc, 0x08016c4a, 0x08016c7e, 0x08016d22,
###        0x08016dac, 0x08016fe4, 0x08017b30, 0x08017b60, 0x08017df0, 0x08017e22, 0x08018186, 0x080181b2,
###        0x0801829c, 0x080182cc, 0x08018320, 0x08018350, 0x08018402, 0x08018434, 0x08018466, 0x08018498,
###        0x08018562, 0x08018594, 0x0801865a, 0x0801868c, 0x0801871a, 0x080187de, 0x08018966, 0x08018b94,
###        0x08018c24, 0x08018ce0, 0x08018d10, 0x08018da4, 0x08018e68, 0x08019038, 0x080190d8, 0x08019318,
###        0x080193a2, 0x0801942a, 0x080195ba, 0x08019688, 0x08019714, 0x08019b30, 0x08019bc4, 0x08019c4c,
###        0x08019cce, 0x08019f2a, 0x08019fae, 0x0801a2d0, 0x0801a360, 0x0801a3e0, 0x0801a458, 0x0801a4cc,
###        0x0801a624, 0x0801a652, 0x0801a682, 0x0801a6fe, 0x0801a7b8, 0x0801a85e, 0x0801a91c, 0x0801a9f2,
###        0x0801aa22, 0x0801aa54, 0x0801abb2, 0x0801ac1c, 0x0801ac4e, 0x0801af14, 0x0801b07a, 0x0801b1c0,
###        0x0801b2ca, 0x0801b2f2, 0x0801b364, 0x0801ad50, 0x0801ad82, 0x0801adec, 0x0801ae1e, 0x0801245c,
###        0x080198a2, 0x0801992c];
###    ### only for debug and information addiction
###    #for adr in Create_MenuEntrylist:
###    #    merger.hookbl(adr,sapplet.getadr("create_menu_entry_hook"),0);
###

    ### only for debug and information addiction
    OSMboxPost_hook_list=[
        0x08011140 ,
        0x080111ec ,
        0x08012994 ,
        0x08012b0a ,
        0x08012c7a ,
        0x08012e0a ,
        0x08012f60 ,
        0x080179a4 ,
        0x08017a3c ,
        0x08017ad8 ,
        0x08017b78 ,
        0x08017c18 ,
        0x0801ff36 ,
        0x0801ff86 ,
        0x08021434 ,
        0x080246a4 ,
        0x080257fa ,
        0x0802c966 ,
        0x0802c9c0 ,
        0x0802d38a ,
        0x0802d422 ,
        0x0802d4a6 ,
        0x0802d5d4 ,
        0x0802db3a ,
        0x0802dc84 ,
        0x0802dcc8 ,
        0x0802dda2 ,
        0x0802ddd6 ,
        0x0802dee8 ,
        0x0802e05e ,
        0x0802e2ce ,
        0x0802e41e ,
        0x0802e46c ,
        0x0802e4de ,
        0x0802e51a ,
        0x0802e6c6 ,
        0x0802e76c ,
        0x0802e79c ,
        0x0802e822 ,
        0x0802e896 ,
        0x0802e8aa ,
        0x0802e946 ,
        0x0802e976 ,
        0x0802e9b4 ,
        0x0802e9e6 ,
        0x0802ea3c ,
        0x0802ea6e ,
        0x0802eb0a ,
        0x0802eba0 ,
        0x0802ec18 ,
        0x0802ec3a ,
        0x0802ec5c ,
        0x0802ec74 ,
        0x0802ecf2 ,
        0x0802ed42 ,
        0x0802ed94 ,
        0x0802edc4 ,
        0x0802ee30 ,
        0x0802ee62 ,
        0x0802eee4 ,
        0x0802ef08 ,
        0x0802ef46 ,
        0x0802efa6 ,
        0x0802f104 ,
        0x0802f350 ,
        0x0803207e ,
        0x080321be ,
        0x08032264 ,
        0x08032342 ,
        0x080325c0 ,
        0x0803261a ,
        0x0803bb96 ,
        0x0803bbb6 ,
        0x0803bc02 ,
        0x0803bc7e ,
        0x0803bde2 ,
        0x0803bee4 ,
        0x0803c038 ,
        0x0803c0f6 ,
        0x0803c1c8 ,
        0x0803c22e ,
        0x0803c2a6 ,
        0x0803c2be ,
        0x0803c38a ,
        0x0803c482 ,
        0x0803c4bc ,
        0x0803c4d8 ,
        0x0803c4ec ,
        0x0803c512 ,
        0x0803c5e0 ,
        0x0803c612 ,
        0x0803c6a8 ,
        0x0803c6da ,
        0x0803c6f0 ,
        0x0803c74e ,
        0x0803c7f8 ,
        0x0803c94e ,
        0x0803c9f4 ,
        0x0803ca8a ,
        0x0803cac0 ,
        0x0803cd42 ,
        0x0803cd84 ,
        0x0803ce18 ,
        0x0803cef0 ,
        0x0803cf5c ,
        0x0803cf7c ,
        0x0803d0f6 ,
        0x0803d1e2 ,
        0x0803d20a ,
        0x0803d2a4 ,
        0x0803d2cc ,
        0x0803d35a ,
        0x0803d378 ,
        0x0803d3a6 ,
        0x0803d3c4 ,
        0x0803d41e ,
        0x0803d486 ,
        0x0803d4ba ,
        0x0803d4f2 ,
        0x0803d526 ,
        0x0803d574 ,
        0x0803d5b4 ,
        0x0803dbd8 ,
        0x0803df50 ,
        0x0803e1ba ,
        0x0803e28e ,
        0x0803f958 ,
        0x0803fa70 ,
        0x0803ff2e ,
        0x08040824 ,
        0x0804090c ,
        0x0804096e ,
        0x08040984 ,
#        0x08040c1c , # hooked for promisc
#        0x08040ce0 , # hooked for promisc
        0x08040d9c ,
        0x08040f5a ,
        0x0804105a ,
        0x080410a8 ,
        0x080410b8 ,
        0x08041280 ,
        0x080412f0 ,
        0x08041352 ,
        0x0804136e ,
        0x080413de ,
        0x080414f0 ,
        0x080415b4 ,
        0x08041628 ,
        0x0804178a ,
        0x080418ca ,
        0x080418ee ,
        0x08041930 ,
        0x08041958 ,
        0x08041984 ,
        0x080419c2 ,
        0x080419e2 ,
        0x08041a0e ,
        0x08041e56 ,
        0x08041eb2 ,
        0x0804249a ,
        0x08042534 ,
        0x080428da ,
        0x08042af2 ,
        0x08042e84 ,
        0x08042ee0 ,
        0x080433c8 ,
        0x080433e8 ,
        0x0804347a ,
        0x080434ce ,
        0x0804580c ,
        0x0804636a ,
        0x08046942 ,
        0x08046a66 ,
        0x08046a94 ,
        0x08046ab6 ,
        0x0804de68 ,
        0x0804df18 ,
        0x0808edae ,
        0x0808edc4 ,
        0x0808ee0a ,
        0x0808ee3c ,
        0x0808eeea ,
        ];
#    for adr in OSMboxPost_hook_list:
#        merger.hookbl(adr,sapplet.getadr("OSMboxPost_hook"),0);

###    #Throwaway hook to see if adr is called.
###    #merger.hookstub(0x0803f03c,
###    #                sapplet.getadr("demo"));
###
###    f_4137_hook_list=[
###        0x8027fe2, 0x8028288, 0x8028298, 0x80282f0];
###
####    for adr in f_4137_hook_list:
####        merger.hookbl(adr,sapplet.getadr("f_4137_hook"),0);
####    merger.hookbl(0x804464a,sapplet.getadr("f_4520_hook"),0);
####    merger.hookbl(0x8044642,sapplet.getadr("f_4098_hook"),0);
####    merger.hookbl(0x804c1e8,sapplet.getadr("f_4102_hook"),0);
###
###    # display hooks is in d13.020 included
###    ## display flip workaround see issue #178 not necessary on 0X3.020
###    merger.hookbl(0x08031fde,sapplet.getadr("display_init_hook_1"),0);
###    merger.hookbl(0x0803200e,sapplet.getadr("display_init_hook_2"),0);

    
    # DL4YHF : We don't know here if the PWM'ed backlight, and thus
    #  SysTick_Handler() shall be included (depends on config.h) .
    # IF   the applet's symbol table contains a function named 'SysTick_Handler',
    # THEN patch its address, MADE ODD to indicate Thumb-code, into the
    # interrupt-vector-table as explained in applet/src/irq_handlers.c :
    # ex: new_adr = sapplet.getadr("SysTick_Handler"); # threw an exception when "not found" :(
    new_adr = sapplet.try_getadr("SysTick_Handler");
    if new_adr != None:
        vect_adr = 0x800C03C;  # address inside the VT for SysTick_Handler
        exp_adr  = 0x8093F1D;  # expected 'old' content of the above VT entry
        old_adr  = merger.getword(vect_adr); # original content of the VT entry
        new_adr |= 0x0000001;  # Thumb flag for new content in the VT
        if( old_adr == exp_adr ) :
           print("Patching SysTick_Handler in VT addr 0x%08x," % vect_adr)
           print("  old value in vector table = 0x%08x," % old_adr)
           print("   expected in vector table = 0x%08x," % exp_adr)
           print("  new value in vector table = 0x%08x." % new_adr)
           merger.setword( vect_adr, new_adr, old_adr);
           print("  SysTick_Handler successfully patched.")
        else:
           print("Cannot patch SysTick_Handler() !")
    else:
           print("No SysTick_Handler() found in the symbol table. Building firmware without.")

    print("Merging %s into %s at %08x" % (
          sys.argv[2],
          sys.argv[1],
          index));

    #Change TIM12 IRQ Handler to new one
    merger.setword(0x0800c0ec, sapplet.getadr("New_TIM12_IRQHandler")+1);
    
    #    for adr in drwbmplist:
    #        merger.hookbl(adr, sapplet.getadr("dummy"));

    # ######### i2c hooks
    #    I2C_GenerateSTART_hook_list=[
    #    0x8046a3a, 0x8046ac6, 0x8046b6c];
    #    for adr in I2C_GenerateSTART_hook_list:
    #        merger.hookbl(adr,sapplet.getadr("I2C_GenerateSTART_hook"),0);
    #
    #    I2C_GenerateSTOP_hook_list=[
    #    0x8046b50, 0x8046c3c];
    #    for adr in I2C_GenerateSTOP_hook_list:
    #        merger.hookbl(adr,sapplet.getadr("I2C_GenerateSTOP_hook"),0);
    #
    #    I2C_ReceiveData_hook_list=[
    #    0x8046b1c];
    #    for adr in I2C_ReceiveData_hook_list:
    #        merger.hookbl(adr,sapplet.getadr("I2C_ReceiveData_hook"),0);
    #
    #    I2C_Send7bitAddress_hook_list=[
    #    0x8046a66, 0x8046af2, 0x8046b98];
    #    for adr in I2C_Send7bitAddress_hook_list:
    #        merger.hookbl(adr,sapplet.getadr("I2C_Send7bitAddress_hook"),0);
    #
    #    I2C_SendData_hook_list=[
    #    0x8046a90, 0x8046bc2, 0x8046c06];
    #    for adr in I2C_SendData_hook_list:
    #        merger.hookbl(adr,sapplet.getadr("I2C_SendData_hook"),0);
    #
    # ##########  Debug and training hooks
    #    OSTaskCreateExt_hook_list=[
    #        0x8042368, 0x8044028, 0x80442c4, 0x80442f8, 0x804432c, 0x8044360, 0x8044394, 0x80443c8,
    #        0x80443fc, 0x8044430, 0x8044464, 0x8044498, 0x80444cc, 0x8044500, 0x8044534, 0x8049150,
    #        0x804ae5c];
    #    ### only for debug and information addiction
    # #    for adr in OSTaskCreateExt_hook_list:
    # #        merger.hookbl(adr, sapplet.getadr("OSTaskCreateExt_hook"),0);
    #
    #    OSTaskNameSet_hook_list=[
    #        0x8042374, 0x8044034, 0x80442d0, 0x8044304, 0x8044338, 0x804436c, 0x80443a0, 0x80443d4,
    #        0x8044408, 0x804443c, 0x8044470, 0x80444a4, 0x80444d8, 0x804450c, 0x8044540, 0x804915c,
    #        0x804ae68];
    #    ### only for debug and information addiction
    # #   for adr in OSTaskNameSet_hook_list:
    # #       merger.hookbl(adr, sapplet.getadr("OSTaskNameSet_hook"),0);
    #
    #    Create_MenuEntrylist=[
    #        0x0800c278, 0x0800c2c0, 0x0800c2f4, 0x0800c326, 0x0800c358, 0x0800c38a, 0x0800c3bc, 0x0800c468,
    #        0x0800c4a8, 0x0800c4d2, 0x0800c4fa, 0x0800c522, 0x0800c54a, 0x0800c572, 0x0800c5ec, 0x0800c614,
    #        0x0800c64a, 0x0800c674, 0x0800c69c, 0x0800c6c4, 0x0800c6ec, 0x080191f2, 0x08019226, 0x0801925a,
    #        0x0801928a, 0x0802d264, 0x0802d2ca, 0x0802d326, 0x0802d356, 0x080197d8, 0x0801980a, 0x08019a08,
    #        0x08019a3a, 0x08019a6c, 0x08019a9e, 0x08019e78, 0x08019ea6, 0x0801a070, 0x0801a0a2, 0x0802c060,
    #        0x0802c096, 0x0802c0cc, 0x0802c102, 0x0802c138, 0x08018f70, 0x08018fa2, 0x08012872, 0x080128a4,
    #        0x080128d6, 0x08012908, 0x08012948, 0x0801297c, 0x080129bc, 0x080129ee, 0x08012a22, 0x08012a54,
    #        0x08012b3a, 0x0802c1d4, 0x0801535a, 0x0801538c, 0x0801674e, 0x08016782, 0x080167ca, 0x0801681c,
    #        0x08016866, 0x080168ba, 0x08016904, 0x0801694e, 0x08016986, 0x080169d0, 0x08016a20, 0x08016a4a,
    #        0x08016a90, 0x08016ac4, 0x08016afc, 0x08016b32, 0x08016b7c, 0x08038c66, 0x08038cd4, 0x08038d38,
    #        0x08038d6c, 0x080194e0, 0x0801950e, 0x080179d6, 0x08017a08, 0x08017ff2, 0x08018022, 0x08018052,
    #        0x08018080, 0x080180ae, 0x080180dc, 0x08016e2a, 0x0802c558, 0x0801a158, 0x0801a18a, 0x0801a1bc,
    #        0x0801a1ee, 0x0801270e, 0x08012782, 0x0802c610, 0x0802c63c, 0x0801273c, 0x0802d1ce, 0x0802ce84,
    #        0x0802c8aa, 0x0802c8d6, 0x0802cc28, 0x0802c952, 0x0802ca62, 0x0802c6ca, 0x0802c6f6, 0x0802c76a,
    #        0x0802c798, 0x0802c7c6, 0x0802fa46, 0x0801b130, 0x0800f50c, 0x0800f53e, 0x0800f588, 0x08019d54,
    #        0x0801b240, 0x0800e01e, 0x0800e040, 0x080205cc, 0x0802dfe0, 0x0802e014, 0x0802e048, 0x080346da,
    #        0x08034704, 0x08012c44, 0x08012c76, 0x08012ca8, 0x08012cda, 0x08012d1a, 0x08012d4e, 0x08012d8e,
    #        0x08012dc0, 0x08012df4, 0x08012e26, 0x08012ea4, 0x080154bc, 0x08016c4a, 0x08016c7e, 0x08016d22,
    #        0x08016dac, 0x08016fe4, 0x08017b30, 0x08017b60, 0x08017df0, 0x08017e22, 0x08018186, 0x080181b2,
    #        0x0801829c, 0x080182cc, 0x08018320, 0x08018350, 0x08018402, 0x08018434, 0x08018466, 0x08018498,
    #        0x08018562, 0x08018594, 0x0801865a, 0x0801868c, 0x0801871a, 0x080187de, 0x08018966, 0x08018b94,
    #        0x08018c24, 0x08018ce0, 0x08018d10, 0x08018da4, 0x08018e68, 0x08019038, 0x080190d8, 0x08019318,
    #        0x080193a2, 0x0801942a, 0x080195ba, 0x08019688, 0x08019714, 0x08019b30, 0x08019bc4, 0x08019c4c,
    #        0x08019cce, 0x08019f2a, 0x08019fae, 0x0801a2d0, 0x0801a360, 0x0801a3e0, 0x0801a458, 0x0801a4cc,
    #        0x0801a624, 0x0801a652, 0x0801a682, 0x0801a6fe, 0x0801a7b8, 0x0801a85e, 0x0801a91c, 0x0801a9f2,
    #        0x0801aa22, 0x0801aa54, 0x0801abb2, 0x0801ac1c, 0x0801ac4e, 0x0801af14, 0x0801b07a, 0x0801b1c0,
    #        0x0801b2ca, 0x0801b2f2, 0x0801b364, 0x0801ad50, 0x0801ad82, 0x0801adec, 0x0801ae1e, 0x0801245c,
    #        0x080198a2, 0x0801992c];
    #    ### only for debug and information addiction
    #    #for adr in Create_MenuEntrylist:
    #    #    merger.hookbl(adr,sapplet.getadr("create_menu_entry_hook"),0);

    # ## only for debug and information addiction
    OSMboxPost_hook_list = [
        0x08011140,
        0x080111ec,
        0x08012994,
        0x08012b0a,
        0x08012c7a,
        0x08012e0a,
        0x08012f60,
        0x080179a4,
        0x08017a3c,
        0x08017ad8,
        0x08017b78,
        0x08017c18,
        0x0801ff36,
        0x0801ff86,
        0x08021434,
        0x080246a4,
        0x080257fa,
        0x0802c966,
        0x0802c9c0,
        0x0802d38a,
        0x0802d422,
        0x0802d4a6,
        0x0802d5d4,
        0x0802db3a,
        0x0802dc84,
        0x0802dcc8,
        0x0802dda2,
        0x0802ddd6,
        0x0802dee8,
        0x0802e05e,
        0x0802e2ce,
        0x0802e41e,
        0x0802e46c,
        0x0802e4de,
        0x0802e51a,
        0x0802e6c6,
        0x0802e76c,
        0x0802e79c,
        0x0802e822,
        0x0802e896,
        0x0802e8aa,
        0x0802e946,
        0x0802e976,
        0x0802e9b4,
        0x0802e9e6,
        0x0802ea3c,
        0x0802ea6e,
        0x0802eb0a,
        0x0802eba0,
        0x0802ec18,
        0x0802ec3a,
        0x0802ec5c,
        0x0802ec74,
        0x0802ecf2,
        0x0802ed42,
        0x0802ed94,
        0x0802edc4,
        0x0802ee30,
        0x0802ee62,
        0x0802eee4,
        0x0802ef08,
        0x0802ef46,
        0x0802efa6,
        0x0802f104,
        0x0802f350,
        0x0803207e,
        0x080321be,
        0x08032264,
        0x08032342,
        0x080325c0,
        0x0803261a,
        0x0803bb96,
        0x0803bbb6,
        0x0803bc02,
        0x0803bc7e,
        0x0803bde2,
        0x0803bee4,
        0x0803c038,
        0x0803c0f6,
        0x0803c1c8,
        0x0803c22e,
        0x0803c2a6,
        0x0803c2be,
        0x0803c38a,
        0x0803c482,
        0x0803c4bc,
        0x0803c4d8,
        0x0803c4ec,
        0x0803c512,
        0x0803c5e0,
        0x0803c612,
        0x0803c6a8,
        0x0803c6da,
        0x0803c6f0,
        0x0803c74e,
        0x0803c7f8,
        0x0803c94e,
        0x0803c9f4,
        0x0803ca8a,
        0x0803cac0,
        0x0803cd42,
        0x0803cd84,
        0x0803ce18,
        0x0803cef0,
        0x0803cf5c,
        0x0803cf7c,
        0x0803d0f6,
        0x0803d1e2,
        0x0803d20a,
        0x0803d2a4,
        0x0803d2cc,
        0x0803d35a,
        0x0803d378,
        0x0803d3a6,
        0x0803d3c4,
        0x0803d41e,
        0x0803d486,
        0x0803d4ba,
        0x0803d4f2,
        0x0803d526,
        0x0803d574,
        0x0803d5b4,
        0x0803dbd8,
        0x0803df50,
        0x0803e1ba,
        0x0803e28e,
        0x0803f958,
        0x0803fa70,
        0x0803ff2e,
        0x08040824,
        0x0804090c,
        0x0804096e,
        0x08040984,
        #        0x08040c1c , # hooked for promisc
        #        0x08040ce0 , # hooked for promisc
        0x08040d9c,
        0x08040f5a,
        0x0804105a,
        0x080410a8,
        0x080410b8,
        0x08041280,
        0x080412f0,
        0x08041352,
        0x0804136e,
        0x080413de,
        0x080414f0,
        0x080415b4,
        0x08041628,
        0x0804178a,
        0x080418ca,
        0x080418ee,
        0x08041930,
        0x08041958,
        0x08041984,
        0x080419c2,
        0x080419e2,
        0x08041a0e,
        0x08041e56,
        0x08041eb2,
        0x0804249a,
        0x08042534,
        0x080428da,
        0x08042af2,
        0x08042e84,
        0x08042ee0,
        0x080433c8,
        0x080433e8,
        0x0804347a,
        0x080434ce,
        0x0804580c,
        0x0804636a,
        0x08046942,
        0x08046a66,
        0x08046a94,
        0x08046ab6,
        0x0804de68,
        0x0804df18,
        0x0808edae,
        0x0808edc4,
        0x0808ee0a,
        0x0808ee3c,
        0x0808eeea,
    ]
    #    for adr in OSMboxPost_hook_list:
    #        merger.hookbl(adr,sapplet.getadr("OSMboxPost_hook"),0);

    #    #Throwaway hook to see if adr is called.
    #    #merger.hookstub(0x0803f03c,
    #    #                sapplet.getadr("demo"));
    #
    #    f_4137_hook_list=[
    #        0x8027fe2, 0x8028288, 0x8028298, 0x80282f0];
    #
    # #    for adr in f_4137_hook_list:
    # #        merger.hookbl(adr,sapplet.getadr("f_4137_hook"),0);
    # #    merger.hookbl(0x804464a,sapplet.getadr("f_4520_hook"),0);
    # #    merger.hookbl(0x8044642,sapplet.getadr("f_4098_hook"),0);
    # #    merger.hookbl(0x804c1e8,sapplet.getadr("f_4102_hook"),0);
    #
    #    # display hooks is in d13.020 included
    #    ## display flip workaround see issue #178 not necessary on 0X3.020
    #    merger.hookbl(0x08031fde,sapplet.getadr("display_init_hook_1"),0);
    #    merger.hookbl(0x0803200e,sapplet.getadr("display_init_hook_2"),0);

    print("Merging %s into %s at %08x" % (
        sys.argv[2],
        sys.argv[1],
        index))

    i = 0
    for b in bapplet:
        merger.setbyte(index + i, bapplet[i])
        i += 1

    merger.export(sys.argv[1])
