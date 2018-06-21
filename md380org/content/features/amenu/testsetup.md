+++
date = "2018-06-20T22:56:00-04:00"
draft = false
title = "Test/Setup"

[menu.main]
parent = "amenu"
identifier = "testsetup"

+++

The Application Menu also has some testing and setup submenus, found here.
![amenu test/setup](amenu_test_setup.bmp)

![amenu test/setup inside](amenu_test_setup_inside.bmp)


Backlight
---------
Your first options are for the backlight - here, I have a bug, where "Level Hi" should be equal to 9, the brightest backlight setting. This is likely my fault, so I'll just go fix that number. I'd also like to set "Lo" (where the screen would be "off" in the stock firmware) to "1", which is very dim but still readable in low light. "0" is, of course, off - no backlight. You change these values like any other field on the MD380 - select with the arrow keys, green button to edit the field, arrow keys to modify the value, and green to save the value.
![amenu setup backlight levels](amenu_setup_backlight_levels.bmp)

The Time/sec field is how many seconds of inactivity before the backlight is set to "Lo".


Morse Output
------------
Morse output is off by default but can be "enabled" or even "verbose". Morse output will play the various menu entries using morse code, allowing blind operation of the radio.
Menu entries are read with the label in a lower pitch, and the value in a higher pitch.
![morse off](morse_off.bmp)
Morse off naturally means nothing is "read" aloud to you in morse code.
![morse enabled](morse_enabled.bmp)
Enabled will read menu options and channel numbers.
![morse verbose](morse_verbose.bmp)
Enabled will read menu options and channel names.
![morse volume 100](morse_volume_100.bmp)
You can also change the speed, pitch, and volume of the morse narrator to your liking.

Colours
-------
Change your color scheme, or use existing presets.
![amenu setup colors](amenu_setup_colors.bmp)
Choose a color scheme by selecting with arrow keys, and saving with the green entry.
![amenu setup colorscheme menu](amenu_setup_colors_menu.bmp)
There are a few options, including green on dark green:
![amenu setup green colors](amenu_setup_green_color.bmp)
You can edit colors manually. 
![amenu setup colorscheme menu manual color](amenu_setup_green_color_manual.bmp)
This is the color editor screen. 
The green radio button will save the current color.
The 1, 2, and 3 buttons will choose which color channel (red, green, blue) you are editing.
The arrow keys can adjust that channel up or down.
![amenu setup colorscheme menu color editor](color_editor.bmp)


Test/Debug
----------
Data and options for test and debugging purposes.
![amenu setup colorscheme menu](amenu_setup_green_color_test_debug.bmp)
