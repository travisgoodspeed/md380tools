
radare:
	radare2 -a arm -m 0x0800C000  -b 16 -i flash.r ../../firmware/unwrapped/D013.020.img
	
experiment:
	radare2 -a arm -m 0x0800C000  -b 16 -i flash.r ../../applet/experiment.img

asm.lst:
	radare2 -e search.from=0 -i disasm.script -q -a arm -m 0x0800C000  -b 16 -i flash.r ../../firmware/unwrapped/D013.020.img
	