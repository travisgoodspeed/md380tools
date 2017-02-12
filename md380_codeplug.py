#!/usr/bin/env python2

# md380-codeplug by KK6RWR and Friends

# This is a tool to perform various functions to MD-380 codeplugs and SPI flash.
# Some overlap with md380-tool and md380-dfu, but this file should be kept to editing codeplugs when appropriate

import sys


def sidebutton():
    #   """Sets the side buttons to a new function"""
    if sys.argv[2] == '?':
        sidebuttonhelp()
        exit()
    elif 1 <= int(sys.argv[2]) <= 4:
        address = buttonAddress[(int(sys.argv[2]) - 1)]
        if 0 <= int(sys.argv[3]) <= len(buttonFunctions):
            newfunction = buttonFunctions[int(sys.argv[3])]
            editcodeplug(sys.argv[4], address, newfunction)
            exit(0)
        else:
            print "Not a valid button function."
            sidebuttonhelp()
            exit(0)
    else:
        print "Not a valid button ID"
        sidebuttonhelp()
        exit(0)


def editcodeplug(filename, addr, data):
    if filename.lower().endswith('.rdt'):
        addr += 549
    fh = open(filename, "r+b")
    fh.seek(addr)
    fh.write(chr(data))
    fh.close()


buttonAddress   = [0x2102, 0x2103, 0x2104, 0x2105]
buttonFunctions = [0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
                   0x0b, 0x0c, 0x0d, 0x0e, 0x15, 0x16, 0x17, 0x18, 0x1e, 0x1f, 0x26, 0x50]


def usage():
    print("""
Usage: md380-codeplug <command> <arguments>

Set programmable side button functions
    md380-codeplug sidebutton <buttonID> <function> <filename>
    use "md380-codeplug sidebutton ?" for help
""")

def sidebuttonhelp():
    print("""
    Set programmable side button functions
        md380-codeplug sidebutton <buttonID> <function> <filename>
        ButtonID:
            1 - Top Side Button Short Press
            2 - Top Side Button Long Press
            3 - Bottom Side Button Short Press
            4 - Bottom Side Button Long Press

        function:
            Built-in (available in CPS software)
            0  - Unassigned
            1  - All Tone Alert On/Off
            2  - Emergency On
            3  - Emergency Off
            4  - High/Low Power
            5  - Monitor
            6  - Nuisance Delete
            7  - One Touch Access 1
            8  - One Touch Access 2
            9  - One Touch Access 3
            10 - One Touch Access 4
            11 - One Touch Access 5
            12 - One Touch Access 6
            13 - Repeater/Talkaround
            14 - Scan On/Off
            15 - Tight/Normal Squelch
            16 - Privacy On/Off
            17 - Vox On/Off
            18 - Zone Increment
            19 - Manual Dial
            20 - Lone work On/Off
            21 - 1750hz Tone

            Custom
            22 - Disable/Enable LCD backlight

        filename:
            File Formats Supported: .RDT (Official Tytera Codeplug)
                                    .bin (Binary from radio)
    """)


def main():
    try:
        if len(sys.argv) == 5 or sys.argv[2] == '?':
            if sys.argv[1] == 'sidebutton':
                sidebutton()
        else:
            usage();

    except IndexError:
        usage()
        exit(0)
    except RuntimeError, e:
        print(e.args[0])
        exit(1)
    except Exception, e:
        print e
        exit(1)


if __name__ == '__main__':
    main()
