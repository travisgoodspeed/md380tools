rem File  : C:\tools\md380tools\annotations\d13.020\disasm_yhf.bat
rem Author: Wolf, DL4YHF 
del listing.txt
C:\tools\Radare2\Radare2.exe -a arm -m 0x0800C000 -b 16 -i disasm_yhf.r ../../firmware/unwrapped/D013.020.img
rem Something was severely bugged, when Radare2 redirected its output under windoze 'cmd.exe' .
rem Unbelievable but true: EVERY LINE IN THE TEXT FILES ENDED WITH "\n\r" instead of "\r\n" or just "\n" .
rem This Python script (in the same folder as this ancient batch) fixes these crappy "text files" :
python freplace.py listing.txt "\n\r" "\r\n"
python disasm2htm.py listing
pause