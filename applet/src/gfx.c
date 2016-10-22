/*! \file gfx.c
  \brief Graphics wrapper functions.
*/

#define DEBUG

#include "md380.h"
#include "version.h"
#include "tooldfu.h"
#include "config.h"
#include "gfx.h"
#include "printf.h"
#include "string.h"
#include "addl_config.h"
#include "display.h"
#include "console.h"
#include "netmon.h"
#include "debug.h"

//Needed for LED functions.  Cut dependency.
#include "stm32f4_discovery.h"
#include "stm32f4xx_conf.h" // again, added because ST didn't put it here ?

//! Draws text at an address by calling back to the MD380 function.
void drawtext(wchar_t *text,
	      int x, int y){
#ifdef CONFIG_GRAPHICS
  gfx_drawtext(text,
	       0,0,
	       x,y,
	       15); //strlen(text));
#endif
}
//! Draws text at an address by calling back to the MD380 function.
void drawascii(char *ascii,
	       int x, int y){
  //Widen the string.  We really ought to do proper lengths.
  wchar_t wide[15];
  for(int i=0;i<15;i++)
    wide[i]=ascii[i];

#ifdef CONFIG_GRAPHICS
  //Draw the wide string, not the original.
  gfx_drawtext(wide,
	       0,0,
	       x,y,
	       15); //strlen(text));
#endif
}

void drawascii2(char *ascii,
                int x, int y){
  wchar_t wide[40];

  for(int i=0;i<25;i++)
        {
        wide[i]=ascii[i];
        if (ascii[i]=='\0')
           break;
        }
#ifdef CONFIG_GRAPHICS
  gfx_drawtext2(wide, x, y, 0);
  //con_redraw();
#endif
}

void green_led(int on) {
  if (on) {
    GPIO_SetBits(GPIOE, GPIO_Pin_0);
  } else {
    GPIO_ResetBits(GPIOE, GPIO_Pin_0);
  }
}


void red_led(int on) {
  /* The RED LED is supposed to be on pin A0 by the schematic, but in
     point of fact it's on E1.  Expect more hassles like this.
  */

  if (on) {
    GPIO_SetBits(GPIOE, GPIO_Pin_1);
  } else {
    GPIO_ResetBits(GPIOE, GPIO_Pin_1);
  }
}

void lcd_background_led(int on) {
  if (on) {
    GPIO_SetBits(GPIOC, GPIO_Pin_6);
  } else {
    GPIO_ResetBits(GPIOC, GPIO_Pin_6);
  }
}

/*
void dump_ram_to_spi_flash() {
  static int run = 0;
  if ( run == 100) {
    printf("dump\n");
    for ( int i=0; i < (112+16); i++) {
      md380_spiflash_write((void *) 0x20000000+(1024*i), 0x400000+(1024*i), 1024);
    }
  }
  run++;
}

*/

void print_date_hook(void) 
{  // copy from the md380 code
    
    if( is_netmon_visible() ) {
        return ;
    }
    
#ifdef CONFIG_GRAPHICS
  wchar_t wide[40];
  RTC_DateTypeDef RTC_DateStruct;
  md380_RTC_GetDate(RTC_Format_BCD, &RTC_DateStruct);

    switch( global_addl_config.datef ) {
        default:
            // fallthrough
        case 0 :
            wide[0]='2';
            wide[1]='0';
            md380_itow(&wide[2], RTC_DateStruct.RTC_Year);
            wide[4]='/';
            md380_itow(&wide[5], RTC_DateStruct.RTC_Month);
            wide[7]='/';
            md380_itow(&wide[8], RTC_DateStruct.RTC_Date);
            break ;
        case 1 :
            md380_itow(&wide[0], RTC_DateStruct.RTC_Date);
            wide[2]='.';
            md380_itow(&wide[3], RTC_DateStruct.RTC_Month);
            wide[5]='.';
            wide[6]='2';
            wide[7]='0';
            md380_itow(&wide[8], RTC_DateStruct.RTC_Year);
            break ;
        case 2 :
            md380_itow(&wide[0], RTC_DateStruct.RTC_Date);
            wide[2]='/';
            md380_itow(&wide[3], RTC_DateStruct.RTC_Month);
            wide[5]='/';
            wide[6]='2';
            wide[7]='0';
            md380_itow(&wide[8], RTC_DateStruct.RTC_Year);
            break ;
        case 3 :
            md380_itow(&wide[0], RTC_DateStruct.RTC_Month);
            wide[2]='/';
            md380_itow(&wide[3], RTC_DateStruct.RTC_Date);
            wide[5]='/';
            wide[6]='2';
            wide[7]='0';
            md380_itow(&wide[8], RTC_DateStruct.RTC_Year);    
            break ;
        case 4 :
            wide[0]='2';
            wide[1]='0';
            md380_itow(&wide[2], RTC_DateStruct.RTC_Year);
            wide[4]='-';
            md380_itow(&wide[5], RTC_DateStruct.RTC_Month);
            wide[7]='-';
            md380_itow(&wide[8], RTC_DateStruct.RTC_Date);
            break ;
  }

  wide[10]='\0';
  gfx_chars_to_display( wide, 0xa, 0x60, 0x5e);

//  dump_ram_to_spi_flash();

//  gfx_drawbmp((char *) &bmp_eye, 20, 2);
#endif //CONFIG_GRAPHICS
}

