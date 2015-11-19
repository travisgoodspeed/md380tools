#!/usr/bin/env python
# Promiscuous Mode Patch for MD380 Firmware
# Applies to version 2.032

from Patcher import Patcher

if __name__ == '__main__':
    print "Creating patches from unwrapped.img.";
    patcher=Patcher("unwrapped.img");

    #All patched images should be indicated by a patched welcome screen.
    fhello=open("welcome.txt","rb");
    hello=fhello.read();
    patcher.str2sprite(0x08094610,hello);
    print patcher.sprite2str(0x08094610,0x14,760);
    
    
    patcher.nopout(0x0803ee36,0xd1ef);  #Matches first group for public calls.
    patcher.export("prom-public.img");

    patcher.nopout(0x0803ef10,0xd11f);  #Matches private calls too.
    patcher.export("prom-private.img");

    #Everything after here is experimental.
    #Everything after here is experimental.
    #Everything after here is experimental.
    
    #This cuts out the Chinese font, freeing ~200k for code patches.
    patcher.ffrange(0x809c714,0x80d0f80);
    patcher.export("experiment.img");
    
