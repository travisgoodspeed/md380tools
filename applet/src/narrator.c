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

 To include the 'Morse narrator' in the patched firmware:
    
  1. Add the following lines in applet/Makefile (after SRCS += irq_handlers.o) :
      SRCS += narrator.o 
      SRCS += app_menu.o    (not a requirement but preferred - easier to use)
      SRCS += lcd_driver.o  (required by app_menu.c - details there)
      SRCS += font_8_8.o
  
  2. #define CONFIG_MORSE_OUTPUT 1  in  md380tools/applet/config.h,
                            and (preferred for easier use) also
     #define CONFIG_APP_MENU 1
    

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
#include "netmon.h"       // is_netmon_visible(), etc
#include "codeplug.h"     // zone_name[], etc (beware, unknown for old firmware)
#include "console.h"      // text screen buffer for Netmon and the 'red menu'
#include "irq_handlers.h" // contains the API for the Morse code generator
#include "app_menu.h" // optional 'application' menu, activated by red BACK-button.
#include "narrator.h" // announces channel, zone, and maybe current menu selection in Morse code

// Configuration for the 'narrator' and his companion, Mr. Morse:
//  global_addl_config.narrator_mode = Mode/verbosity for text output.
//  global_addl_config.cw_pitch_10Hz = Audio tone frequency in TEN HERTZ units
//  global_addl_config.cw_volume     = output volume for CW messages [percent]
//  global_addl_config.cw_speed_WPM  = output speed in WPM. Scaled into timer ticks somewhere

#if( CONFIG_MORSE_OUTPUT )
T_Narrator Narrator;  // a single CW narrator, aka "storyteller" instance

wchar_t narrator_temp_wstring[10];

