

radare: orig.img
	radare2 -a arm -m 0x0800C000  -b 16 -i ../cpu.r -i flash.r orig.img
	
