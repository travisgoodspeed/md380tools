# This Makefile grabs firmware from the web, then decompresses it.

# We used to download from VA3XPR, but since they started requiring
# a newsletter subscription to access newer files, we have migrated
# to using the Internet Archive.
# Previously, we used to check the MD5 hashes of each file, but this
# caused portability issues and we're not too concerned about VA3XPR
# messing with our firmware sources, so for now we just check filesize
# to identify accidental corruption.  Anyone causing intentional
# corruption is politely asked to ensure that their joke is funny.

DLDIR = dl
BINDIR = bin
UNWRAPDIR = unwrapped

DLFILES = $(DLDIR)/D002.022.zip $(DLDIR)/D002.025.zip $(DLDIR)/D002.026.zip $(DLDIR)/D002.030.zip $(DLDIR)/D002.032.zip $(DLDIR)/D002.034.zip $(DLDIR)/D003.008.zip $(DLDIR)/D013.009.bin
BINARIES = $(addprefix $(BINDIR)/,$(patsubst %.zip,%.bin,$(notdir $(DLFILES))))
UNWRAPPEDFILES = $(addprefix $(UNWRAPDIR)/,$(patsubst %.bin,%.img,$(notdir $(BINARIES))))

.PHONY: all download binary unwrap clean cleanall

all: download binary unwrap

download: $(DLFILES)
binary: $(BINARIES)
unwrap: $(UNWRAPPEDFILES)

clean:
	rm -rf $(BINDIR) $(UNWRAPDIR)

cleanall: clean
	rm -rf $(DLDIR)

$(DLDIR):
	mkdir -p $@
$(BINDIR):
	mkdir -p $@
$(UNWRAPDIR):
	mkdir -p $@

$(UNWRAPDIR)/%.img: $(BINDIR)/%.bin | $(UNWRAPDIR)
	../md380-fw --unwrap $< $@

$(BINDIR)/D002.022.bin: $(DLDIR)/D002.022.zip | $(BINDIR)
	which unzip >>/dev/null #Check that we have unzip
	unzip -p $< 'TYT-MD-380-Firmware-Upgrade/Upgraded software for newest version.bin' >$@
	test -n "$$(find $@ -size 999936c)"

$(BINDIR)/D002.025.bin: $(DLDIR)/D002.025.zip | $(BINDIR)
	which unzip >>/dev/null #Check that we have unzip
	unzip -p $< 'MD-380-D2/MD-380-D2.25.bin' >$@
	test -n "$$(find $@ -size 993792c)"

$(BINDIR)/D002.026.bin: $(DLDIR)/D002.026.zip | $(BINDIR)
	which unzip >>/dev/null #Check that we have unzip
	unzip -p $< 'TYT-MD-380-Firmware-D002-2/MD-380-D2.26.bin' >$@
	test -n "$$(find $@ -size 993792c)"

$(BINDIR)/D002.030.bin: $(DLDIR)/D002.030.zip | $(BINDIR)
	which unzip >>/dev/null #Check that we have unzip
	unzip -p $< 'MD-380-Firmware-v2/MD-380-D2.30.bin' >$@
	test -n "$$(find $@ -size 994816c)"

$(BINDIR)/D002.032.bin: $(DLDIR)/D002.032.zip | $(BINDIR)
	which unzip >>/dev/null #Check that we have unzip
	unzip -p $< 'Firmware 2.32/MD-380-D2.32(AD).bin' >$@
	test -n "$$(find $@ -size 994816c)"

$(BINDIR)/D002.034.bin: $(DLDIR)/D002.034.zip | $(BINDIR)
	unzip -p $< 'TYT-MD380-FW-2-34/MD-380-D2.34.bin' >$@
	test -n "$$(find $@ -size 994816c)"

$(BINDIR)/D003.008.bin: $(DLDIR)/D003.008.zip | $(BINDIR)
	unzip -p $< 'D003.008.bin' >$@
	test -n "$$(find $@ -size 995840c)"

$(BINDIR)/D013.009.bin: $(DLDIR)/D013.009.bin | $(BINDIR)
	cp $< $@
	test -n "$$(find $@ -size 994816c)"


$(DLDIR)/D002.022.zip: | $(DLDIR)
	which curl >>/dev/null #Check that we have curl.
	curl -L 'https://archive.org/download/TYTMD380FW2/TYT-Tytera-MD-380-FW-v222.zip' >$@

$(DLDIR)/D002.025.zip: | $(DLDIR)
	which curl >>/dev/null #Check that we have curl.
	curl -L 'https://archive.org/download/TYTMD380FW2/TYT-Tytera-MD-380-FW-v225.zip' >$@

$(DLDIR)/D002.026.zip: | $(DLDIR)
	which curl >>/dev/null #Check that we have curl.
	curl -L 'https://archive.org/download/TYTMD380FW2/TYT-Tytera-MD-380-FW-v226.zip' >$@

$(DLDIR)/D002.030.zip: | $(DLDIR)
	which curl >>/dev/null #Check that we have curl.
	curl -L 'https://archive.org/download/TYTMD380FW2/TYT-Tytera-MD-380-FW-v230.zip' >$@

$(DLDIR)/D002.032.zip: | $(DLDIR)
	which curl >>/dev/null #Check that we have curl.
	curl -L 'https://archive.org/download/TYTMD380FW2/TYT-Tytera-MD-380-FW-v232.zip' >$@

$(DLDIR)/D002.034.zip: | $(DLDIR)
	which curl >>/dev/null #Check that we have curl.
	curl -L 'https://archive.org/download/TYTMD380FW2/TYT-MD380-FW-2-34.zip' >$@

$(DLDIR)/D003.008.zip: | $(DLDIR)
	which curl >>/dev/null #Check that we have curl.
	curl -L 'https://archive.org/download/TYTMD380FW2/D003.008.zip' >$@

$(DLDIR)/D013.009.bin: | $(DLDIR)
	which curl >>/dev/null #Check that we have curl.
	curl -L 'https://archive.org/download/TYTMD380FW2/D013.009_tytera_walking_dead_firmware_by_ea8cwb.bin' >$@
