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
              
    def try_getadr(self,name): # DL4YHF 2017-01, used to CHECK if a symbol exists
        try:                   # to perform patches for 'optional' C functions 
            return self.addresses[name];
        except KeyError:
            return None;
          
    def getadr(self, name):
        return self.addresses[name]

    def getname(self, adr):
        return self.names[adr]


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
        self.sethword(adr + 4, 0x4802)  # ldr r0, [pc, 8]
        self.sethword(adr + 6, 0x9001)  # str r0, [sp, 4] (pc)
        self.sethword(adr + 8, 0xbc01)  # pop {r0}
        self.sethword(adr + 10, 0xbd00)  # pop {pc}
        self.sethword(adr + 12, 0x4600)  # NOP
        self.sethword(adr + 14, 0x4600)  # NOP, might be overwritten
        if adr & 2 > 0:
            self.setword(adr + 14, handler)
        else:
            self.setword(adr + 16, handler)

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
    # ############  All address comments from D13.020
    merger.hookstub(0x080974de,  # 0x0809661e,    #USB manufacturer string handler function.
                    sapplet.getadr("getmfgstr"))
    merger.hookstub(0x080229ae,  # 0x080226d2, #startup_botline
                    sapplet.getadr("splash_hook_handler"))
    merger.hookstub(0x08016b7e,  # 0x08016a96,
                    sapplet.getadr("loadfirmwareversion_hook"))
    merger.hookbl(0x0808f9a6,  # 0x0808eb66, #Call to usb_dfu_upload().
                  sapplet.getadr("usb_upld_hook"))

    merger.hookbl(0x080413f8,  # 0x80408e0, #Call to dmr_call_end()
                  sapplet.getadr("dmr_call_end_hook"))

    dmr_call_start_hook_list = [0x08041282, 0x080413e2]  # 0x804076a,0x80408ca];
    for adr in dmr_call_start_hook_list:
        merger.hookbl(adr, sapplet.getadr("dmr_call_start_hook"))

    dmr_handle_data_hook_list = [0x08041456, 0x0804146e, 0x080414b4]  # 0x804093e,0x8040956,0x804099c];
    for adr in dmr_handle_data_hook_list:
        merger.hookbl(adr, sapplet.getadr("dmr_handle_data_hook"))

    merger.hookbl(0x0804143e, sapplet.getadr("dmr_sms_arrive_hook"))

    merger.hookbl(0x0801da8a, sapplet.getadr("display_init"))

    # os semaphore hook .. now we can crate own semaphores
    merger.hookbl(0x0804717a, sapplet.getadr("OSSemCreate_hook"), 0)  # 0x804647a

    gch = [
        0x0802cd0c,
        0x0802cfdc,
        0x0802d00a,
        0x0802d044,
        0x0802d04c,
        0x0802d090,
        0x0802d0be,
        0x0802d0f4,
        0x0802d0fc,
    ]
    for adr in gch:
        merger.hookbl(adr, sapplet.getadr("gui_control_hook"))

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
        0x0802dabe,
        0x0802daca,
        0x0802dadc,
        0x0802dda4,
        0x0802ddb0,
        0x0802ddc2,
    ]
    for adr in gfxdrawcharpos:
        merger.hookbl(adr, sapplet.getadr("gfx_drawchar_pos_hook"))

    drwbmplist = [
        0x0800cdd0,
        0x0802058a,
        0x0802161a,
        0x08021628,
        0x08021a84,
        0x08021a90,
        0x08025fd6,
        0x08026024,
        0x08026030,
        0x0802609a,
        0x080260ea,
        0x08026102,
        0x08028a5e,
        0x08028a88,
        0x08028ab2,
        0x08028ad0,
        0x08028b9a,
        0x08028bdc,
        0x08028c00,
        0x08028c4c,
        0x08028c60,
        0x08028cc0,
        0x08028cd4,
        0x08028d0c,
        0x08028d4a,
        0x08028d9a,
        0x08028dda,
        0x08028e34,
        0x0802bbdc,
        0x0802bc7a,
        0x0802bd5e,
        0x0802bec2,
        0x0802bf56,
        0x0802c06e,
        0x0802c150,
        0x0802c1d8,
        0x0802c32c,
        0x0802c338,
        0x0802c45a,
        0x0802c4e6,
        0x0802c5d6,
        0x0802c63a,
        0x0802c6b6,
        0x0802cde4,
        0x0802da0e,
        0x0802dcdc,
        0x0802dcf8,
        0x0802de0c,
        0x08031eb4,
        0x08031ee6,
        0x08034936,
        0x08034986,
        0x080474fc,
        0x0804cd46,
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
        0x0800c9fe,  # ?
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
        0x0801e888,
        0x0801e896,
        0x0801e8a6,
        0x0801e93c,
        0x0801e94c,
        0x08020202,
        0x0802071c,
        0x080208f6,
        0x08020af2,
        0x08020f66,
        0x08020f72,
        0x08020f7e,
        0x08020f8a,
        0x08020f96,
        0x08020fa2,
        0x08021304,
        0x080213ea,
        0x08021452,
        0x0802145e,
        0x0802146a,
        0x08021476,
        0x08021482,
        0x0802148e,
        0x080214fa,
        0x08025e24,
        0x08025e4a,
        0x08025eec,
        0x08025f80,
        0x08025f9a,
        0x08025fa6,
        0x08025fb2,
        0x080260ce,
        0x0802611e,
        0x0802612a,
        0x0802bfe8,
        0x0802c13a,
        0x0802c470,
        0x0802c542,
        0x0802c62e,
        0x0802c998,
        0x0802ca14,
        0x0802d2ba,
        0x0802d314,
        0x0802d326,
        0x0802d332,
        0x0802d346,
        0x0802d352,
        0x0802d990,
        0x0802d9b0,
        0x0802d9bc,
        0x0802d9c8,
        0x0802da22,
        0x0802da2e,
        0x0802da3a,
        0x0802da46,
        0x0802da52,
        0x0802da5e,
        0x0802daf0,
        0x0802dafc,
        0x0802db14,
        0x0802db28,
        0x0802db34,
        0x0802db40,
        0x0802dc4a,
        0x0802dc56,
        0x0802dc62,
        0x0802dca8,
        0x0802dd0e,
        0x0802dd36,
        0x0802dd42,
        0x0802ddd6,
        0x0802dde2,
        0x0802ddfa,
        0x0802de28,
        0x0802de34,
        0x0802de40,
        0x0802de4c,
        0x0802de58,
        0x0802de64,
        0x0802e5bc,
        0x0802e5c8,
        0x0802e6a6,
        0x0802e6b2,
        0x080349aa,
        0x08037bca,
        0x08037be6,
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
        0x0800df92,
        0x0800dff6,
        0x0800e174,
        0x0800e18e,
        0x0800e1b4,
        0x0800e1da,
        0x0800ea82,
        0x0801eb62,
        0x0801f160,
        0x0801f178,
        0x0801f1ae,
        0x0801f1c6,
        0x0802d9f4,
        0x0802daa2,
        0x0802dc86,
        0x0802dd88,
    ]
    for adr in dt2list:
        merger.hookbl(adr, sapplet.getadr("gfx_drawtext2_hook"))

    dt4list = [
        0x0800e5ba,
        0x0800e604,
        0x0800e618,
        0x0800e634,
        0x0800e696,
        0x0802da86,
        0x0802dd6c,
    ]
    for adr in dt4list:
        merger.hookbl(adr, sapplet.getadr("gfx_drawtext4_hook"))

    # gfx_ primitives hooks, to be overriden later in this file.
    #########

    #########
    # status line

    # date format  hook, this hook can modify the date format on the status line
    merger.hookbl(0x0800df92, sapplet.getadr("print_date_hook"), 0)

    merger.hookbl(0x08021a5e, sapplet.getadr("draw_statusline_hook"))

    draw_datetime_row_list = [
        0x08020f0a,
        0x08020f56,
        0x08021442,
        0x08021576,
        0x0802db00,
        0x0802dde6,
    ]
    for adr in draw_datetime_row_list:
        merger.hookbl(adr, sapplet.getadr("draw_datetime_row_hook"))

    # rx popup overrides of gfx_drawbmp
    merger.hookbl(0x08026024, sapplet.getadr("rx_screen_blue_hook"), 0)  # 0x08025d54
    merger.hookbl(0x08026102, sapplet.getadr("rx_screen_blue_hook"), 0)  # 0x08025e26
    merger.hookbl(0x0802058a, sapplet.getadr("rx_screen_gray_hook"), 0)  # 0x08020428

    # Hook the startup AES check.
    merger.hookbl(0x08048474, sapplet.getadr("aes_startup_check_hook"), 0)  # 0x0804764c

    # Patch a single call in the wrapper function so catch all
    # aes_loadkey() calls.
    merger.hookbl(0x0803774a, sapplet.getadr("aes_loadkey_hook"), 0)  # 0x08036c32

    # Function that calls aes_cipher() twice.  When are these called?
    # there a 3 calls on d02.32
    aes_cipher_hook_list = [0x08022936, 0x0804070a]  # 0x802265a,0x803fbf2];
    for adr in aes_cipher_hook_list:
        merger.hookbl(adr, sapplet.getadr("aes_cipher_hook"))

    # Hook lots of AMBE2+ encoder code and hope our places are correct.
    ambelist = [
        0x804b036, 0x804b16e, 0x804b1c6, 0x804b314, 0x804b38a, 0x804b504,
        0x804b584, 0x804b6f0, 0x804b726
    ]
    #              0x804a20a,0x804a342,0x804a39a,0x804a4e8,0x804a55e,
    #              0x804a6d8,0x804a758,0x804a8c4,0x804a8fa
    for adr in ambelist:
        merger.hookbl(adr, sapplet.getadr("ambe_encode_thing_hook"))

    # Hook calls within the AMBE2+ decoder.
    unpacklist = [
        0x8034c1e, 0x8034c2a, 0x8034c42, 0x8034c4e, 0x804bcea, 0x804c216,
        0x804c26a, 0x804c2c0
        #        0x8034106, 0x8034112, 0x803412a, 0x8034136, 0x804aebe, 0x804b3ea, 0x804b43e,
        #        0x804b494
    ]
    for adr in unpacklist:
        merger.hookbl(adr, sapplet.getadr("ambe_unpack_hook"))

    # Hook calls that produce WAV audio.  (Maybe.)
    wavdeclist = [
        0x804adc4, 0x804ba38, 0x804bb8e, 0x804bea0, 0x804bedc, 0x804bf9c, 0x804bfd8
        #        0x8049f98, 0x804ac0c, 0x804ad62, 0x804b074, 0x804b0b0, 0x804b170, 0x804b1ac
    ]
    for adr in wavdeclist:
        merger.hookbl(adr, sapplet.getadr("ambe_decode_wav_hook"))

    # Hooks the squelch routines, so we can do monitor mode in C.
    merger.hookbl(0x080417f8, sapplet.getadr("dmr_apply_privsquelch_hook"), 0)  # Private calls. # 0x08040ce0
    # ########  this function has been changed
    merger.hookbl(0x08041734, sapplet.getadr("dmr_apply_squelch_hook"), 0)  # Public calls. # 0x08040c1c

    # additional menu hook
    merger.hookbl(0x08013642, sapplet.getadr("create_menu_utilies_hook"), 0)  # 0x080135a8

    # init the addl global config struct from spi flash
    merger.hookbl(0x08047026, sapplet.getadr("init_global_addl_config_hook"), 0)  # 0x08046326

    # no menu exit on RX hook 
    #    merger.hookbl(0x0801ffb0,sapplet.getadr("f_4225_internel_hook"),0);#0x0801fe7c

    # OSMboxPend Hook to intercept Beep_Process
    merger.hookbl(0x0802fe54, sapplet.getadr("beep_OSMboxPend_hook"))

    # other OSMboxPend hooks
    mbx_pend_list = [
        #        0x080315e4,
        0x0803c412,
        0x0803ceb0,
        0x0803d31e,
        0x0804793e,
        0x08047956,
    ]
    for adr in mbx_pend_list:
        merger.hookbl(adr, sapplet.getadr("OSMboxPend_hook"))

    # hooks regarding the beep_process
    beep_process_list = [
        0x08030082, 0x08030094,  # beep "9"
        #      0x0802fc2e, 0x0802fc40, # beep "9"
        0x08030048, 0x0803005a,  # roger beep
        #      0x0802fbf4, 0x0802fc06, # roger beep
        0x080301c0, 0x080301d2, 0x080301e0, 0x080301ee
        #      0x0802fd6c, 0x0802fd7e, 0x0802fd8c, 0x0802fd9a #dmr sync
    ]
    for adr in beep_process_list:
        merger.hookbl(adr, sapplet.getadr("F_294_replacement"), 0)

    merger.hookbl(0x0802ded6, sapplet.getadr("f_4225_hook"), 0)  # 0x080468e6
    merger.hookbl(0x08047640, sapplet.getadr("f_4225_hook"), 0)  # 0x0802db42

    #    merger.hookstub2(0x0800c72e, sapplet.getadr("create_menu_entry_rev"));

    # keyboard
    merger.hookbl(0x0804fa12, sapplet.getadr("kb_handler_hook"))

    #Change TIM12 IRQ Handler to new one
    merger.setword(0x0800c0ec, sapplet.getadr("New_TIM12_IRQHandler")+1);

    #    merger.hookbl(0x08047628,sapplet.getadr("f_4520_hook"));
    #    merger.hookbl(0x0802cca8,sapplet.getadr("dummy"));
    #    merger.hookbl(0x0802ccb4,sapplet.getadr("dummy"));

    print("Merging %s into %s at %08x" % (
        sys.argv[2],
        sys.argv[1],
        index))


    # DL4YHF : We don't know here if the PWM'ed backlight, and thus
    #  SysTick_Handler() shall be included (depends on config.h) .
    # IF   the applet's symbol table contains a function named 'SysTick_Handler',
    # THEN patch its address, MADE ODD to indicate Thumb-code, into the
    # interrupt-vector-table as explained in applet/src/irq_handlers.c :
    # ex: new_adr = sapplet.getadr("SysTick_Handler"); # threw an exception when "not found" :(
    new_adr = sapplet.try_getadr("SysTick_Handler");
    if new_adr != None:
        vect_adr = 0x800C03C;  # address inside the VT for SysTick_Handler
        exp_adr  = 0x8094d5b;  # expected 'old' content of the above VT entry
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
    

    i = 0
    
    for b in bapplet:
        merger.setbyte(index + i, bapplet[i])
        i += 1

    merger.export(sys.argv[1])
