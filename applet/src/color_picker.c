// File:    md380tools/applet/src/color_picker.c
// Author:  Wolf (DL4YHF) [initial version] 
//
// Date:    2017-04-17
//  Implements a simple 'colour picker' dialog for the alterative menu.
//  For details and usage, see applet/src/app_menu.c .
//  Intended as a simple example for own, simple dialog screens .

#include "config.h"

#include <stm32f4xx.h>
#include <string.h>
#include "lcd_driver.h"   // alternative LCD driver (DL4YHF), doesn't depend on 'gfx'
#include "app_menu.h"     // 'simple' alternative menu activated by red BACK-button


//---------------------------------------------------------------------------
static void ColorPicker_DrawBargraph(int y1, int y2, int index, int value)
{
  uint16_t wColor;
  int y  = y2 - value * (y2-y1) / 255/*max_value*/;
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
  int x = 0; // kind of 'graphic output cursor', but just a local var
  int y = 0;
  int y2;
  int i;
  uint16_t wColor, wSelColor;
  uint8_t rgb[3];
  
  
  uint16_t fg_color, bg_color;
  // get the two drawing colours, for non-selected text: 
  Menu_GetColours( SEL_FLAG_NONE, &fg_color, &bg_color );
  x = LCD_DrawStringAt( "  Mix Color", x, y, fg_color, bg_color,
                        LCD_OPT_FONT_12x24 | LCD_OPT_CLEAR_EOL );
  y += 24; // -> upper edge of the three vertical colour bars
  y2 = LCD_SCREEN_HEIGHT-32-1;
  // Split the 'edited' colour into red, green, blue; each ranging from 0 to 255 .
  // In this case, the menu's "edit value" (apm_menu_t.iEditValue) is
  // a colour value in the LCD CONTROLLER'S NATIVE FORMAT, so let the
  // hardware-specific driver split it into red, green, blue :
  wColor = (uint16_t)pMenu->iEditValue;
  LCD_NativeColorToRGB( wColor, &rgb[0], &rgb[1], &rgb[2] ); 
  for(i=0; i<=2; ++i )
   { ColorPicker_DrawBargraph(y, y2, i, rgb[i] );
   }
  // Show the "resulting colour mix" :
  y  = y2+1;
  y2 = LCD_SCREEN_HEIGHT-1;
  LCD_FillRect( 0, y, LCD_SCREEN_WIDTH-1, y2, (uint16_t) wColor );
  y += 4;
  x = (LCD_SCREEN_WIDTH/6) - (3*6)/2; // text centered under 1st bar
  wSelColor = (pMenu->dialog_field_index==0) ? wColor : ~wColor; 
  LCD_DrawStringAt( "1:R", x, y, wSelColor, ~wSelColor, // ~complementary colour, "always visible"
                    LCD_OPT_FONT_6x12 );
  x += LCD_SCREEN_WIDTH/3;
  wSelColor = (pMenu->dialog_field_index==1) ? wColor : ~wColor; 
  LCD_DrawStringAt( "2:G", x, y, wSelColor, ~wSelColor,
                    LCD_OPT_FONT_6x12 );
  x += LCD_SCREEN_WIDTH/3;
  wSelColor = (pMenu->dialog_field_index==2) ? wColor : ~wColor; 
  LCD_DrawStringAt( "3:B", x, y, wSelColor, ~wSelColor,
                    LCD_OPT_FONT_6x12 );
  
  pMenu->redraw = FALSE; // framebuffer is up-to-date, no need to redraw now 
}

//---------------------------------------------------------------------------
static void ColorPicker_IncDec(app_menu_t *pMenu, int delta )
{
  uint8_t rgb[3];
  int n, iField = pMenu->dialog_field_index;
  LCD_NativeColorToRGB( (uint16_t)pMenu->iEditValue, &rgb[0], &rgb[1], &rgb[2] ); 
  if( iField>=0 && iField<=2)
   { // RGB components range from 0 to 255 but the LCD suports BRG565 only.
     // With 5 bits resolution, there are 32 intensities, 256/32 = 8, thus:
     n = (int)rgb[iField] + 8 * delta;
     LimitInteger( &n, 0, 255 );
     rgb[iField] = n;
     pMenu->iEditValue = LCD_RGBToNativeColor( rgb[0], rgb[1], rgb[2] ); 
   }

} // end ColorPicker_IncDec()


//---------------------------------------------------------------------------
int am_cbk_ColorPicker(app_menu_t *pMenu, menu_item_t *pItem, int event, int param )
  // This is the dialog's callback function, invoked from the "app menu" framework
  // whenever something happened (which may be a period screen update, key, etc).
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
