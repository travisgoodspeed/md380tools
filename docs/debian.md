# Debian specifics #

## Preparing build environment ##

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
  
