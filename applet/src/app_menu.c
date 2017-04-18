// File:    md380tools/applet/src/app_menu.c
// Author:  Wolf (DL4YHF) [initial version] 
//          Please respect the author's coding style, indentations,
//          and don't poison this soucecode with TAB characters . 
//
// Date:    2017-04-17
//  Implements a simple menu opened with the red BACK button,
//             which doesn't rely on ANY of Tytera's firmware
//             functions at all (neither "gfx" nor "menu").
//  Module prefix 'am_' for "Application Menu" .
//  Added 2017-03-31 for the Morse output for visually impaired hams,
//  because strings in 'our own' menu can be sent out much easier
//  than with the original menu by Tytera (opened via green 'MENU' button).
// 
// To include these functions in the patched firmware:
//    
//  1. Add the following lines in applet/Makefile (after SRCS += narrator.o) :
//      SRCS += app_menu.o 
//      SRCS += lcd_driver.o
//      SRCS += font_8_8.o
//  
//  2. #define CONFIG_APP_MENU 1  in  md380tools/applet/config.h  .


#include "config.h"

#include <stm32f4xx.h>
#include <string.h>
#include <wchar.h>  // wcscmp(), wscpy() .. dreadful "wide strings" instead of UTF-8 !
#include <limits.h> // INT_MIN, INT_MAX, ...
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
#include "codeplug.h"     // zone_name[], etc (mostly unknown for old firmware)
#include "console.h"      // text screen buffer for Netmon (may be displayed through the 'app menu', too)
#include "irq_handlers.h" // low-level beep generator (mainly used for Morse output)
#include "narrator.h"     // announces channel, zone, and maybe current menu selection in Morse code
#include "lcd_driver.h"   // alternative LCD driver (DL4YHF), doesn't depend on 'gfx'
#include "app_menu.h"     // 'simple' alternative menu activated by red BACK-button


// Variables used by the 'app menu' :
#if (CONFIG_APP_MENU)
uint8_t am_key;   // one-level keyboard buffer, fed by irq_handlers.c

// For shortest code, put everything inside a SMALL struct, and reference it
// whereever possible via a pointer in a LOCAL variable:
app_menu_t AppMenu;  // data for a single instance of the 'application menu'

// Before entering a SUB-menu, num_items, item_index, and pItems are stacked here:
# define APPMENU_STACKSIZE 4 // ~~maximum nesting level
struct
{ menu_item_t *pItems;
  uint8_t item_index;
  uint8_t vert_scroll_pos;
} submenu_stack[APPMENU_STACKSIZE];


//---------------------------------------------------------------------------
// Internal 'forward' references (not 'static', who knows if....) 
int  Menu_DrawLineWithItem(app_menu_t *pMenu, int y, int iTextLineNr);
int  Menu_DrawSeparatorWithHotkey(app_menu_t *pMenu, int y, char *cpHotkey );
BOOL Menu_IsFormatStringDelimiter( char c );
int  Menu_ParseDecimal( char **ppszSource );
BOOL Menu_ProcessHotkey(app_menu_t *pMenu, char c);
void Menu_OnEnterKey(app_menu_t *pMenu);
void Menu_OnExitKey(app_menu_t *pMenu);
void Menu_BeginEditing( app_menu_t *pMenu, menu_item_t *pItem );
void Menu_FinishEditing( app_menu_t *pMenu, menu_item_t *pItem );
void Menu_OnIncDecEdit( app_menu_t *pMenu, int delta );
void Menu_PushSubmenuToStack(app_menu_t *pMenu);
BOOL Menu_PopSubmenuFromStack(app_menu_t *pMenu);
menu_item_t *Menu_GetFocusedItem(app_menu_t *pMenu);
int  Menu_InvokeCallback(app_menu_t *pMenu, menu_item_t *pItem, int event, int param);
int  Menu_ReadIntFromPtr( void *pvValue, int data_type );


// Prototypes and forward references for some menu items :
int am_cbk_ColorTest(app_menu_t *pMenu, menu_item_t *pItem, int event, int param );
int am_cbk_Backlt(app_menu_t *pMenu, menu_item_t *pItem, int event, int param );
int am_cbk_Morse(app_menu_t *pMenu, menu_item_t *pItem, int event, int param );
const menu_item_t am_Setup[]; // referenced from main menu
const am_stringtable_t am_stringtab_opmode2[]; // for gui_opmode2
const am_stringtable_t am_stringtab_255Auto[];
const am_stringtable_t am_stringtab_narrator_modes[];

//---------------------------------------------------------------------------
// Alternative 'main' menu, opened with the RED 'BACK'-button :

const menu_item_t am_Main[] = 
{ // { "Text__max__13", data_type,  options,opt_value,
  //     pvValue,iMinValue,iMaxValue, string table, callback }
  { "Chnl",             DTYPE_WSTRING, APPMENU_OPT_NONE,0, 
         channel_name,0,0,          NULL,         NULL     },
  { "Zone",             DTYPE_WSTRING, APPMENU_OPT_NONE,0, 
         zone_name,0,0,             NULL,         NULL     },
  { "Cont",             DTYPE_WSTRING, APPMENU_OPT_NONE,0, 
         contact.name,0,0,          NULL,         NULL     },
   // yet to be found out: Relation between 'contact.name', 'tx_id',
   // DMR-"talkgroup", -"reflector", current_channel_info, 
   // and how all this sticks together in the original firmware.
   // See (old) menu.c : create_menu_entry_set_tg_screen_store() .
   
  { "TkGr",             DTYPE_NONE, APPMENU_OPT_NONE,0, 
         NULL,0,0,                  NULL,         NULL     },
  { "[1]Test/Setup",       DTYPE_SUBMENU, APPMENU_OPT_NONE,0, 
   // |__ hotkey to get here quickly (press RED BUTTON followed by this key)
     (void*)am_Setup,0,0,           NULL,         NULL     },
  { "Exit",             DTYPE_NONE, APPMENU_OPT_BACK,0,
         NULL,0,0,                  NULL,         NULL     },
  // (an extra "Exit to main screen" may be redundant here, 
  //  and didn't work really well because Tytera's menu immediately
  //  popped up when pressing the green 'Enter' key in THIS menu.
  //  Alternative: Press the RED key to leave this alternative menu (sic!).
  //  One fine day, we may find a more reliable way to intercept keys
  //  than the current stuff in keyb.c : kb_handler_hook() ... )

  // End of the list marked by "all zeroes" :
  { NULL, 0/*dt*/, 0/*opt*/, 0/*ov*/, NULL/*pValue*/, 0,0, NULL, NULL }

};

const menu_item_t am_Setup[] = // setup menu, nesting level 1 ...
{ // { "Text__max__13", data_type,  options,opt_value,
  //     pvValue,iMinValue,iMaxValue, string table, callback }
  { "Setup:Back",       DTYPE_NONE,    APPMENU_OPT_BACK,0,
         NULL,0,0,                  NULL,         NULL     },

  // { "Text__max__13", data_type,  options,opt_value,
  //    pvValue,iMinValue,iMaxValue, string table, callback }
  { "[1 Backlight]Level Lo", DTYPE_INTEGER, 
      APPMENU_OPT_EDITABLE|APPMENU_OPT_IMM_UPDATE|APPMENU_OPT_BITMASK_R,0x0F,
        &global_addl_config.backlight_intensities,0,9,NULL,NULL},
  { "Level Hi", DTYPE_INTEGER, 
      APPMENU_OPT_EDITABLE|APPMENU_OPT_IMM_UPDATE|APPMENU_OPT_BITMASK_R,0xF0, 
        &global_addl_config.backlight_intensities,1,9,NULL,NULL},
  { "Time/sec", DTYPE_UNS8,   APPMENU_OPT_EDITABLE|APPMENU_OPT_FACTOR,5/*!*/, 
        &md380_radio_config.backlight_time,0,120, NULL, NULL   },

  // { "Text__max__13", data_type,  options,opt_value,
  //    pvValue,iMinValue,iMaxValue,           string table, callback }
  { "[2 Morse output]Mode", DTYPE_UNS8,
        APPMENU_OPT_EDITABLE|APPMENU_OPT_BITMASK,
            NARRATOR_MODE_OFF|NARRATOR_MODE_ENABLED|NARRATOR_MODE_VERBOSE, // <- here: bitmask !
        &global_addl_config.narrator_mode,0,9, am_stringtab_narrator_modes,am_cbk_Morse },  
  { "Speed/WPM",        DTYPE_UNS8, 
        APPMENU_OPT_EDITABLE|APPMENU_OPT_IMM_UPDATE, 0, 
        &global_addl_config.cw_speed_WPM,10,60, NULL,am_cbk_Morse },  
  { "Pitch/Hz",         DTYPE_UNS8, 
        APPMENU_OPT_EDITABLE|APPMENU_OPT_IMM_UPDATE|APPMENU_OPT_FACTOR,10, 
        &global_addl_config.cw_pitch_10Hz,200,2000,NULL,am_cbk_Morse },  
  { "Volume",           DTYPE_UNS8, 
        APPMENU_OPT_EDITABLE|APPMENU_OPT_IMM_UPDATE, 0, 
        &global_addl_config.cw_volume,0,100,   am_stringtab_255Auto, am_cbk_Morse },

  // { "Text__max__13", data_type,  options,opt_value,
  //     pvValue,iMinValue,iMaxValue, string table, callback }
  { "[3 Test/Debug]bl_timer",   DTYPE_UNS16, APPMENU_OPT_NONE, 0, 
        &backlight_timer,0,0,      NULL,         NULL     },
  { "opmode2",          DTYPE_UNS8,  APPMENU_OPT_NONE, 0, 
        &gui_opmode2,0,0,      am_stringtab_opmode2, NULL },
  { "[b8]opmode1",      DTYPE_UNS8,  APPMENU_OPT_NONE, 0, 
        &gui_opmode1,0,0,          NULL,         NULL     },
  { "Colour test",      DTYPE_NONE, APPMENU_OPT_NONE,0, 
        NULL,0,0,                  NULL, am_cbk_ColorTest },
  { "Setup:Back",       DTYPE_NONE, APPMENU_OPT_BACK,0,
        NULL,0,0,                  NULL,         NULL     },

  // End of the list marked by "all zeroes" :
  { NULL, 0/*dt*/, 0/*opt*/, 0/*ov*/, NULL/*pValue*/, 0,0, NULL, NULL }

}; // end am_Setup[]

