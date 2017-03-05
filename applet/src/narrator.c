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

/*
 Revision history (latest entry first) :
 2017-03-01, DL4YHF : Added non-intrusive methods to report
            the current channel, menu title, selected item.  
            Reading out the COMPLETE menu ("almost the full screen")
            is possible, but takes long. Some brainstorming:
   - the CURRENTLY selected item (text line) could be
     'narrated' in a slightly higher pitch than normal text.
   - the Morse generator (running in the background)
     needs to change audio frequency in the gaps between
     words (or characters), using special characters
     that cannot be sent in Morse code anyway, e.g.:
     "MD380Tools  \x02Set Talkgroup\x01  Mic bargraph  Experimental"
       (string assembled by the narrator, passed to WB's 
        Morse generator / audio 'modulator' in irq_handlers.c )
     Special 'control' characters for Morse output :
         \x02 = slightly increase audio pitch to emphasize text
         \x01 = back to normal text attributes
         \x00 : cannot be a 'control' character because it marks
                it marks the end of a string (0x0000 for WIDE chars)
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
#include "menu.h"         // currently_selected_menu_entry / currently_focused_item_index ?
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
static void    report_channel(void);
static void    report_menu_title(void);
static void    report_menu_item(void);
static uint8_t get_current_channel_number(void);
static int     get_focused_menu_item_index(void);
static wchar_t *get_menu_title(void);
static wchar_t *get_menu_item_text(int itemIndex);
#endif // CONFIG_MORSE_OUTPUT ?




//---------------------------------------------------------------------------
// Implementation of the "story teller" (narrator) - not the CW generator !
//---------------------------------------------------------------------------

#if( CONFIG_MORSE_OUTPUT )

//---------------------------------------------------------------------------
void narrate(void) // "tell a story", in german: "erzähle!", "lies vor!"
  // Assembles text for output in CW (voice will not fit into ROM).
  // If this 'narrator' tells a long story, or just a verbose info, etc,
  // can be configured in the MD380Tools menu under 'Morse output'.
  // Even if NOT enabled in that menu, the op can let the narrator talk
  // by pressing one of the programmable buttons (see menu.c),
  // which then calls narrator_start_talking() - the name speaks for itself.
  //
  // narrate() is periodically called from rtc_timer.c::f_4225_hook()
  //      and possibly other functions (time will tell..),
  //      whenever a change in gui_opmode_X, etc, etc .
  //
  // The (CW-) narrator looks for changes in several global
  // variables, to decide if/when/what to send in Morse code .
  //
{
  T_Narrator *pNarrator = &Narrator;
  uint8_t u8Temp;
  int i;

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
         { MorseGen_AppendString( "menu " ); // .. in morse code (VERY helpful, isn't it ? Well, it's a TEST)
         }
        report_menu_title();
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
            { report_channel();
            }
         } // end else < just entered the MENU >
      }
     pNarrator->old_opmode2 =  gui_opmode2;
   } // end if < gui_opmode2 changed > 

  if( gui_opmode2 == OPM2_MENU ) // care about the currently focused menu item ?
   { i = get_focused_menu_item_index();
     if( pNarrator->focused_item_index != i )
      {
        pNarrator->focused_item_index = i;
        MorseGen_ClearTxBuffer();
        report_menu_item();
      } // end if < FOCUSED menu item changed ? >
   } // end if < currently in a MENU ? >
  else // currently NOT in a menu, so the main interest will be the current CHANNEL
   { 
       u8Temp = get_current_channel_number();  
       if( pNarrator->channel_number != u8Temp )
        {  pNarrator->channel_number =  u8Temp;
           MorseGen_ClearTxBuffer();
           report_channel();
        }
   }

} // end narrate()

//---------------------------------------------------------------------------
void narrator_start_talking(void)
{ // Lets Mr Narrator 'talk' (with the help of Sam Morse and Mr Beep),
  // but -unlike narrate()- will talk even if 'Morse output' is NOT enabled
  // in the MD380Tools menu. Called from keyb.c via programmable sidekey.
  // This feature is intended for visually impaired ops who don't want
  // a radio that starts beeping whenever the channel knob is turned,
  // or a cursor key pressed in the menu.  In other words, "talk on request".
  T_Narrator *pNarrator = &Narrator;

  pNarrator->mode = global_addl_config.narrator_mode;  
  pNarrator->channel_number = get_current_channel_number();

  MorseGen_ClearTxBuffer(); // stop telling old storys

  if( gui_opmode2 == OPM2_MENU ) 
   {   // guess we're in the menu so start talking about it :
     report_menu_title();
     report_menu_item();  
   }
  else // we're not in the menu so talk about channel and (maybe) zone...
   { report_channel();
   }

  if( pNarrator->mode & NARRATOR_MODE_TEST )
   { // even more 'debug output' in Morse code ?
#   if(1)  // report index of the currently selected menu item ?
     MorseGen_AppendString(" sel ");
     MorseGen_AppendDecimal( md380_menu_entry_selected );
#   endif 
   } // end if( pNarrator->mode & NARRATOR_MODE_TEST )

} // end narrator_start_talking()

//---------------------------------------------------------------------------
static void report_channel(void)
{
  // Report the channel NUMBER (short) or NAME (verbose) ?
  if( Narrator.mode & NARRATOR_MODE_VERBOSE )
   { // Report the CHANNEL NAME, not just the number.
     // In the D13.020 disassembly, somewhere near 'draw_channel_label',
     // 'channel_name' [at 0x2001e1f4] is referenced. Try it:
     MorseGen_AppendWideString( channel_name );
     // FIXME: When switching to an 'Unprogrammed' channel,
     //  channel_name isn't 'Unprogrammed' but *UNCHANGED* !
     //  (so sends the name of the last 'existing' channel)
   }
  else // not verbose but short: don't report the NAME
   {   // but the shortest possible channel NUMBER :
     MorseGen_AppendDecimal( Narrator.channel_number );
   }

} // end report_channel()

//---------------------------------------------------------------------------
static void report_menu_title(void)
{
  wchar_t *pwsText = get_menu_title();
  if( pwsText )
   { MorseGen_AppendWideString( pwsText );
   }
  else if( Narrator.mode & NARRATOR_MODE_TEST )
   { MorseGen_AppendString( "title?" );
   }
  MorseGen_AppendChar(' '); 
}


//---------------------------------------------------------------------------
static void report_menu_item(void)
{
  wchar_t *pwsText = get_menu_item_text( Narrator.focused_item_index );

  if( pwsText )  // get_menu_item_text() may return NULL when invalid !
   { // report the MENU ITEM TEXT, if possible without calling anything
     // in the original firmware (to avoid bad side-effects).
     MorseGen_AppendWideString( pwsText );
   }
  else if( Narrator.mode & NARRATOR_MODE_TEST )
   { MorseGen_AppendString( "item?" );
   }
  MorseGen_AppendChar(' '); 
} // end report_menu_item()



//---------------------------------------------------------------------------
// Helper functions to retrieve channel-, zone-, and menu strings 
//  (whereever possible, without calling Tytera's half-cooked menu API)
//---------------------------------------------------------------------------

// typedefs copied from other modules (not found in a header, only 'internal')
typedef struct 
{
   wchar_t* label;  // don't care if 'const' - we won't modify this item here
   void* green ;    // handler invoked when pressing 'Config'
   void* red ;      // handler invoked when pressing 'Back' (cancel)
   uint8_t off12 ;  //
   uint8_t off13 ;  //
   uint16_t item_count;  // number of sub-items ? - see create_menu_entry_rev()
   uint8_t off16 ;
   uint8_t off17 ;
   uint16_t unknown2;
   // sizeof() == 20 (0x14)
} menu_entry_t;
extern menu_entry_t md380_menu_mem_base[];

typedef struct
{ wchar_t *menu_title;
  menu_entry_t  *entries;
  uint8_t numberof_menu_entries;
  uint8_t unknown_00;
  uint8_t unknown_01;
  uint8_t filler ;
  // sizeof() == 12 (0x0C)
} menu_t; 
extern menu_t md380_menu_memory[];


//---------------------------------------------------------------------------
static menu_t * get_current_menu(void) // may return NULL !!
{
  // Part of a 'non-intrusive' method  to retrieve the currently
  // visible menu title (top line) and the currently selected item
  // (the 'blue line') would not call a subroutine in the original
  // firmware. So which menu-related *global variables* do we have ?
  // Collected from various modules, headers, and the symbol file :
  // 
  // (a) menu_t md380_menu_memory[]  with the following members:
  //             |_  wchar_t  *menu_title; 
  //             |_  menu_entry_t *entries; index = md380_menu_entry_selected ? see (d) 
  //             |                  |_ label, OnConfirmHandler, OnCancelHandler, 
  //             |                     .., item_count(!), ...  
  //             |_  uint8_t numberof_menu_entries;
  //     md380_menu_depth : used as an index into md380_menu_memory[],
  //            see FW D13.020 disassembly at 0x800c72c .
  //       Values peeked from md380_menu_depth (BYTE @ 0x20004acc) : 
  //         - when NOT in a menu : 0xFF (!)
  //         - in main 'Menu'     : 0x00
  //         - in 'Scan'          : 0x01
  //         - in 'Scan.ViewList' : 0x02
  //         - in 'Zone', 1st item: 0x01
  //         - in 'Zone', 2nd item: 0x01 (same, as expected)
  // 
  // (b) menu_entry_t md380_menu_mem_base[] :
  //      md380_menu_mem_base[] is not a 'dynamic' pool but a static array. 
  //      (used by md380_create_menu_entry(menuId, wsLabel, OnConfirm, OnCancel, 'e', 'f', enabled)
  //            see FW D13.020 disassembly at 0x800c72c )
  //       The first argument, menuId, is actually an index into md380_menu_mem_base[].
  //       But knowing the current 'menuid' alone doesn't tell the CURRENT ITEM .... 
  //  
  // (c) menu_memory_poi : at 0x2001e700 in D13.020 . 
  //       First member sometimes seems to be a COLOUR (?? - see 0x80209a8),
  //              in other cases a POINTER to a struct (see 0x80288c2).
  //       Values peeked from menu_memory_poi (@0x2001e700) : 
  //         - in the MAIN menu (title = 'menu') : 0x2001D5CC
  //         - in 'Scan'          : 0x2001D5D8 .
  //         - in 'Scan.ViewList' : 0x2001D5D8 (unchanged!)
  //         - in 'Utilities'     : 0x2001D5D8 (ok, abandon this)
  //
  // (d) md380_menu_memory[md380_menu_depth].entries[md380_menu_entry_selected].label :
  //        |__ menu_t                        |__ menu_entry_t
  //       After carefully checking the two array indices, this may be one way
  //       to retrieve at least the text of the CURRENTLY SELECTED item,
  //       without calling a function (which may have bad side effects) .
  //
  int iMenu = md380_menu_depth;  // array index into md380_menu_memory[]
  if( iMenu>=0 && iMenu<10/*?*/) // guess the NESTING LEVEL won't exceed 10
   { return &md380_menu_memory[iMenu];
   }
  else // invalid index into md380_menu_memory[], guess we're not in a menu:
   { return NULL;
   }

} // get_ptr_to_current_menu()