// internal 'forward' references :
static void report_channel(void);
static void report_zone(void);
static void report_menu_title(void);
static void report_menu_item(void);
static void report_battery_voltage(void);
static uint8_t get_current_channel_number(void); // 0..15; bit 7 indicates "unprogrammed" channel
static int  get_focused_menu_item_index(void);
static void *get_current_menu(void); // may return NULL !!
static wchar_t *get_menu_title(void);
static wchar_t *get_menu_item_text(int itemIndex);
static void start_reading_console(void);
static void continue_reading_console(void);
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
  //      on any change in gui_opmode_X, etc, etc .
  //
  // The (CW-) narrator looks for changes in several global
  // variables, to decide if/when/what to send in Morse code .
  //
{
  T_Narrator *pNarrator = &Narrator;
  void *pMenu;
  uint8_t u8Temp;
  int i;

  pNarrator->mode = global_addl_config.narrator_mode;

  if( IS_PTT_PRESSED ) // stop (or pause?) morse output when PTT pressed ?
   { MorseGen_ClearTxBuffer(); // abort ongoing Morse transmission (if any)
     pNarrator->to_do = 0;
     StartStopwatch( &pNarrator->stopwatch );   // gap after releasing PTT
   }
  else // PTT not pressed..
  if( pNarrator->mode & NARRATOR_MODE_ENABLED ) // AUTOMATIC reporting ?
   { i = -1;
#   if (CONFIG_APP_MENU)
     if( Menu_IsVisible() ) // <- things are MUCH easier in the "alternative" menu 
      { // .. and there's nothing to do HERE, because app_menu.c will send Morse by itself
      }
     else // umm.. not in "our" (alternative) menu but in Tytera's 
#   endif // CONFIG_APP_MENU ?
      {   // (includes the "MD380Tools" menu, which was there long before the "app-menu")
        i = get_focused_menu_item_index(); 
        pMenu = get_current_menu(); 

        if( (i != pNarrator->focused_item_index ) // ENTERED or LEFT a menu ?
          ||(pMenu != pNarrator->current_menu) ) 
         { MorseGen_ClearTxBuffer(); // abort ongoing transmission (if any)
           pNarrator->to_do = 0;
           StartStopwatch( &pNarrator->stopwatch );   // restart stopwatch (don't "start talking" immediately)
           if( i >= 0 )                               // now IN a menu ..
            { if( (pNarrator->focused_item_index < 0 ) // .. and previously wasn't:
               || (pMenu != pNarrator->current_menu) )
               { pNarrator->to_do |= NARRATOR_REPORT_TITLE;
               }
              pNarrator->to_do |= NARRATOR_REPORT_MENU;
            }   // end if < just entered the MENU >
           else // seems we RETURNED from the main menu to the 'main screen'
            { pNarrator->to_do = NARRATOR_REPORT_CHANNEL;
            }
           pNarrator->focused_item_index = i; // set new values to detect..
           pNarrator->current_menu = pMenu;   // ..the NEXT menu-related change
   
         } // end if < ENTERED or LEFT a menu > 
      }
     if( i<0 ) // currently NOT in a menu, so the main interest will be the current CHANNEL
      { 
        u8Temp = get_current_channel_number();  
        if( pNarrator->channel_number != u8Temp )
         {  pNarrator->channel_number =  u8Temp;
            MorseGen_ClearTxBuffer();
            StartStopwatch( &pNarrator->stopwatch );   // also here, don't "start talking" immediately
            pNarrator->to_do = NARRATOR_REPORT_CHANNEL; 
            // Also report the zone here ? Unnecessary in most cases.
            // So use a sidekey to 'request' a full report, including the zone.
         }
      }
   } // end if( pNarrator->mode & NARRATOR_MODE_ENABLED )


  // Start output if a new announcement is pending, but only
  // if the operator didn't rotate the channel knob, or select
  // a different menu item  within the previous XXX milliseconds :
  if( (pNarrator->to_do != 0 )
   && (ReadStopwatch_ms(&pNarrator->stopwatch) > 200/*ms*/ ) )
   {
     if( MorseGen_GetTxBufferUsage() < 2 ) // enough space in TX buffer now
      { // (don't wait for the buffer to run empty, avoid gaps,
        //  we don't know how frequently this function is called)
        // What to send next (ordered by priority) ?
        if( pNarrator->to_do & NARRATOR_REPORT_CHANNEL ) 
         { // report the current channel (highest prio)
           MorseGen_AppendChar(' '); // short gap instead of a "line break" ..
           report_channel();         // followed by channel name or -number
           pNarrator->to_do &= ~NARRATOR_REPORT_CHANNEL; // "done" !
         } 

        if( pNarrator->to_do & NARRATOR_REPORT_ZONE ) 
         { // next lower priority, etc...
           MorseGen_AppendChar(' '); // gap between channel and zone
           report_zone();
           pNarrator->to_do &= ~NARRATOR_REPORT_ZONE;
         } 

        if( pNarrator->to_do & NARRATOR_REPORT_TITLE ) 
         { MorseGen_AppendChar(' ');
           report_menu_title();
           pNarrator->to_do &= ~NARRATOR_REPORT_TITLE;
         } 

        if( pNarrator->to_do & NARRATOR_REPORT_MENU ) 
         { MorseGen_AppendChar(' ');
           report_menu_item();
           pNarrator->to_do &= ~NARRATOR_REPORT_MENU;
         } 

        if( pNarrator->to_do & NARRATOR_READ_CONSOLE ) 
         { // Such a 'long story' doesn't fit in the Morse TX buffer,
           // so send the contents of the console screen line by line :
           continue_reading_console(); // clears NARRATOR_READ_CONSOLE when finished
         } 

        if( pNarrator->to_do & NARRATOR_REPORT_BATTERY )
         { report_battery_voltage();
           pNarrator->to_do &= ~NARRATOR_REPORT_BATTERY;
         }

        if( pNarrator->to_do & NARRATOR_APPEND_DEBUG_1 )
         { // lowest priority :
           // report index of the currently selected menu item ?
           MorseGen_AppendString(" sel ");
           MorseGen_AppendDecimal( md380_menu_entry_selected );
           MorseGen_AppendString(" foc ");
           MorseGen_AppendDecimal( get_focused_menu_item_index() ); // "-1" when NOT in a menu
           pNarrator->to_do &= ~NARRATOR_APPEND_DEBUG_1; // done
         }

      } // end if < enough space in the TX-buffer for another line >
   } // end if < "ok to say something now" >
} // end narrate()

