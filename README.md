#MD380/CS700 USB Tools

by Travis Goodspeed, KK4VCZ

This code is used to upload or download codeplugs into a TYT-MD380 DMR
radio.  Some parts of this can be adapted for the CS700 and CS750,
but that work has yet to be done.

##License:##

The majority of this software is licensed in exchange for two liters
of India Pale Ale, to be delivered at a neighborly bar, preferably
without a a god-damned squary-stary-box.

The DFU tools are GPL licensed, forked from old examples in the
Ubertooth Project.

Tytera's firmware is of unknown license and is not included in this
repository.

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

##Firmware Patching:##

This archive does not ship with firmware.  Instead it grabs firmware
from the Internet, decrypts it, and applies patches to that revision.

You can reproduce the patched firmware with `make clean all`.

Files from this process include:

* unwrapped.img: factory firmware after decrypting.
* prom-public.img and prom-public.bin: patched to monitor all talk groups.
* prom-private.img and prom-private.bin: patched to monitor all talk groups, private calls.
* experiment.img and experiment.bin: patched to monitor all talk groups, private calls, and sideload alternate firmware.

You can install any of these patched firmware files into your MD380 by using the respective .bin file with the Tytera Windows firmware upgrade tool, "upgrade.exe", available inside their firmware upgrade downloads. Here are the steps:

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

An article on reversing the MD380 will be out in the next PoC||GTFO,
sometime in January 2016.

Pat Hickey has some notes and tools up in his own repository,
https://github.com/pchickey/md380-re
