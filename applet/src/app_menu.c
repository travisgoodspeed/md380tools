// File:    md380tools/applet/src/app_menu.c
// Author:  Wolf (DL4YHF) [initial version] 
//          Please don't poison this soucecode with TAB characters . 
//
// Date:    2017-05-12
//  Implements a simple menu opened with the red BACK button,
//             which doesn't rely on ANY of Tytera's firmware
//             functions at all (neither "gfx" nor "menu").
//  Module prefix 'am_' for "Application Menu" .
//  Initially written for the Morse output for visually impaired hams,
//  because strings in 'our own' menu can be sent out much easier
//  than with the original menu by Tytera (opened via green 'MENU' button).
//  Later adapted to invoke some other screens (Netmon, etc),
//  and to show temporary (timed) messages without the original gfx functions.
// 
// To include these functions in the patched firmware:
//    
//  1. Add the following lines in applet/Makefile (after SRCS += narrator.o) :
//      SRCS += app_menu.o 
//      SRCS += amenu_utils.o
//      SRCS += amenu_set_tg.o
//      SRCS += amenu_codeplug.o
//      SRCS += color_picker.o
//      SRCS += lcd_driver.o
//      SRCS += font_8_8.o
//  
//  2. #define CONFIG_APP_MENU 1  in  md380tools/applet/config.h  .

#include "config.h"

#if (CONFIG_APP_MENU) // <- this condition ends near end of file.. compile as dummy when not opted-in in config.h


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
#include "addl_config.h"  // customizeable colours stored in global_addl_config
#include "radio_config.h"
#include "syslog.h"
#include "usersdb.h"
#include "keyb.h"
#include "display.h"      // old display functions (using the original firmware)
#include "netmon.h"       // is_netmon_visible(), etc
#include "unclear.h"      // radio_status_1, etc (displayed here to find out what they do)
#include "codeplug.h"     // zone_name[], etc (mostly unknown for old firmware)
#include "console.h"      // text screen buffer for Netmon (may be displayed through the 'app menu', too)
#include "irq_handlers.h" // backlight PWM, timers, Morse generator
#include "narrator.h"     // announces channel, zone, and maybe current menu selection in Morse code
#include "lcd_driver.h"   // alternative LCD driver (DL4YHF), doesn't depend on 'gfx'
#include "app_menu.h"     // 'simple' alternative menu activated by red BACK-button
#include "amenu_codeplug.h" // codeplug-related displays, e.g. zone list, etc
#include "amenu_set_tg.h" // helper to set a new talkgroup ad-hoc (and keep it!) 
#include "amenu_hexmon.h" // hex-monitor to watch RAM-, internal Flash-, and SPI-flash contents 

#if( ! CONFIG_MORSE_OUTPUT )
#  error "No 'app menu' without Morse output !" 
#endif 

// Variables used by the 'app menu' :

static uint8_t  am_key;              // one-level keyboard buffer
static uint8_t  morse_activation_pending = TRUE;
static uint32_t backlight_stopwatch; // SysTick-based stopwatch for the backlight

// For short code, put everything inside a SMALL struct, and reference it
// whereever possible via a pointer in a LOCAL variable:
app_menu_t AppMenu;  // data for a single instance of the 'application menu'

// Before entering a SUB-menu, num_items, item_index, and pItems are stacked here:
# define APPMENU_STACKSIZE 4 // ~~maximum nesting level
struct
{ menu_item_t *pItems;
  uint8_t item_index;
  uint8_t vert_scroll_pos;
} submenu_stack[APPMENU_STACKSIZE];

uint8_t Menu_old_channel_num = 0; // to defeat trouble with 'ad-hoc' TALKGROUPS
        // when setting channel_num=0 to force screen update in Menu_Close()


//---------------------------------------------------------------------------
// Internal 'forward' references
int  Menu_DrawLineWithItem(app_menu_t *pMenu, int y, int iTextLineNr);
int  Menu_DrawSeparatorWithHotkey(app_menu_t *pMenu, int y, char *cpHotkey );
BOOL Menu_ProcessHotkey(app_menu_t *pMenu, char c);
BOOL Menu_ProcessEditKey(app_menu_t *pMenu, char c);
void Menu_UpdateEditValueIfNotEditingYet(app_menu_t *pMenu, menu_item_t *pItem);
void Menu_UpdateEditLengthAndSetCursorPos(app_menu_t *pMenu, int cursor_pos );
void Menu_OnEnterKey(app_menu_t *pMenu);
void Menu_OnExitKey(app_menu_t *pMenu);
void Menu_BeginEditing( app_menu_t *pMenu, menu_item_t *pItem, uint8_t edit_mode );
void Menu_FinishEditing( app_menu_t *pMenu, menu_item_t *pItem );
void Menu_OnIncDecEdit( app_menu_t *pMenu, int delta );
void Menu_PushSubmenuToStack(app_menu_t *pMenu);
BOOL Menu_PopSubmenuFromStack(app_menu_t *pMenu);
menu_item_t *Menu_GetFocusedItem(app_menu_t *pMenu);
int  Menu_InvokeCallback(app_menu_t *pMenu, menu_item_t *pItem, int event, int param);
BOOL Menu_CheckLongKeypressToActivateMorse(app_menu_t *pMenu);


// Callback function prototypes and forward references for a few menu items :
int am_cbk_ColorTest(app_menu_t *pMenu, menu_item_t *pItem, int event, int param );
int am_cbk_NetMon(app_menu_t *pMenu, menu_item_t *pItem, int event, int param );
const menu_item_t am_Setup[]; // referenced from main menu
const am_stringtable_t am_stringtab_opmode2[]; // for gui_opmode2
const am_stringtable_t am_stringtab_255Auto[];
const am_stringtable_t am_stringtab_narrator_modes[];
const am_stringtable_t am_stringtab_color_names[];

//---------------------------------------------------------------------------
// Alternative 'main' menu, opened with the RED 'BACK'-button :

