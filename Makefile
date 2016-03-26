RELEASE=md380tools-`date "+%Y-%m-%d"`


all: applets
clean:
	cd patches/2.032 && make clean
#	cd firmware && make clean
	cd applet && make clean
	rm -f *~ *.pyc
	cd db && make clean
	
patches: firmwares
	cd patches/2.032 && make all
dbs:
	cd db && make all

applets: patches dbs
	cd applet && make all
firmwares:
	cd firmware && make all
flash:	applets
#	cd patches/2.032 && make flash
	cd applet && make flash
dist: applets
	rm -rf $(RELEASE)
	mkdir -p $(RELEASE)/python
	cp applet/experiment.bin $(RELEASE)/firmware-`date "+%Y-%m-%d"`.bin
	cd $(RELEASE) && unzip ../firmware/D002.032.zip
	mv $(RELEASE)/Firmware\ 2.32 $(RELEASE)/windows
	rm $(RELEASE)/windows/MD-380-D2.32\(AD\).bin $(RELEASE)/windows/Operation.doc.pdf
	cp DFU.py 99-md380.rules md380-dfu md380-tool $(RELEASE)/python/
