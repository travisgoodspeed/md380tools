#!/usr/bin/env python2

import urllib2
import json
import sys

url=urllib2

bm_master = urllib2.Request("http://registry.dstar.su/api/node.php")

try:
    response = urllib2.urlopen(bm_master)
except url.URLError as e:
    print("Unable to fetch list of BM masters")
    print e.reason
    sys.exit(1)

data = json.load(response)
print "Fetching list of special IDs from BM master servers."

file = open('special_IDs.csv', 'a')
# Add ID 5000 manually as this is not listed anywhere
file.write('5000,Status,,,,,,\n')

for idx, item in enumerate(data):
    print str(idx) + ": ID->" + item['ID'] + " Country->" + item['Country'] + " Address->" + item['Address']
    req = urllib2.Request("http://"+item['Address']+"/md380tools/special_IDs.csv")
    try:
        response = urllib2.urlopen(req)
    except url.HTTPError as e:
        #if e.code == 404:
        print "List with special IDs not found!"
    except url.URLError as e:
        print "Could not talk to master server in " + item['Country'] + "!"
    else:
        content = response.read()
        # Handle servers which answer with HTTP 200 but give file not found
        if "DOCTYPE" in content:
            print "List with special IDs not found!"
        else:
            print content
            file.write(content)

file.close()
sys.exit(0)