//---------------------------------------------------------------------------
void narrator_start_talking(void) // called on programmed sidekey from keyb.c
{ // 'Starts talking' in Morse code, even if NOT enabled in the menu. 
  // Called from keyb.c via programmable sidekey.
  // This feature is intended for visually impaired ops who don't want
  // a radio that starts beeping whenever the channel knob is turned,
  // or a cursor key pressed in the menu.  In other words, "talk on request".
  // Sends a few more details than when simply turning the channel knob,
  // e.g. zone, and (in 'verbose' mode) battery charge state (etc?).
  T_Narrator *pNarrator = &Narrator;

  // Take a 'snapshot' of the most important 'radio states' :
  pNarrator->mode = global_addl_config.narrator_mode;  
  pNarrator->channel_number = get_current_channel_number();
  pNarrator->focused_item_index = get_focused_menu_item_index(); 

  MorseGen_ClearTxBuffer(); // stop telling old storys

  // If the 'alternative' menu is open, it has priority over anything else:
#if (CONFIG_APP_MENU)
  if( Menu_IsVisible() ) // <- this is the "alternative" menu, not Tytera's ! 
   { pNarrator->to_do = NARRATOR_REPORT_MENU;
     // Items in "our menu" speak for themselves, they don't need a title
     // to understand what they mean. At least, that was the plan in 2017-04 .
   }
  else
#endif // CONFIG_APP_MENU ?
  if( pNarrator->focused_item_index >= 0 ) 
   { // guess we're in the menu so start talking about it :
     pNarrator->to_do = NARRATOR_REPORT_TITLE | NARRATOR_REPORT_MENU;
   }
  else // we're not in the menu so talk about something else ..
  if( is_netmon_visible() )
   { start_reading_console(); 
   }
  else // none of the above, so tell what's on the 'main' screen
   { pNarrator->to_do = NARRATOR_REPORT_CHANNEL | NARRATOR_REPORT_ZONE;
     if( pNarrator->mode & NARRATOR_MODE_VERBOSE )
      { pNarrator->to_do |= NARRATOR_REPORT_BATTERY;
      }
   }

  if( pNarrator->mode & NARRATOR_MODE_TEST ) // "debug output" in Morse code ?
   { pNarrator->to_do |= NARRATOR_APPEND_DEBUG_1;
   }

} // end narrator_start_talking()


//---------------------------------------------------------------------------
void narrator_repeat(void) // called on another programmed sidekey
  // Repeats the last (autonomously) sent message like channel or menu item.
  // See github issue #580 / suggestion by KB5ELV .
{ T_Narrator *pNarrator = &Narrator;

  MorseGen_ClearTxBuffer(); // abort ongoing transmission (if any)
  pNarrator->focused_item_index = get_focused_menu_item_index(); 
  StartStopwatch( &pNarrator->stopwatch ); // restart stopwatch (don't start immediately)
  if( pNarrator->focused_item_index >= 0 ) // in a menu ?
   { pNarrator->to_do = NARRATOR_REPORT_MENU;
   }
  else // currently NOT in a menu -> must be the 'main screen'
   { pNarrator->channel_number = get_current_channel_number();  
     pNarrator->to_do = NARRATOR_REPORT_CHANNEL; // "short story" : only report the CHANNEL 
   }
} // end narrator_repeat()

