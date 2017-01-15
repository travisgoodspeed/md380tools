#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Howdy!

# This is a quick and dirty tool to dump WAV files from an MD380 over
# USB.  The firmware must be patched with AMBEWAVPRINT enabled in
# config.h, and the USB patch must be fast enough to catch the frames
# without corruption, while still leaving enough time for the C5000
# process to run.

# If you have things configured correctly, the resulting output.wav
# file ought to sound rather good.

# Cheers from Knoxville,
# --Travis


import sys
import wave

outfile = wave.open("output.wav", 'w')


def writeheader():
    """Writes the .WAV file's header."""
    outfile.setparams((1, 2, 8000, 0, 'NONE', 'not compressed'))


def writeframe(frame):
    frames = [frame]
    outfile.writeframes(chr(frame & 0xFF) + "" + chr(frame >> 8))


# This part is god-awful ugly.  Kill it with fire!

writeheader()

for r in sys.stdin:
    if len(r) == 486:  # Ugly check, but I'm in a hurry.
        bits = r.split()[1:]
        for i in range(0, 160, 2):
            sample = (int(bits[i + 1], 16) << 8) | int(bits[i], 16)
            # if sample&0x8000:
            #    sample= 0-(sample^0xFFFF)

            writeframe(sample)
