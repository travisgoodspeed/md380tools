ifeq ($(MAKE_VERSION),3.81)
OPT = download
else
endif

all: unwrapped/D002.032.img unwrapped/S013.020.img unwrapped/D013.020.img unwrapped/D003.020.img
	
clean:
	"${MAKE}" -f Makefile_orig clean
	
download: 
	"${MAKE}" -f Makefile_orig download
	
unwrapped/%.img: $(OPT)
	"${MAKE}" -f Makefile_orig $@