#!/usr/bin/env python2
# Vocoder Patch for MD380 Firmware
# Applies to version S013.020

from Patcher import Patcher

#Match all public calls.
monitormode=False
#Match all private calls.
monitormodeprivate=False

if __name__ == '__main__':
    print "Creating patches from unwrapped.img."
    patcher=Patcher("unwrapped.img")
    
    # bypass vocoder copy protection on S013.020
    patcher.nopout((0x8034a60))
    patcher.nopout((0x8034a60+0x2))
    patcher.nopout((0x8034a76))
    patcher.nopout((0x8034a76+0x2))
    patcher.nopout((0x8034a8c))
    patcher.nopout((0x8034a8c+0x2))
    patcher.nopout((0x8034aa2))
    patcher.nopout((0x8034aa2+0x2))
    patcher.nopout((0x8034ab8))
    patcher.nopout((0x8034ab8+0x2))
    patcher.nopout((0x8034ace))
    patcher.nopout((0x8034ace+0x2))
    patcher.nopout((0x8049f9a))
    patcher.nopout((0x8049f9a+0x2))
    patcher.nopout((0x804a820))
    patcher.nopout((0x804a820+0x2))
    patcher.nopout((0x804aae4))
    patcher.nopout((0x804aae4+0x2))
    patcher.nopout((0x804af70))
    patcher.nopout((0x804af70+0x2))
    patcher.nopout((0x804b9fc))
    patcher.nopout((0x804b9fc+0x2))

    #Marks the version as "md380tools"
    patcher.setwstring(0x080d0b6c,
                       "MD380Tools Ver.");

#     #Change the manufacturer string.
#     patcher.setstring(0x080f86c8,
#                       "Travis Goodspeed KK4VCZ");
#     #Change the device name.
#     patcher.setstring(0x080cfff4,
#                       "Patched MD380");

    patcher.export("patched.img")
