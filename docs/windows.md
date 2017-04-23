# Windows specifics #

[GNUwin32 instructions](gnuwin32.md)


## Preparing build environment ##

####Windows (using MSYS2):####

Direct USB access has not yet been tested on Windows, and will not work with these instructions. Stay tuned for updates here.
Manipulating the firmware and compiling the patches is supported, and instructions follow.

Install MSYS2 from https://msys2.github.io, and update it by following the instructions on the homepage.
Install needed MSYS2 dependencies:

    pacman -S git make unzip zip perl python2

Restart the MSYS2 shell to ensure default paths are updated.
Download the latest [GNU ARM Embedded Toolchain](https://launchpad.net/gcc-arm-embedded) and unpack to a desired location, ideally without spaces in the path.
Clone the repo to a desired location.
In MSYS2, the C drive is located at ```/c/```. Add the ARM toolchain bin directory to the MSYS2 path:

    export PATH=$PATH:/c/path/to/gcc-arm-embedded/bin

To make the PATH variable permanent, add the above line to the .bashrc 
file located at ~/.bashrc

If you are interested in using radare2 on Windows, [download it](http://www.radare.org/) and unpack to a desired location. As radare2 does not currently work in the MSYS2 shell, it is suggested to run the commands in the Makefile directly from the CMD shell:

    cd \path\to\md380tools\annotations\2.032
	\path\to\radare2.exe -a arm -m 0x0800C000 -b 16 -i flash.r flash.img
