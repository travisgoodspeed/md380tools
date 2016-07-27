.PHONY: all update clean

update: users.csv
all: users.csv repeaters.csv users.json repeaters.json

clean:
	rm -f users.csv repeaters.csv users.json repeaters.json

users.csv:
	curl -L -f 'http://registry.dstar.su/reflector.db' | perl -pe '$$_ = "" if ( $$. == 1 ); s#@#,#; s#@.*#,,,,,,#' >$@
	perl -e 'print "\n"' >>$@
	perl -ne 'print $$_;' < special_IDs.csv >>$@
	curl -L -f 'http://www.dmr-marc.net/cgi-bin/trbo-database/datadump.cgi?table=users&format=csv&header=0' | perl -pe 's,<br/>,,' >>$@
users.json:
	curl -L -f 'http://www.dmr-marc.net/cgi-bin/trbo-database/datadump.cgi?table=users&format=json&header=0' >$@
repeaters.json:
	curl -L -f 'http://www.dmr-marc.net/cgi-bin/trbo-database/datadump.cgi?table=repeaters&format=json&header=0' >$@
repeaters.csv:
	curl -L -f 'http://www.dmr-marc.net/cgi-bin/trbo-database/datadump.cgi?table=repeaters&format=csv&header=0' | perl -pe 's,<BR/>,,' >$@