//---------------------------------------------------------------------------
int get_focused_menu_item_index(void)
{
  // For the Morse "narrator", it's important to report the
  //     CURRENTLY FOCUSED item (menu/submenu/check-or-radio-button), 
  //     i.e. the line with dark blue background, controlled 
  //     via cursor up/down keys (or, maybe, an autorepeat-thing one day).
  // So how to find that info, which md380_menu_entry_selected doesn't provide ?
  // 
  // * 'md380_menu_entry_selected' doesn't care about which item
  //            is "focused" (after cursor up/down, blue background)
  // * 'currently_selected_menu_entry' seemed just right here
  //     ( watch it in D13.020 while navigating in ANY menu
  //       or button-list:  > python md380-tool.py hexwatch 0x20004cba 
  //       - it changes immediately after cursor up/down ),
  //    

  int itemIndex = currently_selected_menu_entry;
  // ( watch it in D13.020 while navigating in the menus or radio-button-lists: 
  //    > python md380-tool.py hexwatch 0x20004cba 
  // )
  if( itemIndex == 255 ) // 0xFF possibly means 'invalid' / 'not in a menu or button-list'
   {  itemIndex = -1;
   }
  return itemIndex;

} // end get_focused_menu_item_index()


//---------------------------------------------------------------------------
wchar_t *get_menu_title(void)  // may return NULL when not in a menu !
{
  menu_t *pMenu = get_current_menu();
  if( pMenu )
   { return pMenu->menu_title;
   }
  else
   { return NULL;
   }
}

