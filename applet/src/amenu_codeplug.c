// File:    md380tools/applet/src/amenu_codeplug.c
// Author:  Wolf (DL4YHF) [initial version]
//
// Date:    2017-04-29
//  Highly experimental 'alternative zone list' and similar codeplug-related displays.
//  Most list-like displays are implemented as a callback function 
//  for the 'application menu' (app_menu.c) .

#include "config.h"

#if (CONFIG_APP_MENU) // this module is only available along with app_menu.c ...

#include <stm32f4xx.h>
#include <string.h>
#include "irq_handlers.h"
#include "lcd_driver.h"
#include "app_menu.h" // 'simple' alternative menu activated by red BACK-button
#include "printf.h"
#include "spiflash.h" // md380_spiflash_read()
#include "codeplug.h" // codeplug memory addresses, struct- and array-sizes
#include "amenu_codeplug.h" // header for THIS module (to check prototypes,etc)


//---------------------------------------------------------------------------
BOOL ZoneList_ReadNameByIndex( int index,             // [in] zero-based zone index
                                char *psz20ZoneName ) // [out] zone name as a C string
  // Return value : TRUE when successfully read a zone name for this index,
  //                FALSE when failed or not implemented for a certain firmware.
{
  wchar_t wc16Temp[20]; // don't use global variables here.. so roll our zone-list-reader
  if( index>=0 && index<CODEPLUG_MAX_ZONE_LIST_ENTRIES )
   { md380_spiflash_read( wc16Temp, index * CODEPLUG_SIZEOF_ZONE_LIST_ENTRY 
                                          + CODEPLUG_SPIFLASH_ADDR_ZONE_LIST,
                          16 * sizeof(wchar_t) );
     wc16Temp[16] = 0;  // convert from wasteful 'wide' string into a good old "C"-string:
     wide_to_C_string( wc16Temp, psz20ZoneName, 16+1 );
     // unused entries in the codeplug appeared 'zeroed' (filled with 0x00),
     // thus 0x00 in the first character seems to mark the end of the list:
     return wc16Temp[0] != 0;
   }
  else
   { psz20ZoneName[0] = '\0';
     return FALSE;
   }
} // end ZoneList_ReadNameByIndex()

