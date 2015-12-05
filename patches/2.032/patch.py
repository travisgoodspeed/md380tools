#!/usr/bin/env python
# Promiscuous Mode Patch for MD380 Firmware
# Applies to version 2.032

from Patcher import Patcher

if __name__ == '__main__':
    print "Creating patches from unwrapped.img.";
    patcher=Patcher("unwrapped.img");
    
    #Old logo patcher, no longer used.
    #fhello=open("welcome.txt","rb");
    #hello=fhello.read();
    #patcher.str2sprite(0x08094610,hello);
    #print patcher.sprite2str(0x08094610,0x14,760);
    
    #Old patch, matching on the first talkgroup.
    #We don't use this anymore, because the new patch is better.
    #patcher.nopout(0x0803ee36,0xd1ef);
    
    # New patch for monitoring all talk groups , matched on first
    # entry iff no other match.
    #wa mov r5, 0 @ 0x0803ee86 # So the radio thinks it matched at zero.
    patcher.sethword(0x0803ee86, 0x2500);
    #wa b 0x0803ee38 @ 0x0803ee88 # Branch back to perform that match.
    patcher.sethword(0x0803ee88,0xe7d6); #Jump back to matched condition.
    
    patcher.export("prom-public.img");
    
    # This should be changed to only show missed calls for private
    # calls directed at the user, and to decode others without
    # triggering a missed call.
    patcher.nopout(0x0803ef10,0xd11f);  #Matches all private calls.
    patcher.export("prom-private.img");

    #Everything after here is experimental.
    #Everything after here is experimental.
    #Everything after here is experimental.
    
    #This cuts out the Chinese font, freeing ~200k for code patches.
    patcher.ffrange(0x809c714,0x80d0f80);
    patcher.export("experiment.img");
    