//---------------------------------------------------------------------------
// 'Test' menu to inspect some of the original firmware's variables:
const am_stringtable_t am_stringtab_opmode2[] = 
{
  { OPM2_IDLE,      "idle" },
  { OPM2_VOICE,     "voice"},
  { OPM2_TERM,      "term" }, 
  { OPM2_MSG_POPUP, "popup"},
  { OPM2_ALARM_RECV,"alarm"},
  { OPM2_MENU,      "menu" },
  { 0,              NULL   } // <- marks the end of a string table
};

const am_stringtable_t am_stringtab_255Auto[] = 
{ { 255, "auto" },
  { 0,   NULL   }  // <- marks the end of a string table
};

const am_stringtable_t am_stringtab_narrator_modes[] = 
{ { NARRATOR_MODE_OFF,     "off"     },
  { NARRATOR_MODE_ENABLED, "enabled" }, 
  { NARRATOR_MODE_ENABLED|NARRATOR_MODE_VERBOSE, "verbose" },
  // Note: If a menu item's parameter value is connected to
  // a string table (like this), the only values that can be
  // selected in the menu are those from the table - no integers.
};

//---------------------------------------------------------------------------
void Menu_OnKey( uint8_t key) // called on keypress from some interrupt handler
{ am_key = key;
}

//---------------------------------------------------------------------------
int Menu_GetNumItems( menu_item_t *pItems )
{ int nItems = 0;
  if( pItems )
   { while( pItems->pszText != NULL )
      { ++nItems;
        ++pItems;
      } 
   }
  return nItems;
} 

//---------------------------------------------------------------------------
void Menu_Open(app_menu_t *pMenu)
{
  memset( pMenu, 0, sizeof( app_menu_t ) );
  pMenu->pItems  = (void*)am_Main; // VERY unfortunately, this cast is necessary
  pMenu->num_items = Menu_GetNumItems( pMenu->pItems );
  pMenu->visible = APPMENU_VISIBLE; 
  pMenu->redraw  = TRUE;
} // end Menu_Open()

//---------------------------------------------------------------------------
void Menu_Close(app_menu_t *pMenu)
{
  if( kb_row_col_pressed ) // some key (especially the green MENU key) still pressed ?
   { pMenu->visible = APPMENU_VISIBLE_UNTIL_KEY_RELEASED;
   } // end if < trying to exit but a key is still pressed >
  else // green MENU key not pressed, so the normal main screen should appear..
   {
     // The original firmware didn't COMPLETELY redraw it's own 'main screen'
     // now, because it's not aware of the display been hijacked temporarily.
     LCD_FillRect( 0,0, LCD_SCREEN_WIDTH-1, LCD_SCREEN_HEIGHT-1, LCD_COLOUR_MD380_BKGND_BLUE ); 
     // Calling display_idle_screen() from here would be asking for trouble. 
     // Better let the ugly monster 'f_4225()' (@0x801fe5c) decide what
     // to draw. 
     // ex: gui_opmode2 = OPM2_IDLE; // this did NOT convince Tytera to redraw the "idle"-screen
     // ex: from keyb.c(!) : "cause transient -> switch back to idle screen"
     // ex:   gui_opmode2 = OPM2_MENU ;
     // ex:   gui_opmode1 = SCR_MODE_IDLE | 0x80 ;
     // The above "trick" from keyb.c didn't work. Instead the display locked up,
     // so not even the 'app menu' was usable anymore.
     // The only way to bring the normal idle screen back was to turn the
     // channel knob (ummm..sounds familiar from a problem with Netmon).
     // The value returned by 'Read_Channel_Switch_maybe' (@0x0804fd24 in D13.020)
     // is compared with 'channel_num' (@0x2001e8c1 in D13.020) .
     // Existing channel numbers are 1 to 16, so :
     channel_num = 0;  // <- kludge to force re-drawing the idle screen
     // (very ugly, but none of the other "tricks" listed above actually worked)
   } // end else <exiting into the "main screen">
} // end Menu_Close()


//---------------------------------------------------------------------------
int Menu_IsVisible(void)
  // Return value: 1=currently visible (open), 0=not open; don't intercept keys
{
  return AppMenu.visible != APPMENU_OFF; // used in keyb.c to disable the GREEN-key-menu
  // (whichever menu was opened first "wins" the keyboard :
  //  Pressing the "green key" first opens Tytera's menu.
  //  Pressing the "red key" first opens this 'app menu'.
} // end Menu_IsVisible() .


//---------------------------------------------------------------------------
static void Menu_CheckVertScrollPos(app_menu_t *pMenu)
{ int i,n = LCD_SCREEN_HEIGHT / ( 2 * LCD_FONT_HEIGHT );
  pMenu->n_items_visible = n; // <- this is just an INITIAL GUESS,
                              // since the items may have different heights !
  // Scroll the currently selected item into view :
  // Visible range of items is vert_scroll_pos to vert_scroll_pos+n-1 .
  // If item_index is outside that range, adjust vert_scroll_pos:
  if( pMenu->item_index < pMenu->vert_scroll_pos ) // "invisible above the top"
   {  pMenu->vert_scroll_pos = pMenu->item_index; 
   }
  i = pMenu->vert_scroll_pos + n - 1;
  if( pMenu->item_index > i ) // selected item "invisible below the bottom"
   {  pMenu->vert_scroll_pos = pMenu->item_index-(n-1);
   }
} // end Menu_CheckVertScrollPos()

//---------------------------------------------------------------------------
__attribute__ ((noinline)) int Fletcher32( uint32_t prev_sum, uint16_t *pwData, int nWords ) 
{ // principle similar as 'Fletcher32', but it doesn't matter much..
  //  .. as long as if it gives a different result when A FEW BITS in the input change.
  uint32_t sum1 = prev_sum;
  uint32_t sum2 = prev_sum >> 16;
  while( nWords-- ) 
   {
     sum1 = (sum1 + *(pwData++)) % 0xFFFF;  
     sum2 = (sum2 + sum1) % 0xFFFF;
   }
  return (sum2 << 16) | sum1;
}

//---------------------------------------------------------------------------
int wide_to_C_string( wchar_t *wide_string, char *c_string, int maxlen )
  // Notes:
  //  * If the source (wide_string) exceeds the capacity
  //    of c_string, the latter will be truncated.
  //    Unlike the seriously mis-named "strncpy", the destination
  //    is ALWAYS terminated with a zero - it's a classic "C"-string.
  //  * For safety, maxlen is considered the SIZE of a char-array,
  //    and because the trailing zero is part of that array,
  //    the maximum payload with maxlen=40 will be 39 (!) characters.
  //  * If maxlen is 0 (or even negative), c_string isn't modified.
  //  * If maxlen is 1, c_string will be '\0' !
  //  * This function returns the NUMBER OF CHARACTERS copied,
  //    not including the trailing zero.
{ int n_chars_copied = 0;
  if( maxlen>0 )
   { while( *wide_string && (maxlen>1/*!!*/) )
      { *(c_string++) = (char)(*(wide_string++));
        --maxlen;
        ++n_chars_copied;
      }
     *c_string = '\0'; // ALWAYS terminate C-strings with a trailing zero !
   }
  return n_chars_copied;
} // end wide_to_C_string()


