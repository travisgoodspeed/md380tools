// File:    md380tools/applet/src/color_picker.c
// Author:  Wolf (DL4YHF) [initial version] 
//
// Date:    2017-04-23
//  Implements a simple 'colour picker' dialog for the app-menu.
//  For details and usage, see applet/src/app_menu.c .

#include "config.h"

#if (CONFIG_APP_MENU) // only available in combination with app_menu.c

#include <stm32f4xx.h>   // <- fancy stuff, 'uint8_t', etc. I miss my BYTE .... ..
#include <string.h>
#include "lcd_driver.h"  // alternative LCD driver (DL4YHF), doesn't depend on 'gfx'
#include "app_menu.h"    // 'simple' alternative menu activated by red BACK-button
#include "addl_config.h" // customizeable menu colours stored in global_addl_config


//---------------------------------------------------------------------------
static void ColorPicker_DrawBargraph(int y1, int y2, int index, int value)
{
  uint16_t wColor;
  int y  = y2 - value * (y2-y1-2) / 255/*max_value*/;
  int x1 = index * LCD_SCREEN_WIDTH / 3;
  int x2 = x1 + LCD_SCREEN_WIDTH / 3;
  int w3 = LCD_SCREEN_WIDTH / 8;

  LimitInteger( &x2, x1, LCD_SCREEN_WIDTH-1 );
  LimitInteger( &y,  y1, y2-1/*!*/ );

  switch( index )
   { case 0 : wColor = LCD_COLOR_RED;   break;
     case 1 : wColor = LCD_COLOR_GREEN; break;
     default: wColor = LCD_COLOR_BLUE;  break;
   }

  LCD_FillRect( x1, y1, x2, y2, LCD_COLOR_BLACK ); // erase background (moderate flicker..)
  LCD_FillRect( x1+w3, y, x2-w3, y2, wColor );     // draw foreground (poor example)

  
} // end ColorPicker_DrawBargraph()


//---------------------------------------------------------------------------
static void ColorPicker_Draw(app_menu_t *pMenu, menu_item_t *pItem)
{ // For simplicity, the "colour picker" always redraws its screen COMPLETELY.
  int x, y, y2, i, sel;
  char *cp;
  uint16_t color[2]; // [0] = fg_color, [1] = bg_color, but indexable
  rgb_quad_t rgb;
  lcd_context_t dc;

  // Draw the COMPLETE screen, without clearing it initially to avoid flicker
  LCD_InitContext( &dc ); // init context for 'full screen', no clipping
  color[1] = (uint16_t)pMenu->iEditValue; // background colour = "mixed sample"
  color[0] = LCD_GetGoodContrastTextColor( color[1] ); // black or white text,
         // whichever gives the better contrast for a given background colour
  dc.fg_color = color[0];
  dc.bg_color = color[1];
  dc.font = LCD_OPT_FONT_8x16;
  LCD_Printf( &dc, "\tRGB Colour Mixer\r" ); // title line, opaque, centered
  cp = Menu_GetParamsFromItemText( (char*)pItem->pszText, NULL,NULL,NULL ); // skip formatting info
  if( cp != NULL )
   { dc.font = LCD_OPT_FONT_8x8;
     LCD_Printf( &dc, "\t%s  %04X\r", cp/*menu item*/, (int)pMenu->iEditValue/*hex colour*/ );
   }
  // Split the colour into red, green, blue; each ranging from 0 to 255 :
  rgb.u32 = LCD_NativeColorToRGB( color[1] ); 
  // Show the "resulting colour mix" in 3 vertical bargraphs:
  y  = dc.y;                 // upper end (after printing the title)
  y2 = LCD_SCREEN_HEIGHT-22; // lower end of the 3 bargraphs
  for(i=0; i<=2; ++i )
   { ColorPicker_DrawBargraph( y, y2, i, rgb.ba[2-i/*!*/] );
   }
  y2++;
  LCD_FillRect( 0, y2, LCD_SCREEN_WIDTH-1, LCD_SCREEN_HEIGHT-1, color[1] );
  y = y2+2; // vertical position of the info boxes inside the above "colour mix field".
  // First filling the background, and then printing over it causes some moderate
  // flicker, but the screen is only redrawn if the edit value changed... don't worry.
  x = dc.x = (LCD_SCREEN_WIDTH/6) - (3*6)/2; // text centered under 1st bar
  sel = (pMenu->dialog_field_index==0); 
  // To show which of the three bars (colour components) are currently selected,
  // simply flip forward and backward colour (appears like a marked block, 3*2 chars).
  dc.fg_color = color[sel];
  dc.bg_color = color[!sel];
  dc.font = LCD_OPT_FONT_8x8;
  dc.y = y;
  LCD_Printf( &dc, "1:R\n%03d",(int)rgb.s.r );
  x += LCD_SCREEN_WIDTH/3;
  dc.x = x;
  sel = (pMenu->dialog_field_index==1); 
  dc.fg_color = color[sel];
  dc.bg_color = color[!sel];
  dc.y = y;
  LCD_Printf( &dc, "2:G\n%03d",(int)rgb.s.g );
  x += LCD_SCREEN_WIDTH/3;
  dc.x = x;
  sel = (pMenu->dialog_field_index==2); 
  dc.fg_color = color[sel];
  dc.bg_color = color[!sel];
  dc.y = y;
  LCD_Printf( &dc, "3:B\n%03d",(int)rgb.s.b );
  pMenu->redraw = FALSE; // "done"
}

