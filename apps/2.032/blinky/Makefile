# put your *.o targets here, make should handle the rest!

SRCS = main.c stm32f4xx_it.c system_stm32f4xx.c

# all the files will be generated with this name (main.elf, main.bin, main.hex, etc)

PROJ_NAME=main

# that's it, no need to change anything below this line!

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

CFLAGS += -Iinc -Ilib -Ilib/inc 
CFLAGS += -Ilib/inc/core -Ilib/inc/peripherals 

SRCS += lib/startup_stm32f4xx.s # add startup file to build
#SRCS += src/rcc.c

OBJS = $(SRCS:.c=.o)

#0x0809D000 -0x0800C000
LOADAT = 0x00091000

###################################################

.PHONY: lib proj pre

all: lib proj wrapped

experiment.img:
	cd ../../../patches/2.032 && make all
	cp ../../../patches/2.032/experiment.img ./


lib:
	$(MAKE) -C lib

proj: 	$(PROJ_NAME).elf experiment.img
#	dd if=$(PROJ_NAME).img bs=1 seek=0x00091000 of=experiment-patch.img #Correct the offset.
	python2 merge.py experiment.img $(PROJ_NAME).img 0x0809D000

wrapped: lib proj
	../../../md380-fw --wrap experiment.img experiment.bin
flash: wrapped
	../../../md380-dfu upgrade experiment.bin


$(PROJ_NAME).elf: $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@ -Llib -lstm32f4
#	$(OBJCOPY) -O ihex $(PROJ_NAME).elf $(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).img
	$(OBJDUMP) -t main.elf >main.img.sym


clean:
	$(MAKE) -C lib clean
	rm -f *.img *.bin *.elf *.hex *~ *.sym


