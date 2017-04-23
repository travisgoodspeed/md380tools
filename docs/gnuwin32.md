# GNUwin32 instructions #

Install git for windows from: https://git-scm.com/download/win
Select 'use windows default console window' near the end of the install - this step may not be required

Download and install make for windows from: http://gnuwin32.sourceforge.net/packages/make.htm

Download and install python 2.7 from: https://www.python.org/downloads/
In the `C:\Python27` folder, make a copy of `python.exe` and name it `python2.exe`.

Add the following two lines to your path variable
(Right-click Start button -> System -> Advanced System Settings -> Environment Varables -> Select Path -> Edit):
`C:\Progra~1\GnuWin32\bin`  [use `Progra~1` (`Progra~2` if 64-bit Windows) and NOT `Program Files` otherwise `make` will fail]
`C:\Python27`

Download and install gcc-arm-none-eabi from: https://launchpad.net/gcc-arm-embedded/4.8/4.8-2014-q1-update
At the end of the install, tick the 'add path to environment variable' box before clicking finish.

Reboot your PC

Download and extract PyUSB from: https://sourceforge.net/projects/pyusb/    
Open the extracted folder, hold shift and right click -> open command window here    
Type: `python setup.py install`

Download and extract libusb-win32 from: https://sourceforge.net/projects/libusb-win32/
Open the extracted folder and enter the `bin` folder
Plug programming cable into USB port and radio. While holding top button & PTT, switch on radio,
run inf-wizard.exe -> Next    
select digital radio in USB mode -> Next -> Next    
Save the .inf file    
Click Install Now...

Run Git (from start menu)    
Select 'Clone Existing Repository'

Source: http://github.com/travisgoodspeed/md380tools
Target: C:/Users/<username>/Documents/git/travisgoodspeed/md380tools

Once cloned:
Find `md380tools` folder, right-click -> Git Bash Here

To build and flash experimental firmware (make sure radio in firmware download mode):

    make clean flash

To flash user database (radio in normal mode):

    make flashdb

Before running `make flashdb`, run the file `{extracted location}\libusb-win32-bin-1.2.6.0\bin\x86\testlibusb-win.exe` after plugging in and switching on the radio. This needs to be done every time you wish to flash the user database, otherwise the SPI flash ID will be wrong.
