
SYMBOLS=../../applet/src/symbols_d13.020

radare:
	radare2 -a arm -m 0x0800C000  -b 16 -i ../cpu.r -i flash.r ../../firmware/unwrapped/D013.020.img
	
clean:
	rm -f *.tmp
	
sync:
	../conv/upd_r2.sh $(SYMBOLS) flash.r >flash_new.tmp
	../conv/ra2lnk.sh <flash.r >$(SYMBOLS)_new.tmp
	
sortlink:	
	<$(SYMBOLS) ../conv/sortlink.sh >$(SYMBOLS).tmp
	
#	radare2 -a arm -m 0x0800C000  -b 16 -i flash2.r ../../firmware/unwrapped/D013.020.img
	
symfiles:
	../conv/lnk2sym-unthumb.sh <$(SYMBOLS) >lsym.tmp
	../conv/ra2sym_strip.sh <flash.r >rsym.tmp
	
	
experiment:
	radare2 -a arm -m 0x0800C000  -b 16 -i flash.r ../../applet/experiment.img

asm.lst: flash.r Makefile disasm.r
	-mv asm.lst asm1.lst
	radare2 -a arm -m 0x0800C000  -b 16 -i disasm.r -q ../../firmware/unwrapped/D013.020.img
	
ci:
	./conv.sh
