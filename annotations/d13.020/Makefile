
radare:
	radare2 -a arm -m 0x0800C000  -b 16 -i flash.r ../../firmware/unwrapped/D013.020.img
	
tmp:
	../conv/conv.sh ../../applet/src/symbols_d13.020 flash.r >flash2.r
#	radare2 -a arm -m 0x0800C000  -b 16 -i flash2.r ../../firmware/unwrapped/D013.020.img
	
experiment:
	radare2 -a arm -m 0x0800C000  -b 16 -i flash.r ../../applet/experiment.img

asm.lst: flash.r Makefile disasm.r
	-mv asm.lst asm1.lst
	radare2 -a arm -m 0x0800C000  -b 16 -i disasm.r -q ../../firmware/unwrapped/D013.020.img
	
clean:
	-rm *.tmp
	
ci:
	./conv.sh