//---------------------------------------------------------------------------
static void report_channel(void)
{
  // Report the channel NUMBER (short) or NAME (verbose) ?
  if( (Narrator.mode & NARRATOR_MODE_VERBOSE)
    &&( !(Narrator.channel_number&0x80) ) ) // suppress name when UNPROGRAMMED
   { // Report the CHANNEL NAME, not just the number.
     // In the D13.020 disassembly, somewhere near 'draw_channel_label',
     // 'channel_name' [at 0x2001e1f4] is referenced. Try it:
     MorseGen_AppendWideString( channel_name );
     // Note: When switching to an 'Unprogrammed' channel,
     //  channel_name isn't 'Unprogrammed' but remains UNCHANGED !
   }
  else // not verbose but short (or UNPROGRAMMED) : 
   { // don't report the NAME but the channel NUMBER...
     MorseGen_AppendDecimal( Narrator.channel_number & 0x7F );
     // ..followed by "unp" (short) / "unprogrammed" (verbose)
     if( Narrator.channel_number & 0x80 )
      { MorseGen_AppendString( " unp" );
        if(Narrator.mode & NARRATOR_MODE_VERBOSE)
         { MorseGen_AppendString( "rogrammed" ); 
         }
      }
   }

} // end report_channel()

//---------------------------------------------------------------------------
static void report_zone(void)
{
  MorseGen_AppendString( "zone " );
#if defined(FW_D13_020) || defined(FW_S13_020)
  MorseGen_AppendWideString( zone_name );
#endif
}

//---------------------------------------------------------------------------
static void report_menu_title(void)
{
  wchar_t *pwsText = get_menu_title();
  MorseGen_AppendChar( '\x09' ); // decrease pitch by approx one whole tone
     
  if( pwsText )
   { MorseGen_AppendWideString( pwsText );
   }
  else
   { 
#   if defined(FW_D13_020) /* || .. ? */
     MorseGen_AppendString( menu_title_cstring );  // menu title in RAM; WB 2017-03: for D13.020 only !
     // (Not sure what this variable @0x2001E3C0 in D13.020 REALLY is.
     //  Sometimes it contained the text 'Back' after pressing 'Back'.
     //  So taking the title from the menu_t struct is the better choice,
     //  and the above (0x2001E3C0) is only a kludge for the MAIN menu,
     //  because in the MAIN menu (just "Menu") pMenu->menu_title was invalid.
#  else  // address of the MENU TITLE IN RAM isn't known -> guess it's "Menu" !
     MorseGen_AppendString( "menu" );
#  endif // any <firmware for which the address of 'menu_title_cstring' is known> 
   } // end if < menu_t without a valid TITLE > ?
  MorseGen_AppendChar( '\x10' ); // SPACE + back to the normal CW pitch
 
}


//---------------------------------------------------------------------------
static void report_menu_item(void)
{
  wchar_t *pwsText;

#if (CONFIG_APP_MENU)
  if( Menu_IsVisible() ) // this is for the "alternative" menu (not Tytera's):
   { Menu_ReportItemInMorseCode(  AMENU_MORSE_REQUEST_ITEM_TEXT 
                                | AMENU_MORSE_REQUEST_ITEM_VALUE );
   }
  else
#endif // CONFIG_APP_MENU ?
   { // arrived here ? Guess we're in one of the deeply nested "Tytera"-menus,
     // or in the MD380Tools menu (which hooks into the original menu) ...
     // Fasten seat belt, retrieving text strings from that beast is tricky !
     pwsText = get_menu_item_text( Narrator.focused_item_index );

     if( pwsText )  // get_menu_item_text() may return NULL when invalid !
      { // report the MENU ITEM TEXT, without calling anything
        // in the original firmware to avoid bad surprises and side-effects
        MorseGen_AppendWideString( pwsText );
      }
     else // get_menu_item_text() failed to retrieve a string.
      { // This happened in the ZONE- and in the CONTACTS menu,
        // and maybe in other menus with data from the codeplug, too.
        // But how to find out if we're in the ZONE- or CONTACTS menu ? 
        // * md380_menu_id (BYTE at 0x2001e915 in D13.020) ? 
        //   |___ 0x07 in menu "Zone";  0x0A in menu "Contacts". Let's try:
        switch( md380_menu_id )
         { case 0x07: // guess we're in Tytera's "Zone" menu
#            ifdef HAVE_ZONE_NAME
              MorseGen_AppendWideString(zone_name ); 
#            endif
              break;
           case 0x0A: // guess we're in Tytera's "Contacts" menu
#            ifdef HAVE_SELECTED_CONTACT_NAME
              MorseGen_AppendWideString(selected_contact_name_wstring );
#            endif
              break;
           default:
              if( Narrator.mode & NARRATOR_MODE_TEST )
               { MorseGen_AppendString( "item?" );
               }
              break;
         } // end switch( md380_menu_id )
      }   // end else < get_menu_item_text() failed >
   }
  MorseGen_AppendChar(' '); // separator between menu item and whatever may follow
} // end report_menu_item()

