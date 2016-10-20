# Raspi specifics #

## Preparing build environment ##

####Raspberry Pi Debian Jessie: #####

```
Tested on 2016-05-10-raspbian-jessie by IZ2XBZ

sudo apt-get install gcc-arm-none-eabi binutils-arm-none-eabi libusb-1.0 \
             libnewlib-arm-none-eabi python-usb make curl

sudo pip install pyusb -U
