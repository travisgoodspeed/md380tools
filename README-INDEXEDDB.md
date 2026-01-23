# MD380 Indexed User DB Description #
This file contains a description of an alternative indexed format
for storing the users database inside an md380 radio.  Instead of
a linear string format, the indexed format is organized in a tree
structure, such that every string associated with the user (callsign,
name, city, state, and country) is stored only once and referenced by
each of the users to which it applies.  The motivation for producing
a new format is to reduce the space required for the users database.
This format typically uses less than half the space used by the
standard format.

The first bytes of the header cause firmware that doesn't support
the indexed format so see the database as an empty database in
the original database format.  This avoids crashes.

The code to handle the indexed format co-exists with the existing
code that handles the original database format.  This will permit
a possibly lengthy transition period before the number of DMR users
exceeds the capacity of the original database format.

The database is composed of 3 sections: 1) header section, 2) index
table section, and 3) node data section.  Unless otherwise stated,
offsets are measured from the beginning of the header section.

## Header Section ##
The header section has 3 parts, each 3 bytes in length: 1) a magic
number with value 0x300a01, 2) the number of user entries in in the
index table section, and 3) the size of the entire database in bytes.
The last entry is useful for a utility that needs to extract the
database from the radio.

Note that the magic number's first two characters are ascii '0'
and '\n'.  Older firmware expecting the original linear database
format will see that as an empty database.


          Header +------------------------------+
                 |         Magic number         |
                 +------------------------------+
                 |      # of user entries       |
                 +------------------------------+
                 |       Size of database       |
                 +------------------------------+

  
## Index Table Section ##
The index table section contains an array of entries, one for each DMR
user in the database, sorted by DMR ID. Each entry has two 3-byte parts:
1) the user's DMR ID, and 2) the offset of the user node.

All offsets are stored most-significant byte first (big endian).

     Index Table +------------------------------+
                 |           DMR ID 0           |
                 +------------------------------+
                 |    Offset of user node 0     |
                 +------------------------------+
                 |           DMR ID 1           |
                 +------------------------------+
                 |    Offset of user node 1     |
                 +------------------------------+
                 |           DMR ID 2           |
                 +------------------------------+
                 |    Offset of user node 2     |
                 +------------------------------+
                               .
                               .
                               .
                 +------------------------------+
                 |           DMR ID N-1         |
                 +------------------------------+
                 |   Offset of user node N-1    |
                 +------------------------------+

  
## Node Data Section ##
The node data section contains a pool of user data nodes.  The nodes
are in arbitrary order, though they are arranged in a tree by offsets
(serving as pointers) contained in other nodes.  Note that the country
nodes are stored at the beginning of the node data section.  This permits
them to be referenced by a 2-byte offset contained in a state node.

       Node Data +------------------------------+
                 |    Beginning of node data    |
                 +------------------------------+
                               .
                               .
                               .
                 +------------------------------+
                 |     End of user database     |
                 +------------------------------+


## User Node ##
A user node contains the user's callsign and several optional fields.
The optional fields are:

  A 3-byte offset to the user's name node,

  A 3-byte offset to the user's nickname node.

  A 3-byte offset to the user's city node.

  A 3-byte offset to the user's state node.

  A 2-byte offset to the user's counry node.

The callsign character string is preceded by one or two bytes.
The first byte contains 5 flag bits and a 3-bit callsign length.
If the callsign length exceeds 7 characters, the 3-bit length
field is set to 0 and an additional byte is added containing
the callsign length.  The flag bits are set to indicate that
the corresponding node exists in this node or one of the nodes
pointed to by one of the offsets contained in this node.

Note that only the first of city node offset, state node offset,
or country node offset will be contained in the user node, since
each of these nodes contains the offset of the following node types.

        User Node +-------------------+
                  |      flag/len     |
                  +-------------------+
                  |  alen (optional)  |
                  +-------------------+
                  |   callsign char 0 |
                  +-------------------+
                  |   callsign char 1 |
                  +-------------------+
                  |   callsign char 2 |
                  +-------------------+
                            .
                            .
                            .
                  +-------------------+
                  |callsign char len-1|
                  +------------------------------------+
                  |   Offset of name node (optional)   |
                  +------------------------------------+
                  | Offset of nickname node (optional) |
                  +------------------------------------+
                  |   Offset of city node (optional)   |
                  +------------------------------------+
                  |   Offset of state node (optional)  |
                  +------------------------------------+
                  | Offset of country node* (optional) |
                  +------------------------------------+

        * The offset of the country node is a 2-byte field whose
          value is relative to the start of the node data section.

 
## Name Node ##
A name node contains the user's name.  The len field
contains the length of the following user name string.

        Name Node +-------------------+
                  |        len        |
                  +-------------------+
                  |    name char 0    |
                  +-------------------+
                  |    name char 1    |
                  +-------------------+
                  |    name char 2    |
                  +-------------------+
                            .
                            .
                            .
                  +-------------------+
                  |  name char len-1  |
                  +-------------------+
 

## Nickname Node ##
A nickname node contains the user's nickname.  The len field
contains the length of the following user nickname string.

    Nickname Node +-------------------+
                  |        len        |
                  +-------------------+
                  |  nickname char 0  |
                  +-------------------+
                  |  nickname char 1  |
                  +-------------------+
                  |  nickname char 2  |
                  +-------------------+
                            .
                            .
                            .
                  +-------------------+
                  |nickname char len-1|
                  +-------------------+


## City Node ##
A city node contains the user's city name.  It also contains: an
optional 3-byte offset of the user's state node and an optional
2-byte offset of the user's country node.  The len field contains
the length of the following city string.

Note that if the city node contains a state node offset it will
not contain a country node offset.

        City Node +---------------+
                  |      len      |
                  +---------------+
                  |  city char 0  |
                  +---------------+
                  |  city char 1  |
                  +---------------+
                  |  city char 2  |
                  +---------------+
                          .
                          .
                          .
                  +---------------+
                  |city char len-1|
                  +----------------------------------+
                  | Offset of state node (optional)  |
                  +----------------------------------+
                  |Offset of country node* (optional)|
                  +----------------------------------+
 
        * The offset of the country node is a 2-byte field whose
          value is relative to the start of the node data section.
  

## State Node ##
A state node contains the user's state name.  It also optionally
contains a 2-byte offset of the user's country node.  The len field
contains the length of the following state string.

       State Node +----------------+
                  |      len       |
                  +----------------+
                  |  state char 0  |
                  +----------------+
                  |  state char 1  |
                  +----------------+
                  |  state char 2  |
                  +----------------+
                          .
                          .
                          .
                  +----------------+
                  |state char len-1|
                  +----------------------------------+
                  |Offset of country node* (optional)|
                  +----------------------------------+
 
        * The offset of the country node is a 2-byte field whose
          value is relative to the start of the node data section.
  
  
## Country Node ##
A country node contains the country name for a user.  The len
field contains the length of the following country name string.
Country nodes are stored at the beginning of the node data section.
This permits them to be referenced by a 2-byte offset in a state
node.

     Country Node +------------------+
                  |       len        |
                  +------------------+
                  |  country char 0  |
                  +------------------+
                  |  country char 1  |
                  +------------------+
                  |  country char 2  |
                  +------------------+
                           .
                           .
                           .
                  +------------------+
                  |country char len-1|
                  +------------------+
