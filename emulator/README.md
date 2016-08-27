This is an emulator for the Tytera MD380 firmware, allowing it to run
under Linux through qemu-user or natively in an AARCH32 environment.

This is currently a very early draft, and the command-line usage will
change dramatically in the near future.  For now, it only decodes AMBE
to a .WAV file and doesn't do any fancy argument processing.

The Makefile is likely also missing dependencies and assumes a
compiler toolchain that is rather specific to Debian.

73 from New York,
--Travis