//---------------------------------------------------------------------------
BOOL ZoneList_SetZoneByIndex( int index )  // [in] zero-based zone index
{
  wchar_t wc16Temp[20];
#ifdef CODEPLUG_RAM_ADDR_ZONE_NUMBER_STRUCT
  zone_number_t *pZoneStruct;
#endif

  // Summary of 'zone-related' variables and functions, with addresses for D13.020 (RAM, SPI-Flash) :
  // - zone_data_64byte @ 0x2001e218 : Seems to be a 64 byte structure with the 
  //    NAME (16 wide chars) in the first 32 bytes, and something unknown in the remaining 32 bytes.
  //    0x2001e218 is the destination address when reading *64* bytes from SPI-flash, see 0x08022d74.
  // - zone_name @  first part often had the same content as zone_name at 0x2001cddc .
  // - the "zone number" is part of another FOUR- or FIVE-BYTE struct, begin at 0x2001e57c,
  //    ONE-based index in byte[3] (@0x2001e57F). 
  //    The 'last used zone-number-struct' is stored in SPI-flash at offset 0x02F000, see 0x08022ece .
  //    That SPI-flash memory region doesn't seem to belong to the CODEPLUG !
  //    Examined the 'zone-number-struct' in RAM and in SPI-flash with 'HexMon' (amenu_hexmon.c) :
  // > 2001E57C : FF FF FF 01 FF  (when the FIRST zone was active)
  // > C002F000 : FF FF FF 01 FF  (which confirms the non-volatile storage address for the ZONE NUMBER)
  // ( |_ (address bits 31..28 = "C" informed HexMon to read from SPI-Flash, not the CPU's address space)
  // Maybe we can switch to a different zone here WITHOUT overwriting the setting in SPI-flash ? See below..
  if( index>=0 && index<CODEPLUG_MAX_ZONE_LIST_ENTRIES )
   {
#  if( 1 )
     // Simply overwriting the two incarnations of 'zone_name' below
     // doesn't switch to a different zone. The *CHANNEL* data must 
     // be re-loaded from the codeplug, too ! 
     md380_spiflash_read( wc16Temp, 
        index * CODEPLUG_SIZEOF_ZONE_LIST_ENTRY 
              + CODEPLUG_SPIFLASH_ADDR_ZONE_LIST, 
        16 * sizeof(wchar_t) );
     if( wc16Temp[0] != 0 ) // name looks valid..
      {
#      if( HAVE_ZONE_NAME )
        memcpy( zone_name, wc16Temp, 16 * sizeof(wchar_t) );
#      endif
#      if( HAVE_ZONE_NAME_2 )
        memcpy( zone_name_2, wc16Temp, 16 * sizeof(wchar_t) );
#      endif
      }
     // Imitate some of the code at 0x08013418 in D13.020 ...
     // This is possibly the stuff called when 'confirming' a new zone in Tytera's original menu.
     // There seems to be a FIVE-BYTE struct with the ZONE NUMBER at byte-offset #3. 
     // It lives in SPI-flash (at 0x2F000?), and there's a copy in Tytera's part of the RAM:
#   ifdef CODEPLUG_RAM_ADDR_ZONE_NUMBER_STRUCT /* 0x2001E57C in D13.020, ymmv.. */
     pZoneStruct = (zone_number_t *)CODEPLUG_RAM_ADDR_ZONE_NUMBER_STRUCT;
     pZoneStruct->zone_index = index+1;  // written by a Pascal fan, array-indices start at ONE
     // Leave the other four (unknown) bytes in the 'zone-number-struct' unchanged.
     // The above switches the ZONE (temporarily, until power-off) but it doesn't
     // reload the parameters for the CURRENT CHANNEL from that zone.
     // Rotating the channel knob would fix that, but we can persuade the original firmware
     // to 'load' all channel-depending parameter for the NEW ZONE this way:
     channel_num = 0;  // <- lets Tytera's part of the firmware "re-load" 
           // data for the current channel in the NEW ZONE from the codeplug .
           // The effect can be seen almost immediately in the application menu:
           // Set a new zone (selected from the list), and the name of the CHANNEL
           // also changes in the menu.
#   endif // < do we know the address of the "zone-number-struct" in RAM > ?

#  else // try something else because the above didn't work...

#  endif // (old,new) ?

   } // end if < valid array index for the zone list >
  return FALSE;

} // end ZoneList_SetZoneByIndex()

//---------------------------------------------------------------------------
static void ZoneList_OnEnter(app_menu_t *pMenu, menu_item_t *pItem)
  // Called ONCE when "entering" the 'Zone List' display.
  // Tries to find out how many zones exist in the codeplug,
  // and the array-index of the currently active zone . 
{ int i=0;
  char sz20[22];
  char sz20CurrZone[22];
  scroll_list_control_t *pSL = &pMenu->scroll_list;

  ScrollList_Init( pSL ); // set all struct members to defaults
  pSL->current_item = -1; // currently active zone still unknown

  wide_to_C_string( zone_name, sz20CurrZone, 16+1/*maxlen*/ );

  while( i < CODEPLUG_MAX_ZONE_LIST_ENTRIES )
   { if(! ZoneList_ReadNameByIndex( i, sz20 ) ) // guess all zones are through
      { break; 
      }
     if( strcmp( sz20, sz20CurrZone) == 0 )
      { pSL->current_item = i;
      }
     ++i;
   }
  pSL->num_items = i;

  // Begin navigating through the list at the currently active zone:
  if( pSL->current_item >= 0 )
   {  pSL->focused_item = pSL->current_item;
#    if( CONFIG_MORSE_OUTPUT ) // autonomously report the first item in Morse code:
      pMenu->morse_request = AMENU_MORSE_REQUEST_ITEM_TEXT | AMENU_MORSE_REQUEST_ITEM_VALUE;
#    endif
   }

} // end ZoneList_OnEnter()

