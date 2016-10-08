
ifeq ($(MAKE_VERSION),3.16)
OPT = download
else
endif


all: $(OPT) unwrapped/D002.032.img unwrapped/S013.020.img unwrapped/D013.020.img
	
show:
	echo $(MAKE_VERSION)
	
clean:
	$(MAKE) -f Makefile_orig clean
	
download: 
	echo $(MAKE_VERSION)
	$(MAKE) -f Makefile_orig download
	
unwrapped/D002.032.img:
	$(MAKE) -f Makefile_orig unwrapped/D002.032.img
	
unwrapped/S013.020.img:
	$(MAKE) -f Makefile_orig unwrapped/S013.020.img
	
unwrapped/D013.020.img:	
	$(MAKE) -f Makefile_orig unwrapped/D013.020.img
		
