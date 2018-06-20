include ../curl.mak

.PHONY: all update clean

all: stripped.csv

clean:
	rm -f special_IDs.csv *.tmp stripped.csv

update:
	"${MAKE}" clean
	"${MAKE}" stripped.csv

update_eur:
	"${MAKE}" clean
	"${MAKE}" stripped.csv
	<stripped.csv awk -F, -f privacy_filter.awk >stripped2.csv
	mv stripped2.csv stripped.csv

#This strips out all unicode characters.
#We'd rather just drop the accents.
ICONV=iconv -c -f UTF-8 -t ascii//TRANSLIT

custom.csv:
	touch custom.csv

stripped.csv: custom.csv special.tmp fixed.csv dmrmarc2.tmp reflector.tmp
	awk -F, -f combine.awk $^ | $(ICONV)  | sort -t ',' -k1g,1 | awk -F, -f strip.awk >stripped.csv

dmrmarc2.tmp: dmrmarc.tmp
	awk -F, -f insert_nick.awk <$< >$@

dmrmarc.tmp:
	wget --timeout=120 --no-check-certificate --wait=3 'https://ham-digital.org/status/users.csv' -O $@

reflector.tmp:
	curl $(CURL_FLAGS) 'http://registry.dstar.su/reflector.db' | perl -pe '$$_ = "" if ( $$. == 1 ); s#@#,#; s#@.*#,,,,,,#' >$@

special.tmp:
	python2 get_special_IDs.py

ci: clean all
