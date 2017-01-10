
OBJS = md380-emu.o ambe.o
CC=arm-linux-gnueabi-gcc -static -g

all: md380-emu
clean:
	rm -f *.o *~ md380-emu *.wav *.raw *.elf


md380-emu: $(OBJS) firmware.o ram.o
	$(CC) -o md380-emu $(OBJS) firmware.o ram.o \
		-Xlinker --just-symbols=../applet/src/symbols_d02.032 \
		-Xlinker --section-start=.firmware=0x0800C000 \
		-Xlinker  --section-start=.sram=0x20000000 
run: all
	rm -f out.raw out.wav
	./md380-emu -d -i sample.amb -o out.raw 
	sox -r 8000 -e signed-integer -L -b 16 -c 1 -v 5 out.raw out.wav
play: run
	mplayer out.wav

torvalds-says-linux.wav:
	wget http://suseroot.com/misc/torvalds-says-linux.wav

demo: torvalds-says-linux.wav md380-emu
	rm -f torvalds-says-linux.amb
	rm -f torvalds-says-linux-dsd.wav
	rm -f torvalds-says-linux.md380-emu.raw
	rm -f torvalds-says-linux.md380-emu.wav

	sox torvalds-says-linux.wav --bits 16 --encoding signed-integer -c1 -r 8000 -L torvalds-says-linux.raw
	./md380-emu -e -i torvalds-says-linux.raw -o torvalds-says-linux.amb
	dsd -n -w torvalds-says-linux-dsd.wav -r torvalds-says-linux.amb
	./md380-emu -d -i torvalds-says-linux.amb -o torvalds-says-linux.md380-emu.raw
	sox -r 8000 -e signed-integer -L -b 16 -c 1 torvalds-says-linux.md380-emu.raw torvalds-says-linux.md380-emu.wav

	echo -e "\n\nplaying orignal\n\n"
	mplayer	-msglevel all=-1 torvalds-says-linux.wav 2>/dev/null
	echo -e "\n\nplaying decoded by dsd ----- AMB -----\n\n"
	mplayer -msglevel all=-1 torvalds-says-linux-dsd.wav 2>/dev/null
	echo -e "\n\nplaying decoded by md380-emu ----- AMB -----\n\n"
	mplayer -msglevel all=-1 torvalds-says-linux.md380-emu.wav 2>/dev/null

dsdplay: all
#This plays the audio in DSD, for comparison.
	dsd -n -w sample.wav -r sample.amb
	mplayer sample.wav

firmware.o:
#First we make sure the firmware is ready.
	"${MAKE}" -C ../firmware
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



