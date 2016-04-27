# This Makefile grabs firmware from the web, then de 

all: D002.032.bin

clean:
	rm -f *.zip *~ *.bin *.img

D002.032.bin: D002.032.zip
	@unzip -p D002.032.zip 'Firmware 2.32/MD-380-D2.32(AD).bin' >D002.032.bin
	@if md5sum -c D002.032.bin.md5 2>&1 > /dev/null; \
	then echo "MD5 OK"; \
	else \
		echo "Firmware 2.32/MD-380-D2.32(AD).bin wrong md5sum"; \
		rm D002.032.bin; \
	fi;
	
D002.032.zip:
	curl 'http://www.va3xpr.net/?ddownload=9197' >D002.032.zip
	@if md5sum -c D002.032.zip.md5 2>&1 > /dev/null; \
	then echo "Download OK"; \
	else \
		echo "Download wrong md5sum"; \
		rm D002.032.zip; \
		exit; \
	fi;


D002.034.zip:
	curl 'http://www.va3xpr.net/?ddownload=9420' >D002.034.zip
D002.034.bin: D002.034.zip
	unzip -p D002.034.zip MD-380-D2.34.bin >D002.034.bin

