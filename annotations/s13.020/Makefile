
SYMBOLS=../../applet/src/symbols_s13.020

.PHONY: all
all: radare
	
clean:
	rm -f *.tmp *.img
	
orig.img: 
	make -C ../../firmware unwrapped/S013.020.img
	cp ../../firmware/unwrapped/S013.020.img orig.img
    
sortlink:	
	<$(SYMBOLS) ../conv/sortlink.sh >$(SYMBOLS).tmp
	
#	radare2 -a arm -m 0x0800C000  -b 16 -i flash2.r ../../firmware/unwrapped/D013.020.img

experiment:
	radare2 -a arm -m 0x0800C000  -b 16 -i flash.r ../../applet/merged.img
	
symfiles:
	../conv/lnk2sym-unthumb.sh <../../applet/src/symbols_s13.020 >lsym.tmp
	../conv/ra2sym_strip.sh <flash.r >rsym.tmp

include ../common.mk