//---------------------------------------------------------------------------
static void start_reading_console(void) 
{ // gadget, useful feature, or an alternative 'Morse trainer' ?
  T_Narrator *pNarrator = &Narrator;
  pNarrator->item_index = 0; // here: console text line index
  pNarrator->num_items  = CONSOLE_Y_SIZE;
  pNarrator->to_do |= NARRATOR_READ_CONSOLE;
} // end start_reading_console()

//---------------------------------------------------------------------------
static void continue_reading_console(void)
{
  T_Narrator *pNarrator = &Narrator;
  char *cp;
  int  i,j;

  // Look for the next NON-EMPTY line in the console text buffer:
  while( pNarrator->item_index < CONSOLE_Y_SIZE )
   { cp = con_buf[pNarrator->item_index++];
     // truncate trailing spaces or zero-bytes, and skip empty lines...
     i = CONSOLE_MAX_XPOS-1;
     while( i>0 && (cp[i]==' ' || cp[i]=='\0') )
      { --i;
      }
     if( i>0 ) // another non-empty line: send it without trailing spaces
      { for(j=0; j<=i; ++j)
         { MorseGen_AppendChar( cp[j] );
         }
        MorseGen_AppendChar(' ');  // ONE space since there's no '\n' in Morse code
        return; // wait for empty TX buffer before sending the next line
      }
   }
  // Arrived here ? All lines of the text console are through. End of this story.
  pNarrator->to_do &= ~NARRATOR_READ_CONSOLE;  // done (reading the console screen)

} // end continue_reading_console()

//---------------------------------------------------------------------------
static void report_battery_voltage(void)
{ int voltage = battery_voltage_mV; // <- grabbed from DMA and filtered in irq_handlers.c
  char sz15[16];
  sprintf(sz15, " bat %d.%d v ", voltage/1000, (voltage % 1000)/100/*0..9*/ );
  MorseGen_AppendString( sz15 );
}

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
static void* get_current_menu(void) // may return NULL !!
{
  // Part of a 'non-intrusive' method  to retrieve the currently
  // visible menu title (top line) and the currently selected item
  // (the 'blue line').
  // To avoid calling anything in the original firmware, which
  //    menu-related *global variables* are available ?
  //    A very incomplete summary (collected by DL4YHF
  //   from various modules, headers, the monster disassembly listing,
  //   and the symbol file (md380tools/applet/src/symbols_d13.020) :
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
  //         - 'volume' indicator : 0x01 ("behaves like a menu"!)
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
  menu_t *pMenu = NULL;
  int iMenu = md380_menu_depth;  // array index into md380_menu_memory[]
  if( iMenu>=0 && iMenu<10/*?*/) // guess the NESTING LEVEL won't exceed 10
   { // As noted above (a), when rotating the volume knob, or in the
     // presence of noise on the analog input sensing the knop position,
     // the volume indicator bargraph pops up, and appears 'like a menu'
     // (with md380_menu_depth=1). But we don't want to indicate the
     // useless (and often erroneously-popped-up) volume bar in Morse code.
     // So how to find out if pMenu is a "real" menu or just the volume bar ?
     //  (1) While on the MAIN SCREEN, 'gui_opmode2' was ONE .
     //      This didn't change when the volume indicator bar popped up.
     //  (2) If the parameters "e" and "f", passed to md380_create_menu_entry(),
     //      were stored somewhere as part of the menu_t structure,
     //      f=2 ("remove after timeout", see menu.c) could also indicate
     //      that md380_menu_depth=1 is NOT a menu but the volume bar. 
     pMenu = &md380_menu_memory[iMenu];
     if( gui_opmode2 != OPM2_MENU ) 
      { if( md380_menu_depth==1 ) // guess not a MENU but the VOLUME BAR !
         { pMenu = NULL; // ignore the volume bar here
         }
      }
   }
  return (void*)pMenu; // 'void*' because 'menu_t' isn't exposed in narrator.h

} // get_current_menu()

