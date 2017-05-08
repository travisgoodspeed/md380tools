# MD380 Tool Applet

This is a quick little applet that can be sideloaded into the
firmware, with its own global/static variables and callbacks to the
native firmware, as well as extensions to the USB protocol.

## Important Headers ##

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

## Important Modules ##

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

* `menu.c` contains the additions merged into the stock firmware's menu,
  i.e. the MD380Tools menu. 

* `gfx.c` exposes some of the graphic functions in the stock firmware.
  

## Optional Headers and Modules ##

The use of these optional headers and modules (listed below) is 
controlled via `config.h`. Some depend on others, for example the
alternative menu isn't possible without the alternative (faster)
LCD driver, etc.

* `netmon.c` started as an experimental network monitor, with different
  display screens (originally black-on-white text console screens)
  primarily intended for development. Some of the netmon screens can
  also be opened from the alternative menu (details below). 
  Originally, the netmon screens could only be opened via numeric keys
  after being enabled somewhere in the MD380Tools menu.

* `lcd_driver.c / .h` contains an alternative LCD driver, designed for
  speed and simplicity. Drawing characters into the framebuffer is much
  faster than with the original 'gfx' functions, because this driver
  copies pixels data into the framebuffer *without* sending
  the graphic coordinate *for each pixel* (a nice feature of the LCD
  controller completely ignored by Tytera's graphic functions). 

* `app_menu.c / .h` uses the alternative LCD driver to implement an own,
  simpler menu which already shows the current value without the need
  to enter a sub-dialog, screen.
  The user can open this alternative menu by pressing the red 'BACK'
  button on the main screen (where the 'BACK'-button had no function).

* `color_picker.c` is a tiny extension for the alternative menu,
  to modify colours as an R,G,B mixture instead of entering them
  as hex values. It is implemented as a callback function, used 
  by several menu items in the alternative 'setup' menu.

* `font_8_8.c` is an old but simple bitmap font with 256 characters,
  8 x 8 pixels per character, from *Codepage 437*. It contains some 
  western diacritics (which Tytera doesn't have), and the ancient 
  'line drawing characters' that allow drawing alert boxes and similar
  with a single via 'printf' (here: LCD_PrintfAt) call.
  
* `narrator.c / .h` can read out menus, channel names, zones, etc in Morse
  code for visually impaired hams. Requires `irq_handlers.c` to generate
  the tones in Morse code.

* `irq_handlers.c / .h` once contained multiple interrupt handlers,
  but (at the time of this writing) only hooks into the SysTick 
  interrupt handler.
  Polls keys for app_menu, generates Morse output for the 'Narrator',
  and can optionally dim the display backlight. 



## USB Protocol ##

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


