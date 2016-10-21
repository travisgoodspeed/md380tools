
####SUSE LEAP (42.1/42.2) 

as root (obviously) -- most packages may already be installed.

cp 99-md380.rules /etc/udev/rules.d/ 
usermod -a -G dialout <your_username>
zypper ar  -t yum  -c http://mirror.nl.leaseweb.net/fedora/linux/releases/24/Everything/x86_64/os/ fedora24
rpm --import https://getfedora.org/static/81B46521.txt
zypper in -y git make unzip curl libusb-1_0-0 python-pip pyusb gcc-arm-linux-gnu arm-none-eabi-newlib arm-none-eabi-gcc-cs-c++
pip install --upgrade pip
 
reason for using fedora24 for the crosscompiler is that the suse one 
is for a slightly different hw platform casing linker issues with
the floating point stuff. Should be 'hard' but is 'soft' 

(you will see errors like: main.o uses VFP register arguments, main.elf does not .....)