const menu_item_t am_Main[] = 
{ // { "Text__max__13", data_type,  options,opt_value,
  //     pvValue,iMinValue,iMaxValue, string table, callback }
  { "Chnl",             DTYPE_WSTRING, APPMENU_OPT_NONE,0, 
         channel_name,0,0,          NULL,         NULL     },
  { "Zone",             DTYPE_WSTRING, APPMENU_OPT_NONE,0, 
         zone_name,0,0,               NULL, am_cbk_ZoneList},
  { "Cont",             DTYPE_WSTRING, APPMENU_OPT_NONE,0, 
         contact.name,0,0,          NULL,         NULL     },
   // yet to be found out: Relation between 'contact.name', 'tx_id',
   // DMR-"talkgroup", -"reflector", current_channel_info, 
   // and how all this sticks together in the original firmware.
   // See (old) menu.c : create_menu_entry_set_tg_screen_store() .
  { "TkGrp",            DTYPE_INTEGER, APPMENU_OPT_EDITABLE,0, 
         NULL/*pvValue*/,0/*min*/,0x00FFFFFF/*max:24 bit*/, NULL,am_cbk_SetTalkgroup},
  { "[1]Test/Setup",       DTYPE_SUBMENU, APPMENU_OPT_NONE,0, 
   // |__ hotkey to get here quickly (press RED BUTTON followed by this key)
     (void*)am_Setup,0,0,           NULL,         NULL     },
  { "Netmon",           DTYPE_NONE, APPMENU_OPT_NONE,0, 
         NULL,0,0,                  NULL,     am_cbk_NetMon},
  { "Exit",             DTYPE_NONE, APPMENU_OPT_BACK,0,
         NULL,0,0,                  NULL,         NULL     },

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
        &global_addl_config.narrator_mode,0,9, am_stringtab_narrator_modes,NULL },  
  { "Speed/WPM",        DTYPE_UNS8, 
        APPMENU_OPT_EDITABLE|APPMENU_OPT_IMM_UPDATE, 0, 
        &global_addl_config.cw_speed_WPM,10,60,     NULL,NULL },  
  { "Pitch/Hz",         DTYPE_UNS8, 
        APPMENU_OPT_EDITABLE|APPMENU_OPT_IMM_UPDATE|APPMENU_OPT_FACTOR,10, 
        &global_addl_config.cw_pitch_10Hz,200,2000, NULL,NULL },  
  { "Volume",           DTYPE_UNS8, 
        APPMENU_OPT_EDITABLE|APPMENU_OPT_IMM_UPDATE, 0, 
        &global_addl_config.cw_volume,0,100, am_stringtab_255Auto,NULL },

  // { "Text__max__13", data_type,  options,opt_value,
  //     pvValue,iMinValue,iMaxValue, string table, callback }
  // In the items below, colour values are shown in short HEX format [h4].
  // When ENTERING one of these items, the 'colour picker' takes over control,
  // where an own palette can be created (unfortunately no effect on the normal screen).
  { "[3 Colours;h4]Colour Scheme", DTYPE_NONE, APPMENU_OPT_NONE, 0, 
         NULL,0,0,                  NULL, am_cbk_ColorSchemes },
  { "[h4]Foregnd", DTYPE_UNS16, APPMENU_OPT_NONE, 0, 
        &global_addl_config.fg_color,0,0, am_stringtab_color_names, am_cbk_ColorPicker },
  { "[h4]Backgnd",          DTYPE_UNS16,  APPMENU_OPT_NONE, 0, 
        &global_addl_config.bg_color,0,0, am_stringtab_color_names, am_cbk_ColorPicker },
  { "[h4]Sel/nav fg",       DTYPE_UNS16, APPMENU_OPT_NONE,0, 
        &global_addl_config.sel_fg_color,0,0, am_stringtab_color_names,am_cbk_ColorPicker },
  { "[h4]Sel/nav bg",       DTYPE_UNS16, APPMENU_OPT_NONE,0, 
        &global_addl_config.sel_bg_color,0,0, am_stringtab_color_names,am_cbk_ColorPicker },
  { "[h4]Editor fg",        DTYPE_UNS16, APPMENU_OPT_NONE,0, 
        &global_addl_config.edit_fg_color,0,0, am_stringtab_color_names,am_cbk_ColorPicker},
  { "[h4]Editor bg",        DTYPE_UNS16, APPMENU_OPT_NONE,0, 
        &global_addl_config.edit_bg_color,0,0, am_stringtab_color_names,am_cbk_ColorPicker},

  // { "Text__max__13", data_type,  options,opt_value,
  //     pvValue,iMinValue,iMaxValue, string table, callback }
  { "[4 Test/Debug;h8]HexMon", DTYPE_UNS32, APPMENU_OPT_EDITABLE,0,
        &HexMon_u32StartAddress,0,0,  NULL, am_cbk_HexMon },
  { "bl_timer",   DTYPE_UNS16, APPMENU_OPT_NONE, 0, 
        &backlight_timer,0,0,      NULL,         NULL     },
  { "opmode2",          DTYPE_UNS8,  APPMENU_OPT_NONE, 0, 
        &gui_opmode2,0,0,      am_stringtab_opmode2, NULL },
  { "[b8]opmode1",      DTYPE_UNS8,  APPMENU_OPT_NONE, 0, 
        &gui_opmode1,0,0,          NULL,         NULL     },
  { "[b8]opmode3",      DTYPE_UNS8,  APPMENU_OPT_NONE, 0, 
        &gui_opmode3,0,0,          NULL,         NULL     },
  { "[h8]SysTicks",     DTYPE_UNS32,  APPMENU_OPT_NONE, 0, // notice the increased QRM when this item..
      (void*)&IRQ_dwSysTickCounter,0,0, NULL,    NULL     }, // is scrolled into view (->rapid updates)

#if (0) && ( defined(FW_D13_020) || defined(FW_S13_020) ) // removed 2017-05-12 because Netmon1 can show these:
  { "[b8]radio_s0",     DTYPE_UNS8,  APPMENU_OPT_NONE, 0, 
        &radio_status_1.m0,0,0,    NULL,         NULL     },
  { "[b8]radio_s1",     DTYPE_UNS8,  APPMENU_OPT_NONE, 0, 
        &radio_status_1.m1,0,0,    NULL,         NULL     },
  { "[b8]radio_s2",     DTYPE_UNS8,  APPMENU_OPT_NONE, 0, 
        &radio_status_1.m2,0,0,    NULL,         NULL     },
  { "[b8]radio_s3",     DTYPE_UNS8,  APPMENU_OPT_NONE, 0, 
        &radio_status_1.m3,0,0,    NULL,         NULL     },
#endif
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

const am_stringtable_t am_stringtab_color_names[] =
{
  { LCD_COLOR_BLACK, "black" },
  { LCD_COLOR_BLUE,  "blue"  },
  { LCD_COLOR_GREEN, "green" },
  { LCD_COLOR_RED,   "red"   },
  { LCD_COLOR_YELLOW,"yellow"},
  { LCD_COLOR_CYAN,  "cyan"  },
  { LCD_COLOR_PURPLE,"purple"},
  { LCD_COLOR_WHITE, "white" },
  { 0, NULL }
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
  { 0, NULL }
};

//---------------------------------------------------------------------------
void Menu_OnKey( uint8_t key) // called on keypress from some interrupt handler
{ am_key = key;
}

//---------------------------------------------------------------------------
void Menu_Open(
        app_menu_t  *pMenu,  // [in] menu instance data in RAM, NULL for "main menu instance" 
        menu_item_t *pItems, // [in] address of the first item in ROM, NULL for "main"
        char *cpJumpToItem,  // [in] optional name (item text) to "jump to" (for programmable side buttons) .
        int  edit_mode)      // [in] new edit mode, e.g. APPMENU_EDIT_OFF. Negative: don't modify.
{
  int i,n;
  char *cp;
  menu_item_t *pItem;
  if( pMenu == NULL )
   {  pMenu = &AppMenu; // per default, use "main" instance (which in 2017-04 was THE ONLY one)
   }
  if( pItems == NULL )
   {  pItems = (menu_item_t*)am_Main; // per default, use the "main" menu items (array in ROM)
   }
  memset( pMenu, 0, sizeof( app_menu_t ) ); // also STOPS the menu's stopwatches
  pMenu->pItems  = (void*)pItems;
  pMenu->num_items = Menu_GetNumItems( pMenu->pItems );
  // Special service for programmable sidebutton to open the menu
  //         with a certain item already selected (e.g. "TkGrp" ):
  if( cpJumpToItem != NULL )
   { n = strlen( cpJumpToItem );
     for(i=0; i<pMenu->num_items; ++i)
      { pItem = &((menu_item_t*)pMenu->pItems)[i];
        cp = (char*)pItem->pszText;
        if( cp != NULL )
         { // Skip the "output formatting options" at the begin of the item text:
           cp = Menu_GetParamsFromItemText( cp, NULL, NULL, NULL );
           if( strncmp( cpJumpToItem, cp, n) == 0 )
            { pMenu->item_index = (uint8_t)i;
            }
         }
      }
   } // end if < jump to a certain item, identified by the MENU ITEM TEXT > ?
  if( edit_mode >= 0 )
   { pMenu->new_edit_mode = (uint8_t)edit_mode;
   }
  else
   { pMenu->new_edit_mode = -1;
   }
  pMenu->visible = APPMENU_VISIBLE; 
  pMenu->redraw  = TRUE;
  StartStopwatch( &pMenu->stopwatch );    // start timer for the next periodic screen update
  StartStopwatch( &backlight_stopwatch ); // start timer for periodically decrementing backlight_timer

  // If the app-menu has never been opened (or after "config reset"), the customizeable colours
  // will not be set (all zero). If all colours are zero (or equal), use defaults:
  if( LCD_GetColorDifference( global_addl_config.fg_color, global_addl_config.bg_color ) < 32 )
   {  global_addl_config.fg_color = LCD_COLOR_BLACK;
      global_addl_config.bg_color = LCD_COLOR_WHITE;
   }
  if((LCD_GetColorDifference(global_addl_config.sel_fg_color,global_addl_config.sel_bg_color) < 32 )
   ||(LCD_GetColorDifference(global_addl_config.sel_bg_color,global_addl_config.bg_color) < 32) )
   {  global_addl_config.sel_fg_color = LCD_COLOR_WHITE;
      global_addl_config.sel_bg_color = LCD_COLOR_BLUE;
   }
  if((LCD_GetColorDifference(global_addl_config.edit_fg_color,global_addl_config.edit_bg_color) < 32 )
   ||(LCD_GetColorDifference(global_addl_config.edit_bg_color,global_addl_config.bg_color) < 32) )
   {  global_addl_config.edit_fg_color = LCD_COLOR_WHITE;
      global_addl_config.edit_bg_color = LCD_COLOR_RED;
   }
} // end Menu_Open()

