/*  \file rtc_timer.c
  \brief wrapper functions for the "RTC Timer"-Task.
*/

#define DEBUG
#define CONFIG_GRAPHICS

#ifdef DEBUG
#define PRINT(fmt, args...)    printf(fmt, ## args)
#else
#define PRINT(fmt, args...)    /* Don't do anything in release builds */
#endif

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

/* @ 0x2001e600 */
/*
 *
 * uint8_t 
 * 
 * 0x80 set = no time displayed.
 *
 */

/*  see 0x0801f06a there are a lot of modes */

// 16  = ? not seen
// 17  = public call rx
// 18  = ? not seen
// 19  = ? after rx call?
// 27  = menu refresh
// 28  = idle channel screen
// 35  = volume screen
//
// high bit (0x80) signals transition
// 156 = channel switch
// 115 = menu start

#define SCR_MODE_16 16
#define SCR_MODE_17 17 // rx/tx in progress.
#define SCR_MODE_18 18
#define SCR_MODE_CHAN_19 19 // when channel RX but other timeslot.
#define SCR_MODE_20 20
#define SCR_MODE_21 21 // initial screen?
#define SCR_MODE_22 22
#define SCR_MODE_MENU 27
#define SCR_MODE_CHAN_IDLE 28
#define SCR_MODE_29 29
#define SCR_MODE_30 30
#define SCR_MODE_31 31
#define SCR_MODE_32 32
#define SCR_MODE_33 33
#define SCR_MODE_VOLUME 35
#define SCR_MODE_36 36

#define MAX_STATUS_CHARS 40

#define RX_POPUP_Y_START 12
wchar_t status_line[MAX_STATUS_CHARS] = { L"12345678901234567890" };

char progress_info[] = { "|/-\\" } ;

int progress = 0 ;

extern int g_dst;  // transferbuffer users.csv
extern int g_src;
  
uint8_t *mode2 = 0x2001e94b ;
uint16_t *cntr2 = 0x2001e844 ;
    
// 1 idle
// 2 rx
// 4 post-rx?
// 10 menu

void update_status_line()
{
    int progress2 = progress ; // sample (thread safe) 

    progress2 %= sizeof( progress_info );
    char c = progress_info[progress2];
    
    int dst = g_dst ;
    
    char buf[MAX_STATUS_CHARS];
//    sprintf(buf,"%c|%02d|%5d", c, md380_f_4225_operatingmode & 0x7F, dst ); // potential buffer overrun!!!
    sprintf(buf,"%c|%02d|%2d|%4d", c, md380_f_4225_operatingmode & 0x7F, *mode2, *cntr2 ); // potential buffer overrun!!!
        
    for(int i=0;i<MAX_STATUS_CHARS;i++) {
        status_line[i]= buf[i];
    }
    status_line[MAX_STATUS_CHARS-1]='\0';    
}

extern void draw_status_line()
{
    gfx_set_fg_color(0);
    gfx_set_bg_color(0x00ff8032); 
    gfx_select_font(gfx_font_small );
    
    gfx_chars_to_display(status_line,10,55,94+20); 
}

extern void draw_updated_status_line()
{
    progress++ ;
    progress %= sizeof( progress_info );
    
    update_status_line();
    draw_status_line();
}

extern void mode17_hook()
{
    draw_status_line();
}

// this hook switcht of the exit from the menu in case of RX
void * f_4225_internel_hook() 
{
    if ( global_addl_config.experimental == 1 ) {
        return &md380_f_4225_operatingmode ;
    }
    
#ifdef DEBUG
//    printf("<%d> \n", md380_f_4225_operatingmode);
#endif
  if (md380_f_4225_operatingmode == SCR_MODE_MENU) {
    flag=1;
  }
  if (md380_f_4225_operatingmode == SCR_MODE_CHAN_IDLE ) {
    flag=0;
  }
  if (flag == 1) {
    md380_f_4225_operatingmode = SCR_MODE_MENU;
  }
  return &md380_f_4225_operatingmode ;
}


void print_rx_screen(unsigned int bg_color) {
#ifdef CONFIG_GRAPHICS

  char buf[160];
  int n,i,ii;
  int dst;
  int src;

  // clear screen
  gfx_set_fg_color(bg_color);
  gfx_blockfill(2, 16, 157, 130); // if we go any lower, we wrap around to the top

  gfx_set_bg_color(bg_color);
  gfx_set_fg_color(0x000000);
  gfx_select_font(gfx_font_small);

 int primask=OS_ENTER_CRITICAL();  // for form sake
 dst=g_dst;
 src=g_src;
 OS_EXIT_CRITICAL(primask);
 if (find_dmr_user(buf, src, (void *) 0x100000, 80) == 0) {
   sprintf(buf, ",ID not found,in users.csv,see README.md,on Github");   // , is line seperator ;)
 }
  ii=0;
  n=0;
  int y_index = RX_POPUP_Y_START;

  for (i=0;i<strlen(buf) || n < 6 ;i++) {
    if (buf[i] == ',' || buf[i] == '\0') {
      if (n == 1) {  // This line holds the call sign
        gfx_select_font(gfx_font_norm);
      } else {
        gfx_select_font(gfx_font_small);
      } 

      if (n == 2) {
        y_index = y_index + 16;  // previous line was in big font
      } else {
        y_index = y_index + 12;  // previous line was in small font
      }

      buf[ii++]='\0';
      drawascii2(buf, 10, y_index);
      ii=0;
      n++;
    } else {
      if (ii<29) buf[ii++]=buf[i];
      }
  }

  sprintf(buf, "%d -> %d", src, dst ); // overwrite DMR id with source -> destination
  drawascii2(buf, 10, RX_POPUP_Y_START + 12);

  gfx_select_font(gfx_font_norm);
  gfx_set_fg_color(0xff8032);
  gfx_set_bg_color(0xff000000);
#endif //CONFIG_GRAPHICS
}

