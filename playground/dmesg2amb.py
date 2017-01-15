#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Howdy!  This is a quick and dirty tool to convert AMBE2+ frames from
# the dmesg log of my MD380 into the .amb file format expected by DSD,
# so that it can be converted to audio.  Later on, we can move this
# conversion into the radio, so that audio can be extracted over USB
# without involving ugly host-side Python scripts.

# You must define AMBEPRINT in config.h.  The output is very scratchy,
# but you can make out the audio over the noise.

# AMBECORRECTEDPRINT taps the same bits from a different function
# call.  You probably shouldn't use it.

# Thanks to David and Jonathan.

from __future__ import print_function

import sys


def writeheader(f):
    """Writes the .AMB file's header, as checked by DSD."""
    f.write(".amb")


def writeframe(f, frame):
    """Writes an 8-byte frame consisting of a status code and a packet."""
    # First we write the zero byte (success status)
    f.write(chr(0))
    # Then we write the bits to 7 bytes, in a somewhat janky manner.
    i = 0
    b = 0
    for bit in frame:
        # Load the new bit.
        b = (b << 1) | (int(bit))
        # Increase the bit count.
        i += 1
        if i == 8:
            # Dump the byte and start over.
            f.write(chr(b))
            i = 0
            b = 0
    # There's one bit left over, which takes the LSBit in its own byte.
    f.write(chr(b))


outfile = open("output.amb", "w")
writeheader(outfile)
for r in sys.stdin:
    if len(r) == 63:  # Ugly check, but I'm in a hurry.
        bits = r.split()[2]
        print(bits)
        writeframe(outfile, bits)
