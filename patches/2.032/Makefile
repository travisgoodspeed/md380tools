
all: patched.bin
clean:
	rm -f *~ *.bin *.img

unwrapped.img:
	cd ../../firmware && make D002.032.bin
	../../md380_fw.py --unwrap ../../firmware/D002.032.bin unwrapped.img

patched.img: unwrapped.img
	./patch.py

#Apply the logo.
	../../md380_gfx.py --firmware=patched.img --gfx=0x80f9ca8-poc-gs.ppm relocate
	#../../md380_gfx.py --firmware=patched.img --gfx=0x80f9ca8-poc.ppm write
	#../../md380_gfx.py --firmware=patched.img --gfx=0x80f9ca8-shmoo.ppm write
	#../../md380_gfx.py --firmware=patched.img --gfx=0x80f9ca8-shmoocon.ppm write
	#../../md380_gfx.py --firmware=patched.img --gfx=0x80f9ca8-volnet.ppm write
	#../../md380_gfx.py --firmware=patched.img --gfx=0x80f9ca8-troopers-color.ppm relocate

#Apply the font.
	../../md380_gfx.py --firmware=patched.img --gfx=replacement-font-big-latin.pbm fontreplace
	../../md380_gfx.py --firmware=patched.img --gfx=replacement-font-small-latin.pbm fontreplace

patched.bin: patched.img
	../../md380_fw.py --wrap patched.img patched.bin


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
	../../md380_dfu.py upgrade patched.bin