//---------------------------------------------------------------------------
wchar_t *get_menu_item_text(int itemIndex) // may return NULL when invalid !
  // [in] itemIndex : 0..N to retrieve *ALL* strings
  //                       (repeatedly call until return = NULL);
  //                  -1 to retrieve only the CURRENTLY SELECTED item's text.
{
  menu_t *pMenu = get_current_menu();
  if( itemIndex < 0 )
   {  itemIndex = get_focused_menu_item_index();
   }
  if( pMenu )
   { if( itemIndex>=0 && itemIndex<pMenu->numberof_menu_entries )
      { return pMenu->entries[itemIndex].label;
      }
   }
  // Arrived here: Not in a menu, or the item-index is invalid (i.e. "reached the end")
  return NULL;
}

//---------------------------------------------------------------------------
uint8_t get_current_channel_number(void)
{
  // How to retrieve the current channel (number) ? 
  // In md380.h, there was only an address of 'channelnum', and only for D002.032 .
  // In netmon.c, there was a well-hidden external 'channel_num' (in 2017-02-20).
  // The D13.020 firmware sets at address 0x0804fd70 (see disassembly).
  // For D02.032, the address will be different. Thus:
#if defined(FW_D13_020) || defined(FW_S13_020)
  return channel_num;           // read the current channel from a global variable
#else
  return read_channel_switch(); // read the current channel from rotary switch
#endif
} // end get_current_channel_number()


#endif // CONFIG_MORSE_OUTPUT ?

/* EOF < narrator.c > .  Leave an empty line after this. */
