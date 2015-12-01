#MD380/CS700 USB Tools

by Travis Goodspeed, KK4VCZ  
forked from Ubertooth code by Jared Boone

This code is used to upload or download codeplugs into a TYT-MD380 DMR
radio.  I believe it will also work for the CS700, but have not tested
that.

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
not been tested on all platforms.

##Usage:##

To download a raw (headerless) codeplug into the MD380.

    m380-dfu write <filename.img>

To upload a codeplug from the MD380.

    md380-dfu read <filename.img>

To dump the bootloader from the MD380.  (Only in radio mode.)

    md380-dfu readboot <filename.bin>

To exit programming mode, returning to radio mode.

    md380-dfu detach

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

An article on reversing the MD380 will be out in the next PoC||GTFO,
sometime in January 2016.

Pat Hickey has some notes and tools up in his own repository,
https://github.com/pchickey/md380-re