#!/usr/bin/env python2
# -*- coding: utf-8 -*-

# Copyright 2021 Dale Farnsworth
#
# This file converts a canonical md380 users database file
# into an indexed database file containing the same information,
# but uilizing about half of the space.

from __future__ import print_function

import sys
import os

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
    MAGIC         = 0x300a01
    MAGICOFFSET   = 0
    HEADERSIZE    = 9
    INDEXSIZE     = 6

    NAMEFLAG      = 1 << 7
    NICKNAMEFLAG  = 1 << 6
    CITYFLAG      = 1 << 5
    STATEFLAG     = 1 << 4
    COUNTRYFLAG   = 1 << 3

    SHORTCALLSIGN = 7

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

    def append_flag(self, flag):
        self.buffer[self.end_offset] = flag
        self.end_offset += 1

    def append_2byte_offset(self, offset):
        self.buffer[self.end_offset] = (offset >> 8) & 0xff
        self.buffer[self.end_offset+1] = offset & 0xff
        self.end_offset += 2

    def append_string_with_flag(self, str, flag):
        self.buffer[self.end_offset] = flag | len(str)
        self.end_offset += 1

        bytes = str.encode("utf-8")
        for i, byte in enumerate(bytes):
            self.buffer[self.end_offset+i] = byte

        self.end_offset += len(str)

    def append_string(self, str):
        self.append_string_with_flag(str, 0)

    def append_string_node(self, string):
        if string not in self.string_dict:
            self.string_dict[string] = self.end_offset
            self.append_string(string)

        return self.string_dict[string]

    def append_name_node(self, name):
        return self.append_string_node(name)

    def append_nickname_node(self, nickname):
        return self.append_string_node(nickname)

    def append_city_node(self, city, state, country):
        str = ",".join([city, state, country])
        if str not in self.city_dict:
            if len(state) > 0:
                state_offset = self.append_state_node(state, country)
            elif len(country) > 0:
                country_offset = self.append_country_node(country)

            self.city_dict[str] = self.end_offset

            self.append_string(city)

            if len(state) > 0:
                self.append_offset(state_offset)
            elif len(country) > 0:
                self.append_2byte_offset(country_offset)

        return self.city_dict[str]

    def append_state_node(self, state, country):
        str = ",".join([state, country])

        if str not in self.state_dict:
            if len(country) > 0:
                country_offset = self.append_country_node(country)

            self.state_dict[str] = self.end_offset

            self.append_string(state)

            if len(country) > 0:
                self.append_2byte_offset(country_offset)
        
        return self.state_dict[str]


    def append_country_node(self, country):
        return self.append_string_node(country) - self.node_pool_offset

    def append_callsign_node(self, callsign, name, nickname, city, state, country):
        str = ",".join([callsign, name, nickname, city, state, country])
        if str not in self.callsign_dict:
            flag = 0

            if len(name) > 0:
                flag |= self.NAMEFLAG
                name_offset = self.append_name_node(name)

            if len(nickname) > 0:
                flag |= self.NICKNAMEFLAG
                nickname_offset = self.append_nickname_node(nickname)

            if len(city) > 0:
                flag |= self.CITYFLAG
                city_offset = self.append_city_node(city, state, country)

            if len(state) > 0:
                flag |= self.STATEFLAG
                state_offset = self.append_state_node(state, country)

            if len(country) > 0:
                flag |= self.COUNTRYFLAG
                country_offset = self.append_country_node(country)

            self.callsign_dict[str] = self.end_offset

            if len(callsign) == 0 or len(callsign) > self.SHORTCALLSIGN:
                self.append_flag(flag)
                flag = 0

            self.append_string_with_flag(callsign, flag)

            if len(name) > 0:
                self.append_offset(name_offset)

            if len(nickname) > 0:
                self.append_offset(nickname_offset)

            if len(city) > 0:
                self.append_offset(city_offset)
            elif len(state) > 0:
                self.append_offset(state_offset)
            elif len(country) > 0:
                self.append_2byte_offset(country_offset)

        return self.callsign_dict[str]

    def update_header(self):
        self.offset = self.MAGICOFFSET
        self.put3(self.MAGIC)
        self.put3(self.length)
        self.put3(self.end_offset)

    def add_user(self, id, callsign, name, nickname, city, state, country):
        self.put3(id)
        self.put3(self.append_callsign_node(callsign, name, nickname, city, state, country))

    def write_file(self, filename):
        self.update_header()
        f = open(filename, "w+b")
        f.write(self.buffer[:self.end_offset])

def linearDB_to_indexedDB(linear_filename, indexed_filename):
    linear = LinearDB(linear_filename)
    indexed = IndexedDB(len(linear.users))

    for id, user in sorted(linear.users.items()):
        (idstr, callsign, name, city, state, nickname, country) = user
        if len(country) > 0:
            indexed.append_country_node(country)

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