//---------------------------------------------------------------------------
static void Menu_CheckDynamicValues(app_menu_t *pMenu)
  // Checks if any of the currently visible values (in the menu)
  // has been changed. If so, sets pMenu->redraw = TRUE .
{ menu_item_t *pItem;
  int item_index, imax, value;
  uint32_t checksum = 0;
  item_index = pMenu->vert_scroll_pos;
  imax = item_index + pMenu->n_items_visible - 1; 
  if (imax >= pMenu->num_items )
   {  imax =  pMenu->num_items-1;
   }
  while( item_index <= imax )
   { pItem = &((menu_item_t *)pMenu->pItems)[item_index++];
     if( pItem->pvValue )
      { switch( pItem->data_type )
         { case DTYPE_STRING : // the 'value' is a good old "C"-string (8 bit) .
              // If they had used glorious UTF-8 encoding this would work for anything,
              // but they didn't - see DTYPE_WSTRING below.
              checksum = Fletcher32( checksum, (uint16_t *)pItem->pvValue, (strlen((char*)pItem->pvValue)+1) / 2 );
              // Depending on the string length, Mr Fletcher may look at the trailing zero or not.
              // For this purpose(!), it doesn't matter. 
              break;   
           case DTYPE_WSTRING: // the 'value' is wasteful "wide" string
              // Like it or not, must support this stuff because Tytera
              // uses wide strings for channel-, zone-, "contact"-, 
              // and possibly some other names which may change "in the background".
              checksum = Fletcher32( checksum, (uint16_t *)pItem->pvValue, wcslen((wchar_t*)pItem->pvValue) );
              // btw "wcslen" was used elsewhere, so don't bother using that bulk here.
              break;
           default: // anything else should be convertable into an INTEGER: 
              value = Menu_ReadIntFromPtr( pItem->pvValue, pItem->data_type );
              checksum = Fletcher32( checksum, (uint16_t *)&value, sizeof(int)/sizeof(uint16_t) );
              break;
         }
      } // end if < item displaying a "value" -> include it in the hash >
   } // end for < all CURRENTLY VISIBLE items >
  if( pMenu->value_chksum != checksum )
   {  pMenu->value_chksum =  checksum;
      pMenu->redraw = TRUE;  
   }
} // end Menu_CheckDynamicValues()


//---------------------------------------------------------------------------
void Menu_GetColours( int sel_flags, uint16_t *pFgColor, uint16_t *pBgColor )
{ 
  // For a start, imitate Tytera's menu colours .
  // For later, make some of these colours adjustable,
  //     using a few bits(!) in global_addl_config .  
  if( sel_flags & SEL_FLAG_CURSOR )
   { *pFgColor = LCD_COLOUR_WHITE;
     *pBgColor = LCD_COLOUR_RED;
   }
  else if( sel_flags & SEL_FLAG_FOCUSED ) 
   { *pFgColor = LCD_COLOUR_WHITE;
     *pBgColor = LCD_COLOUR_BLUE;
   }
  else // neither the edit cursor nor not marked : 
   { *pFgColor = LCD_COLOUR_BLACK;
     *pBgColor = LCD_COLOUR_WHITE;
   }
} // end Menu_GetColours()

//---------------------------------------------------------------------------
char *Menu_FindInStringTable( const am_stringtable_t *pTable, int value)
  // Returns the address of a string if 'value' was found,
  // otherwise returns NULL.
{
  if( pTable )
   { while( pTable->pszText != NULL )
      { if( pTable->value == value )
         { return pTable->pszText;
         }
        ++pTable;
      }
   }
  return NULL;
} // end Menu_FindInStringTable()

//---------------------------------------------------------------------------
int Menu_ReadIntFromPtr( void *pvValue, int data_type )
{
  if(pvValue==NULL)  // safety first !
   { return 0;
   }
  switch( data_type )
   { case DTYPE_NONE   /*0*/ :
        break;  
     case DTYPE_BOOL   /*1*/ :
        break;  
     case DTYPE_INT8   /*2*/ :  
        return *(int8_t*)pvValue;  
     case DTYPE_INT16  /*3*/ :  
        return *(int16_t*)pvValue;  
     case DTYPE_INTEGER/*4*/ :
        return *(int*)pvValue;  
     case DTYPE_UNS8   /*5*/ :
        return *(uint8_t*)pvValue;  
     case DTYPE_UNS16  /*6*/ :  
        return *(uint16_t*)pvValue;  
     case DTYPE_UNS32  /*7*/ :
        return *(uint32_t*)pvValue;  
     case DTYPE_FLOAT  /*8*/ :
        return (int)(*(float*)pvValue);  
     case DTYPE_STRING /*9*/ : // dare to PARSE A STRING here ?
        break;
     case DTYPE_WSTRING/*10*/: // "wide"-string: let this nonsense die, use UTF-8 instead !
        // (unfortunately Tytera uses this RAM-wasting garbage for zone_name[], etc)
        break;
   }
  return 0;
} // Menu_ReadIntFromPtr()

//---------------------------------------------------------------------------
void Menu_WriteIntToPtr( int iValue, void *pvValue, int data_type )
{
  if(pvValue==NULL)  // safety first !
   { return;
   }
  switch( data_type )
   { case DTYPE_NONE   /*0*/ :
        break;  
     case DTYPE_BOOL   /*1*/ :
        break;  
     case DTYPE_INT8   /*2*/ :  
        *(int8_t*)pvValue = (int8_t)iValue;
        break;  
     case DTYPE_INT16  /*3*/ :  
        *(int16_t*)pvValue = (int16_t)iValue;
        break;  
     case DTYPE_INTEGER/*4*/ :
        *(int*)pvValue = (int)iValue;
        break;  
     case DTYPE_UNS8   /*5*/ :
        *(uint8_t*)pvValue = (uint8_t)iValue;
        break;  
     case DTYPE_UNS16  /*6*/ :  
        *(uint16_t*)pvValue = (uint16_t)iValue;
        break;  
     case DTYPE_UNS32  /*7*/ :
        *(uint32_t*)pvValue = (uint32_t)iValue;
        break;  
     case DTYPE_FLOAT  /*8*/ :
        *(float*)pvValue = (float)iValue;
        break;  
     case DTYPE_STRING /*9*/ :
     case DTYPE_WSTRING/*10*/: // string types not supported HERE
        break;
   }
} // end Menu_WriteIntToPtr()

//---------------------------------------------------------------------------
void Menu_GetMinMaxForDataType( int data_type, int *piMinValue, int *piMaxValue )
{
  int min=0, max=0;
  switch( data_type )
   { case DTYPE_NONE   /*0*/ :
        break;  
     case DTYPE_BOOL   /*1*/ :
        max = 1;
        break;  
     case DTYPE_INT8   /*2*/ :  
        min = -128;
        max = 127;
        break;  
     case DTYPE_INT16  /*3*/ :  
        min = -32768;
        max = 32767;
        break;  
     case DTYPE_INTEGER/*4*/ :
        min = INT_MIN;
        max = INT_MAX;
        break;  
     case DTYPE_UNS8   /*5*/ :
        max = 255;
        break;  
     case DTYPE_UNS16  /*6*/ :  
        max = 65535;
        break;  
     case DTYPE_UNS32  /*7*/ :
     // max = 0xFFFFFFFF; // dilemma.. this wouldn't work with SIGNED 32 BIT INTEGERS !
        max = INT_MAX; // better than a MAX LIMIT of "-1" (=0xFFFFFFFF)..
        break;  
     case DTYPE_FLOAT  /*8*/ : // permitted value range cannot be expressed with an int..
        min = INT_MIN;
        max = INT_MAX;
        break;  
     case DTYPE_STRING /*9*/ :
     case DTYPE_WSTRING/*10*/: // string types not supported HERE
        break;
   }
  if( piMinValue != NULL )
   { *piMinValue = min;
   }
  if( piMaxValue != NULL )
   { *piMaxValue = max;
   }
} // end Menu_GetMinMaxForDataType()


//---------------------------------------------------------------------------
void IntToBinaryString(
        int iValue,      // [in] value to be converted
        int nDigits,     // [in] number of fixed digits, 
                         //      0=variable length without leading zeroes
        char *psz40Dest) // [out] string, expect up to 33(!) bytes
  // Converts an integer into a binary representation (string).
  // Beware: nDigits=0 means 'as many digits as necessary'.
  //         For a 32-bit integer, the destination string may
  //         be up to 32 characters long, plus one byte for the trailing zero !
{
  int iBitNr;
  if( nDigits <= 0 )
   { nDigits = 32;  // begin testing for nonzero bits at the MSB (31)
     while( nDigits>1 && (iValue & (1<<(nDigits-1)))==0 )
      { --nDigits;  // eliminate trailing zeroes 
      }
   }
  for( iBitNr=nDigits-1; iBitNr>=0; --iBitNr )
   { if( iValue & (1<<iBitNr) )
      { *psz40Dest++ = '1';
      }
     else
      { *psz40Dest++ = '0';
      }
   }
  *psz40Dest = '\0'; // never forget the trailing zero in a "C"-string !
} // end IntToBinaryString()

