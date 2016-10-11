UNWRAPPED=../firmware/unwrapped
#GOLD=../firmware/unwrapped/D002.032.img
GOLD=../firmware/unwrapped/D013.020.img
GOLDSYM=../applet/src/symbols_d13.020

all: symgrate run

clean:
	rm -f symgrate symbols_*
images:
	cd ../firmware && "${MAKE}" all

symgrate: symgrate.c
	cc -std=c99 symgrate.c -o symgrate

test: all images
	./symgrate $(GOLD) $(UNWRAPPED)/D002.034.img <$(GOLDSYM)

run: symgrate images
	./symgrate $(GOLD) $(UNWRAPPED)/D002.034.img <$(GOLDSYM) >symbols_d02_034
	./symgrate $(GOLD) $(UNWRAPPED)/D003.008.img <$(GOLDSYM) >symbols_d03_008
	./symgrate $(GOLD) $(UNWRAPPED)/D003.020.img <$(GOLDSYM) >symbols_d03_020
	./symgrate $(GOLD) $(UNWRAPPED)/D013.009.img <$(GOLDSYM) >symbols_d13_009
	./symgrate $(GOLD) $(UNWRAPPED)/S003.012.img <$(GOLDSYM) >symbols_s03_012
	./symgrate $(GOLD) $(UNWRAPPED)/S013.012.img <$(GOLDSYM) >symbols_s13_012
	./symgrate $(GOLD) $(UNWRAPPED)/D013.014.img <$(GOLDSYM) >symbols_d13_014
	./symgrate $(GOLD) $(UNWRAPPED)/S013.020.img <$(GOLDSYM) >symbols_s13_020
	./symgrate $(GOLD) $(UNWRAPPED)/D013.020.img <$(GOLDSYM) >symbols_d13_020
	wc -l symbols_*
	@echo "These are temporary symbol files that will be overwritten."
	@echo "Move them elsewhere before editing."



