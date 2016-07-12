This directory is used to auto-generate symbols for unknown MD380
firmware versions from the present gold-standard version, which will
likely be 2.032 for quite some time.

The tool's parser is too primitive to understand addition, so please
ensure that all input lines are either (1) just a comment, (2) just
whitespace, or (3) a symbol name and value with no arithmetic.

'make clean all' ought to produce symbol files for all versions, for
all functions that match.

Cheers,
--Travis

