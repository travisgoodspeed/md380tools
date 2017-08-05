#!/usr/bin/python

# This is a janky little parser for converting symbol files to an
# IDA Python script.  It's very specific to the MD380Tools project,
# and you ought to rewrite it for use in your own projects.

import sys;

for l in sys.stdin:
    words=l.strip().split();

    try:
        name=words[0];
        adrstr=words[2];
        adr=int(adrstr.strip(";"),16);

        #Fix up addresses in Flash that look like functions.
        if(adr&0xFF000000==0x08000000):
            adr=adr&~1;

        print("ida_name.set_name(0x%x,\"%s\");" % (adr,name))
    except:
        # Print warnings when our janky parser goes awry.
        if len(words)>0 and words[0]!="/*" and words[0]!="*/":
            print("#Warning in: %s\n"%words);
