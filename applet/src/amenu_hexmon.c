// File:    md380tools/applet/src/amenu_hexmon.c
// Author:  Wolf (DL4YHF) [initial version]
//
// Date:    2017-04-23
//  Simple 'hex monitor' to inspect blocks of memory.
//  Implemented as a callback function for the 'application menu' (app_menu.c) .

#include "config.h"

#if (CONFIG_APP_MENU) // this module is only available along with app_menu.c ...

#include <stm32f4xx.h>
#include <string.h>
#include "irq_handlers.h"
#include "lcd_driver.h"
#include "app_menu.h" // 'simple' alternative menu activated by red BACK-button
#include "printf.h"
#include "amenu_hexmon.h" // header for THIS module (to check prototypes,etc)

uint32_t HexMon_u32StartAddress = 0x2001E780; // "something alive here" (in RAM, on D13.020)

//---------------------------------------------------------------------------
static void HexMon_Draw(app_menu_t *pMenu, menu_item_t *pItem)
  // Draws the 'hex monitor' screen. Gets visible after entering/confirming
  //  the start address in the 'test / debug' menu (or whatever the final name will be) .
{ int x, y, i, rd_value, font;
  char *cp;
  uint16_t fg_color, bg_color;
  uint32_t addr = HexMon_u32StartAddress; 

  // Draw the COMPLETE screen, without clearing it initially to avoid flicker
  Menu_GetColours( SEL_FLAG_NONE, &fg_color, &bg_color );
  
  LCD_PrintfAt( 0/*x*/,0/*y*/, fg_color, bg_color, LCD_OPT_FONT_8x8,
                "\tHexMon %08X\r", (int)addr );
  y = LCD_pos_y; // after '\r' or '\n', output position for the next line
  font = LCD_OPT_FONT_6x12;
  while( y < LCD_SCREEN_HEIGHT )
   { x = LCD_PrintfAt( 0/*x*/,y, fg_color, bg_color, font, "%02X:", (int)(addr & 0x00FF) );
     // 160 pixels per line, 8 pixels per char, 20 chars per line, 
     //  3 chars per hex-byte (incl. space), 8 hex-bytes per line:
     for(i=0; i<8; ++i)
      { rd_value = Menu_ReadIntFromPtr( (void *)addr++, DTYPE_UNS8 );
        if( rd_value < 0 ) // looks like a non-accessable address
         { x = LCD_PrintfAt( x,y, fg_color, bg_color, font, "-- " );
         }
        else
         { x = LCD_PrintfAt( x,y, fg_color, bg_color, font, "%02X ", rd_value );
         }
      }
     // '\r' clears the *R*est of the line, before entering the *N*ew line:
     LCD_PrintfAt( x,y, fg_color, bg_color, font, "\r" ); 
     y = LCD_pos_y; // after '\r' or '\n', output position for the next line
   } 
  // If necessary, fill the rest of the screen (at the bottom) with the background colour:
  LCD_FillRect( 0, y, LCD_SCREEN_WIDTH-1, LCD_SCREEN_HEIGHT-1, bg_color );
  pMenu->redraw = FALSE; // "done"
} // HexMon_Draw()


//---------------------------------------------------------------------------
int am_cbk_HexMon(app_menu_t *pMenu, menu_item_t *pItem, int event, int param )
  // Callback function, invoked from the "app menu" framework .
{
  switch( event ) // what happened, why did the menu framework call us ?
   { case APPMENU_EVT_END_EDIT: // the operator finished or aborted editing,
        if( param ) // "finished" inputting the start address 
         { return AM_RESULT_OCCUPY_SCREEN;
         } // end if < FINISHED (not ABORTED) editing >
        return AM_RESULT_OK; // "event was processed HERE"
     case APPMENU_EVT_PAINT : // paint into the framebuffer here ?
        if( pMenu->visible == APPMENU_USERSCREEN_VISIBLE ) // only if HexMon already 'occupied' the screen !
         { HexMon_Draw(pMenu, pItem); 
           return AM_RESULT_OCCUPY_SCREEN; // keep the screen 'occupied' as long as we need
         }
        break;
     case APPMENU_EVT_KEY : // own keyboard control only if the screen is owned by HexMon : 
        if( pMenu->visible == APPMENU_USERSCREEN_VISIBLE ) // only if HexMon already 'occupied' the screen !
         { switch( (char)param ) // here: message parameter = keyboard code (ASCII)
            {
              case 'M' :  // green "Menu" key : kind of ENTER. But here, EXIT ;)
              case 'B' :  // red "Back"-key : return from this screen.
                 return AM_RESULT_EXIT_AND_RELEASE_SCREEN;
              case 'U' :  // cursor UP : here "page up"
                 HexMon_u32StartAddress -= 0x40;
                 pMenu->redraw = TRUE;
                 break;
              case 'D' :  // cursor DOWN: here "page down"
                 HexMon_u32StartAddress += 0x40;
                 pMenu->redraw = TRUE;
                 break;
              default:    // Other keys .. editing or treat as a hotkey ?
                 break;
            } // end switch < key >
           return AM_RESULT_OCCUPY_SCREEN; // keep the screen 'occupied' as long as we need
         }
        break;
     default: // all other events are not handled here (let the sender handle them)
        break;
   } // end switch( event )
  return AM_RESULT_NONE; // "proceed as if there was NO callback function"
} // end am_cbk_HexMon()


#endif // CONFIG_APP_MENU ?
