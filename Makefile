RELEASE=md380tools-`date "+%Y-%m-%d"`

#This strips out all unicode characters.
#We'd rather just drop the accents.
ICONV=iconv -c -f UTF-8 -t ascii//TRANSLIT

all: applets
clean:
	cd patches/2.032 && $(MAKE) clean
	cd patches/d13.020 && $(MAKE) clean
	cd firmware && $(MAKE) clean
	cd applet && $(MAKE) clean
	rm -f *~ *.pyc
patches: firmwares
	cd patches/2.032 && $(MAKE) all
applets: patches
	cd applet && $(MAKE) all
firmwares:
	cd firmware && $(MAKE) all
flash:
	cd applet && $(MAKE) clean flash
flash_d02.032:
	cd applet && $(MAKE) -f Makefile.d02.032 clean flash
flashdb:
	cd db && $(MAKE)
	$(ICONV) db/users.csv | cut -d',' -f1-3,5-6 | sed 's/,\s+/,/g' > data.csv
	wc -c < data.csv > data
	cat data.csv >> data
	./md380-tool spiflashwrite data 0x100000
dist: applets
	rm -rf $(RELEASE)
	mkdir -p $(RELEASE)/python
	cp applet/experiment.bin $(RELEASE)/firmware-`date "+%Y-%m-%d"`.bin
	cd $(RELEASE) && unzip ../firmware/dl/D002.032.zip
	mv $(RELEASE)/Firmware\ 2.32 $(RELEASE)/windows
	rm $(RELEASE)/windows/MD-380-D2.32\(AD\).bin $(RELEASE)/windows/Operation.doc.pdf
	cp DFU.py 99-md380.rules md380-dfu md380-tool $(RELEASE)/python/

