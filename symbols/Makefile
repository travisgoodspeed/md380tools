.SECONDEXPANSION:

UNWRAPPED=../firmware/unwrapped
GOLD=../firmware/unwrapped/D013.020.img
GOLDSYM=../applet/src/symbols_d13.020
TEST=../firmware/unwrapped/D002.032.img

mangle = $(shell echo $(1) | sed -r "s/symbols_(.)(..)_(...)/\u\10\2\.\3\.img/")

.PHONY: all clean imageclean run test

all: symgrate run

clean:
	rm -f symgrate symbols_*

$(UNWRAPPED)/%:
	cd ../firmware && "${MAKE}" unwrapped/$*

symgrate: symgrate.c
	cc -std=c99 symgrate.c -o symgrate

test: all $(GOLD) $(TEST) $(GOLDSYM)
	./symgrate $(GOLD) $(TEST) <$(GOLDSYM)

symbols_%: $(UNWRAPPED)/$$(call mangle,symbols_%) symgrate $(GOLD) $(GOLDSYM)
	./symgrate $(GOLD) $(UNWRAPPED)/$(call mangle,symbols_$*) <$(GOLDSYM) >$@

run: symbols_d02_034 symbols_d03_008 symbols_d03_020 symbols_d13_009 symbols_s03_012 symbols_s13_012 symbols_d13_014 symbols_s13_020 symbols_d13_020 
	@wc -l symbols_*
	@echo "These are temporary symbol files that will be overwritten."
	@echo "Move them elsewhere before editing."