
all: patches
clean:
	rm -f unwrapped.bin

unwrapped.bin: MD-380-D2.32(AD).bin
	../../md380-fw --unwrap "MD-380-D2.32(AD).bin" unwrapped.img

patches: unwrapped.bin
	./patch.py

wrapped: patches
	../../md380-fw --wrap prom-public.img prom-public.bin
	../../md380-fw --wrap prom-private.img prom-private.bin

