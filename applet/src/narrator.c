/*! \file narrator.c
    \brief 'Narrator' for visually impared operators .
      Assembles ASCII output for sending in Morse code with:
      - the currently selected channel 
      - the currently selected zone (on request)
      - the currently selected menu item (future plan) .
    The Morse code generator itself runs in the background
    (implemented in irq_handlers.c, called from SysTick_Handler).
    The 'narrator' tells the Morse generator what to send,
     after being informed about about changes on the ...
      - main screen (e.g. after rotating the channel knob)
      - main menu (expect this to be the trickiest part)
      - MD380Tools menu (easier, because it's under OUR control)
  
*/

#include "config.h"

#include <stm32f4xx.h>
#include <string.h>
#include "printf.h"
#include "dmesg.h"
#include "md380.h"
#include "version.h"
#include "printf.h"
#include "spiflash.h"
#include "addl_config.h"
#include "radio_config.h"
#include "syslog.h"
#include "usersdb.h"
#include "keyb.h"
#include "display.h"      // gui_opmode_x, OPM2_MENU, etc
#include "irq_handlers.h" // contains the API for the Morse code generator
#include "narrator.h" // announces channel, zone, and maybe current menu selection in Morse code

// Configuration for the 'narrator' and his companion, Mr. Morse:
//  global_addl_config.narrator_mode = Mode/verbosity for text output.
//  global_addl_config.cw_pitch_10Hz = Audio tone frequency in TEN HERTZ units
//  global_addl_config.cw_volume     = output volume for CW messages [percent]
//  global_addl_config.cw_speed_WPM  = output speed in WPM. Scaled into timer ticks somewhere

#if( CONFIG_MORSE_OUTPUT )
T_Narrator Narrator;  // a single CW narrator, aka "storyteller" instance

// internal 'forward' references :
static void report_channel(T_Narrator *pNarrator);
static void report_menu_entry(T_Narrator *pNarrator);
#endif // CONFIG_MORSE_OUTPUT ?


//---------------------------------------------------------------------------
// Implementation of the "story teller" (narrator) - not the CW generator !
//---------------------------------------------------------------------------

#if( CONFIG_MORSE_OUTPUT )

//---------------------------------------------------------------------------
void narrate(void) // "tell a story", in german: "erzähle!", "lies vor!"
  // Assembles text for output in CW (voice will not fit into ROM).
  // If this 'narrator' tells a long story, or just a verbose info,
  // can be configured in the 
  //
  // Periodically called from rtc_timer.c::f_4225_hook()
  //      and possibly other functions (time will tell..),
  //      whenever a change in gui_opmode_X, etc, etc .
  // This appeared to be a good place because it should be safe
  // to call functions in the original firmware from here,
  // just in case we need to CALL something to retrieve,
  // for example, the text of the currently selected menu item.
  // (in SysTick_Handler, it's definitely NOT safe to call anything
  //  in the original firmware functions, thus if necessary,
  //  such functions will be called FROM HERE )
  //
  // The (CW-) narrator looks for changes in the following global
  // variables, to decide if/when/what to send in Morse code :
  //   - 
{
  T_Narrator *pNarrator = &Narrator;
  uint8_t u8Temp;

  pNarrator->mode = global_addl_config.narrator_mode;  

  if( ! (pNarrator->mode & NARRATOR_MODE_ENABLED ) )
   { return; // nothing to do, don't waste time checking for 'events'
   }

  if( pNarrator->old_opmode2 != gui_opmode2 )
   { // What does gui_opmode2 tell us - important for Mr Narrator ?
     if( gui_opmode2 == OPM2_MENU ) 
      { // it's getting tricky - guess we entered the MENU !
        MorseGen_ClearTxBuffer(); // abort previous transmission (if any)
        if( pNarrator->mode & NARRATOR_MODE_TEST ) // TEST: say 'menu'..
         { MorseGen_AppendString( "menu" ); // .. in morse code (VERY helpful, isn't it ? Well, it's a TEST)
         }
        else // not in TEST MODE ...
         { 
         } // end else < just entered the MENU >
      }   // end if < just entered the MENU >
     else if( pNarrator->old_opmode2 == OPM2_MENU )
      { // seems we RETURNED from a menu..
        MorseGen_ClearTxBuffer();
        if( pNarrator->mode & NARRATOR_MODE_TEST ) // TEST: say 'menu'..
         { MorseGen_AppendString( "back" ); // back from menu !
         }
        else // not in TEST MODE ...
         { // say anything (but don't say goodnight tonight) ?
           if( !( pNarrator->mode & NARRATOR_MODE_VERBOSE)  ) 
            { // don't say it.. 
            }
           else // returned from a menu, with VERBOSE output:
            { report_channel(pNarrator);
            }
         } // end else < just entered the MENU >
      }
     pNarrator->old_opmode2 =  gui_opmode2;
   } // end if < gui_opmode2 changed > 

  if( gui_opmode2 == OPM2_MENU ) // care about the currently selected menu item ?
   { if( pNarrator->menu_entry != md380_menu_entry_selected )
      {
        pNarrator->menu_entry = md380_menu_entry_selected;
        MorseGen_ClearTxBuffer();
        report_menu_entry(pNarrator);
      } // end if < current MENU ITEM changed ? >
   } // end if < currently in a MENU ? >
  else // currently NOT in a menu, so the main interest will be the current CHANNEL
   {   // How to retrieve the current channel (number) ? 
       // In md380.h, there was only an address of 'channelnum', and only for D002.032 .
       // In netmon.c, there was a well-hidden external 'channel_num' (in 2017-02-20).
       // The D13.020 firmware sets at address 0x0804fd70 (see disassembly).
       // For D02.032, the address will be different.
       // Thus:
#     if defined(FW_D13_020) || defined(FW_S13_020)
       u8Temp = channel_num;
#     else
       u8Temp = read_channel_switch(); // read the current channel from rotary switch
#     endif 
       if( pNarrator->channel_number != u8Temp )
        {  pNarrator->channel_number =  u8Temp;
           MorseGen_ClearTxBuffer();
           report_channel(pNarrator);
        }
   }

} // end narrate()

//---------------------------------------------------------------------------
static void report_channel(T_Narrator *pNarrator)
{
  char sz20Temp[24];

  // Report the channel NUMBER (short) or NAME (verbose) ?
  if( pNarrator->mode & NARRATOR_MODE_VERBOSE )
   { // Report the CHANNEL NAME, not just the number.
     // As often, the question was HOW to find out.
     // Is it already 'waiting for us' in a variable,
     // or must it be retrieved by calling some function ? 
     // In the D13.020 disassembly, somewhere near 'draw_channel_label',
     // 'channel_name' [at 0x2001e1f4] is referenced.  
     // This was also NOT declared in any header; now it's in narrator.h .
     MorseGen_AppendWideString( channel_name, 16/*maxchars*/ );
   }
  else // not verbose but short: don't report the NAME
   {   // but the shortest possible channel NUMBER :
     sprintf( sz20Temp, "%d", (int) pNarrator->channel_number );
     MorseGen_AppendString( sz20Temp );
   }

} // end report_channel()

//---------------------------------------------------------------------------
static void report_menu_entry(T_Narrator *pNarrator)
{
  if( pNarrator->mode & NARRATOR_MODE_TEST )
   { MorseGen_AppendString( "m " );
   }
  else 
   { // report the MENU ITEM TEXT .
     // Again, the question is HOW to find out...
   }

} // end report_menu_entry()


#endif // CONFIG_MORSE_OUTPUT ?

/* EOF < narrator.c > .  Leave an empty line after this. */