//---------------------------------------------------------------------------
void IntToDecHexBinString(
        int iValue,      // [in] value to be converted
        int num_base,    // [in] 2=binary, 10=decimal, 16=hex
        int nDigits,     // [in] number of fixed digits, 
                         //      0=variable length without leading zeroes
        char *psz40Dest) // [out] string, expect up to 33(!) bytes
  // Converts an integer into a binary representation (string).
  // Beware: nDigits=0 means 'as many digits as necessary'.
  //         For a 32-bit integer, the binary representation (string) may
  //         be up to 32 characters long, plus one byte for the trailing zero !
  //   Thus, for safety, psz40Dest should be a buffer with up to 40 characters.
{
  char sz7Format[8] = "%d";
  switch( num_base )
   { case 2: // binary: not supported by printf.c : sprintf() -> tfp_format() 
        IntToBinaryString( iValue, nDigits, psz40Dest );
        return;
     case 10:
        if( nDigits>0 )
         { sprintf( sz7Format+1, "%dd", nDigits );
         }
        else
         { // default ("%d") already set
         }
        break;
     case 16:
        if( nDigits>0 )
         { sprintf( sz7Format+1, "%dX", nDigits );
         }
        else
         { strcpy(  sz7Format+1, "%X" );
         }
        break;
     default:  // whatever the intention was, it's not supported here yet:
        sprintf( psz40Dest, "?base%d?", num_base );
        return;
   }
  // Arrived here ? Should be something supported by the
  //      small-footprint implementation of sprintf in printf.c  !
  sprintf( psz40Dest, sz7Format, iValue );
} // end IntToDecHexBinString()

//---------------------------------------------------------------------------
char *Menu_GetParamsFromItemText( char *pszText, int *piNumBase, int *piFixedDigits, char **cppHotkey )
  // Returns a pointer to the first "plain text" character 
  // after the list of printing options in squared brackets
{
  int num_base = 10;    // default: decimal numeric output
  int fixed_digits = 0; // default: no FIXED number of digits

  if( cppHotkey != NULL )
   { *cppHotkey = NULL;
   }
  if( (pszText!=NULL) && (*pszText == '[') ) // begin of a headline / hotkey indicator ?
   { ++pszText; 
     if( *pszText>='0' && *pszText<='9' ) // Digit -> Hotkey !
      { if( cppHotkey != NULL )
         { *cppHotkey = pszText;
         }
        // skip hotkey (with optional text for the 'separator line') :
        while( !Menu_IsFormatStringDelimiter( *pszText ) ) 
         { ++pszText;
         }
        if( *pszText!='\0' && *pszText!=']' ) // Skip delimiter after hotkey name ?
         { ++pszText;  // e.g. comma or semicolon
         }
      } 
     // After the hotkey, parse optional parameters .
     // Examples for formatting- and output options :
     //  [b]  = binary with as many digits as necessary
     //  [b8] = binary with 8 digits (fixed)
     //  [d3] = decimal with 3 digits
     //  [h8] = hexadecimal with 8 digits, but no hex prefix ("0x")
     //  [5h4] = item invokable via hotkey '5', with 4-digit hex display
     //  [5 Chapter Five;h4] = first item in "Chapter Five", 
     //          invokable via hotkey '5', with 4-digit hex display
     while( !Menu_IsFormatStringDelimiter( *pszText ) )
      { switch( *pszText++ )
         { case 'b': // [b] = binary display (for the value) ..
              num_base = 2; 
              fixed_digits = Menu_ParseDecimal( &pszText ); // FIXED number of digits ?
              break;
           case 'd': // [d] = decimal display with a fixed number of digits
              num_base = 10; 
              fixed_digits = Menu_ParseDecimal( &pszText );
              break;
           case 'h': // [d] = hex display with a fixed number of digits, no prefix
              num_base = 16; 
              fixed_digits = Menu_ParseDecimal( &pszText );
              break;
           default:  // "normal" character (not a delimiter) : just skip and ignore
              break;
         }
      } 
     if( *pszText == ']' ) // what began with a '[' *should* end with a ']' ..
      { ++pszText; 
      }
   } // end if < fixed menu text beginning with '[' >

  if( piNumBase != NULL )  // pass back optional outputs
   { *piNumBase = num_base;
   }
  if( piFixedDigits != NULL )
   { *piFixedDigits = fixed_digits;
   }
  return pszText; // returns a pointer to the first "plain text" character 
                  // after the list of printing options in squared brackets.

} // end Menu_GetParamsFromItemText()

//---------------------------------------------------------------------------
void Menu_ItemValueToString( menu_item_t *pItem,  int iValue, char *sz40Dest )
  // For simplicity (and to keep the code size low), the destination buffer
  // should be large enough for 40 characters . Thus sz40Dest .
{
  int num_base,fixed_digits;
  char *cp;

  sz40Dest[0] = '\0';
  if( pItem->pvValue != NULL )
   { 
     Menu_GetParamsFromItemText( (char*)pItem->pszText, &num_base, &fixed_digits, NULL );

     switch( pItem->data_type )
      { case DTYPE_STRING : // normal string with 8 bits/char (any encoding)
           strlcpy( sz40Dest, pItem->pvValue, 20 );
           break;
        case DTYPE_WSTRING: // "wide"-string.. eeek.. ever heard of UTF-8 ?
           // We don't really support this 'wide string' madness here. 
           // All characters in the 'wide string' are treated as if they were 8-bit !
           // Fortunately neither zone- nor channel names are Chinese :)
           wide_to_C_string( (wchar_t*)pItem->pvValue, sz40Dest, 40 );
           break;
        case DTYPE_SUBMENU:
           break;
        default : 
           cp = Menu_FindInStringTable( pItem->pStringTable, iValue );
           if( cp ) 
            { strlcpy( sz40Dest, cp, 40 );
            }
           else // don't show string from table but numeric value:
            { IntToDecHexBinString( iValue, num_base, fixed_digits, sz40Dest );
            }
           break;
      }
   }
} // end Menu_ItemValueToString()

//---------------------------------------------------------------------------
BOOL Menu_IsFormatStringDelimiter( char c )
  // Part of a very simple parser for a optional format specifications
  // in menu_item_t.pszText .
  // Notes:
  //  - the trailing zero is also considered a 'delimiter' here !
  //  - space is NOT a delimiter because it may be used in separator labels. 
{ return (c<' ' || c==',' || c==';' || c=='}' || c==']' );
}

//---------------------------------------------------------------------------
int Menu_ParseDecimal( char **ppszSource )
  // Returns ZERO if there's nothing to parse. KISS.
{ char *cp = *ppszSource;
  int iValue = 0;
  int iSign  = 1;
  if( *cp=='-' )
   { ++cp;
     iSign = -1;
   }
  while( *cp>='0' && *cp<='9' )
   { iValue = 10*iValue + (*(cp++)-'0');
   }
  *ppszSource = cp; // pass back the INCREMENTED 'source pointer'
  return iValue * iSign;
}

//---------------------------------------------------------------------------
int Menu_ScaleItemValue( menu_item_t *pItem, int n )
{ int i = pItem->opt_value; // this 'option value' serves multiple purposes..
  if( i != 0 ) // .. but only if nonzero (to avoid nonsense or endless loops):
   {
     if( pItem->options & (APPMENU_OPT_BITMASK | APPMENU_OPT_BITMASK_R ) )
      { n &= i;
        if( pItem->options & APPMENU_OPT_BITMASK_R )
         { while( !(i & 1) ) // bitwise "right-align" the bitgroup ..
            { i >>= 1;
              n >>= 1;
            } // after this loop, the least significant bit is in bit 0 (mask 1)
         }
        // Note: this bit-fiddling happens BEFORE looking up 
        //  the value in the string table a few lines below !
      }
     if( pItem->options & APPMENU_OPT_FACTOR )
      { n *= pItem->opt_value; 
        // for example, used in the CW pitch (byte), 65 means 650 Hz, factor=10
      }
   } // end if < "option value" nonzero >
  return n;
} // end Menu_ScaleItemValue()

