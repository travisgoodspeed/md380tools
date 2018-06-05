#!/usr/bin/env python2
# -*- coding: utf-8 -*-
# KD4Z
# dmr-marc decided to stop offering CSV output format, so json here we come

import sys
import logging

try:
	import requests
except ImportError as e:
	sys.stderr.write('\n' + str(e) + '\n')
	sys.stderr.write('\nERROR: please install python-requests\n\n')
	exit(127)

DMRMARC_JSON_URL = "https://www.radioid.net/static/users.json"
LOGGER = logging.getLogger(__name__)

def noCommas(field):
	try:
	
		if field is None:
			return ""
			
		field = field.encode('ascii', 'ignore').decode('ascii')
		
		
		
	except (UnicodeEncodeError, TypeError, AttributeError):
		LOGGER.exception("Failed to decode/encode field")
		
	return removeSubstr(field,",")

def removeSubstr(field, subfield):

	try:
		 
		field = field.replace(","," ").strip()
		
	except (UnicodeEncodeError, TypeError, AttributeError):
		LOGGER.exception("Failed to strip/replace field")
		
	return field
	
def dmrmarc_json():
	
	
	LOGGER.info("Requesting DMRMARC users database from %s", DMRMARC_JSON_URL)
	myreq = requests.get(DMRMARC_JSON_URL)

	dmrmarc = myreq.json()

	LOGGER.info("Processing JSON")
	for user in dmrmarc['users']:

		try:
			sir = noCommas(user["surname"])
			fullname = noCommas(user["name"])
			if len(sir) > 0:
				fullname = fullname + " " + sir
			
			line = "{0},{1},{2},{3},{4},{5},".format(
				user["radio_id"],
				noCommas(user["callsign"]),
				fullname,
				noCommas(user["city"]),
				noCommas(user["state"]),
				noCommas(user["country"]))
		    
			print(line)
			
			 
		except (UnicodeEncodeError, TypeError, AttributeError):
			LOGGER.exception("Failed to process user")
			sys.exc_clear()
			
			
def main():
	logging.basicConfig(level=logging.INFO)
	try:
		dmrmarc_json()()
	
	except Exception:
		LOGGER.exception("Overall failure")

	
if __name__ == '__main__':
	main()
