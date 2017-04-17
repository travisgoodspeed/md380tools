rem File  : C:\tools\md380tools\applet\disasm_applet.bat
rem Author: Wolf, DL4YHF 
del listing.txt
rem C:\tools\Radare2\Radare2.exe -a arm -m 0x0800C000 -b 16 -i disasm_applet.r merged.img
C:\tools\Radare2\Radare2.exe -a arm -m 0x0800C000 -b 16 -i disasm_applet.r applet.elf
pause