//---------------------------------------------------------------------------
static void ZoneList_Draw(app_menu_t *pMenu, menu_item_t *pItem)
  // Draws the 'zone list' screen. Gets visible when ENTERING that item in the app-menu.
  // 
{ int i, n_visible_items, sel_flags = SEL_FLAG_NONE;
  lcd_context_t dc;
  char cRadio; // character code for a selected or unselected "radio button"
  char sz20[22];
  scroll_list_control_t *pSL = &pMenu->scroll_list;


  // Draw the COMPLETE screen, without clearing it initially to avoid flicker
  LCD_InitContext( &dc ); // init context for 'full screen', no clipping
  Menu_GetColours( sel_flags, &dc.fg_color, &dc.bg_color );
  ScrollList_AutoScroll( pSL ); // modify pSL->scroll_pos to make the FOCUSED item visible
  dc.font = LCD_OPT_FONT_12x24;
  LCD_Printf( &dc, "\tZone %d/%d\r", (int)(pSL->focused_item+1), (int)pSL->num_items );
  LCD_HorzLine( dc.x1, dc.y++, dc.x2, dc.fg_color ); // spacer between title and scrolling list
  LCD_HorzLine( dc.x1, dc.y++, dc.x2, dc.bg_color );
  i = pSL->scroll_pos;   // zero-based array index of the topmost VISIBLE item
  n_visible_items = 0;   // find out how many items fit on the screen
  while( (dc.y < (LCD_SCREEN_HEIGHT-8)) && (i<pSL->num_items) )
   { 
     if(! ZoneList_ReadNameByIndex( i, sz20 ) ) // oops.. shouldn't have reached the last item yet !
      { break; 
      }
     if( i == pSL->current_item ) // this is the CURRENTLY ACTIVE zone :
      { cRadio = 0x02;  // character code for a 'selected radio button', see applet/src/font_8_8.c 
      }
     else
      { cRadio = 0x01;  // character code for 'unselected radio button'
      }
     if( i == pSL->focused_item )
      { sel_flags = SEL_FLAG_FOCUSED;
#      if( CONFIG_MORSE_OUTPUT )
        // Announcement of the CURRENTLY FOCUSED item for the Morse generator:
        sprintf( pMenu->sz40MorseTextFromFocusedLine, "\x09zone %d\x10%s",i+1, sz20  );
        //  '\x09' = space and lower pitch (kind of 'highlight'), 
        //  '\x10' = space and back to the normal CW pitch (tone).
#      endif
      }
     else if( i == pSL->current_item ) // this is the CURRENTLY ACTIVE zone :
      { sel_flags = SEL_FLAG_CURRENT;  // additional highlighting (besides the selected button)
      }
     else
      { sel_flags = SEL_FLAG_NONE;
      }
     Menu_GetColours( sel_flags, &dc.fg_color, &dc.bg_color );
     dc.x = 0;
     dc.font = LCD_OPT_FONT_8x16;
     LCD_Printf( &dc, " " );  
     dc.font = LCD_OPT_FONT_16x16; // ex: codepage 437, but the useless smileys are radio buttons now,
                                   // to imitate Tytera's zone list (at least a bit) ! 
     LCD_Printf( &dc, "%c", cRadio ); // 16*16 pixels for a circle, not a crumpled egg
     dc.font = LCD_OPT_FONT_8x16;
     LCD_Printf( &dc, " %02d %s\r", i+1, sz20 ); // '\r' clears to the end of the line, '\n' doesn't
     i++;
     n_visible_items++;
   } 
  // If necessary, fill the rest of the screen (at the bottom) with the background colour:
  Menu_GetColours( SEL_FLAG_NONE, &dc.fg_color, &dc.bg_color );
  LCD_FillRect( 0, dc.y, LCD_SCREEN_WIDTH-1, LCD_SCREEN_HEIGHT-1, dc.bg_color );
  pMenu->redraw = FALSE;    // "done" (screen has been redrawn).. except:
  if( n_visible_items > pSL->n_visible_items ) // more items visible than initially assumed ?
   { pSL->n_visible_items = n_visible_items;   // adapt parameters for the scrollbar,
     pMenu->redraw = ScrollList_AutoScroll( pSL ); // and redraw everything (KISS)
   }
} // ZoneList_Draw()


