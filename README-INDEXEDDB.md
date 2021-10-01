# MD380 Indexed User DB Description #
This file contains a description of a new format for storing the
users database inside an md380 radio.  The motivation for producing
a new format is to reduce the space required for the users database.
A significnt space reduction is provided by this format.  As an
example at one point in time the standard linear formatted database
occupies 10966045 bytes and the new format containing the same
information occupies only 4666567 bytes.  The new format uses
43% of the space used by the standard format.

The first bytes of the header cause firmware that doesn't support
the new indexed format so see the database as an empty database
in the original database format.

The code to handle the new indexed format co-exists with the existing
code that handles the original database format.  This will permit
a lengthy transition period before the number of DMR users exceeds
the capacity of the original database format.

The indexed database contains information for a list of DMR users.
It is organized in a tree structure, such that every string associated
with the user (callsign, name, city, state, and country) is stored
only once and referenced by each of the users to which it applies.
The database is composed of 3 sections: 1) header section,
2) index table section, and 3) node data section.  Unless otherwise
stated (see the state node description), offsets are measured from
the beginning of the header section.

## Header Section ##
The header section has 3 parts, each 3 bytes in length: 1) a magic number
with value 0x300a01, 2) the number of user entries in in the index table section,
and 3) the size of the entire database in bytes.  The last entry is useful for
a utility that needs to extract the database from the radio.

Note that the magic number's first two characters are ascii '0' and '\n'.  Older
firmware expecting the original linear database format will see that as an empty
database.


          Offset +------------------------------+
               0 |         Magic number         |
                 +------------------------------+
               3 |      # of user entries       |
                 +------------------------------+
               6 |       Size of database       |
                 +------------------------------+

  
## Index Table Section ##
The index table section contains an array of entries, one for each DMR
user in the database, sorted by DMR ID. Each entry has two 3-byte parts:
1) the user's DMR ID, and 2) the offset of the user's callsign node.

All offsets are stored most-significant byte first (big endian).

          Offset +------------------------------+
               9 |           DMR ID 0           |
                 +------------------------------+
              12 |  Offset of callsign node 0   |
                 +------------------------------+
              15 |           DMR ID 1           |
                 +------------------------------+
              18 |  Offset of callsign node 1   |
                 +------------------------------+
              21 |           DMR ID 2           |
                 +------------------------------+
              24 |  Offset of callsign node 2   |
                 +------------------------------+
                               .
                               .
                               .
                 +------------------------------+
       (N-1)*6+9 |           DMR ID N-1         |
                 +------------------------------+
      (N-1)*6+10 | Offset of Callsign data N-1  |
                 +------------------------------+

  
## Node Data Section ##
The node data section contains a pool of user data nodes.  The nodes
are in arbitrary order, though they are arranged in a tree by offsets
(serving as pointers) contained in other nodes.  Note that the country
nodes are stored at the beginning of the node data section.  This permits
them to be referenced by a 2-byte offset contained in a state node.

          Offset +------------------------------+
          N*6+12 |    Beginning of node data    |
                 +------------------------------+
                               .
                               .
                               .
                 +------------------------------+
                 |     End of user database     |
                 +------------------------------+


## Callsign Node ##
A callsign node contains the callsign for a user as well as
offsets to the user's name node, (optionally) to the user's nickname
node, and (optionally) to the user's city node.  The high bit (0x80)
of the flag/len field is 1 when the node contains a nickname
node offset, 0 otherwise.  The next highest bit (0x40) of the flag/len
field is 1 when the node contains a city node offset, 0 otherwise.
The low 6 bits contain the length of the following callsign string.
Thus callsignes are limited to a length of 63 characters.

          Offset +-------------------+
             x+0 |      flag/len     |
                 +-------------------+
             x+1 |  callsign char 0  |
                 +-------------------+
             x+2 |  callsign char 1  |
                 +-------------------+
             x+3 |  callsign char 2  |
                 +-------------------+
                           .
                           .
                           .
                 +-------------------+
           x+len |callsign char len-1|
                 +----------------------------------+
         x+len+1 |       Offset of name node        |
                 +----------------------------------+
         x+len+4 |Offset of nickname node (optional)|
                 +----------------------------------+
         x+len+7 |  Offset of city node (optional)  |
                 +----------------------------------+

  
## City Node ##
A city node contains the city name for a user as well as
a 3-byte offset to the user's state node.  The len field
contains the length of the following city name string.

          Offset +---------------+
             y+0 |      len      |
                 +---------------+
             y+1 |  city char 0  |
                 +---------------+
             x+2 |  city char 1  |
                 +---------------+
             y+3 |  city char 2  |
                 +---------------+
                         .
                         .
                         .
                 +---------------+
           y+len |city char len-1|
                 +----------------------------------+
         x+len+1 |       Offset of state node       |
                 +----------------------------------+
  

## State Node ##
A state node contains the state name for a user as well as
a 2-byte offset to the user's country node.  The len field
contains the length of the following state name string.

          Offset +----------------+
             z+0 |      len       |
                 +----------------+
             z+1 |  state char 0  |
                 +----------------+
             z+2 |  state char 1  |
                 +----------------+
             z+3 |  state char 2  |
                 +----------------+
                         .
                         .
                         .
                 +----------------+
           z+len |state char len-1|
                 +----------------------------------+
         z+len+1 |     Offset of country node*      |
                 +----------------------------------+

        * The offset of the country node is a 2-byte field whose
          value is relative to the start of the node data section.
  
## Name Node ##
A name node contains the name of a user.  The len field
contains the length of the following user name string.

          Offset +---------------+
             u+0 |      len      |
                 +---------------+
             u+1 |  name char 0  |
                 +---------------+
             u+2 |  name char 1  |
                 +---------------+
             u+3 |  name char 2  |
                 +---------------+
                         .
                         .
                         .
                 +---------------+
           u+len |name char len-1|
                 +---------------+
  
  
## Nickname Node ##
A nickname node contains the nickname of a user.  The len field
contains the length of the following user nickname string.


          Offset +-------------------+
             v+0 |        len        |
                 +-------------------+
             v+1 |  nickname char 0  |
                 +-------------------+
             v+2 |  nickname char 1  |
                 +-------------------+
             v+3 |  nickname char 2  |
                 +-------------------+
                           .
                           .
                           .
                 +-------------------+
           v+len |nickname char len-1|
                 +-------------------+
  

## Country Node ##
A country node contains the country name for a user.  The len field
contains the length of the following country name string.  Country
nodes are stored at the beginning of the node data section.  This
permits them to be referenced by a 2-byte offset in a state node.

          Offset +------------------+
             w+0 |       len        |
                 +------------------+
             w+1 |  country char 0  |
                 +------------------+
             w+2 |  country char 1  |
                 +------------------+
             w+3 |  country char 2  |
                 +------------------+
                          .
                          .
                          .
                 +------------------+
           w+len |country char len-1|
                 +------------------+