//---------------------------------------------------------------------------
void Menu_Close(app_menu_t *pMenu)
{
  if( pMenu->save_on_exit ) // modified global_addl_config but didn't save yet ?
   {  cfg_save();           // [in] global_addl_config,  [out] SPI-Flash
      pMenu->save_on_exit = FALSE; // "done"
   }

  if( kb_row_col_pressed ) // some key (especially the green MENU key) still pressed ?
   { pMenu->visible = APPMENU_VISIBLE_UNTIL_KEY_RELEASED;
   } // end if < trying to exit but a key is still pressed >
  else // green MENU key not pressed, so the normal main screen should appear..
   {
     if( pMenu->visible != APPMENU_OFF ) // avoid to 'close this twice'
      { pMenu->visible = APPMENU_OFF;
        // The original firmware didn't COMPLETELY redraw it's own 'main screen'
        // now, because it's not aware of the display been hijacked temporarily.
        LCD_FillRect( 0,0, LCD_SCREEN_WIDTH-1, LCD_SCREEN_HEIGHT-1, LCD_COLOR_MD380_BKGND_BLUE ); 
        // Calling display_idle_screen() from here would be asking for trouble. 
#      if(0) // the following SOMETIMES worked, 
        // but often the display froze until turning the channel knob (similar in Netmon,
        //  almost always happens on a *BUSY* channel in FM, when Tytera is ashamed
        //  of the QRM from the display cable, and decides NOT to draw anything. Ugh.. )
        // From keyb.c(!) : > "cause transient -> switch back to idle screen"
        gui_opmode2 = OPM2_MENU ;
        gui_opmode1 = SCR_MODE_IDLE | 0x80 ;
#      endif // try the "opmode1 + 2" trick to redraw the idle screen ? 
#      if( 1 )
        if( channel_num != 0 )
         {  Menu_old_channel_num = channel_num;
         }
        channel_num = 0;  // <- kludge to force re-drawing the idle screen
        // (worked reliably to redraw the idle screen COMPLETELY,
        //  even on a busy FM channel where other tricks failed,
        //  BUT setting channel_num = 0 caused the firmware to RE-LOAD
        //  a lot of channel-depending defaults, including contact.id_l/m/h, 
        //  thus resetting the TG number entered manually in amenu_set_tg.c )
#      endif // force redrawing the idle screen via channel_num ?
      } // end if < app-menu *was* really visible >
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
  if( pMenu->item_index >= pMenu->num_items ) // oops.. 
   {  pMenu->item_index = pMenu->num_items-1; // limit item_index BEFORE auto-scroll
   }

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
static void Menu_CheckDynamicValues(app_menu_t *pMenu)
  // Checks if any of the currently visible values (in the menu)
  // has been changed. If so, sets pMenu->redraw = TRUE .
{ menu_item_t *pItem;
  int item_index, imax, value;
  uint32_t checksum = 0xFFFFFFFF;
  item_index = pMenu->vert_scroll_pos;
  imax = item_index + pMenu->n_items_visible - 1; 
  if (imax >= pMenu->num_items )
   {  imax =  pMenu->num_items-1;
   }
  while( item_index <= imax )
   { pItem = &((menu_item_t *)pMenu->pItems)[item_index];
     if( pItem->pvValue ) // directly readable value (via pointer) ?
      { switch( pItem->data_type )
         { case DTYPE_STRING : // the 'value' is a good old "C"-string (8 bit) .
              checksum = CRC16( checksum, (uint16_t *)pItem->pvValue, strlen((char*)pItem->pvValue)/2 );
              break;   
           case DTYPE_WSTRING: // the 'value' is a "wide" string
              checksum = CRC16( checksum, (uint16_t *)pItem->pvValue, wide_strnlen((wchar_t*)pItem->pvValue, 16) );
              break;
           default: // anything else should be convertable into an INTEGER: 
              value = Menu_ReadIntFromPtr( pItem->pvValue, pItem->data_type );
              checksum = CRC16( checksum, (uint16_t *)&value, sizeof(int)/2 );
              break;
         }
      } // end if < item displaying a "value" -> include it in the hash >
     else // even without a "value pointer", menu items can display a value..
      { if( pItem->callback ) // .. delivered via callback on APPMENU_EVT_GET_VALUE
         { value = Menu_InvokeCallback( pMenu, pItem, APPMENU_EVT_GET_VALUE, 0/*param*/ );
           checksum = CRC16( checksum, (uint16_t *)&value, sizeof(int)/2 );
         }
      } // end else < menu item with pvValue==NULL > 
     item_index++;
   } // end for < all CURRENTLY VISIBLE items >
  if( pMenu->value_chksum != checksum )
   {  pMenu->value_chksum =  checksum;
      pMenu->redraw = TRUE;  
      // red_led_timer = 50;
   }

} // end Menu_CheckDynamicValues()


//---------------------------------------------------------------------------
void Menu_ItemValueToString( menu_item_t *pItem,  int iValue, char *sz40Dest )
  // For simplicity (and to keep the code size low), the destination buffer
  // should be large enough for 40 characters . Thus sz40Dest .
{
  int num_base,fixed_digits;
  char *cp;

  // if none of the methods below delivers a value, the result is an empty string:
  sz40Dest[0] = '\0';    

  Menu_GetParamsFromItemText( (char*)pItem->pszText, &num_base, &fixed_digits, NULL );
  switch( pItem->data_type )
   { case DTYPE_STRING : // normal string with 8 bits/char (any encoding)
        if( pItem->pvValue != NULL )
         { strlcpy( sz40Dest, pItem->pvValue, 20 );
         }
        break;
     case DTYPE_WSTRING: // "wide"-string.. eeek.. ever heard of UTF-8 ?
        if( pItem->pvValue != NULL )
         { wide_to_C_string( (wchar_t*)pItem->pvValue, sz40Dest, 40 );
         }
        break;
     case DTYPE_SUBMENU:
     case DTYPE_NONE: // without a data type, a menu item shows no value
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
} // end Menu_ItemValueToString()

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
  // Draws a "line of text with a menu item" into the framebuffer
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
        n = 0;  // if anything else (below) fails, show "zero"
        if( editing )
         { n = pMenu->iEditValue;
         }
        else // not editing, so get an up-to-date value for the display:
         { if( pItem->pvValue != NULL ) // .. if possible, via pointer
            { n = Menu_ReadIntFromPtr( pItem->pvValue, pItem->data_type );
              n = Menu_ScaleItemValue( pItem, n );
            }
           else // even without a "value pointer", menu items can display a value..
            { // .. delivered via callback on APPMENU_EVT_GET_VALUE
              n = Menu_InvokeCallback( pMenu, pItem, APPMENU_EVT_GET_VALUE, 0/*param*/ );
              // If the CALLBACK delivers the display value, don't scale it here.
            } // end else < menu item with pvValue==NULL > 
         }
        if( editing && (  (pMenu->edit_mode == APPMENU_EDIT_INSERT)
                       || (pMenu->edit_mode == APPMENU_EDIT_OVERWRT) ) )
         { // In these 'direct' editing modes, numbers are also edited AS STRINGS, so:
           strlcpy( sz40Temp, pMenu->sz40EditBuf, 25 ); // do NOT update from iEditValue !
           // Limit to 25 characters for the display (160 pixels / 6 = ~~26, minus one space)
           // To mark the cursor also when it's at the end of the edit-string,
           // append a space, which can be highlighted in the loop further below :
           n = strnlen( sz40Temp, 25 ); // number of characters occupied by the "value"
           if( n<25 )
            { sz40Temp[n++] = ' ';
              sz40Temp[n] = '\0';
            }
         }
        else
         { Menu_ItemValueToString( pItem, n, sz40Temp );
         }
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
 
        if( editing && ( (pMenu->edit_mode == APPMENU_EDIT_OVERWRT)
                      || (pMenu->edit_mode == APPMENU_EDIT_INSERT) ) )
         { // In edit modes 'OVERWRITE' or 'INSERT', use a single space as separator,
           // and left-aligned value. Avoids horizontal scrolling when the length increases.
           x = LCD_DrawCharAt( ' ', x, y, fg_color, bg_color, font_nr );
         }
        else // not editing at all, or in 'inc/dec'-editing mode:
         {
           // Fill the gap between LEFT-aligned text and RIGHT-aligned value
           n = LCD_SCREEN_WIDTH - 3 - LCD_GetTextWidth( font_nr, sz40Temp );
                       //         |___ spare for the right margin
           LimitInteger( &n, 0, LCD_SCREEN_WIDTH-8 ); 
           if( n >= x )  // if there's no gap, don't try to fill it...
            { LCD_FillRect( x,y, n-1/*x2*/, y+text_height_pixels-1/*y2*/, bg_color );
            }
           x = n; // graphic position for drawing the right-aligned 'value'
         }

        // Draw the 'value', using different colours to mark the cursor or nav-bar :
        cp = sz40Temp;
        i  = 0; // character counter to mark the cursor position
        while( ((c=*cp++)!=0) && (x<(LCD_SCREEN_WIDTH-6) ) )
         { sel_flags &= ~SEL_FLAG_CURSOR;
           if( editing ) // THIS item is currently being edited ..
            { // so mark only the current edit cursor position ?
              if( pMenu->edit_mode == APPMENU_EDIT_INC_DEC )
               { sel_flags |= SEL_FLAG_CURSOR; // increment/decrement applies to the entire field
               }
              else if( i == pMenu->cursor_pos ) // direct typing mode: only a single digit marked
               { sel_flags |= SEL_FLAG_CURSOR;
               }
              else
               { sel_flags &= ~SEL_FLAG_CURSOR;
               }
              Menu_GetColours( sel_flags, &fg_color, &bg_color );
            } // end if < editing >
           x = LCD_DrawCharAt( c, x, y, fg_color, bg_color, font_nr );
           ++i;
         } // end while < more characters > 
      } // end if < valid item index >
   } // end if( pMenu->pItems != NULL )

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

  // The basic idea was to show hotkey number and 'chapter header'
  // along with a horizontal rule, e.g. :
  //
  // [ 1 Radio >--------
  // Bla, bla..
  // [ 2 Settings >-----
  // Etc, etc..
  // [ 3 Backlight >----
  // Level Lo          1
  // Level Hi          4
  // Time/sec         30
  // [ 4 Morse output>--
  // Mode        verbose 
  // Speed/WPM        30

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
  // When visible(!), paints the 'alternative menu' into the framebuffer.
  //      Also processes pending keyboard events (from its own buffer).
  //      Called from various 'display-update' hook functions .
  // [in] caller : tells which of the half dozen of hooked functions
  //               calls us .  Mostly used for debugging .
  // Return value:  1 when visible (and the framebuffer was filled),
  //                0 when invisible (and someone else should 'draw').
{
  app_menu_t *pMenu = &AppMenu; // load the struct's address only ONCE
  menu_item_t *pItem;
  char c;
  int y,iTextLineNr;
  int age_of_last_update = ReadStopwatch_ms( &pMenu->stopwatch );
  uint16_t fg_color, bg_color;


  if( Menu_old_channel_num != channel_num ) // defeat the trouble in amenu_set_tg :
   { Menu_old_channel_num = channel_num; // Tytera may have overwritten struct contact..
     CheckTalkgroupAfterChannelSwitch(); // so switch to the AD-HOC talkgroup again ?
   }

  // The following really doesn't belong here, but Menu_DrawIfVisible()
  // returns early from half a dozen of "hooked gfx stuff", and since
  // the Netmon screens (text console) can be viewed from this menu,
  // allow some of the netmon*_update() functions to *COLLECT DATA* in
  // their own, static buffers. See netmon.c : netmon6_update() for example.
  if( (caller==AM_CALLER_F_4315_HOOK)
   && (global_addl_config.netmon != 0) )
   { netmon_update(); // replaces the call from f_4315_hook(), whatever that is
   }

  // Special service for visually impaired operators:
  //   Holding down the red button for some seconds after power-on 
  //   activates Morse output with 'reasonable defaults' .
  // While showing the 'Activate Morse' screen, suppress normal display:
  if( Menu_CheckLongKeypressToActivateMorse(pMenu) )
   { return TRUE; // suppress the normal display
   }

  if( pMenu->visible==APPMENU_VISIBLE_UNTIL_KEY_RELEASED )
   { // Menu "almost closed" but still visible until releasing the last key.
     // The following is a kludge to prevent opening Tytera's "green key menu"
     // on the same keystroke that was used here to CLOSE the "red key menu".
     if( kb_row_col_pressed == 0 )
      { Menu_Close(pMenu);
      }
     return pMenu->visible != APPMENU_OFF;
   }

  // Simple keyboard processing..
  c = am_key;  
  if( c ) 
   { am_key=0; // remove key from this buffer

     // Restart the timer for delayed Morse output, for example after modifying a value.
     // Output in Morse code can only start if this stopwatch expires. Avoids excessive chatter.
     StartStopwatch( &pMenu->morse_stopwatch );
     if(pMenu->visible!=APPMENU_OFF)
      { MorseGen_ClearTxBuffer(); // abort ongoing Morse transmission (if any) on keypress 
      }

     // Reload Tytera's "backlight_timer" here, because their own control doesn't work now:
     backlight_timer = md380_radio_config.backlight_time * 500; // unit: 10-millisecond steps 
     if( pMenu->visible == APPMENU_USERSCREEN_VISIBLE )
      { // screen and keyboard occupied by a 'user screen' -> pass on keyboard events to it: 
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
              Menu_OnEnterKey(pMenu);
              break; // end case < green "Menu", aka "Confirm"-key >
           case 'B' :  // red "Back"-key : 
              // red_led_timer  = 20;    // <- poor man's debugging 
              if( pMenu->visible == APPMENU_OFF ) // not visible yet..
               { Menu_Open( pMenu, NULL, NULL, APPMENU_EDIT_OFF );  // so open the default menu (items)
                 StartStopwatch( &pMenu->morse_stopwatch ); 
                 // Reporting the first item in Morse code later (when morse_stopwatch expires): 
                 pMenu->morse_request = AMENU_MORSE_REQUEST_ITEM_TEXT | AMENU_MORSE_REQUEST_ITEM_VALUE;
               }
              else // already in the app menu: treat the RED KEY like "BACK",
               {   // "Exit", "Escape", or "Delete" ?
                 if( ( (pMenu->edit_mode==APPMENU_EDIT_OVERWRT)
                     ||(pMenu->edit_mode==APPMENU_EDIT_INSERT) )
                    && (pMenu->edit_length > 0 ) )
                  { Menu_ProcessEditKey(pMenu, c );
                  }
                 else
                  { Menu_OnExitKey(pMenu);
                  }
               }
              break;
           case 'U' :  // cursor UP : navigate towards the FIRST item ...
              switch( pMenu->edit_mode ) // ... or increment value when editing
               {  case APPMENU_EDIT_INC_DEC:
                     Menu_OnIncDecEdit( pMenu, +1 ); 
                     break;
                  case APPMENU_EDIT_OVERWRT:
                  case APPMENU_EDIT_INSERT :
                     // Similar as in Tytera firmware: cursor "UP" key moves the edit cursor LEFT
                     if( pMenu->cursor_pos > 0 )
                      { --pMenu->cursor_pos;
                      }
                     break;
                  default: // not "editing" but "navigating" ... 
                     if( pMenu->item_index > 0 ) 
                      { --pMenu->item_index; 
                        // If no other keystrokes follow, and if Morse output is enabled,..:
                        pMenu->morse_request = AMENU_MORSE_REQUEST_ITEM_TEXT | AMENU_MORSE_REQUEST_ITEM_VALUE;
                      }
               }
              pMenu->redraw = TRUE;
              break;
           case 'D' :  // cursor DOWN: navigate towards the LAST item
              switch( pMenu->edit_mode ) // ... or decrement value in inc/dec editing mode
               {  case APPMENU_EDIT_INC_DEC:
                     Menu_OnIncDecEdit( pMenu, -1 ); 
                     break;
                  case APPMENU_EDIT_OVERWRT: // ... or move cursor RIGHT in 'direct input' editing mode
                  case APPMENU_EDIT_INSERT :
                     // Similar as in Tytera firmware: cursor "DOWN" key moves edit cursor RIGHT.
                     // To APPEND chars to the string, cursor_pos <= edit_length is ok : 
                     if( pMenu->cursor_pos < pMenu->edit_length )
                      { ++pMenu->cursor_pos;
                      }
                     break;
                  default: // not "editing" but "navigating" ... 
                     if(pMenu->item_index < (pMenu->num_items-1) )  
                      { ++pMenu->item_index;
                        pMenu->morse_request = AMENU_MORSE_REQUEST_ITEM_TEXT | AMENU_MORSE_REQUEST_ITEM_VALUE;
                      }
                     break;
               }
              pMenu->redraw = TRUE;
              break;
           default:  // Other keys .. editing or treat as a hotkey ?
              if( pMenu->edit_mode != APPMENU_EDIT_OFF )
               { Menu_ProcessEditKey(pMenu, c);
               }
              else // NOT editing -> numeric keys used as hotkeys for quick navigation
               { Menu_ProcessHotkey(pMenu, c);
               }
              break;
         } // end switch < key >
        if( pMenu->visible )
         {  pMenu->redraw = TRUE;
         }
      } // end if < "red key" pressed or "red-key-menu" already visible > ?
   } // end if < keyboard-"event" for the App Menu > ? 


  // Start editing 'on request' from a programmable hotkey (e.g. edit talkgroup) ?
  if( pMenu->new_edit_mode >= 0 ) 
   { pItem = Menu_GetFocusedItem(pMenu);
     if( pItem != NULL ) 
      { Menu_UpdateEditValueIfNotEditingYet(pMenu, pItem); // *pItem->pvValue -> pMenu->iEditValue (?)
        Menu_BeginEditing( pMenu, pItem, (uint8_t)pMenu->new_edit_mode );
      }
     pMenu->new_edit_mode = -1; 
   } // end if( pMenu->new_edit_mode >= 0 )


  // After keyboard processing, etc: Draw the 'App Menu' into the framebuffer ?
  if( pMenu->visible == APPMENU_VISIBLE ) 
   { // got here approx 9 times per second...

     // To keep the QRM radiated from the LCD cable low, only redraw the screen
     // if necessary, and with a LIMITED refresh rate. 
     if( age_of_last_update > 20/*ms*/ )
      { // There may be items in the menu with 'dynamic' values,
        // which require redrawing the screen even without a keypress. Check for those:
        Menu_CheckDynamicValues( pMenu ); // sets pMenu->redraw if something changes
      }
     if( pMenu->stopwatch_late_redraw != 0 )   // stopwatch for a 'late redraw' running ?
      { if( ReadStopwatch_ms( &pMenu->stopwatch_late_redraw ) > 500/*ms*/ )
         { // guess the original firmware has updated what may be displayed now,
           // for example the CHANNEL NAME after switching to a DIFFERENT ZONE :
           pMenu->redraw = TRUE;
           pMenu->stopwatch_late_redraw = 0;  // stop until there's the need for a 'late redraw' again
         }
      }
     if( pMenu->redraw )
      { // LED_GREEN_ON; // for speed test (connect a scope to the green LED)
        Menu_CheckVertScrollPos(pMenu); // make sure item-index is valid and 'in view'
        pMenu->redraw = FALSE;
        y=iTextLineNr=0;
        while( y < (LCD_SCREEN_HEIGHT-LCD_FONT_HEIGHT) )
         { y = Menu_DrawLineWithItem( pMenu, y, iTextLineNr++ );
         }
        // If the output cursor (y) didn't reach the end, clear the rest of the screen:
        Menu_GetColours( SEL_FLAG_NONE, &fg_color, &bg_color );
        LCD_FillRect( 0, y, LCD_SCREEN_WIDTH-1, LCD_SCREEN_HEIGHT-1, bg_color );
        StartStopwatch( &pMenu->stopwatch ); // wait some milliseconds before updating the MENU screen again
      } // end if( pMenu->redraw ) 
   } // end if < "app menu" visible >
  else if( pMenu->visible == APPMENU_USERSCREEN_VISIBLE ) // some 'user screen' visible ?
   { 
     if( pMenu->redraw || (age_of_last_update > 20/*ms*/) )
      { y = Menu_InvokeCallback( pMenu, Menu_GetFocusedItem(pMenu), APPMENU_EVT_PAINT, 0 );
        if( y != AM_RESULT_OCCUPY_SCREEN )
         { // framebuffer no longer occupied by the callback, so back to the 'app menu':
           pMenu->visible = APPMENU_VISIBLE;
           pMenu->redraw  = TRUE; // ... redraw menu IN THE NEXT CALL
         }
        StartStopwatch( &pMenu->stopwatch ); // wait some milliseconds before updating the USER screen again
      }
   }

  // If a request to report the edited value in Morse code, AND some time has passed
  // since the last modification (via cursor up/down), start sending the value:
  if( global_addl_config.narrator_mode & NARRATOR_MODE_ENABLED )
   { if( ReadStopwatch_ms( &pMenu->morse_stopwatch ) > 500 )
      { // ok, no further keyboard events for some time, so start sending NOW:
        if( pMenu->morse_request != 0 )
         {  Menu_ReportItemInMorseCode( pMenu->morse_request );
            pMenu->morse_request = 0; // "done" (Morse output started)
          //red_led_timer = 50; // debug: just started Morse output from the app-menu
         }
      }
   }

  // TYTERA's backlight_timer would be decremented once every 10 ms if we were 
  // on their main screen. If we aren't (but in the app-menu or on a user-screen
  // opened from there), decrement backlight_timer here .
  // The calling interval of Menu_DrawIfVisible() is completely unknown,
  // so use a SysTick-based 'stopwatch' to decrement backlight_timer EVERY SECOND (!).
  // This is sufficient because the lowest possible backlight time (except zero) is 5 seconds.
  if( pMenu->visible != APPMENU_OFF ) 
   {
     if( ReadStopwatch_ms( &backlight_stopwatch ) > 1000/*ms*/ )
      { StartStopwatch( &backlight_stopwatch );
        y = (int)backlight_timer - 100; // another second is over, i.e. 100 10-ms-steps
        if( y<0 )
         {  y=0;  // backlight timer expired (the rest happens in irq_handlers.c) 
         }
        backlight_timer = (uint16_t)y; // write back to Tytera's backlight timer
      }
   }

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
  int cbk_result = AM_RESULT_NONE; // "proceed as if there was NO callback function"
                            // (also returned here if there IS NO callback..) 
  if( pItem ) 
   { if( pItem->callback )
      { cbk_result = pItem->callback( pMenu, pItem, event, param );
        // Check a few result codes immediately.. kind of "common default handler",
        // to simplify the implementation of dialog screens and similar gadgets
        // (for example, the 'colour picker' dialog in applet/src/color_picker.c).
        // But there's one exception :
        if( event==APPMENU_EVT_GET_VALUE ) // for the "get value" event,
         { return cbk_result; // the return value is the to-be-displayed value,
           // not a result- or error code. First used for the talkgroup display/editor .
           // APPMENU_EVT_GET_VALUE is only used if pItem->pvValue is NULL .
         }
        switch( cbk_result )
         { case AM_RESULT_EXIT_AND_RELEASE_SCREEN :
              // The whatever-it-is ("dialog screen"?) wants to be closed,
              // and returns the framebuffer control back to the 'app menu':
              if( pMenu->visible == APPMENU_USERSCREEN_VISIBLE )
               { // the callback didn't switch back from 'user screen' to 'menu screen'
                 // so do that here, and prepare redrawing the entire screen (menu):
                 pMenu->visible = APPMENU_VISIBLE;
                 pMenu->redraw  = TRUE; // ... redraw menu a.s.a.p.
                 StartStopwatch( &pMenu->morse_stopwatch ); 
                 // Reporting the first item in Morse code later (when morse_stopwatch expires): 
                 pMenu->morse_request = AMENU_MORSE_REQUEST_ITEM_TEXT | AMENU_MORSE_REQUEST_ITEM_VALUE;
               }
              break; 
           case AM_RESULT_OCCUPY_SCREEN: // the callback has 'occupied' the screen,
              // and doesn't want anyone to paint to the framebuffer 
              // until it agrees to give the screen back:
              pMenu->visible = APPMENU_USERSCREEN_VISIBLE;
              break;
           default:  // no special action required here
              break; 
         } // end switch < callback-result >
      } // end if < callback function exists >
   } // end if < menu-item-pointer not NULL >
  return cbk_result;
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
void Menu_UpdateEditValueIfNotEditingYet(app_menu_t *pMenu, menu_item_t *pItem)
{
  if( (pItem != NULL ) && (pMenu->edit_mode==APPMENU_EDIT_OFF) )  
   { if( pItem->pvValue )
      { pMenu->iEditValue= pMenu->iValueBeforeEditing 
         = Menu_ScaleItemValue(pItem,Menu_ReadIntFromPtr(pItem->pvValue,pItem->data_type));
      }
     else // even menu items without a "value pointer" may have something editable:
      { pMenu->iEditValue= Menu_InvokeCallback( pMenu, pItem, APPMENU_EVT_GET_VALUE, 0/*param*/ );
      }
   }
}

//---------------------------------------------------------------------------
void Menu_OnEnterKey(app_menu_t *pMenu)
  // Called when the 'App Menu' is already opened, 
  //  when pressing the equivalent of an ENTER-key.
{
  int cbk_result;
  menu_item_t *pItem = Menu_GetFocusedItem(pMenu);
  if( pItem ) 
   { 
     Menu_UpdateEditValueIfNotEditingYet(pMenu, pItem); // *pItem->pvValue -> pMenu->iEditValue (?)

     // If the currently focused item has a callback function,
     // invoke that function to let the callback do whatever it wants:
     cbk_result = Menu_InvokeCallback( pMenu, pItem, APPMENU_EVT_ENTER, 0 );
     if( cbk_result==AM_RESULT_ERROR ) // callback disagrees to 'enter' this item !
      { return;
      }
 
     // Open a submenu (without the need for a callback) ? 
     if( (pItem->data_type==DTYPE_SUBMENU) && (pItem->pvValue != NULL) )
      { Menu_EnterSubmenu( pMenu, (menu_item_t*)pItem->pvValue );
        pMenu->morse_request = AMENU_MORSE_REQUEST_ITEM_TEXT | AMENU_MORSE_REQUEST_ITEM_VALUE;
      } // end if < entry to a SUB-MENU >
     else // Return to the parent menu, or to the radio's "main screen" ?
     if( pItem->options == APPMENU_OPT_BACK )
      { if( Menu_PopSubmenuFromStack(pMenu) ) 
         { pMenu->morse_request = AMENU_MORSE_REQUEST_ITEM_TEXT | AMENU_MORSE_REQUEST_ITEM_VALUE;
         }
        else
         { // Cannot return from a SUB-MENU so return to the MAIN SCREEN:
           Menu_Close(pMenu);
         } 
      }
     else // begin or end editing ?
     if( (pItem->options & APPMENU_OPT_EDITABLE ) && MenuItem_HasValue(pItem) )
      { if( pMenu->edit_mode != APPMENU_EDIT_OFF) 
         {  // was editing -> finish input ("write back")
            Menu_FinishEditing( pMenu, pItem ); // [in] pMenu->iEditValue/EditBuffer, [out] *pItem->pvValue 
            // If there's a callback function, inform it that we're not editing anymore:
            Menu_InvokeCallback( pMenu, pItem, APPMENU_EVT_END_EDIT, 1/*finish, not abort*/ );
            // For the visually impaired operator, report the new (edited?) value again ?
            pMenu->morse_request = AMENU_MORSE_REQUEST_ITEM_TEXT | AMENU_MORSE_REQUEST_ITEM_VALUE;
         }
        else // begin editing with the simple "increment/decrement"-mode (up/down keys)
         {  Menu_BeginEditing( pMenu, pItem, APPMENU_EDIT_INC_DEC );
            // Inform the visually impaired operator about editing... but how ?
            // ToDo: Send the ITEM TEXT, the (old) VALUE, followed by a QUESTION MARK (asking for "input") ?
         }
      } // end if < editable > ?
   }
} // end Menu_OnEnterKey()

//---------------------------------------------------------------------------
void Menu_BeginEditing( app_menu_t *pMenu, menu_item_t *pItem, uint8_t edit_mode )
  // Must NOT be invoked from a callback, because callbacks are invoked FROM here
{ int cbk_result;
  if( (edit_mode!=APPMENU_EDIT_OFF) && (pItem->options & APPMENU_OPT_EDITABLE ) && MenuItem_HasValue(pItem) )
   { // Copy the 'current' value into the internal storage,
     //  for both integer and string types :
     if( pItem->pvValue != NULL )
      { pMenu->iEditValue= pMenu->iValueBeforeEditing 
          = Menu_ScaleItemValue(pItem,Menu_ReadIntFromPtr(pItem->pvValue,pItem->data_type));
      }
     else // even menu items without a "value pointer" may have an editable value:
      { pMenu->iEditValue= Menu_InvokeCallback( pMenu, pItem, APPMENU_EVT_GET_VALUE, 0/*param*/ );
      }
     pMenu->edit_mode = edit_mode; 
        // setting edit_mode also "disconnects" the displayed value from pItem->pvValue
     Menu_ItemValueToString( pItem, pMenu->iEditValue, pMenu->sz40EditBuf );
     Menu_UpdateEditLengthAndSetCursorPos( pMenu, -1/*keep old cursor position if valid*/ );

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
{ int i,n,old,num_base,fixed_digits;
  char *cp;

  // In 'overwrite / insert' editing mode, the edit buffer (a string) is used.
  // In 'increment / decrement' editing mode, the new value is already in pMenu->iEditValue.
  if( (pMenu->edit_mode == APPMENU_EDIT_OVERWRT) || (pMenu->edit_mode==APPMENU_EDIT_INSERT) ) 
   { // Parse pMenu->sz40EditBuf .. which format, decimal, hex, bin ?
     Menu_GetParamsFromItemText( (char*)pItem->pszText, &num_base, &fixed_digits, NULL );
     cp = pMenu->sz40EditBuf;  // source for parsing = edit buffer
     switch( num_base )
      { case 10: 
           pMenu->iEditValue = Menu_ParseDecimal( &cp );  
           break;
        case 16: 
           pMenu->iEditValue = Menu_ParseHex( &cp );  
           break;
        case 2:   // very exotic, but supported for DISPLAY so support editing this, too
           pMenu->iEditValue = Menu_ParseBinary( &cp );  
           break;
        default:  // Anything else ? no, thanks.
           break;
      } 
   } // end if < edit modes 'overwrite' or 'insert' ? >

  // Inversely scale and write back the current edit value via pointer .
  n = pMenu->iEditValue;  // to avoid confusion: this is the DISPLAY value,
                          // not the 'raw, unscaled' value !
  i = pItem->opt_value;   // this 'option value' serves multiple purposes..
  old = Menu_ReadIntFromPtr( pItem->pvValue, pItem->data_type ); 
                          // if pvValue==NULL,  old = 0. That's ok
  if( i != 0 ) // .. but only if nonzero :
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
            { i >>= 1; // option value : shifted RIGHT until the first 'one' is in bit 0
              n <<= 1; // edit value   : shifted LEFT, back into the "original" position
            } // after this loop, the least significant bit is in bit 0 (mask 1)
         }
        n &= pItem->opt_value; // all 'foreign' bits in n are zero now.
        // But the byte/word/dword/int that pvValue points to may contain 
        // a few bits that must NOT be modified here. Thus:
        n |= (old & ~pItem->opt_value); 
        // Phew. Enough of this bit-fiddling :)
      } // enf if < value bitwise ANDed with a bitmask > ?
   } // end if < "option value" nonzero >

  // If the edited value is a member of global_addl_config, save it in Flash LATER.
  // (DO NOT save it immediately after every inc/dec-step or similar. 
  //  Writing to Flash takes time, stresses the Flash, may cause audio drop-outs, etc)
  if( (uint8_t*)pItem->pvValue >= (uint8_t*)&global_addl_config
   && (uint8_t*)pItem->pvValue < ((uint8_t*)&global_addl_config+sizeof(global_addl_config) ) )
   { pMenu->save_on_exit = TRUE;
   }

  // Write back the inversely scaled 'edit value' (n) :
  Menu_WriteIntToPtr( n, pItem->pvValue, pItem->data_type );

} // end Menu_WriteBackEditedValue() 