//---------------------------------------------------------------------------
int am_cbk_ZoneList(app_menu_t *pMenu, menu_item_t *pItem, int event, int param )
  // Callback function, invoked from the "app menu" framework for the 'ZONES' list.
  // (lists ALL ZONES, not THE CHANNELS in a zone)
{
  scroll_list_control_t *pSL = &pMenu->scroll_list;

  // what happened, why did the menu framework call us ?
  if( event==APPMENU_EVT_ENTER ) // pressed ENTER (to enter the 'Zone List') ?
   { ZoneList_OnEnter(pMenu, pItem); 
     return AM_RESULT_OCCUPY_SCREEN; // occupy the entire screen (not just a single line)
   }
  else if(event==APPMENU_EVT_PAINT) // someone wants us to paint into the framebuffer
   { // To minimize QRM from the display cable, only redraw when necessary (no "dynamic" content here):
     if( pMenu->visible == APPMENU_USERSCREEN_VISIBLE ) // only if HexMon already 'occupied' the screen !
      { if( pMenu->redraw )
         { pMenu->redraw = FALSE;   // don't modify this sequence
           ZoneList_Draw(pMenu, pItem); // <- may decide to draw AGAIN (scroll)
         }
        return AM_RESULT_OCCUPY_SCREEN; // keep the screen 'occupied' 
      }
   }
  else if( event==APPMENU_EVT_KEY ) // some other key pressed while focused..
   { switch( (char)param ) // here: message parameter = keyboard code (ASCII)
      {
        case 'M' :  // green "Menu" key : kind of ENTER. But here, "apply & return" .
           if( pSL->focused_item>=0 )
            { ZoneList_SetZoneByIndex( pSL->focused_item );
              // The above command switched to the new zone, and probably set
              // channel_num = 0 to 'politely ask' the original firmware to 
              // reload whever is necessary from the codeplug (SPI-Flash). 
              // It's unknown when exactly that happens (possibly in another task). 
              // To update the CHANNEL NAME from the *new* zone in our menu, 
              // let a few hundred milliseconds pass before redrawing the screen:
              StartStopwatch( &pMenu->stopwatch_late_redraw );
            }
           return AM_RESULT_EXIT_AND_RELEASE_SCREEN;
        case 'B' :  // red "Back"-key : return from this screen, discard changes.
           return AM_RESULT_EXIT_AND_RELEASE_SCREEN;
        case 'U' :  // cursor UP
           if(  pSL->focused_item > 0 )
            { --pSL->focused_item;
#            if( CONFIG_MORSE_OUTPUT ) // autonomously report the first item in Morse code:
              pMenu->morse_request = AMENU_MORSE_REQUEST_ITEM_TEXT | AMENU_MORSE_REQUEST_ITEM_VALUE;
#            endif
            }
           break;
        case 'D' :  // cursor DOWN
           if(  pSL->focused_item < (pSL->num_items-1) )
            { ++pSL->focused_item;
#            if( CONFIG_MORSE_OUTPUT ) // autonomously report the first item in Morse code:
              pMenu->morse_request = AMENU_MORSE_REQUEST_ITEM_TEXT | AMENU_MORSE_REQUEST_ITEM_VALUE;
#            endif
            }
           break;
        default:    // Other keys .. editing or treat as a hotkey ?
           break;
      } // end switch < key >
     pMenu->redraw = TRUE;

   } // end if < keyboard event >
  if( pMenu->visible == APPMENU_USERSCREEN_VISIBLE ) // only if HexMon already 'occupied' the screen !
   { return AM_RESULT_OCCUPY_SCREEN; // keep the screen 'occupied' 
   }
  else
   { return AM_RESULT_NONE; // "proceed as if there was NO callback function"
   }
} // end am_cbk_ZoneList()


#endif // CONFIG_APP_MENU ?
