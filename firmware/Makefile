# This Makefile grabs firmware from the web, then de 

clean:
	rm -f *.zip *~ *.bin *.img

all: D002.032.bin D002.034.bin

D002.032.bin: D002.032.zip
	unzip -p D002.032.zip 'Firmware 2.32/MD-380-D2.32(AD).bin' >D002.032.bin
D002.032.zip:
	curl 'http://www.va3xpr.net/?ddownload=9197' >D002.032.zip
D002.034.zip:
	curl 'http://www.va3xpr.net/?ddownload=9420' >D002.034.zip
D002.034.bin: D002.034.zip
	unzip -p D002.034.zip MD-380-D2.34.bin >D002.034.bin

