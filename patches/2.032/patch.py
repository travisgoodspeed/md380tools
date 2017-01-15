#!/usr/bin/env python2
# -*- coding: utf-8 -*-

# Promiscuous Mode Patch for MD380 Firmware
# Applies to version 2.032

from Patcher import Patcher

# Match all public calls.
monitormode = False
# Match all private calls.
monitormodeprivate = False

if __name__ == '__main__':
    print("Creating patches from unwrapped.img.")
    patcher = Patcher("unwrapped.img")

    #     #These aren't quite enough to skip the Color Code check.  Not sure why.
    patcher.nopout(0x0803ea62, 0xf040)  # Main CC check.
    patcher.nopout(0x0803ea64, 0x80fd)
    patcher.nopout(0x0803e994, 0xf040)  # Late Entry CC check.
    patcher.nopout(0x0803e996, 0x8164)
    patcher.nopout(0x0803fd98)  # dmr_dll_parser CC check.
    patcher.nopout(0x0803fd9a)
    patcher.sethword(0x0803fd8e, 0xe02d,  # Check in dmr_dll_parser().
                     0xd02d)
    patcher.nopout(0x0803eafe, 0xf100)  # Disable CRC check, in case CC is included.
    patcher.nopout(0x0803eb00, 0x80af)

    # Disable the ALPU Licence Check (vocoder version)
    patcher.nopout(0x8032a54)
    patcher.nopout(0x8032a54 + 2)
    patcher.nopout(0x8032a6a)
    patcher.nopout(0x8032a6a + 2)
    patcher.nopout(0x8032a80)
    patcher.nopout(0x8032a80 + 2)
    patcher.nopout(0x8032a96)
    patcher.nopout(0x8032a96 + 2)
    patcher.nopout(0x8032aac)
    patcher.nopout(0x8032aac + 2)
    patcher.nopout(0x8032ac2)
    patcher.nopout(0x8032ac2 + 2)
    patcher.nopout(0x8046fda)
    patcher.nopout(0x8046fda + 2)
    patcher.nopout(0x804785a)
    patcher.nopout(0x804785a + 2)
    patcher.nopout(0x8047b20)
    patcher.nopout(0x8047b20 + 2)
    patcher.nopout(0x8047f6c)
    patcher.nopout(0x8047f6c + 2)
    patcher.nopout(0x8048904)
    patcher.nopout(0x8048904 + 2)

    # Patches after here allow for an included applet.

    # This cuts out the Chinese font, freeing ~200k for code patches.
    patcher.ffrange(0x809c714, 0x80d0f80)

    # This mirrors the RESET vector to 0x080C020, for use in booting.
    patcher.setword(0x0800C020,
                    patcher.getword(0x0800C004),
                    0x00000000)

    # This makes RESET point to our stub below.
    patcher.setword(0x0800C004,
                    0x0809cf00 + 1
                    )

    # This stub calls the target RESET vector,
    # if it's not FFFFFFFF.
    patcher.sethword(0x0809cf00, 0x4840)
    patcher.sethword(0x0809cf02, 0x2100)
    patcher.sethword(0x0809cf04, 0x3901)
    patcher.sethword(0x0809cf06, 0x4508)
    patcher.sethword(0x0809cf08, 0xd100)
    patcher.sethword(0x0809cf0a, 0x483c)
    patcher.sethword(0x0809cf0c, 0x4700)

    # ### Disable power on password (to flash an virgin codeplug
    # ### with no password, if you lost this)
    # ## patcher.sethword(0x0801a4fe, 0xbdf7);

    # [0x0809cf00]> pd 7
    #             0x0809cf00      4048           ldr r0, [pc, 0x100]         ; [0x809d004:4]=-1
    #             0x0809cf02      0021           movs r1, 0
    #             0x0809cf04      0139           subs r1, 1
    #             0x0809cf06      0845           cmp r0, r1
    #         ,=< 0x0809cf08      00d1           bne 0x809cf0c
    #         |   0x0809cf0a      3c48           ldr r0, [pc, 0xf0]          ; [0x809cffc:4]=0x80fa969 
    #         `-> 0x0809cf0c      0047           bx r0
    # [0x0809cf00]> 

    # Stores the RESET handler for our stub.
    patcher.setword(0x809cffc,
                    patcher.getword(0x0800C020),
                    0xFFFFFFFF)

    # Marks the version as "md380tools"
    patcher.setwstring(0x080d14d8,
                       "MD380Tools Ver.")

    # Change the manufacturer string.
    patcher.setstring(0x080f9e4c,
                      "Travis Goodspeed KK4VCZ")
    # Change the device name.
    patcher.setstring(0x080d1820,
                      "Patched MD380")

    # Fixes a typo in 2.032.  Unneeded in 2.034.
    patcher.setwstring(0x080d17e8,
                       "Repeater Slot")  # was 'Repeatar Slot'

    patcher.export("patched.img")
