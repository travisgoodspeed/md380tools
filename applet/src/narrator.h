// File:    md380tools/applet/src/narrator.h
// Authors: Wolf (DL4YHF) [initial version], 
// Date:    2017-02-26 
//  Contains defines, variables, and prototypes for the 'narrator'
//  for visually impaired hams. Details and implementation in narrator.c .

#ifndef  CONFIG_DIMMED_LIGHT   // want 'dimmable backlight' ?
# define CONFIG_DIMMED_LIGHT 0 // guess not (else set CONFIG_DIMMED_LIGHT=1 in config.h)
#endif

#ifndef  CONFIG_MORSE_OUTPUT   // want output in Morse code ?
# define CONFIG_MORSE_OUTPUT 0 // guess not (else set CONFIG_MORSE_OUTPUT=1 in config.h)
#endif

// Variables READ BY the 'narrator' (often periodically, in narrator() ):
#if defined(FW_D13_020) || defined(FW_S13_020)
  extern uint8_t channel_num;    // borrowed from netmon.c ..
  // (channel_num wasn't in any header; address in applet/src/symbols_d13.020 + _s13.020 only)
  extern wchar_t channel_name[]; // borrowed from keyb.c (!)
#endif

// Possible values for global_addl_config.narrator_mode :
//  (some of these are combineable bitflags, forget about enums)
#define NARRATOR_MODE_OFF      0x00
#define NARRATOR_MODE_ENABLED  0x01
#define NARRATOR_MODE_VERBOSE  0x02
#define NARRATOR_READ_MENUS    0x04 /* <- not sure if this will ever work - WB, 2017-02-26 */
#define NARRATOR_MODE_TEST     0x08 /* just a test which 'events' narrator() detects */
                 /* (in irq_handlers.c, _TEST shows 'interference' with the red LED) */
// For shortest code, put everything inside a SMALL struct, and reference it
// via a pointer stored in a LOCAL variable (hopefully a register).

#if( CONFIG_MORSE_OUTPUT )
typedef struct tNarrator // instance data. 
{ 
  uint8_t mode;  // bitwise combination of NARRATOR_MODE-flags,
    //  copied from global_addl_config.narrator_mode somewhere

  uint8_t to_do; // what "to do" for the narrator ? bitwise combineable flags..
# define NARRATOR_PASSIVE      0x00 // nothing else to tell / everything 'done'
# define NARRATOR_REPORT_CHANNEL 0x01 // report current channel name / number
# define NARRATOR_REPORT_ZONE    0x02 // report current zone (on main screen)
# define NARRATOR_REPORT_BATTERY 0x04 // report whatever the battery icon does..
# define NARRATOR_REPORT_TITLE   0x08 // report title of the current menu
# define NARRATOR_REPORT_MENU    0x10 // report currently focused menu item
# define NARRATOR_READ_CONSOLE   0x20 // read out Console/Netmon screen
# define NARRATOR_APPEND_DEBUG_1 0x80 // append something.. only for debugging

  uint8_t item_index; // menu item index when reading the ENTIRE menu,
                      // or line number when reading console/netmon.
  uint8_t num_items;  // number of items to be read out,
                      // or number of lines on the console/netmon screen
    // (there's only enough space for ONE LINE in the Morse output FIFO,
    //  to save precious RAM. Thus read the screen line-by-line.)

  uint32_t stopwatch; // stopwatch for DELAYED activation after changes
                      // in the menu, channel knob, or similar
         

  // Purely 'internal stuff', to detect changes in narrator() :
  uint8_t old_opmode2; // previous value of gui_opmode, seen in..
  uint8_t old_opmode3; // ..the last call of narrator() [etc, etc]
  int     focused_item_index; // menu item index "with the cursor line"
              // (not necessarily the previously "Confirmed" option !)
  uint8_t channel_number;    // 0..15; bit 7 indicates UNPROGRAMMED channel

} T_Narrator;

extern T_Narrator Narrator;  // data for a single "storyteller" instance

void narrate(void); // called from various places (also periodically)
           // whenever something happened that may have to be 'told'.
void narrator_start_talking(void); // called per sidekey

// Aux. functions written for, and implemented in narrate.c :
int get_battery_voltage_mV(void);


#endif // CONFIG_MORSE_OUTPUT ?