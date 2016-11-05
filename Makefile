
RELEASE=dist/md380tools-`date "+%Y-%m-%d"`

##This strips out all unicode characters.
##We'd rather just drop the accents.
#ICONV=iconv -c -f UTF-8 -t ascii//TRANSLIT

.PHONY: dist

all: image_D13
	
distclean: clean
	rm -rf dist
		
clean: mostlyclean
	"${MAKE}" -C firmware clean
	
# mostlyclean does not cause re-download firmware
mostlyclean:
	"${MAKE}" -C patches/2.032 clean
	"${MAKE}" -C patches/3.020 clean
	"${MAKE}" -C patches/s13.020 clean
	"${MAKE}" -C patches/d13.020 clean
	"${MAKE}" -C applet clean
	"${MAKE}" -C db clean
	rm -f *~ *.pyc
	-rm *.bin
	
image_D02:
	"${MAKE}" -C applet FW=D02_032 all  

image_D13:
	"${MAKE}" -C applet FW=D13_020 all  
	
image_S13:
	"${MAKE}" -C applet FW=S13_020 all  
	
original_D13: 
	"${MAKE}" -C firmware unwrapped/D013.020.img
	
original_D02: 
	"${MAKE}" -C firmware unwrapped/D002.032.img
	
original_D03: 
	"${MAKE}" -C firmware unwrapped/D003.020.img
	
flash_original_D13: original_D13
	./md380-dfu upgrade firmware/bin/D013.020.bin
	
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
	
#flash:
#	"${MAKE}" -C applet clean flash
#	
#flash_d02.032:
#	"${MAKE}" -C applet FW=D02_032 clean flash
#	
#flash_s13.020:
#	"${MAKE}" -C applet FW=S13_020 clean flash
	
.PHONY: updatedb 
updatedb:
	"${MAKE}" -C db update
	
.PHONY: db/stripped.csv
db/stripped.csv:
	"${MAKE}" -C db stripped.csv
	
user.bin: db/stripped.csv
	wc -c < db/stripped.csv > user.bin
	cat db/stripped.csv >> user.bin
	
.PHONY: flashdb
flashdb: user.bin
	./md380-tool spiflashwrite user.bin 0x100000
	
release:
	"${MAKE}" clean
	mkdir release
	"${MAKE}" mostlyclean image_D13
	cp applet/experiment.bin release/exp_D13.bin
	"${MAKE}" mostlyclean image_S13
	cp applet/experiment.bin release/exp_S13.bin
	"${MAKE}" mostlyclean image_D02
	cp applet/experiment.bin release/exp_S02.bin
	"${MAKE}" mostlyclean user.bin
	cp user.bin release/

dist: 
	rm -rf $(RELEASE) $(RELEASE).zip
	mkdir -p $(RELEASE)/python
#Main release.
	"${MAKE}" mostlyclean image_D13
	cp applet/experiment.bin $(RELEASE)/firmware-`date "+%Y-%m-%d-NoGPS"`.bin
#For GPS radios.
	"${MAKE}" mostlyclean image_S13
	cp applet/experiment.bin $(RELEASE)/firmware-`date "+%Y-%m-%d-GPS"`.bin
#Older
	"${MAKE}" mostlyclean image_D02
	cp applet/experiment.bin $(RELEASE)/firmware-`date "+%Y-%m-%d-OLD"`.bin
#Include the Official Installer
	cd $(RELEASE) && unzip ../../firmware/dl/D002.032.zip
	mv $(RELEASE)/Firmware\ 2.32 $(RELEASE)/windows
	rm $(RELEASE)/windows/MD-380-D2.32\(AD\).bin $(RELEASE)/windows/Operation.doc.pdf
	cp *.py 99-md380.rules md380-dfu md380-tool $(RELEASE)/python/
#Clean out some gunk
	rm -rf $(RELEASE)/__MACOSX
#Zip it up for distribution.
	zip -r $(RELEASE).zip $(RELEASE)


# or else make will fail.
#download:
#	"${MAKE}" -C firmware download

#all_images: 
#	"${MAKE}" -C applet ci
	
dbg:
	@echo ________
	@echo PATH: '${PATH}'
	@echo SHELL: '${SHELL}'
	@echo MAKE: '${MAKE}'
	@echo ________
	@echo AWK version
	-awk -V
	@echo ________
	@echo Make version
	make -v
	@echo ________
    
ci: dbg mostlyclean 
	"${MAKE}" -C applet ci
	"${MAKE}" -C db ci
	"${MAKE}" user.bin
#	"${MAKE}" -C annotations/d13.020 ci
	"${MAKE}" mostlyclean

check-ignore:
	find -type f | git check-ignore -v --stdin | less

