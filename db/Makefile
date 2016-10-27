.PHONY: all update clean

update: users.csv
	
all: users.csv repeaters.csv users.json repeaters.json

clean:
	rm -f users.csv repeaters.csv users.json repeaters.json special_IDs.csv	
	-rm tmp.csv stripped.csv

#This strips out all unicode characters.
#We'd rather just drop the accents.
ICONV=iconv -c -f UTF-8 -t ascii//TRANSLIT


.PHONY: stripped.csv
stripped.csv:: users.csv 
	cat users.csv >tmp.csv
	if [ -e db/custom.csv ]; then cat custom.csv >> tmp.csv; fi
	$(ICONV) tmp.csv | sort | awk -F, -f strip.awk >stripped.csv

users.csv:
	curl -L -f 'http://registry.dstar.su/reflector.db' | perl -pe '$$_ = "" if ( $$. == 1 ); s#@#,#; s#@.*#,,,,,,#' >$@
	perl -e 'print "\n"' >>$@
	python2 get_special_IDs.py
	curl -L -f 'http://www.dmr-marc.net/cgi-bin/trbo-database/datadump.cgi?table=users&format=csv&header=0' | perl -pe 's,<br/>,,' >>$@
	
users.json:
	curl -L -f 'http://www.dmr-marc.net/cgi-bin/trbo-database/datadump.cgi?table=users&format=json&header=0' >$@

repeaters.json:
	curl -L -f 'http://www.dmr-marc.net/cgi-bin/trbo-database/datadump.cgi?table=repeaters&format=json&header=0' >$@

repeaters.csv:
	curl -L -f 'http://www.dmr-marc.net/cgi-bin/trbo-database/datadump.cgi?table=repeaters&format=csv&header=0' | perl -pe 's,<BR/>,,' >$@

ci: clean all
	