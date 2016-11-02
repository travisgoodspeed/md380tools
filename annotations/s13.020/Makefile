
radare:
	radare2 -a arm -m 0x0800C000  -b 16 -i flash.r ../../firmware/unwrapped/S013.020.img
	
asm.lst: flash.r Makefile disasm.r
	-mv asm.lst asm1.lst
	radare2 -a arm -m 0x0800C000  -b 16 -i disasm.r -q ../../firmware/unwrapped/S013.020.img

sync:
	../conv/upd_r2.sh ../../applet/src/symbols_s13.020 flash.r >flash2.tmp
	../conv/mklink.sh flash.r >link.tmp
#	radare2 -a arm -m 0x0800C000  -b 16 -i flash2.r ../../firmware/unwrapped/D013.020.img
	