//---------------------------------------------------------------------------
static void ColorPicker_IncDec(app_menu_t *pMenu, int delta )
{ // Called when modifying one of the 3 colour components
  rgb_quad_t rgb;
  int n, iField = pMenu->dialog_field_index;
  rgb.u32 = LCD_NativeColorToRGB( (uint16_t)pMenu->iEditValue ); 
  if( iField>=0 && iField<=2)
   { // RGB components range from 0 to 255 but the LCD suports BRG565 only.
     // With 5 bits resolution, 32 intensities are enough, 256/32 = 8, thus:
     n = (int)rgb.ba[2-iField] + 8 * delta; // not super-precise but sufficient
     LimitInteger( &n, 0, 255 );
     rgb.ba[2-iField] = n; // 2-iField because .ba[0] is BLUE, not RED
     pMenu->iEditValue = LCD_RGBToNativeColor( rgb.u32 ); 
   }
} // end ColorPicker_IncDec()


//---------------------------------------------------------------------------
int am_cbk_ColorPicker(app_menu_t *pMenu, menu_item_t *pItem, int event, int param )
  // This is the dialog's callback function, invoked from the "app menu" framework
  // whenever something happened ( screen update, keypress, etc ).
{
  switch( event ) // what happened, why did the menu framework call us ?
   { case APPMENU_EVT_PAINT : // .. because it allows us to paint, or..
        if( pMenu->redraw )   // .. because the screen MUST be redrawn (e.g. "first call")
         { ColorPicker_Draw(pMenu, pItem); 
         }
        return AM_RESULT_OCCUPY_SCREEN;
     case APPMENU_EVT_ENTER : // just ENTERED the dialog 
        // (by pressing ENTER in a menu, with an item using this callback function)
        // Typically used to prepare items shown in a SUBMENU. 
        if( pMenu->dialog_field_index<0 || pMenu->dialog_field_index>2 )
         { pMenu->dialog_field_index = 0; // begin with the first 'dialog field', here: RED component
         }
        return AM_RESULT_OCCUPY_SCREEN; // "occupy" the screen (framebuffer), owned by this dialog now
     case APPMENU_EVT_KEY: // keyboard event (only sent to callbacks that occupied the screen)
        switch( (char)param ) // here: message parameter = keyboard code (ASCII)
         {
           case 'M' :  // green "Menu" key : kind of ENTER. 
              // Here: "apply" the new colour mix, and write it back,
              // using the same submenu as the the 'app menu' after numeric input:
              Menu_FinishEditing( pMenu, pItem ); // [in] pMenu->iEditValue [out] *pItem->pvValue
              return AM_RESULT_EXIT_AND_RELEASE_SCREEN;
           case 'B' :  // red "Back"-key : close this dialog...
              // .. without writing back the edited colour into pItem->pvValue: 
              return AM_RESULT_EXIT_AND_RELEASE_SCREEN;
           case 'U' :  // cursor UP : inc selected component (R,G,B)
              ColorPicker_IncDec( pMenu, 1/*delta*/ );
              break;
           case 'D' :  // cursor DOWN: dec selected component (R,G,B)
              ColorPicker_IncDec( pMenu, -1/*delta*/ );
              break;
           case '1' :
              pMenu->dialog_field_index=0; // control the RED component now
              break;
           case '2' :
              pMenu->dialog_field_index=1; // control the GREEN component now
              break;
           case '3' :
              pMenu->dialog_field_index=2; // control the BLUE component now
              break;
           default:    // Other keys .. editing or treat as a hotkey ?
              break;
         } // end switch < key >
        pMenu->redraw = TRUE;
        break;
     default: // all other events are not handled here (let the sender handle them)
        return AM_RESULT_NONE; // "proceed as if there was NO callback function"
   } // end switch( event )
  return AM_RESULT_OCCUPY_SCREEN; // "processed the event HERE; keep the screen occupied"
} // end am_cbk_ColorPicker()

