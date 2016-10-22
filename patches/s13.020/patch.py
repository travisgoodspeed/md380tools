#!/usr/bin/env python2
# Vocoder Patch for MD380 Firmware
# Applies to version S013.020

from Patcher import Patcher

#Match all public calls.
monitormode=False
#Match all private calls.
monitormodeprivate=False

if __name__ == '__main__':
    print "Creating patches from unwrapped.img."
    patcher=Patcher("unwrapped.img")
    
    # bypass vocoder copy protection on S013.020
    patcher.nopout((0x8034a60))
    patcher.nopout((0x8034a60+0x2))
    patcher.nopout((0x8034a76))
    patcher.nopout((0x8034a76+0x2))
    patcher.nopout((0x8034a8c))
    patcher.nopout((0x8034a8c+0x2))
    patcher.nopout((0x8034aa2))
    patcher.nopout((0x8034aa2+0x2))
    patcher.nopout((0x8034ab8))
    patcher.nopout((0x8034ab8+0x2))
    patcher.nopout((0x8034ace))
    patcher.nopout((0x8034ace+0x2))
    patcher.nopout((0x8049f9a))
    patcher.nopout((0x8049f9a+0x2))
    patcher.nopout((0x804a820))
    patcher.nopout((0x804a820+0x2))
    patcher.nopout((0x804aae4))
    patcher.nopout((0x804aae4+0x2))
    patcher.nopout((0x804af70))
    patcher.nopout((0x804af70+0x2))
    patcher.nopout((0x804b9fc))
    patcher.nopout((0x804b9fc+0x2))

    #Marks the version as "md380tools"
    patcher.setwstring(0x080d0b6c,
                       "MD380Tools Ver.");

    #Fix some bad grammar
    patcher.setwstring(0x080f9a94,
                       "No Fix");

    #Change the manufacturer string. *Never Worked...*
#    patcher.setstring(0x080f9588,
#                      "Travis Goodspeed KK4VCZ");
    #Change the device name.
    patcher.setstring(0x080d0eb4,
                      "Patched MD-380/390G");

    # freeing ~200k for code patches
    patcher.ffrange(0x0809bda8,0x80d0614);

    #This mirrors the RESET vector to 0x080C020, for use in booting.
    patcher.setword(0x0800C020,
                    patcher.getword(0x0800C004),
                    0x00000000);

    #This makes RESET point to our stub below.
    patcher.setword(0x0800C004,
                    0x0809bf00+1
    );

    # app start @  0x0809c000
    patcher.sethword(0x0809bf00, 0x4840);
    patcher.sethword(0x0809bf02, 0x2100);
    patcher.sethword(0x0809bf04, 0x3901);
    patcher.sethword(0x0809bf06, 0x4508);
    patcher.sethword(0x0809bf08, 0xd100);
    patcher.sethword(0x0809bf0a, 0x483c);
    patcher.sethword(0x0809bf0c, 0x4700);
                                        
    #Stores the RESET handler for our stub.
    patcher.setword(0x0809bffc,
                    patcher.getword(0x0800C020),
                    0xFFFFFFFF);
                                                                                        
    patcher.export("patched.img")
