# MD380Tools -- Firmware Patches for the Tytera MD380 #

by Travis Goodspeed, KK4VCZ

The Tytera MD-380 is handheld radio transceiver for DMR and FM.  In
2015, this project began patching that firmware by replacing the byte
of the Chinese font with our own code, fixing bugs in the original
firmware and adding new features that are useful to the amateur radio
community.

# More documentation #

* [Make targets](docs/make.md)
* [md380-tool/md380-dfu](docs/md380tool.md)
* [Jenkins](docs/JenkinsBuilder.md)
* [CI](docs/ContinuousIntegration.md)


# Support #

To support users by using the md380tools or the resulting patched
firmware a Google Group is public opened and reachable via
https://groups.google.com/forum/#!forum/md380tools. No extra
registration should be necessary. You could also feed it via e-mail at
md380tools@googlegroups.com. So feel free to put in your questions
into it!

A few of us are also on the `#md380` IRC channel on Freenode.

A helpful site is available at http://md380.org/

There are also some related groups you may find interesting:
- https://www.facebook.com/groups/KD4ZToolkit/
- https://www.facebook.com/groups/DMRTrack/


# Introduction #

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

## Build Status

[![Build Status](https://travis-ci.org/travisgoodspeed/md380tools.svg?branch=master)](https://travis-ci.org/travisgoodspeed/md380tools)


## Supported Hardware ##

The patched firmware is known to work on the following devices:

* The "D"-Version (NoGPS) for radios **without GPS**
  * Tytera/TYT MD380
  * Tytera/TYT MD390
  * Retevis RT3

* The "S"-Version (GPS) for radios **with GPS**
  * Tytera/TYT MD380
  * Tytera/TYT MD390
  * Retevis RT8

Both types of vocoder (old and new vocoder radios) are supported.

The DMR MARC user's database required a 16 MByte SPI Flash memory chip.
In some VHF Radios is only an 1 MByte SPI Flash installed.

Dual band radios such as the MD2017 and MD-UV380 series are not supported.

## Known models ##

| Name | vocoder | GPS | exp FW | original FW |
|---------|-----|---|---------|---------|
| MD-380  | old | N | D02,D13 | D02,D03 |
| MD-380  | new | N | D02,D13 | D02,D13 |
| MD-380G | new | Y | D02,S13 | S13     |
| MD-390  | new | N | D02,D13 | D13     |
| MD-390G | new | Y | D02,S13 | S13     |

  * RT3 = MD-380 (old)
  * RT8 = MD-390G

## License: ##

This software is licensed in exchange for two liters of India Pale
Ale, to be delivered at a neighborly bar, preferably one without
televisions.

Tytera's firmware is of unknown license and is not included in this
repository.  We use a heap-less printf library under the BSD license.


## Specifications: ##

* The MD380 uses a custom variant of DFU that isn't quite compatible
  with the spec.  Their code seems to be forked from an STMicro
  example for the STM32 chip.

* Universal Serial Bus Device Class Specification for Device
  Firmware Upgrade, version 1.1:
  http://www.usb.org/developers/docs/devclass_docs/DFU_1.1.pdf
  


## Requirements: ##

* Python 2.7 or newer:
  http://www.python.org

* PyUSB 1.0:  (0.4 does not work.)
  https://github.com/pyusb/pyusb

* libusb 1.0: (0.4 does not work.)
  http://www.libusb.org/

* python-requests
  http://python-requests.org/

This project should work across Linux, Mac OS, and Windows, but has
not been tested on all platforms.  A separate client,
[MD380Tool](https://github.com/travisgoodspeed/MD380Tool), was under
development for Android.

### Preparation of build environment ###

* [Debian](docs/debian.md)
* [Ubuntu](docs/ubuntu.md)
* [Fedora](docs/fedora.md)
* [SUSE](docs/suse.md)
* [Raspberry Pi](docs/raspi.md)
* [Windows](docs/windows.md)
* [MacOS](docs/macos.md)

### Additional steps for linux based installations ###

```
git clone https://github.com/travisgoodspeed/md380tools.git
cd md380tools
sudo cp 99-md380.rules /etc/udev/rules.d/ 
```

(The `99-md380.rules` file is copied to `/etc/udev/rules.d/` in order to
allow users to access the radio over USB without having to use sudo or
root permissions.)

### Flash updated firmware for linux based installations ###

Turn on radio in DFU mode to begin firmware update with USB cable:
* change your MD380 language setting to English
* insert cable into USB.
* connect cable to MD380.
* power-on MD380 by turning volume knob, while holding PTT button and button above PTT.

For non-GPS-models do:
```
git pull
make flash
```
For GPS-models do:
```
git pull
make flash_S13
```

### Flash updated users database for linux based installations ###

Turn radio normally on to begin database loading with USB cable

For European users:
```
make updatedb_eur flashdb
```

Note: for European users it is probably illegal to use the other
method for updating, due to privacy laws.  (This is no legal advice,
please consult your lawyer to be sure.)

For the rest of the world:
```
make updatedb flashdb
```

(The `users.csv` file located in the db directory must be refreshed this way, 
with `make updatedb`, otherwise it will continue using any already-existing 
`users.csv` file when running `make flashdb`.)

## Convenient Usage: ##

Anything with `md380-tool` requires a recent version of our patched
firmware.  You can check your version in Menu/Utilities/Radio
Info/Version.  If it's a recent date you're good; if it's a number,
you need to upgrade.


To dump the recent dmesg log, run `md380-tool dmesg`.

## Firmware Compilation ##

This archive does not ship with firmware.  Instead it grabs firmware
from the Internet, decrypts it, and applies patches to that revision.

You can reproduce the patched firmware with `make clean dist` after
installing an arm-none-eabi cross compiler toolchain.  The firmware
and a Windows flashing tool will then appear in
`md380tools-yyyy-mm-dd`.  Alternately, you can flash them from Linux
with `make clean flash`, after starting the recovery bootloader by
holding PTT and the button above it during a radio reboot.

## Windows Firmware Installation ##

You can install any of these patched firmware files into your MD380 by
using the respective .bin file with the Tytera Windows firmware
upgrade tool, `upgrade.exe`, available inside their firmware upgrade
downloads. Here are the steps:

* Change your MD380 language setting to English
* Turn off your MD380 using the volume knob.
* Attach the Tytera USB cable to the SP and MIC ports of your MD380.
* Attach the Tytera USB cable to your host computer.
* Hold down the PTT and the button above the PTT button (*not* the button with the "M" on it).
* Turn on your MD380 using the volume knob.
* Release the buttons on the radio.
* The status LED should be on and alternating between red and green, indicating you're in flash upgrade mode.
* Start the Tytera `Upgrade.exe` program.
* Click "Open Update File" and choose one of the .bin files produced from the process above.
* Click "Download Update File" and wait for the flash update process to finish. It takes less than a minute.
* Turn off your MD380 using the volume knob.
* Disconnect the USB cable from your MD380 and host computer.
* Turn the MD380 back on, and you should see the "PoC||GTFO" welcome screen. You're running patched firmware!

## Codeplug Programming: ##

Reading and writing of raw Codeplug images is supported with the
`md380-dfu` command.  For graphical editing of codeplugs, you should
probably use Tytera's MD80 CPS program or Dale Farnsworth's
[EditCP](https://www.farnsworth.org/dale/codeplug/editcp/).

The beginnings of a plugin for [CHIRP](https://chirp.danplanet.com/)
are also in this repository, but they were never completed.

## More Info ##

Some articles from [PoC||GTFO Volume 2](https://nostarch.com/gtfo2)
* Jailbreaking the MD380, PoC||GTFO 10:8
  ([pocorgtfo10.pdf](https://archive.org/details/pocorgtfo10) page
  76.) by Travis Goodspeed
* Running AMBE Firmware in Linux, PoC||GTFO 13:5
  ([pocorgtfo13.pdf](https://archive.org/details/pocorgtfo13) page
  38.) by Travis Goodspeed

Pat Hickey has some notes and tools up in his own repository,
https://github.com/pchickey/md380-re

The [OpenRTX](https://openrtx.org/) project is making from-scratch
firmware for the MD380 and other radios with the [M17
Mode](https://m17project.org/).


## Customization ##

Previously we customized firmware images with new startup
screens.  This is [presently broken](https://github.com/travisgoodspeed/md380tools/issues/221).

