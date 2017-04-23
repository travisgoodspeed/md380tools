// File:    md380tools/applet/src/color_picker.c
// Author:  Wolf (DL4YHF) [initial version] 
//
// Date:    2017-04-17
//  Implements a simple 'colour picker' dialog for the app-menu.
//  For details and usage, see applet/src/app_menu.c .
//  Written as a simple example for simple dialog screens
//  and to customize the colours used for the app-menu itself.

#include "config.h"

#if (CONFIG_APP_MENU) // "colour picker" only available along with app_menu.c

#include <stm32f4xx.h>
#include <string.h>
#include "lcd_driver.h"   // alternative LCD driver (DL4YHF), doesn't depend on 'gfx'
#include "app_menu.h"     // 'simple' alternative menu activated by red BACK-button


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

  // Erasing the (black) background FIRST, 
  // and drawing the red, green, or blue backgraph AFTERWARDS
  // causes minor flickering when a cursor key (with autorepeat)
  // forces frequent screen updates. But it was temptingly simple, so:
  LCD_FillRect( x1, y1, x2, y2, LCD_COLOR_BLACK ); // erase background
  LCD_FillRect( x1+w3, y, x2-w3, y2, wColor );     // draw foreground (poor example)

  
} // end ColorPicker_DrawBargraph()


//---------------------------------------------------------------------------
static void ColorPicker_Draw(app_menu_t *pMenu, menu_item_t *pItem)
{ // For simplicity, the "colour picker" always redraws its screen COMPLETELY.
  int x, y, y2, i, sel;
  char *cp;
  uint16_t color[2]; // [0] = fg_color, [1] = bg_color, but indexable
  rgb_quad_t rgb;

  // Draw the COMPLETE screen, without clearing it initially to avoid flicker
  color[1] = (uint16_t)pMenu->iEditValue; // background colour = "mixed sample"
  color[0] = LCD_GetGoodContrastTextColor( color[1] ); // black or white text,
         // whichever gives the better contrast for a given background colour
  
  LCD_PrintfAt( 0/*x*/,0/*y*/, color[0]/*fg*/, color[1]/*bg*/, LCD_OPT_FONT_8x16,
                "\tRGB Colour Mixer\r" );
  y = LCD_pos_y; // after '\r' or '\n', output position for the next line
  // '\r' works almost like '\n', but fills the rest of the text line with the 
  // specified background colour. Thus no need for "clear screen".
  // '\t' doesn't emit a 'horizontal tab' (ASCII control character) but
  //   HORIZONTALLY CENTERS the text that follows in the string, 
  //   until the end-of-line, end-of-string, or next ASCII control character.
  cp = Menu_GetParamsFromItemText( (char*)pItem->pszText, NULL,NULL,NULL ); // skip formatting info
  if( cp != NULL )
   { LCD_PrintfAt( 0, y, color[0], color[1], LCD_OPT_FONT_8x8, "\t%s\r", cp );
     y = LCD_pos_y; // recommended: store LCD_pos_y immediately after printing.
   }
  // Split the colour into red, green, blue; each ranging from 0 to 255 :
  rgb.u32 = LCD_NativeColorToRGB( color[1] ); 
  // Show the "resulting colour mix" in 3 vertical bargraphs:
  y2 = LCD_SCREEN_HEIGHT-22; // lower end of the 3 bargraphs
  for(i=0; i<=2; ++i )
   { ColorPicker_DrawBargraph( y, y2, i, rgb.ba[2-i/*!*/] );
   }
  y2++;
  LCD_FillRect( 0, y2, LCD_SCREEN_WIDTH-1, LCD_SCREEN_HEIGHT-1, color[1] );
  y = y2+2; // vert position of the info boxes inside the above "colour mix field".
  // First filling the background, and then printing over it causes some moderate
  // flicker, but the screen is only redrawn if the edit value changed... don't worry.
  x = (LCD_SCREEN_WIDTH/6) - (3*6)/2; // text centered under 1st bar
  sel = (pMenu->dialog_field_index==0); 
  // To show which of the three bars (colour components) are currently selected,
  // simply flip forward and backward colour (appears like a marked block, 3*2 chars).
  LCD_PrintfAt( x, y, color[sel], color[!sel], LCD_OPT_FONT_8x8, 
                "1:R\n%03d",(int)rgb.s.r );
  x += LCD_SCREEN_WIDTH/3;
  sel = (pMenu->dialog_field_index==1); 
  LCD_PrintfAt( x, y, color[sel], color[!sel], LCD_OPT_FONT_8x8, 
                "2:G\n%03d",(int)rgb.s.g );
  x += LCD_SCREEN_WIDTH/3;
  sel = (pMenu->dialog_field_index==2); 
  LCD_PrintfAt( x, y, color[sel], color[!sel], LCD_OPT_FONT_8x8, 
                "3:B\n%03d",(int)rgb.s.b );
  pMenu->redraw = FALSE; // "done"
}

//---------------------------------------------------------------------------
static void ColorPicker_IncDec(app_menu_t *pMenu, int delta )
{
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
        return AM_RESULT_OCCUPY_SCREEN; // prevent drawing the "normal" menu screen
     case APPMENU_EVT_ENTER : // just ENTERED the dialog 
        // (by pressing ENTER in a menu, with an item using this callback function)
        // Typically used to prepare items shown in a SUBMENU. 
        pMenu->dialog_field_index = 0;  // begin with the first 'dialog field', here: RED component
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
           case 'U' :  // cursor UP : inc currently selected colour component (R,G,B)
              ColorPicker_IncDec( pMenu, 1/*delta*/ );
              break;
           case 'D' :  // cursor DOWN: dec currently selected colour component (R,G,B)
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
} // end am_cbk_ColorTest()

#endif // CONFIG_APP_MENU ?
