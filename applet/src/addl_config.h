/*! \file addl_config.h
  \brief .
*/

#ifndef MD380TOOLS_ADDL_CONFIG_H_INCLUDED
#define MD380TOOLS_ADDL_CONFIG_H_INCLUDED

#include <stdint.h>

#define FLASH_OFFSET_DMRID           0x2084
#define FLASH_OFFSET_RNAME           0x20B0
#define FLASH_OFFSET_BOOT_BOTTONLINE 0x2054

// Obvious note to newbies... these live in SPI...
// This means they should never be reordered 
// or the settings will get tangled up.
typedef struct addl_config {
    uint8_t crc;
    uint8_t length;
    uint8_t datef;
    uint8_t userscsv;
    uint8_t debug;
    uint8_t promtg;
    uint8_t experimental;
    uint8_t micbargraph;
    uint8_t netmon;
    uint8_t rbeep;
    uint32_t dmrid;
    uint8_t boot_demo; 
    uint8_t _boot_splash; // unused
    char _rname[32]; // unused.
    uint8_t cp_override ;
    char bootline1[10];
    char bootline2[10];
    uint8_t backlight_intensities; // lower nibble = backlight intensity during IDLE,  upper nibble = backlight intensity during ACTIVITY
    uint8_t narrator_mode; // Mode / verbosity for CW output. See narrator.c .
    uint8_t cw_pitch_10Hz; // 'CW pitch' in TEN HERTZ units, to fit in a BYTE.
    uint8_t cw_volume;    // output 'volume' (PWM duty cycle) for CW messages. 
      //  0..100 [%] for fixed volume, or BEEP_VOLUME_AUTO to control via pot:
#     define BEEP_VOLUME_AUTO 255
    uint8_t cw_speed_WPM;  // CW output speed,  measured in words per minute .
    uint16_t fg_color;  // foreground (text) colour for own menus, and maybe console screens too
    uint16_t bg_color;  // normal background colour, also customizeable in app_menu.c ...
    uint16_t sel_fg_color;  // fg colour to mark the selection- or navigation bar
    uint16_t sel_bg_color;  // bg colour to mark the selection- or navigation bar
    uint16_t edit_fg_color; // fg colour of a cell in "edit mode" (or, single char for the CURSOR)
    uint16_t edit_bg_color; // fg colour of a cell in "edit mode" (or, single char for the CURSOR)
       // The above 16-bit colours use the display's native 'BGR565'-format,
       // as used for the 'alternative' menu and the alternative LCD driver:
       // BLUE component in bits 15..11, GREEN in bits 10..5, RED in bits 4..0 .

} addl_config_t ;

#define CPO_BL1 0x1
#define CPO_BL2 0x2
#define CPO_DMR 0x4

extern addl_config_t global_addl_config;

extern void init_global_addl_config_hook(void);

void cfg_fix_radioname();
void cfg_set_radio_name();

void cfg_save();

#endif
