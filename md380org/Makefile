

all: themes site

themes:
	make -C themes all

server: themes
	hugo server --buildDrafts

site:
	hugo
	#Results are in public/

