IMAGE := D002.032.img

all: open

clean:
	rm -f flash.img patch.img

flash.img:
	cd ../../firmware && $(MAKE) unwrapped/$(IMAGE)
	cp ../../firmware/unwrapped/$(IMAGE) flash.img
open: flash.img
	r2 -a arm -m 0x0800C000 -b 16 -i flash.r $<
patcher: flash.img
	cp $< patch.img
	r2 -a arm -m 0x0800C000 -b 16 -w -i flash.r patch.img

