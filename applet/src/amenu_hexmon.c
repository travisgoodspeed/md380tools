// File:    md380tools/applet/src/amenu_hexmon.c
// Author:  Wolf (DL4YHF) [initial version]
//
// Date:    2017-04-28
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
#include "spiflash.h" // md380_spiflash_read()
#include "amenu_hexmon.h" // header for THIS module (to check prototypes,etc)

uint32_t HexMon_u32StartAddress = 0x2001E780; // "something alive here" (in RAM, on D13.020)
uint32_t HexMon_u32Checksum   = 0;    // checksum of the currently displayed data
int      HexMon_nBytesPerPage = 0x40; // initial assumption, updated in HexMon_Draw()
uint8_t  HexMon_u8DisplayMode = 0;



//---------------------------------------------------------------------------
static void HexMon_Draw(app_menu_t *pMenu, menu_item_t *pItem)
  // Draws the 'hex monitor' screen. Gets visible after entering/confirming
  //  the start address in the 'test / debug' menu (or whatever the final name will be) .
{ int i, n, bytes_per_line, rd_value, font;
  lcd_context_t dc;
  uint32_t addr = HexMon_u32StartAddress; 
  uint8_t b32Temp[32];
  char *cp;

  // Draw the COMPLETE screen, without clearing it initially to avoid flicker
  LCD_InitContext( &dc ); // init context for 'full screen', no clipping
  Menu_GetColours( SEL_FLAG_NONE, &dc.fg_color, &dc.bg_color );
  
  switch( HexMon_u8DisplayMode ) 
   { case 1 :  // 8-bit ASCII display:
        cp = "ASCII";
        // 160 pixels per line, 8 pixels per char, 20 chars fit in a line,
        // 3 characters for 2-digit address + ':' + 16 DATA BYTES per line
        font = LCD_OPT_FONT_8x8; // contains 256 different characters
        bytes_per_line = 16;
        break;
     case 2 :  // 16-bit 'wide char' display:
        cp = "WCHAR"; // upper 8 bits in each 16-bit character are simply ignored,
        font = LCD_OPT_FONT_8x8; // .. because this font only contains 256 chars
        bytes_per_line = 32;
        break;
     default:  // normal hex display:
        cp = "HexMon";
        // 160 pixels per line, 6 pixels per char, 3 chars per hex-byte (incl. space)
        // -> just enough space for 8 hex-bytes per line with only two address digits
        font = LCD_OPT_FONT_6x12;
        bytes_per_line = 8;
        break;    
   } // end switch( HexMon_u8DisplayMode )

  dc.font = LCD_OPT_FONT_8x8;
  LCD_Printf( &dc, "\t%s %08X\r", cp, (int)addr ); // title, centered, opaque, full line
  dc.font = font;
  n = 0;         // count the number of bytes per page (depends on the used font)
  while( dc.y < LCD_SCREEN_HEIGHT )
   { dc.x = 0;
     LCD_Printf( &dc, "%02X:", (int)(addr & 0x00FF) );
     if( (addr>=HEXMON_DUMMY_ADDRESS_SPI_FLASH_START) && (addr<=HEXMON_DUMMY_ADDRESS_SPI_FLASH_END) )
      { // Read from SPI flash (not directly accessable like RAM or internal Flash) :
        md380_spiflash_read( b32Temp, addr-HEXMON_DUMMY_ADDRESS_SPI_FLASH_START, bytes_per_line );
      }
     i = 0;
     while(i<bytes_per_line)
      {
        if( (addr>=HEXMON_DUMMY_ADDRESS_SPI_FLASH_START) && (addr<=HEXMON_DUMMY_ADDRESS_SPI_FLASH_END) )
         { // Do NOT calculate a checksum when inspecting the SPI-flash !
           rd_value = b32Temp[i];
         }
        else // address outside the 'dummy address range' for Flash ->
         { rd_value = Menu_ReadIntFromPtr( (void *)addr, DTYPE_UNS8 );
         }

        switch( HexMon_u8DisplayMode ) 
         { case 1 :  // ASCII-only display (8 bits per character) :
              dc.x = LCD_DrawCharAt( (char)rd_value, dc.x, dc.y, dc.fg_color, dc.bg_color, font );
              break;
           case 2 :  // wide-char display (with the upper 8 bits ignored) :
              dc.x = LCD_DrawCharAt( (char)rd_value, dc.x, dc.y, dc.fg_color, dc.bg_color, font );
              ++addr; // incremented TWICE for each char..
              ++i;
              ++n;
              break;
           default:  // normal hex display:
              if( rd_value < 0 ) // looks like a non-accessable address
               { LCD_Printf( &dc, "-- " );
               }
              else
               { LCD_Printf( &dc, "%02X ", rd_value );
               }
              break;    
         } // end switch( HexMon_u8DisplayMode )
        ++addr;
        ++i;
        ++n;
      }
     // '\r' clears the *R*est of the line, before entering the *N*ew line:
     LCD_Printf( &dc, "\r" ); 
   } 
  // If necessary, fill the rest of the screen (at the bottom) with the background colour:
  LCD_FillRect( 0, dc.y, LCD_SCREEN_WIDTH-1, LCD_SCREEN_HEIGHT-1, dc.bg_color );
  HexMon_nBytesPerPage = n; // remember this for the checksum and page up/down
  pMenu->redraw = FALSE;    // "done" (screen has been redrawn)
} // HexMon_Draw()


