# This Makefile grabs firmware from the web, then decompresses it.

# We used to check the MD5 hashes of each file, but this caused
# portability issues and we're not too concerned about VA3XPR messing
# with our firmware sources, so for now we just check filesize to
# identify accidental corruption.  Anyone causing intentional
# corruption is politely asked to ensure that their joke is funny.

all: D002.032.bin

clean:
	rm -f *.zip *~ *.bin *.img

D002.032.bin: D002.032.zip
	unzip --help >>/dev/null #Check that we have unzip
	unzip -p D002.032.zip 'Firmware 2.32/MD-380-D2.32(AD).bin' >D002.032.bin
	du D002.032.bin | grep 972 || (rm D002.032.bin && false)

D002.032.zip:
	curl --help >>/dev/null #Check that we have CURL.
	curl 'http://www.va3xpr.net/?ddownload=9197' >D002.032.zip
	du D002.032.zip | grep 2320 || (rm D002.032.zip && false)

D002.034.zip:
	curl --help >>/dev/null #Check that we have CURL.
	curl 'http://www.va3xpr.net/?ddownload=9420' >D002.034.zip
D002.034.bin: D002.034.zip
	unzip -p D002.034.zip MD-380-D2.34.bin >D002.034.bin

