

all: users.csv repeaters.csv

clean:
	rm -f users.csv repeaters.csv

users.csv:
	curl 'http://www.dmr-marc.net/cgi-bin/trbo-database/datadump.cgi?table=users&format=csv&header=0' >users.csv
repeaters.csv:
	curl 'http://www.dmr-marc.net/cgi-bin/trbo-database/datadump.cgi?table=repeaters&format=csv&header=0' >repeaters.csv
