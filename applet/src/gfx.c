/*! \file gfx.c
  \brief Graphics wrapper functions.
*/

#include "md380.h"
#include "version.h"
#include "tooldfu.h"
#include "config.h"
#include "gfx.h"
#include "printf.h"
#include "string.h"
#include "addl_config.h"

//Needed for LED functions.  Cut dependency.
#include "stm32f4_discovery.h"
#include "stm32f4xx_conf.h" // again, added because ST didn't put it here ?



//! Draws text at an address by calling back to the MD380 function.
void drawtext(wchar_t *text,
	      int x, int y){
  gfx_drawtext(text,
	       0,0,
	       x,y,
	       15); //strlen(text));
}
//! Draws text at an address by calling back to the MD380 function.
void drawascii(char *ascii,
	       int x, int y){
  //Widen the string.  We really ought to do proper lengths.
  wchar_t wide[15];
  for(int i=0;i<15;i++)
    wide[i]=ascii[i];
  
  //Draw the wide string, not the original.
  gfx_drawtext(wide,
	       0,0,
	       x,y,
	       15); //strlen(text));
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
  gfx_drawtext2(wide, x, y, 0);

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

void print_DebugLine(unsigned int bg_color) {
  char buf[30];
  int n,i,ii;
  uint8_t err;

  gfx_set_bg_color(bg_color);
  gfx_set_fg_color(0x000000);
  gfx_select_font((void *) 0x809bcec);

  drawascii2("                  ",10,42);
  drawascii2("                  ",10,54);
  drawascii2("                  ",10,66);
  drawascii2("                  ",10,78);
  drawascii2("                  ",10,90);
  drawascii2("                  ",10,102);


  ii=0;
  n=0;

  OSSemPend(debug_line_sem, 0, &err);
#ifdef DEBUG
  printf("err from OSSemPend(debug_line_sem ... %x",err);
#endif

  for (i=0;i<strlen(DebugLine2) || n < 6 ;i++) {
    if (DebugLine2[i] == ',' || DebugLine2[i] == '\0') {
      buf[ii++]='\0';
      drawascii2(buf, 10, 42+n*12);
      ii=0;
      n++;
    } else {
      if (ii<29) buf[ii++]=DebugLine2[i];
      }
  }

  drawascii2(DebugLine1, 10, 42);
  OSSemPost(debug_line_sem);

  gfx_select_font((void *) 0x80d0fac);
  gfx_set_fg_color(0xff8032);
  gfx_set_bg_color(0xff000000);
}

void print_DebugLine_green(char *bmp, int idx, uint64_t pos) {
  if (global_addl_config.userscsv == 1) {
    print_DebugLine(0x00ff00);
  } else {
    gfx_drawbmp(bmp, idx, pos);
  }
}

void print_DebugLine_gray(void *bmp, int idx, uint64_t pos) {
  if (global_addl_config.userscsv == 1) {
    print_DebugLine(0x888888);
  } else {
    gfx_drawbmp(bmp, idx, pos);
  }
}


void print_date_hook(void) {  // copy from the md380 code
  wchar_t wide[40];
  RTC_DateTypeDef RTC_DateStruct;
    md380_RTC_GetDate(RTC_Format_BCD, &RTC_DateStruct);
  if ( global_addl_config.datef == 0) {
    wide[0]='2';
    wide[1]='0';
    md380_itow(&wide[2], RTC_DateStruct.RTC_Year);
    wide[4]='/';
    md380_itow(&wide[5], RTC_DateStruct.RTC_Month);
    wide[7]='/';
    md380_itow(&wide[8], RTC_DateStruct.RTC_Date);
  } else {
    md380_itow(&wide[0], RTC_DateStruct.RTC_Date);
    wide[2]='.';
    md380_itow(&wide[3], RTC_DateStruct.RTC_Month);
    wide[5]='.';
    wide[6]='2';
    wide[7]='0';
    md380_itow(&wide[8], RTC_DateStruct.RTC_Year);
  }
  wide[10]='\0';
  gfx_chars_to_display( wide, 0xa, 0x60, 0x5e);
}
