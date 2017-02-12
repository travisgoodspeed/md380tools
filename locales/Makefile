
all: localize hu it de es
clean:
	rm -f localize *.bin *.img

applet:
	cd ../applet && make
hu: localize applet
	cat strings.txt | ./localize hu ../applet/merged.img D013.020-hu.img
	make D013.020-hu.bin

it: localize applet
	cat strings.txt | ./localize it ../applet/merged.img D013.020-it.img
	make D013.020-it.bin

de: localize applet
	cat strings.txt | ./localize de ../applet/merged.img D013.020-de.img
	make D013.020-de.bin

es: localize applet
	cat strings.txt | ./localize es ../applet/merged.img D013.020-es.img
	make D013.020-es.bin


flash-hu: hu
	../md380-dfu upgrade D013.020-hu.bin

flash-it: it
	../md380-dfu upgrade D013.020-it.bin

flash-de: de
	../md380-dfu upgrade D013.020-de.bin

flash-es: es
	../md380-dfu upgrade D013.020-es.bin

%.bin: %.img
	../md380-fw --wrap $< $@
run: test