//---------------------------------------------------------------------------
void Menu_OnIncDecEdit( app_menu_t *pMenu, int delta )
{
  int     i; 
  int64_t i64;
  menu_item_t *pItem = Menu_GetFocusedItem(pMenu);
  // If the edited value is NUMERIC, really increment/decrement THE VALUE
  // (not characters in a string). This way, the min/max limits can be 
  // easily checked while editing, and depending on the base (2,10,16),
  // "editing" is in facting adding or subtracting a power of the base.
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
  if( pMenu->iEditValue != i )
   { // report modified value in Morse code (VALUE only, not the leading text)
     pMenu->morse_request |= AMENU_MORSE_REQUEST_ITEM_VALUE;
   }
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
void Menu_UpdateEditLengthAndSetCursorPos( app_menu_t *pMenu, int cursor_pos )
  // [in]  pMenu->sz40EditBuf
  // [out] pMenu->edit_length, pMenu->cursor_pos (limited to valid range)
{
  pMenu->edit_length = strnlen( pMenu->sz40EditBuf, 39 );
  if( cursor_pos < 0 )  // don't set a "new" cursor position but LIMIT to valid range:
   {  cursor_pos = pMenu->cursor_pos;
   }
  // zero-based cursor_pos = string length means "append char to string" ! 
  LimitInteger( &cursor_pos, 0, pMenu->edit_length ); 
  pMenu->cursor_pos = (uint8_t)cursor_pos;
}

//---------------------------------------------------------------------------
BOOL Menu_ProcessEditKey(app_menu_t *pMenu, char c)
  // Keyboard processing when the menu is in 'value editing mode'
{
  int i,n,num_base,fixed_digits;
  unsigned char c2;
  menu_item_t *pItem = Menu_GetFocusedItem(pMenu);
  if( pItem==NULL )
   { return FALSE; // cannot process the key as "edit key" !
   } 
  Menu_GetParamsFromItemText( (char*)pItem->pszText, &num_base, &fixed_digits, NULL );
  if( pMenu->edit_mode == APPMENU_EDIT_INC_DEC )
   { if( (c>='0' && c<='9') || (c=='#') ) 
      { // Switch from 'inc/dec' editing mode to 'direct input':
        // For simplicity, numbers are also edited AS STRINGS, thus:
        if( pItem->data_type!=DTYPE_STRING && pItem->data_type!=DTYPE_WSTRING )
         { Menu_ItemValueToString( pItem, pMenu->iEditValue, pMenu->sz40EditBuf );
         }
        Menu_UpdateEditLengthAndSetCursorPos( pMenu, 0/*new cursor position*/ );
        pMenu->edit_mode = APPMENU_EDIT_OVERWRT;
        // The key just pressed ('0'..'9') will immediately overwrite the FIRST DIGIT
        // (unlike in Tytera's menu, where the initial cursor position is at the string's end).
      }
   }
  Menu_UpdateEditLengthAndSetCursorPos( pMenu, -1 ); // update edit_length, limit cursor_pos
  // At this point, pMenu->cursor_pos is always a valid array index into pMenu->sz40EditBuf,
  // and pMenu->edit_length (0..39) contains the LENGTH of that C-string .
  // pMenu->sz40EditBuf[] is a bit longer than necessary, so setting
  // pMenu->sz40EditBuf[pMenu->edit_length+1] to zero (C string terminator) is ok.
  if( c>='0' && c<='9' )  // INSERT or OVERWRITE with these 'direct input' characters ? 
   { if( pMenu->edit_mode == APPMENU_EDIT_OVERWRT )
      { pMenu->sz40EditBuf[pMenu->cursor_pos++] = c; 
        if( pMenu->cursor_pos >= pMenu->edit_length ) // string may have "grown", so terminate it
         { pMenu->sz40EditBuf[pMenu->cursor_pos] = '\0';
         }
      }
     else if( pMenu->edit_mode == APPMENU_EDIT_INSERT )
      { n = pMenu->edit_length;
        for( i=n+1/*include trailing zero when copying*/; i>pMenu->cursor_pos; --i)
         { pMenu->sz40EditBuf[i] = pMenu->sz40EditBuf[i-2];
         }
        pMenu->sz40EditBuf[pMenu->cursor_pos++] = c; 
      }
     Menu_UpdateEditLengthAndSetCursorPos( pMenu, -1 ); // update edit_length, limit cursor_pos (again)
     return TRUE;
   } // end if < insertable character for the string >
  else if( c=='B' ) // red "Back"-key ~~~ BACKSPACE ?
   { // In "OVERWRITE" mode, the cursor appears like a selected character,
     // and "back" actually DELETES the selected character - not the left neighbour.
     // Only when the cursor is at the end of the string, it behaves like backspace,
     // and 'eats the string from right to left' .
     i = pMenu->cursor_pos;
     if( (i>0) && (i>=pMenu->edit_length) ) 
      { // cursor at the end of the string (where there's nothing to delete)
        --i; //  behave like backspace (with cursor movement)
        pMenu->cursor_pos = (uint8_t)i;
      }
     while(i<pMenu->edit_length) // DELETE (does not move the cursor)
      { pMenu->sz40EditBuf[i] = pMenu->sz40EditBuf[i+1];
        ++i;
      }
     Menu_UpdateEditLengthAndSetCursorPos( pMenu, -1 ); // update edit_length, limit cursor_pos again
   }
  else if( c=='#' ) // 'hashtag/arrow-pointing-up'-key : function depends on edit mode
   { switch( pMenu->edit_mode ) 
      { case APPMENU_EDIT_OVERWRT :
        case APPMENU_EDIT_INSERT  :
           // increment code of the character under the cursor, depending on display format..
           i = pMenu->cursor_pos;
           if( i<=pMenu->edit_length ) 
            { if(i==pMenu->edit_length)
               { pMenu->sz40EditBuf[i+1] = '\0'; // new trailing zero (string gets longer below)
               }
              c2 = (unsigned char)pMenu->sz40EditBuf[i] + 1;
              if( c2 < 32 )
               { if( (pItem->data_type==DTYPE_STRING) || (pItem->data_type==DTYPE_WSTRING) )
                  { c2 = ' ';
                  }
                 else // guess the input is NUMERIC :
                  { c2 = '0';
                  }
               }
              if( (pItem->data_type==DTYPE_STRING) || (pItem->data_type==DTYPE_WSTRING) )
               { if( c2 > 'z' )
                  {  c2 = ' ';
                  }
               }
              else // not editing a STRING but a number..
              switch( num_base ) // the UPPER limit depends on the base
               { case 2:    // only 'binary' digits (but of course ASCII)
                   if( c2 > '1' )
                    {  c2 = '0';
                    }
                   break;
                 case 10:
                   if( c2 > '9' )
                    {  c2 = '0';
                    }
                   break;
                 case 16:   // hex digits, with 'A'..'F' for better visibility
                   if( c2 == ('9'+1) )
                    {  c2 = 'A';
                    }
                   else if( c2 > 'F' )
                    {  c2 = '0';
                    }
                   break;
                 default: // ???
                   break;
               }
              pMenu->sz40EditBuf[i] = (char)c2; 
            }
           break;
        default:
           break;
      }
     Menu_UpdateEditLengthAndSetCursorPos( pMenu, -1 ); // update edit_length, limit cursor_pos again
   } // end if < '#' >
  return FALSE;
} // end Menu_ProcessEditKey()

//---------------------------------------------------------------------------
void Menu_FinishEditing( app_menu_t *pMenu, menu_item_t *pItem ) // API !
{ // Must not invoke a callback, because this function MAY BE called FROM a callback.
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
       { // value was modified .. need to "undo" this ?
         if( pItem->options & APPMENU_OPT_IMM_UPDATE ) // yes..  
          { pMenu->iEditValue = pMenu->iValueBeforeEditing;
            Menu_WriteBackEditedValue( pMenu, pItem ); // "undo"
          }
       }
      Menu_InvokeCallback( pMenu, pItem, APPMENU_EVT_END_EDIT, 0/* abort, not "finish" */ );
      pMenu->redraw = TRUE;
   }
  pMenu->edit_mode = APPMENU_EDIT_OFF;
} // end Menu_AbortEditing()