void rx_screen_blue_hook(char *bmp, int x, int y) 
{
    PRINT("b");
#ifdef CONFIG_GRAPHICS
  if (global_addl_config.userscsv == 1) {
    print_rx_screen(0xff8032);
  } else {
    gfx_drawbmp(bmp, x, y);
  }
#endif //CONFIG_GRAPHICS
}

void rx_screen_gray_hook(void *bmp, int x, int y) 
{
    PRINT("g");
#ifdef CONFIG_GRAPHICS
  if (global_addl_config.userscsv == 1) {
    print_rx_screen(0x888888);
  } else {
    gfx_drawbmp(bmp, x, y);
  }
#endif //CONFIG_GRAPHICS
}






// Lab hooks - for training only :)
#ifdef CONFIG_GRAPHICS
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
#endif //CONFIG_GRAPHICS

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


void draw_micbargraph()
{
    static int rx_active; // flag to syncronice this hook ( operatingmode == 0x11 is also on rx seeded)
    static int fullscale_offset = 0;
    static uint32_t lastframe=0;
    static int red=0;
    static int green=0;

    int relative_peak_cb;
    int centibel_val;
    
    if (fullscale_offset == 0 ) { // init int_centibel()
        fullscale_offset = intCentibel(3000);  // maybe wav max max_level
    }

    if ( md380_f_4225_operatingmode == SCR_MODE_17 && max_level < 4500 && max_level > 10) { // i hope we are on tx
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
          gfx_blockfill(9, 54, 151, 66);

          // paint legend
          gfx_set_fg_color(0x0000ff);
          gfx_blockfill((-30+280)/2+10, 67, 150, 70);
          gfx_set_fg_color(0x00ff00);
          gfx_blockfill((-130+280)/2+10, 67, (-30+280)/2-1+10, 70);
          gfx_set_fg_color(0x555555);
          gfx_blockfill(10, 67, (-130+280)/2-1+10, 70);

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
          gfx_blockfill(10, 55, centibel_val, 65);
          gfx_set_fg_color(0xff8032);
          gfx_set_bg_color(0xff000000);
        }
      }
    }

    if ( md380_f_4225_operatingmode == SCR_MODE_18 && rx_active == 1 ) { // clear screen area
      gfx_set_fg_color(0xff8032);
      gfx_set_bg_color(0xff000000);
      gfx_blockfill(9, 54, 151, 70);
      rx_active = 0;
      red=0;
      green=0;
    }
}

extern void dummy();
void dummy() 
{
} 

void gfx_drawtext_hook(wchar_t *str, short sx, short sy, short x, short y, int maxlen)
{
    PRINT("dt: %d %d %S\n", sx, sy, str);
    gfx_drawtext(str, sx, sy, x, y, maxlen);
}

// r0 = str, r1 = x, r2 = y, r3 = xlen
void gfx_chars_to_display_hook(wchar_t *str, int x, int y, int xlen)
{
    // filter datetime (y=96)
    if( y != 96 ) {
        PRINT("ctd: %d %d %S\n", x, y, str);
    }
    gfx_chars_to_display(str, x, y, xlen);
}

void (*f)(wchar_t *str, int x, int y, int xlen, int ylen) = 0x0801dd1a + 1 ;

void gfx_drawtext4_hook(wchar_t *str, int x, int y, int xlen, int ylen)
{
//    PRINT("dt4: %d %d %S\n", x, y, str);
    f(str,x,y,xlen,ylen);
}

/**
 * write centered horizontally / vertically
 */
void something_write_to_screen_hook(wchar_t *str, int x1, int y1, int x2, int y2)
{
    PRINT("swts: %S %d %d %d %d\n", str, x1, y1, x2, y2);
//    f(str,x,y,xlen,ylen);
}

int old_opmode = 0 ;

void trace_scr_mode()
{
    if( old_opmode != md380_f_4225_operatingmode ) {
        old_opmode = md380_f_4225_operatingmode ;
        PRINT( "mode: %d\n", md380_f_4225_operatingmode);
    } else {
//        printf( "%d ", md380_f_4225_operatingmode);
    }
    
    PRINT( "%d %d\n", *mode2, *cntr2 );
    
}

void f_4225_hook()
{
    // this probably runs on other thread than the display task.
    
#ifdef DEBUG    
    //trace_scr_mode();
#endif    
    
//#ifdef CONFIG_GRAPHICS

    if ( global_addl_config.micbargraph == 1 ) {
        draw_micbargraph();
    }
    
    if ( global_addl_config.debug == 1 ) {
        draw_updated_status_line();
    }
    
    md380_f_4225();
    
    PRINT("%S\n", status_line );
    
    if ( global_addl_config.debug == 1 ) {
        draw_status_line();
    }        
    
//    if ( global_addl_config.experimental == 0 ) {
//        return ;
//    }
    
//#endif
}
