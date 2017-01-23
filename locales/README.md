This is the very beginning of language localization for MD380Tools.
Localization is not yet ready for users, but developers might find
this handy.

The current Makefile only localizes unpatched firmware, and only for
Hungarian, but it should be clear enough how to add new languages.
When locales are ready, we'll patch 'make dist' to apply them as it is
composing the firmware for distribution.

`strings.txt` contains the strings from 2.032, some of which have been
patched or changed in more recent versions.  Strings must match exactly,
and only two-byte wide strings are patched.

Run `make clean all` to build D013.020 for all language targets.
S013.020 patches will come soon, but are not supported by this Makefile.
