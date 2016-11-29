

radare: orig.img
	radare2 -a arm -m 0x0800C000  -b 16 -i ../cpu.r -i flash.r orig.img

asm.lst: flash.r Makefile disasm.r orig.img
	-mv asm.lst asm1.lst
	radare2 -a arm -m 0x0800C000  -b 16 -i disasm.r -q orig.img

sync:
	../conv/upd_r2.sh $(SYMBOLS) flash.r >flash_new.tmp
	../conv/ra2lnk.sh <flash.r >$(SYMBOLS)_new.tmp

