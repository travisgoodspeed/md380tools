
all: applets
clean:
	cd patches/2.032 && make clean
#	cd firmware && make clean
	cd applet && make clean
	rm -f *~ *.pyc
patches: firmwares
	cd patches/2.032 && make all
applets: patches
	cd applet && make all
firmwares:
	cd firmware && make all
flash:
#	cd patches/2.032 && make flash
	cd applet && make flash
