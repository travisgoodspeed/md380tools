.PHONY: all update clean

update: users.csv
all: users.csv repeaters.csv users.json repeaters.json

clean:
	rm -f users.csv repeaters.csv users.json repeaters.json special_IDs.csv

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
