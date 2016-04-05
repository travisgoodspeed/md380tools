#MD380 Tool Applet

This is a quick little applet that can be sideloaded into the
firmware, with its own global/static variables and callbacks to the
native firmware, as well as extensions to the USB protocol.

##Important Headers##

* `config.h` defines configuration options.  Many of these are
  disabled in the standard build, but they can be enabled at compile
  time.

* `printf.h` defines the heapless printf() routines that log to the
  dmesg buffer.  You'll need to include this, not just `stdio.h`,
  to use the buffer.

* `md380.h` defines callbacks to the MD380's official firmware.

* `gfx.h` defines callbacks to the MD380's graphics routines, for
  drawing text and images.  Keep in mind that other threads will draw
  to the screen, possible overwriting or interrupting your changes, as
  we don't yet understand the graphics semaphores of the stock
  firmware.

* `tooldfu.h` defines the USB transfer commands which are implemented
  in `usb.c`.

##Important Modules##

* `md380-2.032.c` defines symbols for version 2.032 of the official
  firmware.  Ideally, we'd like all of the symbols here to also be
  known for 2.034 and later releases, at which point we could switch
  to patching that firmware instead of this one.

* `usb.c` implements our hooks to the USB Device Firmware Update
  handlers on Endpoint 0.  You'll want to add new write commands to
  `usb_dnld_hook()`.  `usb_upld_hook()` reads from raw memory, but
  overloaded addresses for special targets might be supported later.

* `dmesg.c` implements our kernel dmesg buffer.  `printf()` calls go here.

* `dmr.c` implements our hooks to the DMR digital radio stack.
  Headers and Data frames can easily be sniffed, but we're not quite
  sure how to grab raw audio, which is passed through the AMBE+
  emulator instead of the DMR stack code.

##USB Protocol##

The normal Tytera firmware implements something much like the USB
Device Firmware Update protocol, forked from STMicro's code examples
for the STM32F405.  Like traditional DFU, commands involve
transactions from Block 0 and data transactions occur at Block 2 and
higher, corresponding to a base address which is set by command.

Because Block 1 is unused by the official protocol, we hook the UPLOAD
and DNLOAD handlers to replace the behavior of that block in order to
implement our own protocol extensions.  These extensions are
implemented in `usb.c` and defined in `tooldfu.h`.  You can find
example host implementations in `md380-tool`, which is a Python script
from this repository, or `MD380Tool`, which is an Android app
developed separately at https://github.com/travisgoodspeed/MD380Tool .


