# !/usr/bin/env python2
# -*- coding: utf-8 -*-

# Vocoder Patch for MD380 Firmware
# Applies to version D013.020

from Patcher import Patcher

# Match all public calls.
monitormode = False
# Match all private calls.
monitormodeprivate = False

if __name__ == '__main__':
    print("Creating patches from unwrapped.img.")
    patcher = Patcher("unwrapped.img")

    patcher.export("patched.img")
