
all: patches
clean:
	rm -f *~ *.bin *.img

unwrapped.bin:
	cd ../../firmware && make D002.032.bin
	../../md380-fw --unwrap ../../firmware/D002.032.bin unwrapped.img

patches: unwrapped.bin
	./patch.py

wrapped: patches
	../../md380-fw --wrap prom-public.img prom-public.bin
	../../md380-fw --wrap prom-private.img prom-private.bin


flash: patches
	sudo stm32-dfu erase
	sleep 25
	#TODO Replace this with a custom bootloader.
	sudo stm32-dfu writeflash ~/jailbreak.bin
	sudo stm32-dfu write experiment.img 0x0800C000

