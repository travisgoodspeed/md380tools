/*  \file rtc_timer.c
  \brief wrapper functions for the "RTC Timer"-Task.
*/

#include <stdlib.h>

#include "md380.h"
#include "version.h"
#include "tooldfu.h"
#include "config.h"
#include "gfx.h"
#include "printf.h"
#include "string.h"
#include "addl_config.h"
#include "ambe.h"
#include "usersdb.h"
 
static int flag=0;



// this hook switcht of the exit from the menu in case of RX
void * f_4225_internel_hook() {
#ifdef DEBUG
  printf("%x \n", *md380_f_4225_operatingmode);
#endif
  if (*md380_f_4225_operatingmode == md380_f_4225_operatingmode_menu) {
    flag=1;
  }
  if (*md380_f_4225_operatingmode == md380_f_4225_operatingmode_menu_exit) {
    flag=0;
  }
  if (flag == 1) {
    *md380_f_4225_operatingmode=md380_f_4225_operatingmode_menu;
  }
  return(md380_f_4225_operatingmode);
}


void print_rx_screen(unsigned int bg_color) {
  extern int g_dst;  // transferbuffer users.csv
  extern int g_src;

  char buf[160];
  int n,i,ii,y;
  int dst;
  int src;

  gfx_set_bg_color(bg_color);
  gfx_set_fg_color(0x000000);
  gfx_select_font((void *) 0x809bcec);

  for (y=42; y<=102; y=y+12) {
    drawascii2("                  ",10,y);
  }

 int primask=OS_ENTER_CRITICAL();  // for form sake
 dst=g_dst;
 src=g_src;
 OS_EXIT_CRITICAL(primask);

 if (find_dmr_user(buf, src, (void *) 0x100000, 80) == 0) {
   sprintf(buf, ",ID not found,in users.csv,see README.md,on Github");   // , is line seperator ;)
 }

  ii=0;
  n=0;

  for (i=0;i<strlen(buf) || n < 6 ;i++) {
    if (buf[i] == ',' || buf[i] == '\0') {
      buf[ii++]='\0';
      drawascii2(buf, 10, 42+n*12);
      ii=0;
      n++;
    } else {
      if (ii<29) buf[ii++]=buf[i];
      }
  }

  drawascii2("                  ",10,102);
  sprintf(buf, "%d -> %d", src, dst );
  drawascii2(buf, 10, 42);

  gfx_select_font((void *) 0x80d0fac);
  gfx_set_fg_color(0xff8032);
  gfx_set_bg_color(0xff000000);
                                              
}

void rx_screen_green_hook(char *bmp, int x, int y) {
  if (global_addl_config.userscsv == 1) {
    print_rx_screen(0x00ff00);
  } else {
    gfx_drawbmp(bmp, x, y);
    }
}

void rx_screen_gray_hook(void *bmp, int idx, uint64_t pos) {
  if (global_addl_config.userscsv == 1) {
    print_rx_screen(0x888888);
  } else {
    gfx_drawbmp(bmp, idx, pos);
  }
}






// Lab hooks - for training only :)

void f_4137_hook() {
  void *return_addr;
  void *sp;
  __asm__("mov %0,r14" : "=r" (return_addr));
  __asm__("mov %0,r13" : "=r" (sp));
  printf("Call md380_f_4137 from r: %x s: %x\n", return_addr,sp);
  md380_f_4137();
}

void f_4520_hook() {
  void *return_addr;
  void *sp;
  __asm__("mov %0,r14" : "=r" (return_addr));
  __asm__("mov %0,r13" : "=r" (sp));
  printf("Call md380_f_4520 from r: %x s: %x\n", return_addr,sp);
  md380_f_4520();
}


void f_4098_hook() {
  void *return_addr;
  void *sp;
  __asm__("mov %0,r14" : "=r" (return_addr));
  __asm__("mov %0,r13" : "=r" (sp));
  printf("Call md380_f_4098 from r: %x s: %x\n", return_addr,sp);
  md380_f_4098();
}

void f_4102_hook() {
  void *return_addr;
  void *sp;
  __asm__("mov %0,r14" : "=r" (return_addr));
  __asm__("mov %0,r13" : "=r" (sp));
  printf("Call md380_f_4102 from r: %x s: %x\n", return_addr,sp);
  md380_f_4102();
}