//---------------------------------------------------------------------------
int Menu_DrawLineWithItem(app_menu_t *pMenu, int y, int iLineNr)
{
  int x=0,i,n;
  int text_height_pixels = 2 * LCD_FONT_HEIGHT;
  int item_index = iLineNr + pMenu->vert_scroll_pos; 
  int font_nr    = LCD_OPT_FONT_12x24;
  int sel_flags  = SEL_FLAG_NONE;
  BOOL editing   = FALSE;
  uint16_t fg_color, bg_color;
  char c, *cp, *pszText, sz40Temp[44];
  int wanted_length = LCD_SCREEN_WIDTH / ( 2*LCD_FONT_WIDTH );
  int num_base, fixed_digits;
  menu_item_t *pItem;

  if( item_index==pMenu->item_index )
   { if( pMenu->edit_mode != APPMENU_EDIT_OFF )
      { editing = TRUE;
      }
     else
      { sel_flags |= SEL_FLAG_FOCUSED;
      }
   }
  Menu_GetColours( sel_flags, &fg_color, &bg_color );
  if( pMenu->pItems != NULL ) // does this "menu" have items at all ?
   { 
     if( item_index>=0 && item_index<pMenu->num_items ) 
      { pItem = &((menu_item_t *)pMenu->pItems)[item_index];
        if( pItem->pszText != NULL )
         { pszText = (char*)pItem->pszText; // cast to defeat 'const' warning
           pszText = Menu_GetParamsFromItemText( pszText, &num_base, &fixed_digits, &cp );
           if( cp != NULL )  // this menu item has a hotkey, so draw it ...
            { y = Menu_DrawSeparatorWithHotkey( pMenu, y, cp/*Hotkey*/ );
            }
         }
        else // menu item without a leading text !
         { pszText = "";
         }
        // Convert the optional 'value' into a string,
        //  to find the length (in characters) for right-aligned output:
        if( editing )
         { n = pMenu->iEditValue;
         }
        else
         { n = Menu_ReadIntFromPtr( pItem->pvValue, pItem->data_type );
           n = Menu_ScaleItemValue( pItem, n );
         }
        Menu_ItemValueToString( pItem, n, sz40Temp );
        n = strlen( sz40Temp ); // number of characters occupied by the "value" 

        // If there's leading text, emit it (left-aligned) and find out the length:
        i = 0;    
        while( ((c=*pszText)!=0) && (i<wanted_length) )
         { x = LCD_DrawCharAt( c, x, y, fg_color, bg_color, font_nr );
           ++i; // character index and limiting counter
           ++pszText; 
           // (in rare cases, some of the leading text may be OVERWRITTEN below)
         }

        // If the remaining space isn't wide enough for the VALUE,
        //     use a narrower font for the rest of the line.
        // At this point, i = number of characters for the leading text (already printed)
        //                n = number of characters required for the value (not printed yet).
        if( (i+n) >= wanted_length )
         { font_nr &= ~LCD_OPT_DOUBLE_WIDTH; // switch from 12(?) to 6(?) pixel wide characters
         }
 
        // Fill the gap between LEFT-aligned text and RIGHT-aligned value:
        n = LCD_SCREEN_WIDTH - 3 - LCD_GetTextWidth( font_nr, sz40Temp );
                    //         |___ spare because the rightmost ~~3 pixels
                    //              are obstructed by the plastic enclosure
        LimitInteger( &n, 0, LCD_SCREEN_WIDTH-8 ); 
        if( n >= x )  // if there's no gap, don't try to fill it...
         { LCD_FillRect( x,y, n-1/*x2*/, y+text_height_pixels-1/*y2*/, bg_color );
         }
        x = n; // graphic position for drawing the right-aligned 'value'

        // Draw the 'value', char by char, using different colours 
        //  to mark the edit cursor or the selection bar :
        cp = sz40Temp;
        if( editing ) // THIS item is currently being edited ..
         { // save the length for HORIZONTAL cursor control:
           pMenu->edit_length = strlen( sz40Temp );
         }
        i  = 0;
        while( ((c=*cp++)!=0) && (x<(LCD_SCREEN_WIDTH-6) ) )
         { sel_flags &= ~SEL_FLAG_CURSOR;
           if( editing ) // THIS item is currently being edited ..
            { // so mark only the current edit cursor position ?
              if( pMenu->edit_mode == APPMENU_EDIT_INC_DEC )
               { sel_flags |= SEL_FLAG_CURSOR; // increment/decrement applies to the entire field
               }
              else if( i == pMenu->cursor_pos )
               { sel_flags |= SEL_FLAG_CURSOR;
               }
              else
               { sel_flags &= ~SEL_FLAG_CURSOR;
               }
              Menu_GetColours( sel_flags, &fg_color, &bg_color );
            } // end if < "editing", not just "navigating" >
           x = LCD_DrawCharAt( c, x, y, fg_color, bg_color, font_nr );
           ++i; // character index and limiting counter
         } // end while < more characters to print > 
      }
   } // end if < items exist >

  // If the graphic output cursor (x) didn't reach the end,
  // clear up to the end of the current text line:
  Menu_GetColours( sel_flags & ~SEL_FLAG_CURSOR, &fg_color, &bg_color );
  LCD_FillRect( x,y, LCD_SCREEN_WIDTH-1/*x2*/, 
                 y+text_height_pixels-1/*y2*/, bg_color );

  y += text_height_pixels; // -> pixel coord for the NEXT item.
  // The result may be HIGHER than LCD_SCREEN_HEIGHT !
  // If it is, AND this is the currently focused item, 
  // adjust the vertical scrolling position and re-draw everything:
  if( (y>LCD_SCREEN_HEIGHT) && (item_index == pMenu->item_index) )
   { // The 'guesstimate' in Menu_CheckVertScrollPos() was wrong,
     // so scroll further towards the end of the list and redraw all (later):
     ++pMenu->vert_scroll_pos;
     pMenu->redraw = TRUE;
   }

  return y;  // pixel coord for the NEXT line 

} // end Menu_DrawLineWithItem()


//---------------------------------------------------------------------------
int Menu_DrawSeparatorWithHotkey(app_menu_t *pMenu, int y, char *cpHotkey )
{
  int x = 0;
  uint16_t fg_color, bg_color;

  Menu_GetColours( SEL_FLAG_NONE, &fg_color, &bg_color );
  x = LCD_DrawCharAt( ' ', x, y, bg_color/*!*/, fg_color/*!*/, LCD_OPT_FONT_8x8 );
  while( !Menu_IsFormatStringDelimiter( *cpHotkey ) )
   { x = LCD_DrawCharAt( *cpHotkey++, x, y, bg_color/*!*/, fg_color/*!*/, LCD_OPT_FONT_8x8 );
   }
  x = LCD_DrawCharAt( 0x10, x, y, fg_color/*!*/, bg_color/*!*/, LCD_OPT_FONT_8x8 );

  // Fill the rest of the line with CP437's 'horizontal line character' .
  while( x<(LCD_SCREEN_WIDTH-1) )
   { x = LCD_DrawCharAt( 0xC4, x, y, fg_color, bg_color, LCD_OPT_FONT_8x8 );
   }

  // return the pixel coordinate for the NEXT painted menu line (item):
  return y + 8; 

} // end Menu_DrawSeparatorWithHotkey()


