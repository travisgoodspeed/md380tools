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

###The md380tools have D13.020 as basic now.###

to build the "old" d02.32 version

	make flash_d02.032

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

* Tytera/TYT MD380 (old vocoder)
* Tytera/TYT MD380 (new vocoder)
* Tytera/TYT MD390 (new vocoder, no gps)
* Retevis RT3

##Specifications:##

* The MD380 uses a custom variant of DFU that isn't quite compatible
  with the spec.  Their code seems to be forked from an STMicro
  example for the STM32 chip.

* Universal Serial Bus Device Class Specification for Device
  Firmware Upgrade, version 1.1:
  http://www.usb.org/developers/docs/devclass_docs/DFU_1.1.pdf
  


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

###Installation of required packages###
####Debian Stretch:####

    apt-get install gcc-arm-none-eabi binutils-arm-none-eabi libnewlib-arm-none-eabi \
                    libusb-1.0 python-usb make curl

####Debian Jessie (using backports.debian.org):####

Add backports to your sources.list

    deb http://ftp.debian.org/debian jessie-backports main

to your sources.list (or add a new file with the ".list" extension to /etc/apt/sources.list.d/)
You can also find a list of other mirrors at https://www.debian.org/mirror/list
More info at http://backports.debian.org/Instructions/

     apt update

Install python-usb from backports, the rest from Jessie

     apt -t jessie-backports install python-usb
     apt install gcc-arm-none-eabi binutils-arm-none-eabi libnewlib-arm-none-eabi \
                 libusb-1.0 make curl

####Debian Jessie (using python-pip):####

    apt-get install gcc-arm-none-eabi binutils-arm-none-eabi libnewlib-arm-none-eabi \
            libusb-1.0 git make curl python-pip unzip make curl
    pip install pyusb -U # update PyUSB to 1.0
  

####Ubuntu Xenial (16.04.1): ####

    sudo apt-get update
    sudo apt-get dist-upgrade
    sudo apt-get install git gcc-arm-none-eabi binutils-arm-none-eabi python-usb libnewlib-arm-none-eabi make curl

Quick recipe for building new firmware:

    git clone https://github.com/travisgoodspeed/md380tools.git
    cd md380tools
    sudo cp 99-md380.rules /etc/udev/rules.d/ 
    make

Quick recipe for uploading the just build firmware:

* insert cable into USB.
* connect cable to MD380.
* power-on MD380 while holding PTT button and button above PTT.

```
./md380-dfu upgrade applet/experiment.bin
```

For reverse engineering:

    sudo apt-get install radare2 radare2-plugins


####Raspberry Pi Debian Jessie: #####

```
Tested on 2016-05-10-raspbian-jessie by IZ2XBZ

sudo apt-get install gcc-arm-none-eabi binutils-arm-none-eabi libusb-1.0 \
             libnewlib-arm-none-eabi python-usb make curl

sudo pip install pyusb -U

git clone https://github.com/travisgoodspeed/md380tools.git

cd md380tools

sudo make clean

##### turn on radio in DFU mode to begin firmware update with USB cable ######

sudo make all flash

##### turn radio normally on to begin database loading with USB cable #####

sudo make flashdb
```

####Windows (using MSYS2):####

Direct USB access has not yet been tested on Windows, and will not work with these instructions. Stay tuned for updates here.
Manipulating the firmware and compiling the patches is supported, and instructions follow.

Install MSYS2 from https://msys2.github.io, and update it by following the instructions on the homepage.
Install needed MSYS2 dependencies:

    pacman -S make unzip perl python2

Restart the MSYS2 shell to ensure default paths are updated.
Download the latest [GNU ARM Embedded Toolchain](https://launchpad.net/gcc-arm-embedded) and unpack to a desired location, ideally without spaces in the path.
Clone the repo to a desired location.
In MSYS2, the C drive is located at ```/c/```. Add the ARM toolchain bin directory to the MSYS2 path:

    export PATH=$PATH:/c/path/to/gcc-arm-embedded/bin

If you are interested in using radare2 on Windows, [download it](http://www.radare.org/) and unpack to a desired location. As radare2 does not currently work in the MSYS2 shell, it is suggested to run the commands in the Makefile directly from the CMD shell:

    cd \path\to\md380tools\annotations\2.032
	\path\to\radare2.exe -a arm -m 0x0800C000 -b 16 -i flash.r flash.img

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

##Flashing on Linux Notes##

* Please ensure the 99-md380.rules file is copied to /etc/udev/rules.d/ in order to allow users to access the radio over USB without having to use sudo or root permissions.

Special note on the users.csv flashdb process:
* The users.csv file located in the db directory must be manually refreshed by running "make clean" while inside the db directory otherwise it will continue using any already-existing users.csv file when running "make flashdb" from the main md380tools directory. 

To check the type / size of SPI-Flash

    md380-tool spiflashid    

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



## Customization ##
Temponary not avaible, see https://github.com/travisgoodspeed/md380tools/issues/221


<strike>
Images extracted from the firmware have comments at the beginning of
the file, telling md380-gfx where they came from. Comments begin with a '#', and end with a new line.

Image editors like GIMP will discard the original comments, but you can
replace them by opening the file in a text editor and copy-pasting the
comment lines from the original extracted file to your custom image.


#### Boot logo ####

There are several boot logos provided that you can choose from by editing
`patches/2.032/Makefile`, and commenting/uncommenting as you see fit.

The original boot logo is 160x40 pixels, and only 2 colors.  This means
an image that has the same properties can be written into the firmware
as a direct replacement, as seen in the Makefile.

An image with more than two colors requires the "relocate" argument to
md380-gfx. There are examples of this in the Makefile as well.
</strike>
## Support ##

To support users by using the md380tools or the resulting patched firmware 
a Google Group is public opened and reachable via 
https://groups.google.com/forum/#!forum/md380tools. No extra registration 
should be necessary. You could also feed it via e-mail at 
md380tools@googlegroups.com. So feel free to put in your questions into it!