//---------------------------------------------------------------------------
int get_focused_menu_item_index(void) // negative when NOT in a menu !
{
  int itemIndex;
#if (CONFIG_APP_MENU)
  if( Menu_IsVisible() ) // <- things are MUCH easier in the "alternative" menu 
   { return Menu_GetItemIndex();
   }
#endif

  // For the Morse "narrator", it's important to report the
  //     CURRENTLY FOCUSED item (menu/submenu/check-or-radio-button), 
  //     i.e. the line with dark blue background, controlled 
  //     via cursor up/down keys (maybe with autorepeat one fine day).
  // So how to find that info, which md380_menu_entry_selected doesn't provide ?
  // 
  // * 'md380_menu_entry_selected' doesn't care about which item
  //            is "focused" (after cursor up/down, blue background)
  // * 'currently_selected_menu_entry' seemed just right here
  //     ( watch it in D13.020 while navigating in ANY menu
  //       or button-list:  > python tool2.py hexwatch 0x20004cba 
  //       - it changes immediately after cursor up/down ) .
  //    Unfortunately 'currently_selected_menu_entry' sometimes didn't work,
  //    sometimes it indicated the WRONG item, and sometimes contained 0xFF
  //    when *NOT* on the main screen.
  itemIndex = currently_selected_menu_entry; // <-- CANNOT ALWAYS BE TRUSTED !
  // ( watch this in D13.020 while navigating in the menus or radio-button-lists: 
  //     > python tool2.py hexwatch 0x20004ca0 64
  //   In D13.020, the CURRENTLY FOCUSED ITEM INDEX may be at 0x20004CC2 first,
  //      and at 0x20004CBA after entering the MD380Tools menu. Details below. 
  //   No idea where those two 'competing' variables are located in other firmware.
  // )
  if( gui_opmode2 == OPM2_MENU ) // gui_opmode2 (@0x2001e94b) should indicate if we're in a MENU or not..
   { if( itemIndex > 100 ) // ... but currently_selected_menu_entry contains garbage:
      { // (this usually happened when entering the main menu,
        //  before selecting ("Confirm!") the MD380Tools menu.
        //  'currently_selected_menu_entry' (@ 0x20004CBA in D13.020) contained 0xFF then,
        //  and 'another' menu-item-index appeared at address 0x20004CC2 in D13.020 . 
        //  This is crazy: After ENTERING and LEAVING the "MD380Tools" menu,
        //  even in TYTERA'S menu, the 'original' menu-item-index @ 0x20004CC2
        //  wasn't used again !
        //  Instead currently_selected_menu_entry was used from then on,
        //  even for Tytera's menu, until returning to the MAIN screen.
        // Guesswork: 'currently_selected_menu_entry' isn't a simple global var
        //            at a fixed memory location. Instead it's part of a STRUCT,
        //            and for some reason the base address of that struct was 
        //            modified when ENTERING the MD380Tools menu .
        //           (-> create_menu_entry_addl_functions_screen(), etc )
#      if defined(FW_D13_020) || defined(FW_S13_020)
        itemIndex = *((uint8_t*)0x20004CC2); // "competing" menu-item-index in D13.020, BEFORE entering MD380Tools menu (bizarre...)
        // ToDo: Find out if this "competing" menu-item-index is really at the same address in S13.020 .
        // 'currently_selected_menu_item' is at 0x20004CBA in *both* cases, according to the 'symbols' files.
#      endif
      } // end if < bad contents in 'currently_selected_menu_entry' >
     if( itemIndex > 100 ) // still no trustable 'currently FOCUSED menu item index' ?
      { itemIndex = -1;    // give up, something is really screwed up somewhere
      }
   }
  else // gui_opmode2 tells us we're NOT in a menu, so trust neither 0x20004CBA nor 0x20004CC2 :
   { itemIndex = -1;
   } 
  return itemIndex;

} // end get_focused_menu_item_index()


