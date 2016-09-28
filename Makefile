RELEASE=md380tools-`date "+%Y-%m-%d"`

#This strips out all unicode characters.
#We'd rather just drop the accents.
ICONV=iconv -c -f UTF-8 -t ascii//TRANSLIT

all: applets
clean:
	rm -f data data.csv	
	cd patches/2.032 && $(MAKE) clean
	cd patches/d13.020 && $(MAKE) clean
	cd firmware && $(MAKE) clean
	cd applet && $(MAKE) clean
	rm -f *~ *.pyc

patches: firmwares
	cd patches/2.032 && $(MAKE) all
	cd patches/d13.020 && $(MAKE) all

applets: patches
	cd applet && $(MAKE) all

firmwares:
	cd firmware && $(MAKE) all

flash:
	cd applet && $(MAKE) clean flash

flash1:
	cd applet && $(MAKE) --eval=FW=D02_032 clean flash
	
flash2:
	cd applet && $(MAKE) --eval=FW=S13_020 clean flash
	
flash_d02.032:
	cd applet && $(MAKE) -f Makefile.d02.032 clean flash

flash_s13.020:
	cd applet && $(MAKE) -f Makefile.s13.020 clean flash

flashdb:
	cd db && $(MAKE)
	$(ICONV) db/users.csv | cut -d',' -f1-3,5-6 | sed 's/,\s+/,/g' > data.csv
	wc -c < data.csv > data
	cat data.csv >> data
	./md380-tool spiflashwrite data 0x100000

dist: applets
	rm -rf $(RELEASE) $(RELEASE).zip
	mkdir -p $(RELEASE)/python
#Main release.
	cd applet && $(MAKE) clean all
	cp applet/experiment.bin $(RELEASE)/firmware-`date "+%Y-%m-%d-NoGPS"`.bin
#For GPS radios.
	cd applet && $(MAKE) -f Makefile.s13.020 clean all
	cp applet/experiment.bin $(RELEASE)/firmware-`date "+%Y-%m-%d-GPS"`.bin
#Older
	cd applet && $(MAKE) -f Makefile.d02.032 clean all
	cp applet/experiment.bin $(RELEASE)/firmware-`date "+%Y-%m-%d-OLD"`.bin
#Include the Official Installer
	cd $(RELEASE) && unzip ../firmware/dl/D002.032.zip
	mv $(RELEASE)/Firmware\ 2.32 $(RELEASE)/windows
	rm $(RELEASE)/windows/MD-380-D2.32\(AD\).bin $(RELEASE)/windows/Operation.doc.pdf
	cp *.py 99-md380.rules md380-dfu md380-tool $(RELEASE)/python/
#Clean out some gunk
	rm -rf $(RELEASE)/__MACOSX
#Zip it up for distribution.
	zip -r $(RELEASE).zip $(RELEASE)

doflash: applets
	./md380-dfu upgrade applet/experiment.bin
