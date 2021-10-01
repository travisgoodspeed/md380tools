#!/usr/bin/env python2
# -*- coding: utf-8 -*-

from __future__ import print_function

import sys

class LinearDB(object):

    def __init__(self, filename):
        """Loads the database."""
        import csv

        self.users = {}

        try:
            if filename is None:
                filename = sys.path[0] + '/user.bin'
            with open(filename, 'rb') as csvfile:
                reader = csv.reader(csvfile)
                for row in reader:
                    if len(row) > 1:
                        self.users[int(row[0])] = row
        except:
            print("Can't open %s" % filename)


            # print("WARNING: Unable to load user.bin.")
            pass

class IndexedDB(object):
    MAGIC = 0x300a01
    MAGICOFFSET = 0
    HEADERSIZE = 9
    INDEXSIZE = 6
    MAXSTRLEN = 127
    NICKNAMEFLAG = 0x80
    CITYFLAG = 0x40

    def __init__(self, length):
        self.length = length
        self.buffer = bytearray(15*1024*1024)
        self.offset = self.HEADERSIZE
        self.node_pool_offset = self.HEADERSIZE + length * self.INDEXSIZE
        self.end_offset = self.node_pool_offset
        self.string_dict = {}
        self.callsign_dict = {}
        self.city_dict = {}
        self.state_dict = {}

    def set_offset(self, offset):
        self.offset = offset

    def get_offset(self):
        return self.offset

    def set_endoffset(self, offset):
        self.end_offset = offset

    def get_end_offset(self):
        return self.end_offset

    def id_int(self, id_str):
        return int(id_str)

    def put3(self, value):
        self.buffer[self.offset] = (value >> 16) & 0xff
        self.buffer[self.offset+1] = (value >> 8) & 0xff
        self.buffer[self.offset+2] = value & 0xff
        self.offset += 3

    def append_offset(self, offset):
        self.buffer[self.end_offset] = (offset >> 16) & 0xff
        self.buffer[self.end_offset+1] = (offset >> 8) & 0xff
        self.buffer[self.end_offset+2] = offset & 0xff
        self.end_offset += 3

    def append_2byte_offset(self, offset):
        self.buffer[self.end_offset] = (offset >> 8) & 0xff
        self.buffer[self.end_offset+1] = offset & 0xff
        self.end_offset += 2

    def append_string_with_offset(self, str, flag):
        if len(str) > self.MAXSTRLEN:
            str = str[:self.MAXSTRLEN]

        self.buffer[self.end_offset] = flag | len(str)
        self.end_offset += 1

        bytes = str.encode("utf-8")
        for i, byte in enumerate(bytes):
            self.buffer[self.end_offset+i] = byte

        self.end_offset += len(str)

    def append_string(self, str):
        self.append_string_with_offset(str, 0)

    def append_string_node(self, string):
        if string not in self.string_dict:
            self.string_dict[string] = self.end_offset
            self.append_string(string)

        return self.string_dict[string]

    def append_name(self, name):
        return self.append_string_node(name)

    def append_nickname(self, nickname):
        return self.append_string_node(nickname)

    def append_country(self, country):
        return self.append_string_node(country)

    def append_state(self, state, country):
        str = state + "," + country
        if str not in self.state_dict:
            country_offset = self.append_country(country)

            self.state_dict[str] = self.end_offset

            self.append_string(state)
            self.append_2byte_offset(country_offset - self.node_pool_offset)
        
        return self.state_dict[str]

    def append_city(self, city, state, country):
        str = city + "," + state + "," + country
        if str not in self.city_dict:
            state_offset = self.append_state(state, country)

            self.city_dict[str] = self.end_offset

            self.append_string(city)
            self.append_offset(state_offset)

        return self.city_dict[str]

    def append_callsign(self, callsign, name, nickname, city, state, country):
        str = callsign + "," + name + "," + nickname + "," + city + "," + state + "," + country
        if str not in self.callsign_dict:
            name_offset = self.append_name(name)
            nickname_offset = self.append_nickname(nickname)
            city_offset = self.append_city(city, state, country)

            self.callsign_dict[str] = self.end_offset

            flag = 0
            if len(nickname) != 0:
                flag |= self.NICKNAMEFLAG

	    if len(city) != 0 or len(state) != 0 or len(country) != 0:
		flag |= self.CITYFLAG

	    self.append_string_with_offset(callsign, flag)
	    self.append_offset(name_offset)

            if flag & self.NICKNAMEFLAG:
                self.append_offset(nickname_offset)

	    if flag & self.CITYFLAG:
		self.append_offset(city_offset)

        return self.callsign_dict[str]

    def update_header(self):
        self.offset = self.MAGICOFFSET
        self.put3(self.MAGIC)
        self.put3(self.length)
        self.put3(self.end_offset)

    def add_user(self, id, callsign, name, nickname, city, state, country):
        self.put3(id)
        self.put3(self.append_callsign(callsign, name, nickname, city, state, country))

    def write_file(self, filename):
        self.update_header()
        f = open(filename, "w+b")
        f.write(self.buffer[:self.end_offset])

def linearDB_to_indexedDB(linear_filename, indexed_filename):
    linear = LinearDB(linear_filename)
    indexed = IndexedDB(len(linear.users))

    for id, user in sorted(linear.users.items()):
        (idstr, callsign, name, city, state, nickname, country) = user
        indexed.append_country(country)

    for id, user in sorted(linear.users.items()):
        (idstr, callsign, name, city, state, nickname, country) = user
        indexed.add_user(id, callsign, name, nickname, city, state, country)

    indexed.write_file(indexed_filename)

def usage(msg):
    global progname

    if len(msg) > 0:
        print(msg, file=sys.stderr)
    print("Usage: %s [OPTION]... <linearDBfile> <indexedDBfile>" % progname, file=sys.stderr)
    print("\t-n, --nicknames", file=sys.stderr)
    print("\t\tInclude nicknames in database", file=sys.stderr)
    sys.exit(1)

def main():
    global progname
    progname = sys.argv[0]
    args = sys.argv[1:]

    if len(args) != 2:
        usage("")

    infilename = args[0]
    outfilename = args[1]

    linearDB_to_indexedDB(infilename, outfilename)

main()