//---------------------------------------------------------------------------
int Menu_DrawIfVisible(int caller)
  // When visible(!), paints the 'app menu' into the framebuffer. 
  //      Called from various 'display-update' hook functions .
  // [in] caller : tells which of the half dozen of hooked functions
  //               calls us .  Mostly used for debugging .
  // Return value:  1 when visible (and the framebuffer was filled),
  //                0 when invisible (and someone else should 'draw').
{
  app_menu_t *pMenu = &AppMenu; // load the struct's address only ONCE
  char c;
  int y,iTextLineNr;
  uint16_t fg_color, bg_color;

  if( pMenu->visible==APPMENU_VISIBLE_UNTIL_KEY_RELEASED )
   { // Menu "almost closed" but still visible until releasing the last key.
     // This is a kludge to prevent opening Tytera's "green key menu"
     // on the same keystroke that was used here to CLOSE the "red key menu".
     if( kb_row_col_pressed == 0 )
      { 
        LCD_FillRect( 0,0, LCD_SCREEN_WIDTH-1, LCD_SCREEN_HEIGHT-1, LCD_COLOUR_MD380_BKGND_BLUE ); 
        pMenu->visible = APPMENU_OFF; // now "really closed",
        // and Menu_IsVisible() returns FALSE for half a dozen of hooks 
        // (allows the original firmware to use the framebuffer again)
        channel_num = 0; // kludge explained in Menu_Close() to update idle screen
      }
     return pMenu->visible != APPMENU_OFF;
   }

  // Simple keyboard processing.. only one "event" per call:
  c = am_key;
  if( c ) 
   { am_key=0; // remove key from this buffer
#   if( CONFIG_MORSE_OUTPUT )
     // Restart the "stopwatch" for delayed Morse output, for example after modifying a value.
     // Output in Morse code can only start if this stopwatch expires. Avoids excessive chatter.
     StartStopwatch( &pMenu->morse_stopwatch );
#   endif 
     // Reload Tytera's "backlight_timer" here, because their own control doesn't work now:
     backlight_timer = md380_radio_config.backlight_time * 500; 
     if( pMenu->visible == APPMENU_USERSCREEN_VISIBLE )
      { // screen and keyboard occupied by a 'user screen' 
        // -> try to pass on keyboard events to whatever-it-is : 
        y = Menu_InvokeCallback( pMenu, Menu_GetFocusedItem(pMenu), APPMENU_EVT_KEY, c );
        if( y != AM_RESULT_OCCUPY_SCREEN )
         { // framebuffer no longer occupied by the callback, so back to the 'app menu':
           pMenu->visible = APPMENU_VISIBLE;
           pMenu->redraw  = TRUE; // ... redraw menu IN THE NEXT CALL
         }
      }
     else
     if( (pMenu->visible==APPMENU_VISIBLE) || (c=='B') )
      { switch(c) // using ASCII characters for simplicity
         { case 'M' :  // green "Menu" key : kind of ENTER
              green_led_timer = 20;   // <- poor man's debugging
              Menu_OnEnterKey(pMenu);
              break;
           case 'B' :  // red "Back"-key : 
              red_led_timer  = 20;    // <- poor man's debugging
              if( pMenu->visible == APPMENU_OFF ) // not visible yet..
               { Menu_Open( pMenu );
               }
              else // already in the app menu: treat the RED KEY like "BACK",
               {   // "Exit", "Escape", or "Delete" ?
                 Menu_OnExitKey(pMenu);
               }
              break;
           case 'U' :  // cursor UP : navigate towards the FIRST item ...
              switch( pMenu->edit_mode ) // ... or increment value when editing
               {  case APPMENU_EDIT_INC_DEC:
                     Menu_OnIncDecEdit( pMenu, +1 ); 
                     break;
                  case APPMENU_EDIT_OVERWRT:
                  case APPMENU_EDIT_INSERT :
                     // Similar as in Tytera firmware: cursor "UP" key moves edit cursor LEFT
                     if( pMenu->cursor_pos > 0 )
                      { --pMenu->cursor_pos;
                      }
                     break;
                  default: // not "editing" but "navigating" ... 
                     if( pMenu->item_index > 0 ) 
                      { --pMenu->item_index; 
                      }
               }
              pMenu->redraw = TRUE;
              // ex: green_led_timer = 20; // <- poor man's debugging
              break;
           case 'D' :  // cursor DOWN: navigate towards the LAST item
              switch( pMenu->edit_mode ) // ... or increment value when editing
               {  case APPMENU_EDIT_INC_DEC:
                     Menu_OnIncDecEdit( pMenu, -1 ); 
                     break;
                  case APPMENU_EDIT_OVERWRT:
                  case APPMENU_EDIT_INSERT :
                     // Similar as in Tytera firmware: cursor "DOWN" key moves edit cursor RIGHT
                     if( pMenu->cursor_pos < (pMenu->edit_length-1) )
                      { ++pMenu->cursor_pos;
                      }
                     break;
                  default: // not "editing" but "navigating" ... 
                     ++pMenu->item_index;
                     if( pMenu->item_index >= pMenu->num_items )  
                      { pMenu->item_index = pMenu->num_items-1;
                      }
                     break;
               }
              pMenu->redraw = TRUE;
              // ex: green_led_timer = 20; // <- poor man's debugging
              break;
           default:  // Other keys .. editing or treat as a hotkey ?
              if( pMenu->edit_mode != APPMENU_EDIT_OFF )
               {
               }
              else
               { Menu_ProcessHotkey(pMenu, c);
               }
              break;
         } // end switch < key >
        if( pMenu->visible )
         {  pMenu->redraw = TRUE;
         }
      } // end if < "red key" pressed or "red-key-menu" already visible > ?
   } // end if < keyboard-"event" for the App Menu > ? 


  // After the keyboard processing: Draw the 'App Menu' into the framebuffer ?
  if( pMenu->visible == APPMENU_VISIBLE ) 
   { // got here approx 9 times per second...
     // To keep the QRM radiated from the LCD cable low, only redraw the screen
     // if necessary. But there may be items in the menu with 'dynamic' values,
     // which require redrawing the screen even without a keypress. Check for those:
     Menu_CheckDynamicValues( pMenu ); 
     if( pMenu->redraw )
      { // LED_GREEN_ON; // for speed test (connect a scope to the green LED)
        pMenu->redraw = FALSE; // menu screen should be up-to-date after the following,
        // unless the currently focused item isn't completely visible, which
        // would cause Menu_DrawLineWithItem() to modify pMenu->vert_scroll_pos
        // and set pMenu->redraw = TRUE again. Ugly, but keeps things simple.
        Menu_CheckVertScrollPos(pMenu);
        y=iTextLineNr=0;
        while( y < (LCD_SCREEN_HEIGHT-LCD_FONT_HEIGHT) )
         { y = Menu_DrawLineWithItem( pMenu, y, iTextLineNr++ );
         }
        // If the graphic output cursor (y) didn't reach the end,
        // clear up to the bottom of the screen:
        Menu_GetColours( SEL_FLAG_NONE, &fg_color, &bg_color );
        LCD_FillRect( 0, y, LCD_SCREEN_WIDTH-1, LCD_SCREEN_HEIGHT-1, bg_color );
        // LED_GREEN_OFF; // <- end of the speed test with o'scope and photodiode
      } // end if( pMenu->redraw ) 
   } // end if < "app menu" visible >
  else if( pMenu->visible == APPMENU_USERSCREEN_VISIBLE ) // some 'user screen' visible ?
   { y = Menu_InvokeCallback( pMenu, Menu_GetFocusedItem(pMenu), APPMENU_EVT_PAINT, 0 );
     if( y != AM_RESULT_OCCUPY_SCREEN )
      { // framebuffer no longer occupied by the callback, so back to the 'app menu':
        pMenu->visible = APPMENU_VISIBLE;
        pMenu->redraw  = TRUE; // ... redraw menu IN THE NEXT CALL
      }
   }

# if( CONFIG_MORSE_OUTPUT )
  // If a request to report the edited value in Morse code, AND some time has passed
  // since the last modification (via cursor up/down), start sending the value:
  if( global_addl_config.narrator_mode & NARRATOR_MODE_ENABLED )
   { if( ReadStopwatch_ms( &pMenu->morse_stopwatch ) > 500 )
      { // ok, no further keyboard events for some time, so start sending NOW:
        if( pMenu->morse_request != 0 )
         {  Menu_ReportItemInMorseCode( pMenu->morse_request );
            pMenu->morse_request = 0; // "done" (Morse output started)
         }
      }
   }
# endif 


  return pMenu->visible != APPMENU_OFF;

} // end Menu_DrawIfVisible()


//---------------------------------------------------------------------------
menu_item_t *Menu_GetFocusedItem(app_menu_t *pMenu)
  // Returns a pointer to the currently focused item
  //   (with the default colour, "the line with blue background").
  // Caller beware: the result may be NULL !
{
  if( pMenu->pItems!=NULL && (pMenu->item_index < pMenu->num_items) )
   { return &((menu_item_t*)pMenu->pItems)[pMenu->item_index];
   }
  return NULL;
} // end Menu_GetFocusedItem()

//---------------------------------------------------------------------------
void Menu_PushSubmenuToStack(app_menu_t *pMenu)
  // Called shortly before entering a SUB-menu.
  // All we need to remember about the CURRENT menu (and item)
  // are pushed to a tiny stack-like LIFO .
{
  int sp = pMenu->depth; 
  if( sp < APPMENU_STACKSIZE )
   {  submenu_stack[sp].pItems = (menu_item_t*)pMenu->pItems;
      submenu_stack[sp].item_index= pMenu->item_index;
      submenu_stack[sp].vert_scroll_pos= pMenu->vert_scroll_pos;
      pMenu->depth = (uint8_t)(sp+1);
   }
} 

//---------------------------------------------------------------------------
BOOL Menu_PopSubmenuFromStack(app_menu_t *pMenu)
  // "returns" from a submenu to its parent. Details in Menu_PushSubmenuToStack().
  // Return value: TRUE = "ok, returned into a parent menu" .
  //               FALSE= "empty stack, there's no parent-MENU to return to".
{
  int sp = (int)pMenu->depth-1; 
  if( sp >= 0 )
   {  pMenu->pItems = (void*)submenu_stack[sp].pItems;
      pMenu->num_items = Menu_GetNumItems(pMenu->pItems);
      pMenu->item_index = submenu_stack[sp].item_index;
      pMenu->vert_scroll_pos = submenu_stack[sp].vert_scroll_pos;
      pMenu->depth  = (uint8_t)sp;
      pMenu->redraw = TRUE;
      return TRUE;
   }
  return FALSE;  // caller decides what to do now
}

//---------------------------------------------------------------------------
BOOL Menu_IsOnStack(app_menu_t *pMenu, menu_item_t *pItems)
{ int sp;
  for(sp=0; sp<pMenu->depth; ++sp)
   { if( submenu_stack[sp].pItems==pItems )
      { return TRUE;
      } 
   }
  return FALSE;
}

//---------------------------------------------------------------------------
int Menu_InvokeCallback(app_menu_t *pMenu, menu_item_t *pItem, int event, int param)
{
  if( pItem ) 
   { if( pItem->callback )
      { return pItem->callback( pMenu, pItem, event, param );
      }
   }
  return AM_RESULT_NONE; // "proceed as if there was NO callback function"
                            // (because, in this case, there IS NO callback..)
} // end Menu_InvokeCallback()

