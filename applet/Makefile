
MKLISTING=1

FW ?= D13_020

ifeq (,$(wildcard mark.tmp))
	# not exists
  PROBLEM=0	
else
  old_build := $(shell cat mark.tmp)

  ifneq "$(old_build)" "$(FW)"
    PROBLEM=1
  endif	
endif

ifeq ($(FW),D02_032)
    INC_DISPFLIP=1
    INC_I2C=1
    FWDEFINE=FW_D02_032
    SRCVERSION=d02.032
    PATCHDIR=2.032
    MERGEBASE=0x0809D000
else ifeq ($(FW),D13_020)
    FWDEFINE=FW_D13_020
    SRCVERSION=d13.020
    PATCHDIR=d13.020
    MERGEBASE=0x0809b000
else ifeq ($(FW),S13_020)
    FWDEFINE=FW_S13_020
    SRCVERSION=s13.020
    PATCHDIR=s13.020
    MERGEBASE=0x809c000
else 
$(error select firmware version)
endif

SRCS  = main.o printf.o dmesg.o gfx.o stm32f4xx_it.o system_stm32f4xx.o addl_config.o 
SRCS += spiflash.o usb.o menu.o rtc_timer.o beep_process.o usersdb.o os.o aes.o
SRCS += ambe.o  dmr.o 
SRCS += display.o
SRCS += console.o
SRCS += util.o
SRCS += netmon.o
SRCS += debug.o
SRCS += keyb.o
SRCS += radiostate.o
SRCS += mbox.o
SRCS += syslog.o
SRCS += etsi.o
SRCS += sms.o
ifdef INC_DISPFLIP
SRCS +=  dispflip.o  # display flip is included @ d13.020
endif
ifdef INC_I2C
SRCS +=  i2c.o #  ALPU Licence Check is disabled, so we have no i2c
endif

PROJ_NAME=main


###################################################

CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy
OBJDUMP=arm-none-eabi-objdump

GIT_VERSION := $(shell git describe --dirty --always --tags)

LINKSCRIPT=applet_$(SRCVERSION).ld

CFLAGS += -std=gnu99 -g -O2 -Wall -T$(LINKSCRIPT)
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4 -mthumb-interwork
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS += -fshort-wchar
ifdef MKLISTING
CFLAGS += -Wa,-a=$(@:.o=.lst)
endif
CFLAGS += -D$(FWDEFINE)	
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

all: mark lib proj wrapped	
	
.PHONY: mark
mark:
ifeq "$(PROBLEM)" "1"
	@echo "=================="
	@echo "problem, make mostlyclean first"
	@echo "=================="
	@exit 1
else	
	echo $(FW) >mark.tmp
endif	

experiment.img: 
	"${MAKE}" -C ../patches/$(PATCHDIR) patched.bin
	cp ../patches/$(PATCHDIR)/patched.img ./experiment.img


lib: src/version.h config.h
	"${MAKE}" -C lib

proj: 	$(PROJ_NAME).elf experiment.img
	# use a fresh experiment.img
	"${MAKE}" -C ../patches/$(PATCHDIR) clean all
	cp ../patches/$(PATCHDIR)/patched.img ./experiment.img
	python2 merge_$(SRCVERSION).py experiment.img $(PROJ_NAME).img $(MERGEBASE)

#The wrapped file is the one that works with the Windows updater.
wrapped: lib proj
	../md380-fw --wrap experiment.img experiment.bin

##This flashes from unix. (deprecated)
#flash: wrapped
#	../md380-dfu upgrade experiment.bin

$(SRCS): config.h src/*.h | lib	
$(PROJ_NAME).elf: $(SRCS)
	$(CC) $(CFLAGS) -Wl,--no-wchar-size-warning -Wl,-Map,$(basename $@).map $^ -o $@ -Llib -lstm32f4 -Xlinker --just-symbols=src/symbols_$(SRCVERSION)
	$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).img
	$(OBJDUMP) -t main.elf >main.img.sym

#Temporary file containing the build date.
#Might use version numbers when we're ready for releases.
src/version.h: src/*.c
	echo "#define VERSIONDATE L\"`date "+%Y-%m-%d"`\"" >src/version.h
	echo "#define GIT_VERSION \"$(GIT_VERSION)\"" >>src/version.h

clean:
	"${MAKE}" -C ../patches/$(PATCHDIR) clean
	"${MAKE}" -C lib clean
	rm -f *.img *.lst *.bin *.elf *.hex *.o *~ *.sym src/*~ src/version.h *.map *.tmp

image_D02:
	"${MAKE}" FW=D02_032 all  

image_D13:
	"${MAKE}" FW=D13_020 all  
	
image_S13:
	"${MAKE}" FW=S13_020 all  

ci:
	"${MAKE}" clean image_D02
	"${MAKE}" clean image_S13
	"${MAKE}" clean image_D13
