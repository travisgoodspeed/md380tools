#!/usr/bin/env python2
# -*- coding: utf-8 -*-

from __future__ import print_function

import json
import socket
import sys

try:
    from urllib2 import URLError, HTTPError  # Py2
except ImportError:
    from urllib.error import URLError, HTTPError  # Py3

try:
    from urllib2 import Request, urlopen  # Py2
except ImportError:
    from urllib.request import Request, urlopen  # Py3

bm_master = Request("http://registry.dstar.su/api/node.php")

try:
    response = urlopen(bm_master)
except URLError as e:
    print("Unable to fetch list of BM masters!\n")
    sys.exit(0)

data = json.load(response)
print("Fetching list of special IDs from BM master servers.\n")

# file = open('users.csv', 'a')
file = open('special.tmp', 'a')
# Add ID 5000 manually as this is not listed anywhere
# file.write('5000,Status,,,,,,\n')

for idx, item in enumerate(data):
    print(str(idx) + ": ID->" + item['ID'] + " Country->" + item['Country'] + " Address->" + item['Address'])
    req = Request("http://" + item['Address'] + "/md380tools/special_IDs.csv")
    try:
        response = urlopen(req, timeout=20)
    except HTTPError as e:
        # if e.code == 404:
        print("List with special IDs not found!\n")
    except URLError as e:
        print("Could not talk to master server in " + item['Country'] + "!\n")
    except socket.timeout:
        print("Socket Timeout...\n")
    else:
        # Improved error handling for socket errors - MW0MWZ
        try:
            content = response.read()
        except socket.timeout:
            print("Socket Timeout...\n")
        except socket.error:
            print("Socket Error...\n")
        else:
            # Handle servers which answer with HTTP 200 but give file not found
            if "DOCTYPE" in content:
                print("List with special IDs not found!\n")
            else:
                print(content)
                file.write(content)

file.close()
sys.exit(0)