//---------------------------------------------------------------------------
wchar_t *get_menu_title(void)  // may return NULL when not in a menu !
{
  menu_t *pMenu = (menu_t*)get_current_menu();
  if( pMenu )
   { // In the MAIN MENU (simply titled "menu"),
     // pMenu->menu_title was NULL, and (in D13.020)
     // the title was a wide string at 0x2001E3C0 !
     if( pMenu->menu_title != NULL ) 
      { return pMenu->menu_title;
      }
     else  // menu_t.menu_tile is NULL.. what to send instead ?
      { // In D13.020, there's a classic 8-bit string (C-string)
        // which is valid even in the MAIN menu, but due to the 
        // stupid 16-bit string type used in menu_t.title, 
        // we cannot simply return its address from here. Sigh.
        // Why the heck did they use this crappy "wide strings"
        // instead of UTF-8 ? 
      }   
   }
  // Arrived here ? Give up, cannot retrieve the menu title (text)
  return NULL;
} // end get_menu_title()

//---------------------------------------------------------------------------
wchar_t *get_menu_item_text(int itemIndex) // may return NULL when invalid !
  // [in] itemIndex : 0..N to retrieve *ALL* strings
  //                       (repeatedly call until return = NULL);
  //                  -1 to retrieve only the CURRENTLY SELECTED item's text.
{
  int i, nItems, nVisible;
  menu_t *pMenu = (menu_t*)get_current_menu();
  menu_entry_t *pItem;
  if( itemIndex < 0 )
   {  itemIndex = get_focused_menu_item_index();
   }
  if( pMenu )
   { nItems = pMenu->numberof_menu_entries;
     // guesswork: numberof_menu_entries includes the INVISIBLE items, too
     if( itemIndex>=0 && itemIndex<nItems )
      { // ex: return pMenu->entries[itemIndex].label;
        // The above often gave wrong results, it reported 'hidden' items
        // for example 'Contacts' (invisible when on an FM channel).
        i = nVisible = 0;
        while( i<nItems )
         { pItem = &pMenu->entries[i++];
           if( pItem->item_count )  // "item_count" controls VISIBILITY !
            { // THIS menu item is visible. Is it the one we're looking for ?
              if( nVisible == itemIndex )
               { if( pItem->label != NULL )
                  { return pItem->label;
                  }
               }
              ++nVisible;
            } 
         }
      }
   }
  // Arrived here: Not in a menu, or the item-index is invalid (i.e. "reached the end")
  return NULL;
} // end get_menu_item_text() [doesn't work properly for certain menus!]

//---------------------------------------------------------------------------
uint8_t get_current_channel_number(void) // 0..15; bit 7 set when unprogrammed
{ uint8_t chn_nr; 
  // How to retrieve the current channel, and how to tell if it's PROGRAMMED ?
  // In md380.h, there was only an address of 'channelnum', and only for D002.032 .
  // In netmon.c, there was a well-hidden external 'channel_num' (2017-02-20).
  // The D13.020 firmware sets at address 0x0804fd70 (see disassembly).
  // For D02.032, the address will be different. Thus:
#if defined(FW_D13_020) || defined(FW_S13_020)
  chn_nr = channel_num;           // read the current channel from a global variable
#else
  chn_nr = read_channel_switch(); // read the current channel from rotary switch
#endif
  if( (gui_opmode3 & 0x0F) == 3 ) // 'trick' discovered in netmon.c
   { chn_nr |= 0x80; // set bit 7 to indicate UNPROGRAMMED channel
   }
  return chn_nr;
} // end get_current_channel_number()

#endif // CONFIG_MORSE_OUTPUT ?

/* EOF < narrator.c > .  Leave an empty line after this. */
