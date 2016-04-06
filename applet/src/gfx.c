/*! \file gfx.c
  \brief Graphics wrapper functions.
*/

#include "md380.h"
#include "version.h"
#include "tooldfu.h"
#include "config.h"
#include "gfx.h"
#include "printf.h"

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

void print_DebugLine_green(void){
  char buf[10];
  static int cnt=0;
 
  gfx_set_bg_color(0x00ff00);
  gfx_set_fg_color(0x4488dd);
  gfx_select_font((void *) 0x809bcec);

  drawascii2("                  ",10,70);
  drawascii2("                  ",10,80);
  drawascii2("                  ",10,90);

  drawascii2(DebugLine1, 10, 70);//160 154
  drawascii2(DebugLine2, 10, 80);//160 190
  sprintf(buf, "%d", cnt++);
  drawascii2(buf, 10, 90);//160 190

  gfx_select_font((void *) 0x80d0fac);
  gfx_set_fg_color(0xff8032);
  gfx_set_bg_color(0xff000000);
}

void print_DebugLine_gray(void){
  char buf[10];
  static int cnt=0;
   
  gfx_set_bg_color(0x555555);
  gfx_set_fg_color(0x4488dd);
  gfx_select_font((void *) 0x809bcec);

  drawascii2("                  ",10,70);
  drawascii2("                  ",10,80);
  drawascii2("                  ",10,90);

  drawascii2(DebugLine1, 10, 70);//160 154
  drawascii2(DebugLine2, 10, 80);//160 190
  sprintf(buf, "%d", cnt++);
  drawascii2(buf, 10, 90);//160 190

  gfx_select_font((void *) 0x80d0fac);
  gfx_set_fg_color(0xff8032);
  gfx_set_bg_color(0xff000000);
}   
