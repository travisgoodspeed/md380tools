# bootloader.r by Travis Goodspeed

# This is a Radare2 script for annotating the Tytera MD380 bootloader,
# or my jailbreak derived from that bootloader.  I've tried to make
# this as human-readable as possible, but also to only include those
# symbols which are absolutely necessary to understand the patch.

# Begin by opening the bootloader or jailbreak in r2 with this script.
# r2 -a arm -m 0x08000000 -b 16 -i bootloader.r bootloader.bin

# MD5 (bootloader.bin) = 721df1f98425b66954da8be58c7e5d55
# MD5 (jailbreak.bin)  = 32931e5cf5e62400b31a80b1efcd2686


# Define these three functions which relate to the Readout Device
# Protection feature.

CCa 0x08001fb0 rdp_lock(0x55) locks the device, rdp_lock(0xAA) unlocks it.
af+ 0x08001fb0 24 rdp_lock

CCa 0x08001fc8 After calling rdp_lock(), rdp_applylock() sets the state.
af+ 0x08001fc8 28 rdp_applylock

CCa 0x08001fe4 Returns 1 if RDP is not locked.  0 if it is locked.
af+ 0x08001fe4 22 rdp_isnotlocked


# These are child functions, which make things a bit easier to read.

CCa 0x08002060 Waits for a Flash operation to complete.
af+ 0x08002060 40 flash_wait

CCa 0x080049e8 Tests the pins to stay, or not stay, in bootloader mode.
af+ 0x080049e8 98 bootloader_pin_test

# Inside of main(), rdp_lock(0x55) is conditionally called if
# rdp_isnotlocked().  My first jailbreak worked by simply patching
# this to call rdp_lock(0xAA), which leaves the device unlocked.

CCa 0x080043bc This is the main() function of the bootloader.
af+ 0x080043bc 388 main

CCa 0x080044a8 Change this immediate from 0x55 to 0xAA to jailbreak the bootloader.

# This prints the relevant piece of code in main() that is patched to
# jailbreak the bootloader, leaving Readout Device Production (RDP)
# disabled.

# [0x08000000]> pd 8 @ 0x080044a0 
#     0x080044a0    fdf7a0fd       bl rdp_isnotlocked
#     0x080044a4    0028           cmp r0, 0
# ,=< 0x080044a6    04d1           bne 0x80044b2
# |   ; Change this immediate from 0x55 to 0xAA to jailbreak the bootloader.                
# |   0x080044a8    5520           movs r0, 0x55
# |   0x080044aa    fdf781fd       bl rdp_lock
# |   0x080044ae    fdf78bfd       bl rdp_applylock
# `-> 0x080044b2    fdf776fd       bl 0x8001fa2
#     0x080044b6    00f097fa       bl bootloader_pin_test
# [0x08000000]> 


# Inside of bootloader_pin_test, the I/O pins for the push-to-talk
# button and the button above are tested.

CCa 0x8003af2 Tests pin r1 of port r0.
af+ 0x8003af2 22 gpio_input_test
CCa 0x8002384 Starts DFU recovery mode.
af+ 0x8002384 68 bootloader_setup

# Comments inside bootloader_pin_test
CCa 0x080049f2 Test the first button.
CCa 0x080049fe Test the second button.
CCa 0x08004a36 Calls the address stored at 0x800C004, the reset vector of the application.
CCa 0x08004a2e Set the stack pointer to the value at 0x0800C000.
