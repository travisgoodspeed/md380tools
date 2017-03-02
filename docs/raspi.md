# Raspi specifics #

## Preparing build environment ##

####Raspberry Pi Debian Jessie: #####

```
Tested on 2016-05-10-raspbian-jessie by IZ2XBZ

sudo apt-get install gcc-arm-none-eabi binutils-arm-none-eabi libusb-1.0 \
             libnewlib-arm-none-eabi python-usb make curl

sudo pip install pyusb -U
```
```
Tested on 2017-03-02-raspbian-jessie by YT5ZEC (dragstor)
In addition to the notes above, install the following pip package, in order to get cable to work.

sudo pip install pyusb==1.0.0b1
```
