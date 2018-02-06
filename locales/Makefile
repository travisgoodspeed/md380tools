CFLAGS = -g -Wall -std=c11

all: localize hu it de es sr fr
clean:
	rm -f localize *.bin *.img

applet:
	cd ../applet && "${MAKE}"
hu: localize applet
	cat strings.txt | ./localize hu ../applet/merged.img D013.020-hu.img
	"${MAKE}" D013.020-hu.bin

it: localize applet
	cat strings.txt | ./localize it ../applet/merged.img D013.020-it.img
	"${MAKE}" D013.020-it.bin

de: localize applet
	cat strings.txt | ./localize de ../applet/merged.img D013.020-de.img
	"${MAKE}" D013.020-de.bin

es: localize applet
	cat strings.txt | ./localize es ../applet/merged.img D013.020-es.img
	"${MAKE}" D013.020-es.bin

sr: localize applet
	cat strings.txt | ./localize sr ../applet/merged.img D013.020-sr.img

fr: localize applet
	cat strings.txt | ./localize fr ../applet/merged.img D013.020-fr.img
	"${MAKE}" D013.020-fr.bin

flash-hu: hu
	../md380-dfu upgrade D013.020-hu.bin

flash-it: it
	../md380-dfu upgrade D013.020-it.bin

flash-de: de
	../md380-dfu upgrade D013.020-de.bin

flash-es: es
	../md380-dfu upgrade D013.020-es.bin

flash-sr: sr
	../md380-dfu upgrade D013.020-sr.bin

flash-fr: fr
	../md380-dfu upgrade D013.020-fr.bin

%.bin: %.img
	../md380-fw --wrap $< $@
run: test
