
RELEASE=dist/md380tools-`date "+%Y-%m-%d"`

.PHONY: dist all

all: image_D13

# cleans everything also downloaded files
distclean: clean
	rm -rf dist
	"${MAKE}" -C db clean
	"${MAKE}" -C firmware clean

# does not clean external downloads
clean:
	"${MAKE}" -C patches/2.032 clean
	"${MAKE}" -C patches/3.020 clean
	"${MAKE}" -C patches/s13.020 clean
	"${MAKE}" -C patches/d13.020 clean
	"${MAKE}" -C applet clean
	"${MAKE}" -C annotations clean
	"${MAKE}" -C emulator clean
	rm -f *~ *.pyc *.bin

image_D02:
	"${MAKE}" -C applet FW=D02_032 all  

image_D13:
	"${MAKE}" -C applet FW=D13_020 all  

image_S13:
	"${MAKE}" -C applet FW=S13_020 all  

original_D13: 
	"${MAKE}" -C firmware unwrapped/D013.020.img

original_S13: 
	"${MAKE}" -C firmware unwrapped/S013.020.img

original_D02: 
	"${MAKE}" -C firmware unwrapped/D002.032.img

original_D03: 
	"${MAKE}" -C firmware unwrapped/D003.020.img

flash_original_D13: original_D13
	./md380-dfu upgrade firmware/bin/D013.020.bin

flash_original_S13: original_S13
	./md380-dfu upgrade firmware/bin/S013.020.bin

flash_original_D02: original_D02
	./md380-dfu upgrade firmware/bin/D002.032.bin

flash_original_D03: original_D03
	./md380-dfu upgrade firmware/bin/D003.020.bin

flash: image_D13
	./md380-dfu upgrade applet/experiment.bin

flash_D02: image_D02
	./md380-dfu upgrade applet/experiment.bin

flash_D13: image_D13
	./md380-dfu upgrade applet/experiment.bin

flash_S13: image_S13
	./md380-dfu upgrade applet/experiment.bin

sync:
	"${MAKE}" -C annotations sync

.PHONY: updatedb updatedb_eur

updatedb:
	"${MAKE}" -C db update

updatedb_eur:
	"${MAKE}" -C db update_eur

.PHONY: db/stripped.csv
db/stripped.csv:
	"${MAKE}" -C db stripped.csv

user.bin: db/stripped.csv
	wc -c < db/stripped.csv > user.bin
	cat db/stripped.csv >> user.bin

.PHONY: flashdb
flashdb: user.bin
	./md380-tool spiflashwrite user.bin 0x100000

.PHONY: release
release:
	-mkdir release
	"${MAKE}" clean image_D13
	cp applet/experiment.bin release/D13.exp.bin
	"${MAKE}" clean image_S13
	cp applet/experiment.bin release/S13.exp.bin
	"${MAKE}" clean image_D02
	cp applet/experiment.bin release/S02.exp.bin
	"${MAKE}" clean user.bin
	cp user.bin release/
	"${MAKE}" clean original_D13
	cp firmware/bin/D013.020.bin release/
	"${MAKE}" clean original_S13
	cp firmware/bin/S013.020.bin release/
	"${MAKE}" clean original_D02
	cp firmware/bin/D002.032.bin release/
	"${MAKE}" clean original_D03
	cp firmware/bin/D003.020.bin release/

dist: 
	rm -rf $(RELEASE) $(RELEASE).zip
	mkdir -p $(RELEASE)/python
#Main release.
	"${MAKE}" clean image_D13
	cp applet/experiment.bin $(RELEASE)/firmware-`date "+%Y-%m-%d-NoGPS"`.bin
#For GPS radios.
	"${MAKE}" clean image_S13
	cp applet/experiment.bin $(RELEASE)/firmware-`date "+%Y-%m-%d-GPS"`.bin
#Older
	"${MAKE}" clean image_D02
	cp applet/experiment.bin $(RELEASE)/firmware-`date "+%Y-%m-%d-OLD"`.bin
#Include the Official Installer
	cd $(RELEASE) && unzip ../../firmware/dl/D002.032.zip
	mv $(RELEASE)/Firmware\ 2.32 $(RELEASE)/windows
	rm $(RELEASE)/windows/MD-380-D2.32\(AD\).bin $(RELEASE)/windows/Operation.doc.pdf
	cp *.py 99-md380.rules md380-dfu md380-tool $(RELEASE)/python/
#Clean out some gunk
	rm -rf $(RELEASE)/__MACOSX
#Add the latest database
	make -C db clean all
	cp db/stripped.csv $(RELEASE)/callerid.csv
#Zip it up for distribution.
	zip -r $(RELEASE).zip $(RELEASE)

dbg:
	@echo ________
	@echo PATH: '${PATH}'
	@echo SHELL: '${SHELL}'
	@echo MAKE: '${MAKE}'
	@echo ________
	@echo AWK version
	-awk -Wversion 2>/dev/null || awk --version
	@echo ________
	@echo Make version
	make -v
	@echo ________

ci: dbg clean 
	"${MAKE}" -C applet ci
	"${MAKE}" -C db ci
	"${MAKE}" user.bin
#	"${MAKE}" -C annotations/d13.020 ci
	"${MAKE}" clean

check-ignore:
	find -type f | git check-ignore -v --stdin | less