//---------------------------------------------------------------------------
void Menu_OnExitKey(app_menu_t *pMenu)
  // Exit/Back/Escape key pressed when the 'App Menu' was already open.
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
        case AM_RESULT_OCCUPY_SCREEN: // the callback has 'occupied' the screen
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
     pMenu->morse_request = AMENU_MORSE_REQUEST_ITEM_TEXT | AMENU_MORSE_REQUEST_ITEM_VALUE;
   }
  else if( Menu_PopSubmenuFromStack(pMenu) ) 
   { pMenu->morse_request = AMENU_MORSE_REQUEST_ITEM_TEXT | AMENU_MORSE_REQUEST_ITEM_VALUE;
   }
  else // Cannot return from a SUB-MENU so return to the MAIN SCREEN:
   { Menu_Close(pMenu);
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
int am_cbk_ColorTest(app_menu_t *pMenu, menu_item_t *pItem, int event, int param )
{ // Simple example for a 'user screen' opened from the application menu
  if( event==APPMENU_EVT_ENTER ) // pressed ENTER (to launch the colour test) ?
   { LCD_ColorGradientTest(); // only draw the colour test pattern ONCE...
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
int  Menu_GetItemIndex(void)
{ // used by the Morse narrator (narrator.c) to detect "changes"
  app_menu_t *pMenu = &AppMenu; // load the struct's address only ONCE
  if( pMenu->visible==APPMENU_VISIBLE )
   { return pMenu->item_index;
   }
  else if( pMenu->visible==APPMENU_USERSCREEN_VISIBLE ) 
   { // Currently showing a 'user' screen, e.g. the alternative ZONE list.
     // For any 'user screen' that uses pMenu->scroll_list,
     // the index of the currently FOCUSED item can be taken from here:
     if( ( pMenu->scroll_list.focused_item >= 0 )
       &&( pMenu->scroll_list.focused_item < pMenu->scroll_list.num_items) )
      { return pMenu->scroll_list.focused_item;
      }
   }
  // Arrived here ? No chance to retrieve the index of the currently FOCUSED item.
  return -1;
} 


//---------------------------------------------------------------------------
void Menu_ReportItemInMorseCode( 
        int morse_request ) // [in] bitcombination of AM_MORSE_REQUEST_.. 
{ // Called from ..
  //  - the Morse narrator when the operator requested to read out the screen,
  //    using one of the programmable sidebuttons;
  //  - Menu_DrawIfVisible() after selecting a new item, or after editing
  //    something, with some delay to avoid 'too much chatter' from the radio.
  app_menu_t *pMenu = &AppMenu; // load the struct's address only ONCE
  menu_item_t *pItem;
  int  n;
  char *cp, sz40[44];
  if( pMenu->visible==APPMENU_VISIBLE )
   { pItem = Menu_GetFocusedItem(pMenu);
     if( pItem != NULL )
      { if( (pItem->pszText != NULL ) // items WITHOUT a fixed text are rare but possible!
         && (morse_request & AMENU_MORSE_REQUEST_ITEM_TEXT) )
         { // Skip the "output formatting options" at the begin of the item text:
           cp = Menu_GetParamsFromItemText( (char*)pItem->pszText, NULL, NULL, NULL );
           MorseGen_AppendChar( '\x09' ); // decrease pitch (auto frequency),
           MorseGen_AppendString( cp );   // report the "fixed" menu item text
           MorseGen_AppendChar( '\x10' ); // SPACE + back to the normal CW pitch
         }
        // Convert the optional 'value' into a string, here for Morse output:
        if( MenuItem_HasValue(pItem) 
         && (morse_request & AMENU_MORSE_REQUEST_ITEM_VALUE) )
         { if( pMenu->edit_mode != APPMENU_EDIT_OFF )
            { n = pMenu->iEditValue;
            }
           else // not editing, so report the CURRENT (momentary) value:
           if( pItem->pvValue != NULL )
            { n = Menu_ReadIntFromPtr( pItem->pvValue, pItem->data_type );
              n = Menu_ScaleItemValue( pItem, n );
            }
           else
            { n = Menu_InvokeCallback( pMenu, pItem, APPMENU_EVT_GET_VALUE, 0/*param*/ );
            }
           Menu_ItemValueToString( pItem, n, sz40 );
           MorseGen_AppendString( sz40 );
         }
      }
   } // end if( pMenu->visible==APPMENU_VISIBLE )
  else if( pMenu->visible==APPMENU_USERSCREEN_VISIBLE ) 
   { // Currently showing one of the 'user' screens, including zone list, etc.
     // If the 'user screen' has assembled a string for Morse output, send it:
     if( (pMenu->sz40MorseTextFromFocusedLine[0] != 0 )
      && (morse_request & AMENU_MORSE_REQUEST_ITEM_TEXT) )
      { MorseGen_AppendString( pMenu->sz40MorseTextFromFocusedLine );
      }
   } // if( pMenu->visible==APPMENU_USERSCREEN_VISIBLE )
} // end Menu_ReportItemInMorseCode()


//---------------------------------------------------------------------------
BOOL Menu_CheckLongKeypressToActivateMorse(app_menu_t *pMenu)
{ // Checks for a 'very long' press of the red 'Back' key.
  // Allows visually impaired operators to activate the Morse output
  // without seeing anything on the screen, by holding that key
  // pressed while turning on the radio until the first Morse code is heard.
  lcd_context_t dc;
# define WAITING_TIME_MS 5000
  int ms_left = WAITING_TIME_MS - keypress_timer_ms; 
  int x,y;

  if( !morse_activation_pending )    // decision already made
   { return FALSE;
   }
  if( !(boot_flags & BOOT_FLAG_POLLED_KEYBOARD) )
   { return FALSE; // too early, Tytera's part of the firmware may not have polled the keyboard yet
   }

  if( keypress_ascii != 'B' ) // red 'Back' key NOT pressed anymore ?
   {  morse_activation_pending = FALSE; // operator made his decision
      LOGB("morse output: %d\n", (int)global_addl_config.narrator_mode );
      Menu_Close( pMenu );
      return FALSE; // screen not updated here so allow normal display
   }
  // Arrived here: 'Back' button still pressed; show some info:
  LCD_InitContext( &dc ); // init display context for 'full screen', no clipping
  Menu_GetColours( SEL_FLAG_NONE, &dc.fg_color, &dc.bg_color );
  dc.font = LCD_OPT_FONT_8x16;
  pMenu->visible = APPMENU_VISIBLE;
  if( ms_left <= 0 ) // Interval expired ? Enable morse output with defaults:
   { global_addl_config.narrator_mode = NARRATOR_MODE_ENABLED | NARRATOR_MODE_VERBOSE;
     global_addl_config.cw_pitch_10Hz = 65;
     global_addl_config.cw_volume     = 10;  // CW audible even if vol-pot at zero
     global_addl_config.cw_speed_WPM  = 18;
     LCD_Printf( &dc, "\tMorse Defaults set.\r\r Adjust parameters"\
                      "\r in the setup menu.\r\r Release key now.\r" );
     LCD_FillRect( 0, dc.y, LCD_SCREEN_WIDTH-1, LCD_SCREEN_HEIGHT-1, dc.bg_color );
     morse_activation_pending = FALSE; // operator made his decision
     LOGB("morse output: default\n");
     pMenu->save_on_exit = TRUE; 
     Menu_Close(pMenu);  // Narrator starts talking, blind OM releases the key
     return FALSE;  
   }
  else // show 'progress bar' while waiting for the operator's decision
   { LCD_Printf( &dc, "\tSet Morse Defaults ?\r\r Hold red key down\r to confirm.\r\r" ); 
     x = LCD_SCREEN_WIDTH - 1 - ms_left / (WAITING_TIME_MS/LCD_SCREEN_WIDTH); 
     LimitInteger( &x, 2, LCD_SCREEN_WIDTH-3 );
     y = dc.y + 19;
     LCD_FillRect( 0, dc.y, x, y, dc.fg_color ); 
     LCD_FillRect( x, dc.y, LCD_SCREEN_WIDTH-1, y, dc.bg_color );
     LCD_FillRect( 0, y+1,  LCD_SCREEN_WIDTH-1, LCD_SCREEN_HEIGHT-1, dc.bg_color );
     return TRUE; 
   }
}

#endif // CONFIG_APP_MENU ?
