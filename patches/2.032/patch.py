#!/usr/bin/env python
# Promiscuous Mode Patch for MD380 Firmware
# Applies to version 2.032

from Patcher import Patcher

if __name__ == '__main__':
    print "Creating patches from unwrapped.img.";
    patcher=Patcher("unwrapped.img");
    
    patcher.nopout(0x0803ee36,0xd1ef);  #Matches first group for public calls.
    patcher.export("prom-public.img");

    patcher.nopout(0x0803ef10,0xd11f);  #Matches private calls too.
    patcher.export("prom-private.img");

    #Everything after here is experimental.
    #Everything after here is experimental.
    #Everything after here is experimental.
    
    
    #This corrupts the first row of the startup image, which begins at 0x08094610.
    #patcher.ffrange(0x08094610, 0x08094610+0x14);

    sprite="";
    for i in range(0,768):
        b=patcher.getbyte(0x08094610+i);
        if i%0x14==0:
            sprite=sprite+"\n";
        for j in range(0,8):
            c=' ';
            if b&(128>>j)>0:
                c='X';
            sprite=sprite+c;
    print sprite;
    
    #This cuts out the Chinese font, freeing ~200k for code patches.
    patcher.ffrange(0x809c714,0x80d0f80);
    patcher.export("experiment.img");
    