// Takes a positive(!) integer amplitude and computes 200*log10(amp),
// centi Bel, approximtely. If the given parameter is 0 or less, this
// function returns -1.  tnx to sellibitze
int intCentibel(long ampli)
{
    if (ampli <= 0)
	return -1;		// invalid
    int log_2 = 0;
    while (ampli >= 32 * 8) {
	ampli >>= 1 + 3;
	log_2 += 1 + 3;
    }
    while (ampli >= 32) {
	ampli >>= 1;
	log_2 += 1;
    }
    // 1 <= ampli < 32
    static const short fine[] = {
	-1, 0, 60, 95, 120, 140, 156, 169,
	181, 191, 200, 208, 216, 223, 229, 235,
	243, 249, 253, 258, 262, 266, 270, 274,
	278, 281, 285, 288, 291, 294, 297, 300
    };
    return (log_2 * 301 + 2) / 5 + fine[ampli];
}

/*
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    long peak = 6000;
    if (peak > 0) {
	int fullscale_offset = int_centibel(32767);
	int relative_peak_cb = int_centibel(peak) - fullscale_offset;
	printf("%i.%i dBFS\en", relative_peak_cb / 10,
	       abs(relative_peak_cb % 10));
    } else {
	printf("-Inf dBFS\n");
    }
    return 0;
}
*/

void f_4225_hook()
{
  static int rx_active; // flag to syncronice this hook ( operatingmode == 0x11 is also on rx seeded)
  static int fullscale_offset = 0;
  static uint32_t lastframe=0;
  static int red=0;
  static int green=0;

  int relative_peak_cb;
  int centibel_val;

  if ( global_addl_config.micbargraph == 1 ) {

    if (fullscale_offset == 0 ) { // init int_centibel()
      fullscale_offset = intCentibel(3000);  // maybe wav max max_level
      }

    if (*md380_f_4225_operatingmode == 0x11 && max_level < 4500 && max_level > 10) { // i hope we are on tx
      if (lastframe < ambe_encode_frame_cnt) {	// check for new frame
        lastframe = ambe_encode_frame_cnt;
        rx_active=1;

        relative_peak_cb = intCentibel(max_level) - fullscale_offset;
        centibel_val = relative_peak_cb;


        if ( lastframe % 5 == 1 ) { // reduce drawing
          if (centibel_val < -280) { // limit 160 pixel bargraph 10 150 -> 140 pixel for bargraph
            centibel_val = -280;
          } else if (centibel_val > 0) {
            centibel_val = 0;
          }
          centibel_val += 280;  // shift to positive
          centibel_val /= 2;    // scale

          gfx_set_fg_color(0x999999);
          gfx_set_bg_color(0xff000000);
          gfx_blockfill(9, 49, 151, 61);

          // paint legend
          gfx_set_fg_color(0x0000ff);
          gfx_blockfill((-30+280)/2+10, 62, 150, 65);
          gfx_set_fg_color(0x00ff00);
          gfx_blockfill((-130+280)/2+10, 62, (-30+280)/2-1+10, 65);
          gfx_set_fg_color(0x555555);
          gfx_blockfill(10, 62, (-130+280)/2-1+10, 65);

          // set color
          if ( relative_peak_cb > -3 || red > 0) {
            if (red > 0) red--;
            if ( relative_peak_cb > -3) red = 30;
            gfx_set_fg_color(0x0000ff);
          } else if ( relative_peak_cb > -130 || green > 0) {
            if (green > 0) green--;
            if ( relative_peak_cb > -130 ) green = 30;
            gfx_set_fg_color(0x00ff00);
          } else {
            gfx_set_fg_color(0x555555);
          }
          gfx_set_bg_color(0xff000000);
          gfx_blockfill(10, 50, centibel_val, 60);
          gfx_set_fg_color(0xff8032);
          gfx_set_bg_color(0xff000000);
        }
      }
    }

    if (*md380_f_4225_operatingmode == 0x12 && rx_active == 1 ) { // clear screen area
      gfx_set_fg_color(0xff8032);
      gfx_set_bg_color(0xff000000);
      gfx_blockfill(9, 49, 151, 65);
      rx_active = 0;
      red=0;
      green=0;
    }
  }

md380_f_4225();
}
