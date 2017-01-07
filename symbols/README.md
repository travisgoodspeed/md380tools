This directory contains symbols for known versions of of the MD380
firmware, as well as our Symgrate tool for guessing symbols of new
versions.

The tool's parser is too primitive to understand addition, so please
ensure that all input lines are either (1) just a comment, (2) just
whitespace, or (3) a symbol name and value with no arithmetic.


Here is how to generate smybols for D013.014 from the known symbols of
D013.020.

```
./symgrate D013.020.img D013.014.img symbols_d13.014 >symbols_d13_014
```

You can also run it interactively, which is handy when exploring one
version and comparing to another.

```
dell13% ./symgrate D013.020.img D013.014.img 
/* Symbols for D013.014.img imported from D013.020.img. */
foo = 0x0808ebef;
foo                  = 0x0808eb6f; /* 472 byte match */
bar = 0x080903c1;
bar                  = 0x08090341; /* 1024 byte match */
```

Cheers,
--Travis

