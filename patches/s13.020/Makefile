
all: patched.bin
clean:
	rm -f *~ *.bin *.img *.pyc

unwrapped.img:
	cd ../../firmware && "${MAKE}" unwrapped/S013.020.img
	cp ../../firmware/unwrapped/S013.020.img $@

patched.img: unwrapped.img
	./patch.py
	../../md380-gfx --firmware=patched.img --gfx=replacement-font-big-latin.pbm fontreplace
	../../md380-gfx --firmware=patched.img --gfx=replacement-font-small-latin.pbm fontreplace

patched.bin: patched.img
	../../md380-fw --wrap patched.img patched.bin


#Use this to flash a physically jailbroken radio.
#You must have jailbreak.bin in ~/
dfuflash: patched.img
	stm32-dfu erase
	sleep 25
	stm32-dfu writeflash ~/jailbreak.bin
	stm32-dfu write patched.img 0x0800C000

#Use this to flash an unmodified radio.
#No soldering or jailbreak.bin necessary.
flash: patched.bin
	../../md380-dfu upgrade patched.bin

