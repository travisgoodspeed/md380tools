#!/usr/bin/python
# -*- coding: utf-8 -*-

# Small python script (actually used as seperate Jenkins job) to inform
# a telegram group of the downloads of the firmware
# output is somewhat YAML-formatted on purpose (as someone might need it elsewhere)
# there is an ugly non-close present on the socket as well as handwritten yaml instead of pyyaml -> true!

# Requires environment set for
# GITHUB_ORGANISATION - your username at github
# GITHUB_REPO - the repository you want to fetch stats for
# GITHUB_TOKEN - see JenkinsBuilder.md for more info on how to create
# TELEGRAM_PROXY - the telegram-cli IP address
# TELEGRAM_PORT - the telegram-cli TCP port
# TELEGRAM_RECIPIENT - the intended audience :)

# For interfacing with the GitHub v3 api
import json
import os
import socket
from io import StringIO

import pycurl

# Not used anymore
buffer = StringIO()
buffer_size = 1

# Talk to github requesting all releases
# you could do just '/releases/latest' if you want that - note to change the initial for loop
c = pycurl.Curl()
c.setopt(c.URL, 'https://api.github.com/repos/' + os.environ['GITHUB_ORGANIZATION'] + '/' + os.environ['GITHUB_REPO'] + '/releases')
c.setopt(c.HTTPHEADER, ['Authorization: token ' + os.environ['GITHUB_TOKEN']])
c.setopt(c.WRITEFUNCTION, buffer.write)
c.perform()
c.close()

# Reload data to python dict from JSON
data = buffer.getvalue()
data = json.loads(data)

# Prepare output
output = 'Downloads for %s:%s' % (os.environ['GITHUB_REPO'], '\\n')
# Walk releases ... if you do /release/latest you have to discard this for loop and change the next to use data instead of releases :)
for release in data:
    for key, values in release.items():
        # We are only interested in the assets
        if key == 'assets':
            first = 0
            for assets in values:
                name = 'n/a\n'
                downloads = 0
                # Fetch the name and downloadcount
                for asset, details in assets.items():
                    if asset == 'name':
                        file_name = details
                        if first == 0:
                            first = 1
                            output += ' - %s:%s' % (file_name[:7], '\\n')
                    if asset == 'download_count':
                        downloads = details
                # Summarize to output
                if downloads & gt; 0:
                    output += '   %s: %s%s' % (file_name[9:], downloads, '\\n')

# Prepare telegram_cli formatting (note the quotes in here and the double escaped n above)
message = 'msg %s "%s"\n' % (os.environ['TELEGRAM_RECIPIENT'], output)
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.settimeout(1)
s.connect((os.environ['TELEGRAM_IP'], os.environ['TELEGRAM_PORT']))
s.send(message)
# Ugly, trust me, I know ...
# s.recv(buffer_size)
# s.close()
