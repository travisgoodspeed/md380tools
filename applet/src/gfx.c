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
  char buf[30];
  int n,i,ii;
    
  gfx_set_bg_color(0x00ff00);
  gfx_set_fg_color(0x000000);
  gfx_select_font((void *) 0x809bcec);

  drawascii2("                  ",10,70);
  drawascii2("                  ",10,80);
  drawascii2("                  ",10,90);
  drawascii2("                  ",10,100);
  drawascii2("                  ",10,110);
  drawascii2("                  ",10,120);


  ii=0;
  n=0;
  for (i=0;i<strlen(DebugLine2) || n < 4 ;i++) {
    if (DebugLine2[i] == ',') {
      buf[ii++]='\0';
      drawascii2(buf, 10, 70+n*10);
      ii=0;
      n++;
    } else {
      if (ii<29) buf[ii++]=DebugLine2[i];
      }
  }
                                           
  drawascii2(DebugLine1, 10,70);


  gfx_select_font((void *) 0x80d0fac);
  gfx_set_fg_color(0xff8032);
  gfx_set_bg_color(0xff000000);
}

void print_DebugLine_gray(void){
  char buf[30];
  int n,i,ii;
   
  gfx_set_bg_color(0x888888);
  gfx_set_fg_color(0x000000);
  gfx_select_font((void *) 0x809bcec);

  drawascii2("                  ",10,70);
  drawascii2("                  ",10,80);
  drawascii2("                  ",10,90);
  drawascii2("                  ",10,100);
  drawascii2("                  ",10,110);
  drawascii2("                  ",10,120);

  ii=0;
  n=0;
  for (i=0;i<strlen(DebugLine2) || n < 4 ;i++) {
    if (DebugLine2[i] == ',') {
      buf[ii++]='\0';
      drawascii2(buf, 10, 70+n*10);
      ii=0;
      n++;
    } else {
      if (ii<29) buf[ii++]=DebugLine2[i];
      }
  }


  drawascii2(DebugLine1, 10,70);

  gfx_select_font((void *) 0x80d0fac);
  gfx_set_fg_color(0xff8032);
  gfx_set_bg_color(0xff000000);
}   
