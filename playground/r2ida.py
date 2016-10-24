#!/usr/bin/env python
from __future__ import print_function

import argparse
import json
import sys

# Parse r2 scripts into IDA IDC format
# This only handles a few commands but is good enough for our uses.
# Works with Python 2 and 3

ida_front_boilerplate = """
#include <idc.idc>
static main(void)
{
"""
ida_end_boilerplate = "}"
marks = 0
def parse_line(line):
    global marks
    toks = line.split()
    if len(toks) == 0: # blank lines
        return ''
    if toks[0].startswith('#'): # script comments
        return toks[0].replace('#', '//') + " " + ' '.join(toks[1:])
    if toks[0] == 'CCa': # disasm comment
        return "MakeComm (" + toks[1] + ',' + json.dumps(' '.join(toks[2:])) + ');'
    if toks[0] == 'af+': # define func with start and length
        s = "MakeName (" + toks[1] + ',' + json.dumps(' '.join(toks[3:]).replace('/', '_').replace('-', '_')) + ');'
        s += "\nMakeFunction (" + toks[1] + ',' + "0x{0:x}".format(int(toks[1],16)+int(toks[2])) + ");"
        return s
    if toks[0] == 'f':
        s = 'MakeComm(' + toks[3] + ',"' + toks[1] + '");' #add it as a comment, and also as a bookmark
        s += '\nMarkPosition(' + toks[3] + ',0,0,0,' + str(marks) + ',"' + toks[1] + '");'
        marks +=1 # we need an index for idc marks
        return s
    else: # f and fC are UNHANDLED and appear in script
        print("Unhandled command: {}".format(toks[0]), file=sys.stderr)
        return ''
        

# read input file into lines

def main():
    parser = argparse.ArgumentParser(description='Convert Radare2 .r file into IDC script (rudimentary)')
    parser.add_argument('input', nargs=1, help='input file')
    parser.add_argument('output', nargs=1, help='output file')
    args = parser.parse_args()
    
    f = open(args.input[0], 'r')
    g = open(args.output[0], 'w')
    
    g.write(ida_front_boilerplate)
    g.write('\n')

    for line in f:
        ida_line = parse_line(line)
        g.write(ida_line)
        g.write('\n')

    g.write(ida_end_boilerplate)
    g.write('\n')
    
    g.close()
    f.close()

if __name__ == "__main__":
    main()
