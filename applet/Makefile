SRCS  = main.o printf.o dmesg.o gfx.o stm32f4xx_it.o system_stm32f4xx.o addl_config.o 
SRCS += spiflash.o usb.o menu.o rtc_timer.o beep_process.o usersdb.o os.o aes.o
SRCS += ambe.o  dmr.o
#### SRCS +=  display.o  # display flip is included @ d13.020
#### SRCS +=  i2c.o #  ALPU Licence Check is disabled, so we have no i2c

PROJ_NAME=main


###################################################

CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy
OBJDUMP=arm-none-eabi-objdump

GIT_VERSION := $(shell git describe --abbrev=4 --dirty --always --tags)
SRCVERSION?=d13.020

LINKSCRIPT=applet_$(SRCVERSION).ld

CFLAGS += -std=gnu99 -g -O2 -Wall -T$(LINKSCRIPT)
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4 -mthumb-interwork
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS += -fshort-wchar
CFLAGS += -DGIT_VERSION=\"$(GIT_VERSION)\"
CFLAGS += -Wa,-a=$(@:.o=.lst)
# TODO: make this dynamic	
CFLAGS += -DFW_D13_020=1	
###################################################

vpath %.c src
vpath %.a lib

ROOT=$(shell pwd)

CFLAGS += -Iinc -Ilib -Ilib/inc -I.
CFLAGS += -Ilib/inc/core -Ilib/inc/peripherals
CFLAGS += -D "MD380_$(subst .,_,$(SRCVERSION))"  # ISO C99 requires whitespace after

SRCS += lib/startup_stm32f4xx.s # add startup file to build

OBJS = $(SRCS:.c=.o)


###################################################

.PHONY: lib proj pre

all: lib proj wrapped

experiment.img: 
	cd ../patches/$(SRCVERSION) && $(MAKE) all
	cp ../patches/$(SRCVERSION)/patched.img ./experiment.img


lib: src/version.h config.h
	$(MAKE) -C lib

proj: 	$(PROJ_NAME).elf experiment.img
	# use a fresh experiment.img
	cd ../patches/$(SRCVERSION) && $(MAKE) all
	cp ../patches/$(SRCVERSION)/patched.img ./experiment.img
	python2 merge_$(SRCVERSION).py experiment.img $(PROJ_NAME).img 0x0809b000

#The wrapped file is the one that works with the Windows updater.
wrapped: lib proj
	../md380-fw --wrap experiment.img experiment.bin
#This flashes from unix.
flash: wrapped
	../md380-dfu upgrade experiment.bin

$(SRCS): config.h src/*.h | lib
$(PROJ_NAME).elf: $(SRCS)
	$(CC) $(CFLAGS) -Wl,-Map,$(basename $@).map $^ -o $@ -Llib -lstm32f4 -Xlinker --just-symbols=src/symbols_$(SRCVERSION)
	$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).img
	$(OBJDUMP) -t main.elf >main.img.sym

#Temporary file containing the build date.
#Might use version numbers when we're ready for releases.
src/version.h: src/*.c
	echo "#define VERSIONDATE L\"`date "+%Y-%m-%d"`\"" >src/version.h
clean:
	$(MAKE) -C lib clean
	rm -f *.img *.lst *.bin *.elf *.hex *.o *~ *.sym src/*~ src/version.h