//---------------------------------------------------------------------------
BOOL Menu_EnterSubmenu(app_menu_t *pMenu, menu_item_t *pItems )
{ 
  if( pItems )
   { if( !Menu_IsOnStack(pMenu,pItems) ) // don't push the same item twice !
      { Menu_PushSubmenuToStack(pMenu);
        pMenu->pItems = (void*)pItems;
        pMenu->num_items = Menu_GetNumItems( pItems );
        pMenu->vert_scroll_pos = pMenu->item_index = 0;
        pMenu->visible = pMenu->redraw = TRUE;
        return TRUE;
      }
   }
  return FALSE;
}


//---------------------------------------------------------------------------
void Menu_OnEnterKey(app_menu_t *pMenu)
  // Called when the 'App Menu' is already opened, 
  //  when pressing the equivalent of an ENTER-key.
{
  int cbk_result;
  menu_item_t *pItem = Menu_GetFocusedItem(pMenu);
  if( pItem ) 
   { // If the currently focused item has a callback function,
     // invoke that function to let the callback do whatever it wants:
     cbk_result = Menu_InvokeCallback( pMenu, pItem, APPMENU_EVT_ENTER, 0 );
     switch( cbk_result )
      { case AM_RESULT_ERROR: // callback disagrees to 'enter' this item !
           return;
        case AM_RESULT_OCCUPY_SCREEN: // the callback has 'occupied' the screen,
           // and doesn't want anyone to paint to the framebuffer 
           // until it agrees to give the screen back:
           pMenu->visible = APPMENU_USERSCREEN_VISIBLE;
           break;
      } 
 
     // Open a submenu (without the need for a callback) ? 
     if( (pItem->data_type==DTYPE_SUBMENU) &&(pItem->pvValue != NULL) )
      { Menu_EnterSubmenu( pMenu, (menu_item_t*)pItem->pvValue );
      } // end if < entry to a SUB-MENU >
     else // Return to the parent menu, or to the radio's "main screen" ?
     if( pItem->options == APPMENU_OPT_BACK )
      { if( ! Menu_PopSubmenuFromStack(pMenu) ) 
         { // Cannot return from a SUB-MENU so return to the MAIN SCREEN:
           Menu_Close(pMenu);
         } 
      }
     else // begin or end editing ?
     if( (pItem->options & APPMENU_OPT_EDITABLE ) && (pItem->pvValue!=NULL) )
      { if( pMenu->edit_mode != APPMENU_EDIT_OFF) 
         {  // was editing -> finish input ("write back")
            Menu_FinishEditing( pMenu, pItem );
         }
        else // begin editing: copy current value to edit buffer (string or int)
         {  Menu_BeginEditing( pMenu, pItem );
         }
      } // end if < editable > ?
   }
} // end Menu_OnEnterKey()

//---------------------------------------------------------------------------
void Menu_BeginEditing( app_menu_t *pMenu, menu_item_t *pItem )
{ int cbk_result;
  if( (pItem->options & APPMENU_OPT_EDITABLE ) && (pItem->pvValue!=NULL) )
   { // Copy the 'current' value into the internal storage,
     //  for both integer and string types :
     pMenu->iEditValue= pMenu->iValueBeforeEditing 
       = Menu_ScaleItemValue(pItem,Menu_ReadIntFromPtr(pItem->pvValue,pItem->data_type));
     pMenu->edit_mode = APPMENU_EDIT_INC_DEC; // suggest to use simple "increment/decrement"-editing via CURSOR KEYS (!)
        // setting edit_mode also "disconnects" the displayed value from pItem->pvValue
     Menu_ItemValueToString( pItem, pMenu->iEditValue, pMenu->sz40EditBuf );
     pMenu->iMinValue = pItem->iMinValue; // min/max-range copied from menu_item_t...
     pMenu->iMaxValue = pItem->iMaxValue; // the callback below may override these !
     if( pMenu->iMinValue==0 && pMenu->iMaxValue==0 ) // if min/max-range not specified..
      { // ..use suitable limits depending on the data type:
        Menu_GetMinMaxForDataType( pItem->data_type, &pMenu->iMinValue, &pMenu->iMaxValue );
      } // end if < no min/max-range specified for editing >
     // Allow the callback function to modify the above "editing parameters" :
     cbk_result = Menu_InvokeCallback( pMenu, pItem, APPMENU_EVT_BEGIN_EDIT, 0 );
     // As usual, the callback function may DISAGREE with editing, so :
     switch( cbk_result )
      { case AM_RESULT_ERROR: // callback disagrees to 'edit' from this item !
           return;
        case AM_RESULT_OCCUPY_SCREEN: // callback wants to 'edit' this on its own screen,
           // so stop painting into the framebuffer until the callback "gives it back":
           pMenu->visible = APPMENU_USERSCREEN_VISIBLE;
           return;
        default:
           break;
      } 
   }
  else 
   { pMenu->edit_mode = APPMENU_EDIT_OFF;  // cannot edit this !
   } 
} // end Menu_BeginEditing()

//---------------------------------------------------------------------------
void Menu_WriteBackEditedValue( app_menu_t *pMenu, menu_item_t *pItem )
{ int i,n,old;
  // Since callbacks are not required for normal configuation parameters,
  // scale (inversely) and write back the current edit value via pointer .
  n = pMenu->iEditValue;  // to avoid confusion: this is the DISPLAY value,
                          // not the 'raw, unscaled' value !
  i = pItem->opt_value;   // this 'option value' serves multiple purposes..
  if( i != 0 ) // .. but only if nonzero (to avoid nonsense or endless loops):
   {
     if( pItem->options & APPMENU_OPT_FACTOR )
      { n /= i; 
        // Example: CW pitch (Morse tone frequency), scale 650 [Hz] back
        // to 65, which fits in a single byte (global_addl_config.cw_pitch_10Hz).
      }
     if( pItem->options & (APPMENU_OPT_BITMASK | APPMENU_OPT_BITMASK_R ) )
      { // Example: The "higher" backlight intensity, used when ACTIVE,
        // is stored in the upper 4 bits of a byte . Thus the BITMASK is 0xF0,
        // but the EDITED VALUE ranges from 1 to 9 only 
        // (OPT_BITMASK_R means not only bitwise AND, but shift RIGHT for the display).
        // So convert BACK by bitwise shifting LEFT here, until the LSBit is in bit 0:
        if( pItem->options & APPMENU_OPT_BITMASK_R )
         { while( !(i & 1) ) // back from "right-aligned" display value to original..
            { i >>= 1;
              n <<= 1;
            } // after this loop, the least significant bit is in bit 0 (mask 1)
         }
        n &= pItem->opt_value; // all 'foreign' bits in n are zero now...
        // The byte/word/dword/int that pvValue points to may contain bits
        // that must NOT be modified here, so get those bits, and bitwise OR them:
        old = Menu_ReadIntFromPtr( pItem->pvValue, pItem->data_type );
        n |= (old & ~pItem->opt_value); // leave all bits that are NOT set in opt_value unchanged
        // Phew. Enough of this bit-fiddling :)
      }
   } // end if < "option value" nonzero >
  // Write back the inversely scaled 'edit value' :
  Menu_WriteIntToPtr( n, pItem->pvValue, pItem->data_type );

  // If there's a callback function, politely inform it that we're not editing anymore:
  Menu_InvokeCallback( pMenu, pItem, APPMENU_EVT_END_EDIT, 1/*finish, not abort*/ );

} // end Menu_WriteBackEditedValue() 

//---------------------------------------------------------------------------
void Menu_OnIncDecEdit( app_menu_t *pMenu, int delta )
{
  int     i; 
  int64_t i64;
  menu_item_t *pItem = Menu_GetFocusedItem(pMenu);
  // If the edited value is NUMERIC, really increment/decrement THE VALUE
  // (not characters in a string). This way, the min/max limits can be 
  // easily checked while editing, and depending on the numeric based,
  // "editing" is in facting adding or subtracting a power of two, ten, or sixteen.
  if(pItem!=NULL) 
   { if(pItem->options & APPMENU_OPT_FACTOR)
      { if( pItem->opt_value != 0 )
         { delta *= pItem->opt_value; 
           // Example: The CW pitch is stored in a BYTE, 65 means 650 Hz, factor=10,
           // so the smallest meaningful stepwidth for increment/decrement is 10 .
         }
      }
   }

  // To avoid overflow of iEditValue (integer!!) from 0x7FFFFFFF to 0x80000000,
  // use a costly 64-bit calculation internally:
  i64 = (int64_t)pMenu->iEditValue + (int64_t)delta;
  if(i64 < pMenu->iMinValue )
   { i64 = pMenu->iMinValue; 
   }
  if(i64 > pMenu->iMaxValue )
   { i64 = pMenu->iMaxValue; 
   }
  i = (int)i64;
#if( CONFIG_MORSE_OUTPUT )
  if( pMenu->iEditValue != i ) // value was modified..
   { // .. report this in Morse code (the VALUE only, not the entire line) ?
     pMenu->morse_request |= AMENU_MORSE_REQUEST_ITEM_VALUE;
   }
#endif // CONFIG_MORSE_OUTPUT ?
  pMenu->iEditValue = i;
  
  if( pItem != NULL ) // "write back" immediately ?
   { if( pItem->options & APPMENU_OPT_IMM_UPDATE )
      { Menu_WriteBackEditedValue(pMenu, pItem);
      } 
     // If the currently edited item has a callback function,
     // invoke that function to let the callback do whatever it wants
     // *AFTER* the edited value was modified, e.g.: 
     //  - apply the edited colour for the display immediately,
     //  - use the new CW pitch (tone frequency) immediately, etc.
     Menu_InvokeCallback( pMenu, pItem, APPMENU_EVT_BEGIN_EDIT, 0 );
   }
} // end Menu_OnIncDecEdit()


