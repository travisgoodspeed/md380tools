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
  extern wchar_t channel_name[]; // borrowed from keyb.c (!).. not sure if ZERO-terminated !
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
  uint8_t state; // NARRATOR state (not Morse generator state):
# define NARRATOR_PASSIVE      0x00
# define NARRATOR_TELL_CHANNEL 0x01
# define NARRATOR_TELL_ZONE    0x02
# define NARRATOR_TELL_MENU    0x04

  uint8_t mode;  // bitwise combination of NARRATOR_MODE-flags,
    //  copied from global_addl_config.narrator_mode somewhere

  // Purely 'internal stuff', to detect changes in narrator() :
  uint8_t old_opmode2; // previous value of gui_opmode, seen in..
  uint8_t old_opmode3; // ..the last call of narrator() [etc, etc]
  uint8_t menu_entry;
  uint8_t channel_number;

} T_Narrator;

extern T_Narrator Narrator;  // data for a single "storyteller" instance

void narrate(void); // called from various places (also periodically)
           // whenever something happened that may have to be 'told'.

#endif // CONFIG_MORSE_OUTPUT ?
