#!/usr/bin/env python2
# Vocoder Patch for MD380 Firmware
# Applies to version D013.020

from Patcher import Patcher

#Match all public calls.
monitormode=False
#Match all private calls.
monitormodeprivate=False

if __name__ == '__main__':
    print "Creating patches from unwrapped.img."
    patcher=Patcher("unwrapped.img")
    
# bypass vocoder copy protection on D013.020

    patcher.nopout((0x08033f30+0x18))
    patcher.nopout((0x08033f30+0x1a))

    patcher.nopout((0x08033f30+0x2e))
    patcher.nopout((0x08033f30+0x30))

    patcher.nopout((0x08033f30+0x44))
    patcher.nopout((0x08033f30+0x46))

    patcher.nopout((0x08033f30+0x5a))
    patcher.nopout((0x08033f30+0x5c))

    patcher.nopout((0x08033f30+0x70))
    patcher.nopout((0x08033f30+0x72))

    patcher.nopout((0x08033f30+0x86))
    patcher.nopout((0x08033f30+0x88))

    patcher.nopout((0x0804915c+0x12))
    patcher.nopout((0x0804915c+0x14))

    patcher.nopout((0x080499e2+0x12))
    patcher.nopout((0x080499e2+0x14))

    patcher.nopout((0x08049ca8+0x10))
    patcher.nopout((0x08049ca8+0x12))

    patcher.nopout((0x0804a134+0x10))
    patcher.nopout((0x0804a134+0x12))

    patcher.nopout((0x0804abc0+0x10))
    patcher.nopout((0x0804abc0+0x12))

    patcher.export("patched.img")
