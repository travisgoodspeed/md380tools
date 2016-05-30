RELEASE=md380tools-`date "+%Y-%m-%d"`

#This strips out all unicode characters.
#We'd rather just drop the accents.
ICONV=iconv -c -f UTF-8 -t ascii//TRANSLIT

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
	cd db && make
	$(ICONV) db/users.csv | cut -d',' -f1-3,5-6 | sed 's/,\s+/,/g' > data.csv
	wc -c < data.csv > data
	cat data.csv >> data
	./md380-tool spiflashwrite data 0x100000
dist: applets
	rm -rf $(RELEASE)
	mkdir -p $(RELEASE)/python
	cp applet/experiment.bin $(RELEASE)/firmware-`date "+%Y-%m-%d"`.bin
	cd $(RELEASE) && unzip ../firmware/D002.032.zip
	mv $(RELEASE)/Firmware\ 2.32 $(RELEASE)/windows
	rm $(RELEASE)/windows/MD-380-D2.32\(AD\).bin $(RELEASE)/windows/Operation.doc.pdf
	cp DFU.py 99-md380.rules md380-dfu md380-tool $(RELEASE)/python/

