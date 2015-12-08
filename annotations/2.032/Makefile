
all: open

clean:
	rm -f flash.img patch.img

flash.img:
	cd ../../firmware && make D002.032.bin
	../../md380-fw --unwrap ../../firmware/D002.032.bin flash.img
open: flash.img
	r2 -a arm -m 0x0800C000 -b 16 -i flash.r flash.img 
patcher: flash.img
	cp flash.img patch.img
	r2 -a arm -m 0x0800C000 -b 16 -w -i flash.r patch.img

