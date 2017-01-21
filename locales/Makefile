CC=clang


all: localize
clean:
	rm -f localize *.bin *.img
test: localize
	cat strings.txt | ./localize hu ../firmware/unwrapped/S013.020.img S013.020-hu.img
	cat strings.txt | ./localize hu ../firmware/unwrapped/D013.020.img D013.020-hu.img
	make S013.020-hu.bin D013.020-hu.bin
flash-hu-gps: test
	../md380-dfu upgrade S013.020-hu.bin
flash-hu: test
	../md380-dfu upgrade D013.020-hu.bin
%.bin: %.img
	../md380-fw --wrap $< $@
run: test
