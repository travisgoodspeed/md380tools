SRCS = main.c usb.c gfx.c dmr.c printf.c dmesg.c stm32f4xx_it.c system_stm32f4xx.c
PROJ_NAME=main


###################################################

CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy
OBJDUMP=arm-none-eabi-objdump

CFLAGS  = -g -O2 -Wall -Tmd380_applet.ld 
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4 -mthumb-interwork
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS += -fshort-wchar

###################################################

vpath %.c src
vpath %.a lib

ROOT=$(shell pwd)

CFLAGS += -Iinc -Ilib -Ilib/inc -I.
CFLAGS += -Ilib/inc/core -Ilib/inc/peripherals 

SRCS += lib/startup_stm32f4xx.s # add startup file to build
SRCS += src/md380-2.032.c             # MD380 library functions.

OBJS = $(SRCS:.c=.o)


###################################################

.PHONY: lib proj pre

all: lib proj wrapped

experiment.img:
	cd ../patches/2.032 && make all
	cp ../patches/2.032/experiment.img ./


lib: src/version.h
	$(MAKE) -C lib

proj: 	$(PROJ_NAME).elf experiment.img
	python2 merge.py experiment.img $(PROJ_NAME).img 0x0809D000

#The wrapped file is the one that works with the Windows updater.
wrapped: lib proj
	../md380-fw --wrap experiment.img experiment.bin
#This flashses from unix.
flash: wrapped
	../md380-dfu upgrade experiment.bin

$(PROJ_NAME).elf: $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@ -Llib -lstm32f4
	$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).img
	$(OBJDUMP) -t main.elf >main.img.sym

#Temporary file containing the build date.
#Might use version numbers when we're ready for releases.
src/version.h: src/*.c
	echo "#define VERSIONDATE L\"`date "+%Y-%m-%d"`\"" >src/version.h
clean:
	$(MAKE) -C lib clean
	rm -f *.img *.bin *.elf *.hex *~ *.sym src/*~ src/version.h



