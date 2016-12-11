
.PHONY: all update clean

#all: stripped.csv repeaters.csv users.json repeaters.json
all: stripped.csv 

clean:
	rm -f repeaters.csv users.json repeaters.json special_IDs.csv *.tmp stripped.csv

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

stripped.csv: custom.csv special.tmp fixed.csv dmrmarc.tmp reflector.tmp
	awk -F, -f combine.awk $^ | $(ICONV)  | sort -t ',' -k1g,1 | awk -F, -f strip.awk >stripped.csv	
	
dmrmarc.tmp:
	curl -L -f 'http://www.dmr-marc.net/cgi-bin/trbo-database/datadump.cgi?table=users&format=csv&header=0' | perl -pe 's,<br/>,,' >$@
	
reflector.tmp:
	curl -L -f 'http://registry.dstar.su/reflector.db' | perl -pe '$$_ = "" if ( $$. == 1 ); s#@#,#; s#@.*#,,,,,,#' >$@
		
special.tmp:
	python2 get_special_IDs.py
	
users.json:
	curl -L -f 'http://www.dmr-marc.net/cgi-bin/trbo-database/datadump.cgi?table=users&format=json&header=0' >$@

repeaters.json:
	curl -L -f 'http://www.dmr-marc.net/cgi-bin/trbo-database/datadump.cgi?table=repeaters&format=json&header=0' >$@

repeaters.csv:
	curl -L -f 'http://www.dmr-marc.net/cgi-bin/trbo-database/datadump.cgi?table=repeaters&format=csv&header=0' | perl -pe 's,<BR/>,,' >$@

# not used
bmgroups.json:
	curl -L -f 'https://brandmeister.network/dist/js/bm/groups.js' >$@
	

ci: clean all
	
