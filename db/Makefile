.PHONY: all update clean

#update: users.csv
	
all: users.csv repeaters.csv users.json repeaters.json

clean:
	rm -f users.csv repeaters.csv users.json repeaters.json special_IDs.csv	
	-rm stripped.csv

update:
	-rm users.csv
	"${MAKE}" users.csv
	

#This strips out all unicode characters.
#We'd rather just drop the accents.
ICONV=iconv -c -f UTF-8 -t ascii//TRANSLIT

custom.csv:
	touch custom.csv

stripped.csv: users.csv custom.csv
	cat users.csv fixed.csv custom.csv | $(ICONV)| sort -t ',' -k1g,1 | awk -F, -f strip.awk >stripped.csv	
	
dmrmarc.tmp:
	curl -L -f 'http://www.dmr-marc.net/cgi-bin/trbo-database/datadump.cgi?table=users&format=csv&header=0' | perl -pe 's,<br/>,,' >$@
	

users.csv: dmrmarc.tmp
	curl -L -f 'http://registry.dstar.su/reflector.db' | perl -pe '$$_ = "" if ( $$. == 1 ); s#@#,#; s#@.*#,,,,,,#' >$@
	perl -e 'print "\n"' >>$@
	python2 get_special_IDs.py
	cat dmrmarc.tmp >>$@
	
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
	