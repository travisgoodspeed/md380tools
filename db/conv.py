#!/usr/bin/python

import fileinput
import csv

i=0;


print """struct call {
   int id;
   char call[7];
   char name[9];
   };"""
            
            

print "const struct call db[]={"
with open('262.csv', 'rb') as csvfile:
    data = csv.reader(csvfile, delimiter=',', quotechar='|')
    for row in data:
        col1, col2, col3 = row
        if i == 1:
           print ","
        i=1;
        print "{%s,\"%s\", \"%s\"}" % (col1, col2[:7], col3[:8]),
print "};";
    