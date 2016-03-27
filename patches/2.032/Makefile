
all: wrapped
clean:
	rm -f *~ *.bin *.img

unwrapped.bin:
	cd ../../firmware && make D002.032.bin
	../../md380-fw --unwrap ../../firmware/D002.032.bin unwrapped.img

patches: unwrapped.bin
	./patch.py

logo: patches
	#../../md380-gfx --firmware=experiment.img --gfx=0x80f9ca8-poc-gs.ppm relocate
	#../../md380-gfx --firmware=expriment.img --gfx=0x80f9ca8-poc.ppm write
	#../../md380-gfx --firmware=experiment.img --gfx=0x80f9ca8-shmoo.ppm write
#	../../md380-gfx --firmware=experiment.img --gfx=0x80f9ca8-shmoocon.ppm write
	#../../md380-gfx --firmware=experiment.img --gfx=0x80f9ca8-volnet.ppm write
#	../../md380-gfx --firmware=experiment.img --gfx=0x80f9ca8-flauschfunk-color.ppm relocate
	../../md380-gfx --firmware=experiment.img --gfx=a.ppm relocate

#	../../md380-gfx --firmware=experiment.img --gfx=0x80f9ca8-troopers-color.ppm relocate
#	../../md380-gfx --firmware=experiment.img --gfx=0x80f9ce4.ppm write
#	../../md380-gfx --firmware=experiment.img --gfx=0x80f9cf8.ppm write

font: patches
	../../md380-gfx --firmware=experiment.img --gfx=replacement-font-big-latin.pbm fontreplace
	../../md380-gfx --firmware=experiment.img --gfx=replacement-font-small-latin.pbm fontreplace

wrapped: logo font
	../../md380-fw --wrap experiment.img experiment.bin


#Use this to flash a physically jailbroken radio.
#You must have jailbreak.bin in ~/
dfuflash: patches
	stm32-dfu erase
	sleep 25
	stm32-dfu writeflash ~/jailbreak.bin
	stm32-dfu write experiment.img 0x0800C000

#Use this to flash an unmodified radio.
#No soldering or jailbreak.bin necessary.
flash: wrapped
	../../md380-dfu upgrade experiment.bin