// deprecated, left for other versions.
void print_ant_sym_hook(char *bmp, int x, int y)
{
#ifdef CONFIG_GRAPHICS
    gfx_drawbmp(bmp, x, y);
    draw_eye_opt();
#endif
}

/**
 * 
 * @param x_from 0...159
 * @param y_from 0...127
 * @param x_to   0...159
 * @param y_to   0...127
 */

void gfx_blockfill_hook(int x_from, int y_from, int x_to, int y_to)
{
//    if( ymin == 0 && xmin == 61 ) {
//        if( global_addl_config.promtg ) {
//            return ;
//        }
//    }
    
//    PRINTRET();
//    PRINT( "bf: %d %d %d %d\n", x_from, y_from, x_to, y_to );
    
    if( y_from == 0 && x_from == 61 ) {
        con_redraw();
    }
    if( is_netmon_visible() ) {
        // no blockfills
        return ;
    }
    
    gfx_blockfill(x_from,y_from,x_to,y_to);
    
    if( y_from == 0 && x_from == 61 ) {
        // if we have stat var for detecting first draw....
        // we could clear by blockfill only once.
        if( global_addl_config.promtg ) {
            draw_eye_opt();
        }
    }
}

void gfx_drawbmp_hook( void *bmp, int x, int y )
{
//    PRINTRET();
//    PRINT( "db: %d %d\n", x, y );
    
    // supress bmp drawing in console mode.
    if( is_netmon_visible() ) {
        if( x == 0 && y == 0 ) {
            // antenne icon draw.
            con_redraw();
        }
        return ;
    }
    gfx_drawbmp( bmp, x, y );
}

// r0 = str, r1 = x, r2 = y, r3 = xlen
void gfx_chars_to_display_hook(wchar_t *str, int x, int y, int xlen)
{
    // filter datetime (y=96)
    if( y != 96 ) {
//        PRINTRET();
//        PRINT("ctd: %d %d %d %S\n", x, y, xlen, str);
    }
    
    if( is_netmon_visible() ) {
        return ;
    }
    
    gfx_chars_to_display(str, x, y, xlen);
}

void gfx_drawtext4_hook(wchar_t *str, int x, int y, int xlen, int ylen)
{
//    PRINTRET();
//    PRINT("dt4: %d %d %d %d %S (%x)\n", x, y, xlen, ylen, str, str);
    
    if( is_netmon_visible() ) {
        if( x == 45 && y == 34 ) {
            return ;
        }
        if( x == 34 && y == 75 ) {
            return ;
        }
    }
    
#if defined(FW_D13_020)        
    gfx_drawtext4(str,x,y,xlen,ylen);
#else
#warning should find symbol gfx_drawtext4        
#endif    
}

uint32_t gfx_get_fg_color(void)
{
#if defined(FW_D02_032)
    return 0x0000ff; // Default fg color
#else
    return gfx_info.fg_color;
#endif
}