//---------------------------------------------------------------------------
// 'pre-defined' colour palettes, composed with the above 'colour picker' 
//---------------------------------------------------------------------------

typedef struct tColorScheme
{ const char *pszName;     // descriptive name of the palette, e.g. "Cappucino" 
  uint16_t fg_color;       // normal text colour (used when neither selected/marked nor editing)
  uint16_t bg_color;       // normal background colour
  uint16_t sel_fg_color;   // colours used for the 'navigation' bar / selected items
  uint16_t sel_bg_color;
  uint16_t edit_fg_color;  // colours used for the 'edit cursor' (or the entired field in inc/dec edit mode)
  uint16_t edit_bg_color;
} color_scheme_t;

const color_scheme_t color_schemes[] = 
{ // name              fg_color bg_color sel_fg  sel_bg  edit_fg  edit_bg
  { "Black on white",  0x0000,  0xFFFF,  0xFFFF, 0xF800, 0xFFFF,  0x001F },
  { "Cappucino",       0x000D,  0x3C3F,  0x027F, 0x0192, 0x6DBF,  0x019F },
  { "Cobalt",          0xEE11,  0x38C3,  0x5DFF, 0x7AC4, 0x06FF,  0x9940 },
  { "Greenhouse",      0x1FF1,  0x0100,  0x03E6, 0x6FE0, 0x0AA0,  0x2604 }

};

//---------------------------------------------------------------------------
static void ColorSchemes_Draw(app_menu_t *pMenu, menu_item_t *pItem)
  // Draws the 'Color Scheme' (aka 'palette') sample/selection screen. 
  // 
{ int i;
  lcd_context_t dc;
  char cMarker;
  scroll_list_control_t *pSL = &pMenu->scroll_list;
  const color_scheme_t *pScheme, *pCurrScheme;

  // Draw the COMPLETE screen, without clearing it initially to avoid flicker
  LCD_InitContext( &dc ); // init context for 'full screen', no clipping
  if( pSL->focused_item>=0 && pSL->focused_item<pSL->num_items )
   { // use the currently focused colour scheme to draw the title
     pCurrScheme = &color_schemes[pSL->focused_item];
   }
  else // use the first scheme ('Tytera-like') for the title
   { pCurrScheme = &color_schemes[0];
   }
  dc.fg_color = pCurrScheme->fg_color; 
  dc.bg_color = pCurrScheme->bg_color;
  ScrollList_AutoScroll( pSL ); // modify pSL->scroll_pos to make the FOCUSED item visible
  dc.font = LCD_OPT_FONT_12x24;
  LCD_Printf( &dc, "\tColour Scheme\r" ); // '\t'=horz. center, '\r' = clear to end of line, then enter next line
  LCD_HorzLine( dc.x1, dc.y++, dc.x2, dc.fg_color ); // spacer below title
  LCD_HorzLine( dc.x1, dc.y++, dc.x2, dc.bg_color );
  i = pSL->scroll_pos;
  while( (dc.y <= (LCD_SCREEN_HEIGHT-2*16)) && (i<pSL->num_items) )
   { pScheme = &color_schemes[i];
     cMarker = (i==pSL->focused_item) ? 0x1A : 0x20; // codepage 437: arrow pointing right, or space
     dc.x = 0;
     dc.font = LCD_OPT_FONT_8x16;
     dc.fg_color = pScheme->fg_color;
     dc.bg_color = pScheme->bg_color;
     LCD_Printf( &dc, "%c %d %s\r", cMarker, i+1, pScheme->pszName );
     dc.fg_color = pScheme->sel_fg_color;
     dc.bg_color = pScheme->sel_bg_color;
     LCD_Printf( &dc, "%c   navigate ", cMarker );
     dc.fg_color = pScheme->edit_fg_color;
     dc.bg_color = pScheme->edit_bg_color;
     LCD_Printf( &dc, " edit\r" );
     i++;
   } 
  // If necessary, fill the rest of the screen (at the bottom) with the background colour:
  LCD_FillRect( 0, dc.y, LCD_SCREEN_WIDTH-1, LCD_SCREEN_HEIGHT-1, pCurrScheme->bg_color );
  pMenu->redraw = FALSE;    // "done" (screen has been redrawn).. except:
} // ColorSchemes_Draw()


