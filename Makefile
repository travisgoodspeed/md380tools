RELEASE=md380tools-`date "+%Y-%m-%d"`


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
	cd applet && make flash
flashdb:
	wc -c < db/users.csv > data ; cat db/users.csv >> data
	md380-tool spiflashwrite data 0x100000
dist: applets
	rm -rf $(RELEASE)
	mkdir -p $(RELEASE)/python
	cp applet/experiment.bin $(RELEASE)/firmware-`date "+%Y-%m-%d"`.bin
	cd $(RELEASE) && unzip ../firmware/D002.032.zip
	mv $(RELEASE)/Firmware\ 2.32 $(RELEASE)/windows
	rm $(RELEASE)/windows/MD-380-D2.32\(AD\).bin $(RELEASE)/windows/Operation.doc.pdf
	cp DFU.py 99-md380.rules md380-dfu md380-tool $(RELEASE)/python/

