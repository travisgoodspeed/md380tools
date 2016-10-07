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

DLFILES := $(addprefix $(DLDIR)/,$(shell grep -v '^\#' firmware_files.txt | awk '{print $$1}'))
BINARIES := $(addprefix $(BINDIR)/,$(patsubst %.zip,%.bin,$(notdir $(DLFILES))))
UNWRAPPEDFILES := $(addprefix $(UNWRAPDIR)/,$(patsubst %.bin,%.img,$(notdir $(BINARIES))))

.PHONY: all download binary unwrap clean cleanall

all: download binary unwrap

download: $(DLFILES)
binary: $(BINARIES)
unwrap: $(UNWRAPPEDFILES)

clean:
	rm -rf $(BINDIR) $(UNWRAPDIR) $(DLDIR)

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

$(BINDIR)/%.bin: $(DLDIR)/%.zip | $(BINDIR)
	unzip -p $< "$$(cat firmware_files.txt | grep "^$(basename $(notdir $@))" | cut -f3)" >$@
	@test $$(find $@ -size $$(cat firmware_files.txt | grep "^$(basename $(notdir $@))" | cut -f4)c) || (rm -f $@ && echo "$@: filesize mismatch" && false)
	@test $$(shasum -a 256 $@ | awk '{print $$1}') "=" $$(cat firmware_files.txt | grep "^$(basename $(notdir $@))" | cut -f5) || (rm -f $@ && echo "$@: SHA256 mismatch" && false)


$(BINDIR)/%.bin: $(DLDIR)/%.bin | $(BINDIR)
	cp $< $@
	@test $$(find $@ -size $$(cat firmware_files.txt | grep "^$(basename $(notdir $@))" | cut -f4)c) || (rm -f $@ && echo "$@: filesize mismatch" && false)
	@test $$(shasum -a 256 $@ | awk '{print $$1}') "=" $$(cat firmware_files.txt | grep "^$(basename $(notdir $@))" | cut -f5) || (rm -f $@ && echo "$@: SHA256 mismatch" && false)

$(DLDIR)/%: | $(DLDIR)
	which curl >>/dev/null #Check that we have curl.
	curl -f -L $$(cat firmware_files.txt | grep "^$(notdir $@)" | cut -f2) -o $@
