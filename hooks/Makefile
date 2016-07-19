UNWRAPPED=../firmware/unwrapped
GOLD=../firmware/unwrapped/D002.032.img 
GOLDSYM=../applet/src/symbols_2_032

all: blhook

clean:
	rm -f blhook *~

images:
	cd ../firmware && $(MAKE)
test: all images
	./blhook ../firmware/unwrapped/S013.020.img foo < testhooks

run: all
	@echo "Try 'make test' for now."