//---------------------------------------------------------------------------
int am_cbk_HexMon(app_menu_t *pMenu, menu_item_t *pItem, int event, int param )
  // Callback function, invoked from the "app menu" framework .
{
  int n;
  uint16_t two_bytes;
  uint32_t addr, checksum; 
  switch( event ) // what happened, why did the menu framework call us ?
   { case APPMENU_EVT_END_EDIT: // the operator finished or aborted editing,
        if( param ) // "finished" inputting the start address 
         { return AM_RESULT_OCCUPY_SCREEN;
         } // end if < FINISHED (not ABORTED) editing >
        return AM_RESULT_OK; // "event was processed HERE"
     case APPMENU_EVT_PAINT : // paint into the framebuffer here ?
        if( pMenu->visible == APPMENU_USERSCREEN_VISIBLE ) // only if HexMon already 'occupied' the screen !
         { // To minimize QRM from the display cable, only redraw the screen
           // if any of the displayed bytes has been modified.
           // When running at full pace, almost 100 screen updates per second
           // are possible, but you wouldn't hear anyting in FM with the 
           // rubber-duck antenna due to the activity on the 'LCD data bus' !
           // Calculating a checksum causes no QRM because the LCD bus is passive.
           addr     = (HexMon_u32StartAddress+1) & ~1;
           if( (addr>=HEXMON_DUMMY_ADDRESS_SPI_FLASH_START) && (addr<=HEXMON_DUMMY_ADDRESS_SPI_FLASH_END) )
            { // Do NOT calculate a checksum when inspecting the SPI-flash !
              checksum=HexMon_u32Checksum;
            }
           else
            { // only if NOT within the dummy-address-range for the SPI flash ...
              checksum = 0xFFFF;
              n = HexMon_nBytesPerPage / 2;
              while(n--) // check for modified values in the displayed range
               { two_bytes= Menu_ReadIntFromPtr( (void *)addr, DTYPE_UNS16 ); // read two bytes in a row
                 addr += 2;
                 checksum = CRC16( checksum, &two_bytes, 1/*nWords!!*/ );
               }
            }
           if( pMenu->redraw || (checksum!=HexMon_u32Checksum) )
            { HexMon_Draw(pMenu, pItem); 
            }
           HexMon_u32Checksum = checksum;
           return AM_RESULT_OCCUPY_SCREEN; // keep the screen 'occupied'
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
                 HexMon_u32StartAddress -= HexMon_nBytesPerPage;
                 pMenu->redraw = TRUE;
                 break;
              case 'D' :  // cursor DOWN: here "page down"
                 HexMon_u32StartAddress += HexMon_nBytesPerPage;
                 pMenu->redraw = TRUE;
                 break;
              default:    // Other keys : switch between different editing modes
                 if( param>='0' && param<='9' )
                  { HexMon_u8DisplayMode = param-'0';
                    pMenu->redraw = TRUE;
                  }
                 break;
            } // end switch < key >
           return AM_RESULT_OCCUPY_SCREEN; // keep the screen 'occupied'
         }
        break;
     default: // all other events are not handled here (let the sender handle them)
        break;
   } // end switch( event )
  return AM_RESULT_NONE; // "proceed as if there was NO callback function"
} // end am_cbk_HexMon()


#endif // CONFIG_APP_MENU ?
