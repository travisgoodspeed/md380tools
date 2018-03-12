#!/usr/bin/env python2
# -*- coding: utf-8 -*-
# KD4Z
# dmr-marc decided to stop offering CSV output format, so json here we come

import requests
import sys

def noCommas(field):
	try:
	
		if field is None:
			return ""
			
		field = field.encode('ascii', 'ignore').decode('ascii')
		
		
		
	except (UnicodeEncodeError, TypeError, AttributeError):
		pass
		
	return removeSubstr(field,",")

def removeSubstr(field, subfield):

	try:
		 
		field = field.replace(","," ").strip()
		
	except (UnicodeEncodeError, TypeError, AttributeError):
		pass
		
	return field
	
def dmrmarc_json():
	
	
	myreq = requests.get('http://www.dmr-marc.net/cgi-bin/trbo-database/datadump.cgi?table=users&format=json')

	dmrmarc = myreq.json()

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
			sys.exc_clear()
			
			
def main():
	try:
		dmrmarc_json()()
	
	except Exception:
		pass

	
if __name__ == '__main__':
	main()
