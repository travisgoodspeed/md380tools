
SYMBOLS=../../applet/src/symbols_d13.020

.PHONY: all
all: radare
	
clean:
	rm -f *.tmp *.img
	
orig.img: 
	make -C ../../firmware unwrapped/D013.020.img
	cp ../../firmware/unwrapped/D013.020.img orig.img
    
	
sortlink:	
	<$(SYMBOLS) ../conv/sortlink.sh >$(SYMBOLS).tmp
	
#	radare2 -a arm -m 0x0800C000  -b 16 -i flash2.r ../../firmware/unwrapped/D013.020.img
	
symfiles:
	../conv/lnk2sym-unthumb.sh <$(SYMBOLS) >lsym.tmp
	../conv/ra2sym_strip.sh <flash.r >rsym.tmp
	
	
experiment:
	radare2 -a arm -m 0x0800C000  -b 16 -i flash.r ../../applet/experiment.img

ci:
	./conv.sh

include ../common.mk
