
OBJS = md380-emu.o ambe.o
CC=arm-linux-gnueabi-gcc-6 -static -g

all: md380-emu

md380-emu: $(OBJS) firmware.o ram.o
	$(CC) -o md380-emu $(OBJS) firmware.o ram.o \
		-Xlinker --just-symbols=../applet/src/symbols_d02.032 \
		-Xlinker --section-start=.firmware=0x0800C000 \
		-Xlinker  --section-start=.sram=0x20000000 
run: all
	rm -f out.raw out.wav
	touch out.raw
	./md380-emu sample.amb
	sox -r 8000 -e signed-integer -L -b 16 -c 1 -v 5 out.raw out.wav
play: run
	mplayer out.wav


dsdplay: all
#This plays the audio in DSD, for comparison.
	dsd -n -w sample.wav -r sample.amb
	mplayer sample.wav
emu: all
	qemu-arm md380-emu sample.amb
clean:
	rm -f *.o *~ md380-emu *.wav *.raw *.elf
firmware.o:
#May need to adjust these flags for NX compatibility.
	arm-linux-gnueabi-objcopy \
		-I binary ../firmware/unwrapped/D002.032.img \
		--change-addresses=0x0800C000 \
		--rename-section .data=.firmware \
		-O elf32-littlearm -B arm firmware.o
ram.o:
	arm-linux-gnueabi-objcopy \
		-I binary ../cores/d02032-core.img \
		--change-addresses=0x20000000 \
		--rename-section .data=.sram \
		-O elf32-littlearm -B arm ram.o



