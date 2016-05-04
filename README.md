#MD380 USB Tools

by Travis Goodspeed, KK4VCZ

This repository contains tools for working with codeplugs and firmware
of the Tytera MD380, which is also sold under a variety of different
brand names.  The codeplug format is sufficiently similar to the
radios from Connect Systems (CS700, etc) that these tools might
someday be made compatible.

Client Tools:
* `md380-dfu` reads and writes MD380 codeplugs and firmware.
* `md380-tool` communicates with the patched firmware. (Fancy stuff!)

Development Tools:
* `stm32-dfu` modifies firmware for jailbroken devices. (No longer required.)
* `md380-fw` wraps and unwraps devices firmware.
* `md380-gfx` modifies firmware graphics.

These tools are all wrapped into `Makefile`, which will download the
official firmware, patch and flash it.  Run `make flash` after booting
into the recovery bootloader by holding PTT and the button just above
it during power-on.  Once device has been powered on, run `make flashdb`
to write the DMR MARC user's database to SPI Flash memory. This works only
with Radios that have 16 MByte SPI Flash memory chip installed.

##License:##

The majority of this software is licensed in exchange for two liters
of India Pale Ale, to be delivered at a neighborly bar, preferably one
without a god-damned squary-stary-box.

The DFU tools are GPL licensed, forked from old examples in the
Ubertooth Project.  If you want md380-dfu or stm32-dfu under different
terms, you should probably discuss it with the Great Scott Gadgets
folks over some beer.

Tytera's firmware is of unknown license and is not included in this
repository.  We use a heap-less printf library under the BSD license.

##Supported Hardware##

The patched firmware is known to work on the following devices:

* Tytera/TYT MD380
* Retevis RT3

##Specifications:##

* The MD380 uses a custom variant of DFU that isn't quite compatible
  with the spec.  Their code seems to be forked from an STMicro
  example for the STM32 chip.

* Universal Serial Bus Device Class Specification for Device
  Firmware Upgrade, version 1.1:
  http://www.usb.org/developers/devclass_docs/DFU_1.1.pdf


##Requirements:##

* Python 2.7 or newer:
  http://www.python.org

* PyUSB 1.0:  (0.4 does not work.)
  http://sourceforge.net/apps/mediawiki/pyusb/

* libusb 1.0: (0.4 does not work.)
  http://www.libusb.org/

This project should work across Linux, Mac OS, and Windows, but has
not been tested on all platforms.  A separate client, MD380Tool,
is under development for Android.

##Convenient Usage:##

Anything with `md380-tool` requires a recent version of our patched
firmware.  You can check your version in Menu/Utilities/Radio
Info/Version.  If it's a recent date you're good; if it's a number,
you need to upgrade.

To actively watch incoming calls, printing a call log with name and
callsign:

    md380-tool calllog

To dump the recent dmesg log:

    md380-tool dmesg

##Advanced Usage:##

To download a raw (headerless) codeplug into the MD380.

    m380-dfu write <filename.img>

To upload a raw codeplug from the MD380.

    md380-dfu read <filename.img>

To dump the bootloader from the MD380.  (Only in radio mode, only on Mac.)

    md380-dfu readboot <filename.bin>

To exit programming mode, returning to radio mode.

    md380-dfu detach

To extract the raw app binary from an ecrypted Tytera firmware image:

    md380-fw --unwrap MD-380-D2.32\(AD\).bin app.bin

To wrap a raw app binary into a flashable Tytera firmware image:

    md380-fw --wrap app-patched.bin MD-380-D2.32-patched.bin

To export all sprites and glyphs from a raw firmware image:

    md380-gfx --dir=imgout --firmware=patched.bin extract

To re-import a single modified PPM sprite (must restore text header
of the originally exported .ppm file; gimp et al. discard it):

    md380-gfx --firmware=patched.bin --gfx=0x80f9ca8-poc.ppm write

To flash the Ham-DMR UserDB to SPI Flash. **Works only on radios 
with 16MByte SPI-Flash.**
    
    generate the upload file
    
       wc -c < db/users.csv > data ; cat db/users.csv >> data
    
    program to flash with: (very experimental)
    
       md380-tool spiflashwrite data 0x100000


    or (all steps included): (very experimental)

       make flashdb  
    

After successfully flashing, the radio will be restarted.

##Firmware Compilation##

This archive does not ship with firmware.  Instead it grabs firmware
from the Internet, decrypts it, and applies patches to that revision.

You can reproduce the patched firmware with `make clean dist` after
installing an arm-none-eabi cross compiler toolchain.  The firmware
and a Windows flashing tool will then appear in
`md380tools-yyyy-mm-dd`.  Alternately, you can flash them from Linux
with `make clean flash`, after starting the recovery bootloader by
holding PTT and the button above it during a radio reboot.

##Windows Firmware Installation##

You can install any of these patched firmware files into your MD380 by
using the respective .bin file with the Tytera Windows firmware
upgrade tool, "upgrade.exe", available inside their firmware upgrade
downloads. Here are the steps:

* Turn off your MD380 using the volume knob.
* Attach the Tytera USB cable to the SP and MIC ports of your MD380.
* Attach the Tytera USB cable to your host computer.
* Hold down the PTT and the button above the PTT button (*not* the button with the "M" on it).
* Turn on your MD380 using the volume knob.
* Release the buttons on the radio.
* The status LED should be on and alternating between red and green, indicating you're in flash upgrade mode.
* Start the Tytera "Upgrade.exe" program.
* Click "Open Update File" and choose one of the .bin files produced from the process above.
* Click "Download Update File" and wait for the flash update process to finish. It takes less than a minute.
* Turn off your MD380 using the volume knob.
* Disconnect the USB cable from your MD380 and host computer.
* Turn the MD380 back on, and you should see the "PoC||GTFO" welcome screen. You're running patched firmware!

##Chirp Driver:##

Also included is a partial driver for the MD380 in Chirp.  This driver
doesn't yet support the essential DMR features, but it does handle
analog channels and banks well enough to load analog repeaters into
your radio.

This driver can't yet communicate with the radio, so use `md380-dfu
read foo.img` to read an image out of the radio, then open it in Chirp
after installing `chirp/md380.py` as a driver.  Once you've made your
changes, you can load the image back in by running `md380-dfu write
foo.img`.


##More Info##

An article by Travis Goodspeed on jailbreaking the MD380 was released
as PoC||GTFO 10:8.  (`pocorgtfo10.pdf` page 76.)

Pat Hickey has some notes and tools up in his own repository,
https://github.com/pchickey/md380-re
