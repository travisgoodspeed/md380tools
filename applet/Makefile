
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
    INC_DISPINIT=1
else ifeq ($(FW),S13_020)
    FWDEFINE=FW_S13_020
    SRCVERSION=s13.020
    PATCHDIR=s13.020
    MERGEBASE=0x809c000
    INC_DISPINIT=1
else 
$(error select firmware version)
endif

SRCS  = main.o printf.o dmesg.o gfx.o system_stm32f4xx.o addl_config.o 
SRCS += spiflash.o usb.o menu.o rtc_timer.o usersdb.o os.o aes.o
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
SRCS += lastheard.o
SRCS += etsi.o
SRCS += sms.o
SRCS += beep.o
SRCS += irq_handlers.o
SRCS += narrator.o
SRCS += lcd_driver.o
SRCS += font_8_8.o
SRCS += app_menu.o
SRCS += color_picker.o
SRCS += amenu_hexmon.o
SRCS += amenu_utils.o
SRCS += amenu_set_tg.o
SRCS += amenu_codeplug.o
SRCS += timer.o

ifdef INC_DISPFLIP
SRCS +=  dispflip.o  # display flip is included @ d13.020
endif
ifdef INC_I2C
SRCS +=  i2c.o #  ALPU Licence Check is disabled, so we have no i2c
endif
ifdef INC_DISPINIT
SRCS +=  dispinit.o
endif

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

# DL4YHF: Had to modify the name of the startup file, because it was 
#  overwritten by somehing else (*.s = *.S under windows).
SRCS += lib/startup_stm32f4xx_asm.S

OBJS = $(SRCS:.c=.o)


###################################################

.PHONY: lib proj pre

all: mark experiment.bin
	
.PHONY: mark
mark:
ifeq "$(PROBLEM)" "1"
	@echo "=================="
	@echo "problem, make clean first"
	@echo "=================="
	@exit 1
else	
	echo $(FW) >mark.tmp
endif	

# strange dependencies (migrate down to lib makefile if need be)
lib: src/version.h config.h
	"${MAKE}" -C lib libstm32f4.a

.PHONY: base.img
base.img:
	"${MAKE}" -C ../patches/$(PATCHDIR) patched.img
	
merged.img: applet.img base.img   
	cp ../patches/$(PATCHDIR)/patched.img ./merged.img
	python2 merge_$(SRCVERSION).py merged.img applet.img $(MERGEBASE)

#patched.bin: proj
#	"${MAKE}" -C ../patches/$(PATCHDIR) patched.img
#	cp ../patches/$(PATCHDIR)/patched.img ./patched.bin

#The wrapped file is the one that works with the Windows updater.
wrapped.bin: merged.img
	../md380-fw --wrap merged.img wrapped.bin

experiment.bin: wrapped.bin
	cp wrapped.bin experiment.bin
	
##This flashes from unix. (deprecated)
#flash: wrapped
#	../md380-dfu upgrade experiment.bin

$(SRCS): config.h src/*.h | lib	
applet.elf: $(SRCS)
	$(CC) $(CFLAGS) -Wl,--no-wchar-size-warning -Wl,-Map,$(basename $@).map $^ -o $@ -Llib -lstm32f4 -Xlinker --just-symbols=src/symbols_$(SRCVERSION)
	
%.img: %.elf	
	$(OBJDUMP) -t $< >$(basename $@).img.sym
	$(OBJCOPY) -O binary $< $@

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
