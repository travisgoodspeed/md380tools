
.PHONY: themes

all: site

themes:
	make -C themes all

server: themes
	hugo server --buildDrafts

site: themes
	hugo
	#Results are in public/

clean:
	rm -rf public