//---------------------------------------------------------------------------
void Menu_FinishEditing( app_menu_t *pMenu, menu_item_t *pItem )
{
  if( pItem != NULL )
   { Menu_WriteBackEditedValue(pMenu, pItem);
   }
  pMenu->edit_mode = APPMENU_EDIT_OFF;  // not editing anymore
  pMenu->redraw = TRUE;      // for simplicity, redraw everything (let edit-cursor disappear)
} // end Menu_FinishEditing()

//---------------------------------------------------------------------------
void Menu_AbortEditing( app_menu_t *pMenu, menu_item_t *pItem )
{ 
  if( pMenu->edit_mode )
   {  
      if( pMenu->iEditValue != pMenu->iValueBeforeEditing )
       { // The value has been edited.. need to "undo" this ?
         if( pItem->options & APPMENU_OPT_IMM_UPDATE ) // yes..  
          { pMenu->iEditValue = pMenu->iValueBeforeEditing;
            Menu_WriteBackEditedValue( pMenu, pItem ); // "undo" editing,
            // after the 'immediately updated' value was already active
            // but the operator didn't like it (e.g. CW speed or pitch,
            // or the display is unreadable now because colour or brightness
            // has been screwed up).
          }
       }
      Menu_InvokeCallback( pMenu, pItem, APPMENU_EVT_END_EDIT, 0/* abort, not "finish" */ );
      pMenu->redraw = TRUE;
   }
  pMenu->edit_mode = APPMENU_EDIT_OFF;
} // end Menu_AbortEditing()

//---------------------------------------------------------------------------
void Menu_OnExitKey(app_menu_t *pMenu)
  // Called when the 'App Menu' is already opened, 
  //  when pressing the equivalent of an EXIT-, ESCAPE- or DELETE-key.
  //  (here, that key is actually the red "back" key..)
{
  int cbk_result;
  menu_item_t *pItem = Menu_GetFocusedItem(pMenu);
  if( pItem ) 
   { // If the currently focused item has a callback function,
     // invoke that function to let the callback do whatever it wants:
     cbk_result = Menu_InvokeCallback( pMenu, pItem, APPMENU_EVT_EXIT, 0 );
     switch( cbk_result )
      { case AM_RESULT_ERROR: // callback disagrees to 'exit' from this item !?
           return;
        case AM_RESULT_OCCUPY_SCREEN: // the callback has 'occupied' the screen,
           // and doesn't want anyone to paint to the framebuffer 
           // until it agrees to give the screen back:
           pMenu->visible = APPMENU_USERSCREEN_VISIBLE;
           return;
        default:
           break;
      } 
   }
  // Arrived here: The optional callback function didn't intercept "exiting".
  // Exit FROM WHAT (abort editing or return from submenu) ?
  if( pMenu->edit_mode != APPMENU_EDIT_OFF ) // abort editing (don't apply the edited value)
   { Menu_AbortEditing( pMenu, pItem );
   }
  else
  if( ! Menu_PopSubmenuFromStack(pMenu) ) 
   { // Cannot return from a SUB-MENU so return to the MAIN SCREEN:
     Menu_Close(pMenu);
   } 

} // end Menu_OnExitKey()

//---------------------------------------------------------------------------
BOOL Menu_ProcessHotkey(app_menu_t *pMenu, char c)
{ menu_item_t *pItem = (menu_item_t*)pMenu->pItems;
  int item_index = 0;
  while( pItem->pszText != NULL )
   { if( (pItem->pszText[0]=='[') && (pItem->pszText[1]==c)  )
      { // Bingo, found an item with a matching hotkey (digit?) :
        pMenu->item_index = item_index;
        // let the item with the hotkey appear at the top:
        pMenu->vert_scroll_pos = item_index; 
        pMenu->redraw = TRUE;
        return TRUE;
      }
     ++pItem;
     ++item_index;
   }
  return FALSE;

} // end Menu_ProcessHotkey()

//---------------------------------------------------------------------------
void Menu_LimitEditedValue( app_menu_t *pMenu )
{
}


//---------------------------------------------------------------------------
int am_cbk_ColorTest(app_menu_t *pMenu, menu_item_t *pItem, int event, int param )
{
  if( event==APPMENU_EVT_ENTER ) // pressed ENTER (to launch the colour test) ?
   { LCD_ColourGradientTest(); // only draw the colour test pattern ONCE...
     return AM_RESULT_OCCUPY_SCREEN; // screen now 'occupied' by the colour test screen
   }
  else if( event==APPMENU_EVT_PAINT )
   { if( am_key==0 )  // wait for 'any key' ...
      { return AM_RESULT_OCCUPY_SCREEN; // no key pressed -> screen still occupied
      }
   }
  return AM_RESULT_NONE; // "proceed as if there was NO callback function"
} // end am_cbk_ColorTest()

//---------------------------------------------------------------------------
int am_cbk_Backlt(app_menu_t *pMenu, menu_item_t *pItem, int event, int param )
{
  return AM_RESULT_NONE; // "proceed as if there was NO callback function"
} // end am_cbk_Backlt()

//---------------------------------------------------------------------------
int am_cbk_Morse(app_menu_t *pMenu, menu_item_t *pItem, int event, int param )
{
  return AM_RESULT_NONE; // "proceed as if there was NO callback function"
} // end am_cbk_Morse()

#if( CONFIG_MORSE_OUTPUT )
//---------------------------------------------------------------------------
int  Menu_GetItemIndex(void)
{ // used by the Morse narrator (narrator.c) to detect "changes"
  app_menu_t *pMenu = &AppMenu; // load the struct's address only ONCE
  if( pMenu->visible==APPMENU_VISIBLE )
   { return pMenu->item_index;
   }
  else
   { return -1;
   }
} 

//---------------------------------------------------------------------------
void Menu_ReportItemInMorseCode( 
        int morse_request ) // [in] bitwise combination of AM_MORSE_REQUEST_.. 
{ // Called from the Morse narrator when the Morse-transmit-buffer is empty,
  // and some time passed since the last modification of the current menu item.
  // (a few shorter messages may be sent directly from the menu event handlers,
  //  for example after modifying a value with some delay, etc etc)
  app_menu_t *pMenu = &AppMenu; // load the struct's address only ONCE
  menu_item_t *pItem;
  int  n;
  char *cp, sz40[44];
  if( pMenu->visible==APPMENU_VISIBLE )
   { pItem = Menu_GetFocusedItem(pMenu);
     if( pItem != NULL )
      { if( (pItem->pszText != NULL ) // items WITHOUT a fixed text are rare but possible!
         && (morse_request & AMENU_MORSE_REQUEST_ITEM_TEXT) )
         { // but they do occurr.. for example it the "menu" is just a list of names .
           // skip the "output options" at the begin of the fixed item text:
           cp = Menu_GetParamsFromItemText( (char*)pItem->pszText, NULL, NULL, NULL );
           MorseGen_AppendChar( '\x09' ); // decrease pitch by approx one whole tone
           MorseGen_AppendString( cp );
           MorseGen_AppendChar( '\x10' ); // SPACE + back to the normal CW pitch
         }
        // Convert the optional 'value' into a string, here for Morse output:
        if( (pItem->pvValue != NULL) 
         && (pItem->data_type != DTYPE_SUBMENU) 
         && (morse_request & AMENU_MORSE_REQUEST_ITEM_VALUE) )
         { if( pMenu->edit_mode != APPMENU_EDIT_OFF )
            { n = pMenu->iEditValue;
            }
           else
            { n = Menu_ReadIntFromPtr( pItem->pvValue, pItem->data_type );
              n = Menu_ScaleItemValue( pItem, n );
            }
           Menu_ItemValueToString( pItem, n, sz40 );
           MorseGen_AppendString( sz40 );
         }
      }
   }
} // end Menu_ReportItemInMorseCode()
#endif  // CONFIG_MORSE_OUTPUT ?

#endif // CONFIG_APP_MENU ?
