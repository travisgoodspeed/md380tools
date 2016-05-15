#!/usr/bin/env python2

import sys

def insert_comments(filefrom,newcomments):
    with open(filefrom,'rb+') as ppm:
        ppmdata = ppm.read()
        lines = ppmdata.split("\n")
        assert lines[0] == "P6"
        i = 1
        comments = []
        while lines[ i ].startswith("#"):
            comments += lines[ i ]
            i += 1
        image = lines[i:]
        ppm.truncate(0)
        ppm.seek(0)
        ppm.write("P6\n")
        ppm.write('\n'.join(comments + newcomments )+"\n")
        ppm.write('\n'.join(image))


def get_comments(filefrom):
    with open(filefrom,'rb') as ppm:
        ppmdata = ppm.read()
        lines = ppmdata.split("\n")
        assert lines[0] == "P6"
        i = 1
        comments = []
        while lines[ i ].startswith("#"):
            comments.append( lines[ i ] )
            i += 1
        return comments

def remove_comments(filefrom):
    with open(filefrom,'rb+') as ppm:
        ppmdata = ppm.read()
        lines = ppmdata.split("\n")
        assert lines[0] == "P6"
        i = 1
        comments = []
        while lines[ i ].startswith("#"):
            comments.append( lines[ i ] )
            i += 1
        image = lines[i:]
        ppm.truncate(0)
        ppm.seek(0)
        ppm.write("P6\n")
        ppm.write('\n'.join(image))

def copy_comments(filefrom,fileto):
    comments = get_comments(filefrom)
    insert_comments(fileto,comments)

def restore_header(filefrom, fileto):
    remove_comments(fileto)
    copy_comments(filefrom, fileto)


def usage():
    print("""
The MD380tools readme points out you need to restore the PPM comments to
replace a graphic in the MD380 firmware.  The md380-gfx program expects
these comments, or else it chokes - it needs them to know where in the
MD380 firmware to put the graphic.  This script will copy headers from
one ppm image to another, to make it easy to do this with arbitrary ppm
images quickly and easily.

Usage: ./gfx-header copy filefrom.ppm fileto.ppm
Above will copy the header/PPM comments from "filefrom.ppm" to "fileto.ppm".
""")
def main(argv):
    argc = len(argv)
    if argc < 4:
        usage()
        sys.exit(1)
    action   = argv[1]
    filefrom = argv[2]
    fileto   = argv[3]
    restore_header(filefrom,fileto)


if __name__ == "__main__":
    main(sys.argv)
