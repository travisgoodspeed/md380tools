
all: patched.bin
clean:
	rm -f *~ *.bin *.img

unwrapped.img:
	cd ../../firmware && $(MAKE) unwrapped/D013.020.img
	cp ../../firmware/unwrapped/D013.020.img $@

patched.img: unwrapped.img
	./patch.py
	../../md380-gfx --firmware=patched.img --gfx=replacement-font-big-latin.pbm fontreplace
	../../md380-gfx --firmware=patched.img --gfx=replacement-font-small-latin.pbm fontreplace
               

patched.bin: patched.img
	../../md380-fw --wrap patched.img patched.bin

flash: patched.bin
	../../md380-dfu upgrade patched.bin