//---------------------------------------------------------------------------
int am_cbk_ColorSchemes(app_menu_t *pMenu, menu_item_t *pItem, int event, int param )
  // Callback function, invoked from the "app menu" framework to select a colour palette.
{
  scroll_list_control_t *pSL = &pMenu->scroll_list;
  addl_config_t *pCfg = &global_addl_config;
  const color_scheme_t *pScheme;
  int i;

  // what happened, why did the menu framework call us ?
  if( event==APPMENU_EVT_ENTER ) // pressed ENTER (just entered this screen) ?
   { ScrollList_Init( pSL ); // set all struct members to defaults 
     pSL->num_items    = sizeof(color_schemes) / sizeof(color_schemes[0]);
     pSL->current_item = 0;  // default selection if no match below.
     // if the currently used colour scheme is in the table, select it:
     for(i=0; i<pSL->num_items; ++i ) 
      { pScheme = &color_schemes[i];
        if( (pCfg->fg_color      == pScheme->fg_color)
         && (pCfg->bg_color      == pScheme->bg_color)
         && (pCfg->sel_fg_color  == pScheme->sel_fg_color)
         && (pCfg->sel_bg_color  == pScheme->sel_bg_color)
         && (pCfg->edit_fg_color == pScheme->edit_fg_color)
         && (pCfg->edit_bg_color == pScheme->edit_bg_color) )
         { pSL->current_item = pSL->focused_item = i;
         }
      }
     pSL->n_visible_items = 3;
     return AM_RESULT_OCCUPY_SCREEN; // occupy the entire screen (not just a single line)
   }
  else if(event==APPMENU_EVT_PAINT) // someone wants us to paint into the framebuffer
   { // To minimize QRM from the display cable, only redraw when necessary (no "dynamic" content here):
     if( pMenu->visible == APPMENU_USERSCREEN_VISIBLE ) // only if HexMon already 'occupied' the screen !
      { if( pMenu->redraw )
         { pMenu->redraw = FALSE;
           ColorSchemes_Draw(pMenu, pItem);
         }
        return AM_RESULT_OCCUPY_SCREEN; // keep the screen 'occupied' 
      }
   }
  else if( event==APPMENU_EVT_KEY ) // some other key pressed while focused..
   { switch( (char)param ) // here: message parameter = keyboard code (ASCII)
      {
        case 'M' :  // green "Menu" key : kind of ENTER. Here, "apply & return" .
           if( pSL->focused_item>=0 && pSL->focused_item<(sizeof(color_schemes) / sizeof(color_schemes[0])) )
            { pScheme = &color_schemes[pSL->focused_item];
              global_addl_config.fg_color      = pScheme->fg_color;
              global_addl_config.bg_color      = pScheme->bg_color;
              global_addl_config.sel_fg_color  = pScheme->sel_fg_color;
              global_addl_config.sel_bg_color  = pScheme->sel_bg_color;
              global_addl_config.edit_fg_color = pScheme->edit_fg_color;
              global_addl_config.edit_bg_color = pScheme->edit_bg_color;
              pMenu->save_on_exit = TRUE; // don't save in SPI-flash here yet, but when returning from the app-menu
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
  if( pMenu->visible == APPMENU_USERSCREEN_VISIBLE ) // screen already 'occupied' ?
   { return AM_RESULT_OCCUPY_SCREEN; // keep the screen 'occupied' 
   }
  else
   { return AM_RESULT_NONE; // "proceed as if there was NO callback function"
   }
} // end am_cbk_ColorSchemes()


#endif // CONFIG_APP_MENU ?
