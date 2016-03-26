

all: users.csv repeaters.csv users.json repeaters.json data.h

clean:
	rm -f users.csv repeaters.csv users.json repeaters.json data.h

users.csv:
	curl 'http://www.dmr-marc.net/cgi-bin/trbo-database/datadump.cgi?table=users&format=csv&header=0' | sed 's,<br/>,,' >users.csv
users.json:
	curl 'http://www.dmr-marc.net/cgi-bin/trbo-database/datadump.cgi?table=users&format=json&header=0' >users.json
repeaters.json:
	curl 'http://www.dmr-marc.net/cgi-bin/trbo-database/datadump.cgi?table=repeaters&format=json&header=0' >repeaters.json
repeaters.csv:
	curl 'http://www.dmr-marc.net/cgi-bin/trbo-database/datadump.cgi?table=repeaters&format=csv&header=0' | sed 's,<BR/>,,' >repeaters.csv

data.h: users.csv
	egrep ^262  users.csv | cut -d "," -f 1,2,3 | cut -d " " -f1 > 262.csv
	dos2unix 262.csv
	./conv.py > data.h

