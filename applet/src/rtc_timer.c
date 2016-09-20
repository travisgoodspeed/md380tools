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
#include "display.h"
#include "dmr.h"
 
static int flag=0;

/* @ 0x2001e600 */
/*
 *
 * uint8_t 
 * 
 * 0x80 set = no time displayed.
 *
 */

#define MAX_STATUS_CHARS 40

wchar_t status_line[MAX_STATUS_CHARS] = { L"uninitialized statusline" };

char progress_info[] = { "|/-\\" } ;

int progress = 0 ;

uint8_t *mode2 = 0x2001e94b ;
uint16_t *cntr2 = 0x2001e844 ;
    
// 1 idle
// 2 rx
// 4 post-rx?
// 10 menu

//void (*something_write_to_screen)(wchar_t *str, int x1, int y1, int x2, int y2) = 0x0800ded8 + 1 ;
//void (*gfx_drawtext5)(wchar_t *str, int sx, int sy, int maxlen) = 0x0801dd2c + 1 ;

char status_buf[MAX_STATUS_CHARS] = { "" };
    
void update_status_line()
{
    int progress2 = progress ; // sample (thread safe) 

    progress2 %=  sizeof( progress_info ) - 1 ;
    char c = progress_info[progress2];
    
    int dst = g_dst ;
    
    sprintf(status_buf,"%c|%02d|%2d|%4d", c, md380_f_4225_operatingmode & 0x7F, *mode2, *cntr2 ); // potential buffer overrun!!!
        
    for(int i=0;i<MAX_STATUS_CHARS;i++) {
        status_line[i]= status_buf[i];
    }
    status_line[MAX_STATUS_CHARS-1]='\0';    
}

extern void draw_status_line()
{
    gfx_set_fg_color(0xff000000);
    gfx_set_bg_color(0x00ff8032); 
    void *old = gfx_select_font(gfx_font_small);
    
    gfx_chars_to_display(status_line,10,55,0); 

    gfx_select_font(old);
    
//    gfx_drawtext5(status_buf,10,55,0);
//    something_write_to_screen(status_line,0,80,160,100);
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



void rx_screen_blue_hook(char *bmp, int x, int y) 
{
    PRINT("b");
#ifdef CONFIG_GRAPHICS
  if (global_addl_config.userscsv == 1) {
    draw_rx_screen(0xff8032);
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
    draw_rx_screen(0x888888);
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



extern void dummy();
void dummy() 
{
} 

//void gfx_drawtext5_hook(wchar_t *str, int sx, int sy, int maxlen)
//{
//    PRINT("dt5:%S %d %d %d\n", str, sx, sy, maxlen);
//    //gfx_drawtext(str, sx, sy, x, y, maxlen);
//}

void gfx_drawtext8_hook(uint8_t *r0)
{
    uint8_t *p = 0x2001da1c ;
    uint16_t *w = 0x2001da1c ;
    
    gfx_info_t *g = 0x2001da1c ;
    
//    if( g->xpos == 10 && g->ypos == 55 ) {
//        // filter out status.
//        return ;
//    }
    
    PRINT("%s \n",r0);
    PRINT("Dt8: %d %d %08x %08x %x\n", g->xpos, g->ypos, g->fg_color, g->bg_color, g->off44 );
    
//    PRINT("%s\n",r0);
//    printhex(g,72);
//    PRINT("\n");
    //gfx_drawtext(str, sx, sy, x, y, maxlen);
}

void gfx_drawtext_hook(wchar_t *str, short sx, short sy, short x, short y, int maxlen)
{
    PRINT("dt: %d %d %S %x\n", sx, sy, str, str);
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
    PRINT("dt4: %S %d %d %d %d (%x)\n", str, x, y, xlen, ylen, str);
    f(str,x,y,xlen,ylen);
}

#if 0
/**
 * write centered horizontally / vertically
 */
void something_write_to_screen_hook(wchar_t *str, int x1, int y1, int x2, int y2)
{
    PRINT("swts: %S %d %d %d %d\n", str, x1, y1, x2, y2);
//    f(str,x,y,xlen,ylen);
    something_write_to_screen(status_line,x1,y1,x2,y2);
}
#endif

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

#ifdef FW_D13_020
void (*OSTimeDly)(uint32_t delay) = 0x8033eb4 + 1 ;        
#endif        

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
    
//#ifdef FW_D13_020
//        if( (md380_f_4225_operatingmode & 0x7F) == SCR_MODE_MENU ) {
//            PRINT(">");
//            OSTimeDly( 10000 );
//        }
//#endif        
    
    md380_f_4225();

    if ( global_addl_config.debug == 1 ) {
//        PRINT("%S\n", status_line );
//        static long fg = 0xff8032 ;
//        fg += 0x10 ;
//        gfx_set_fg_color(fg);
//        gfx_set_bg_color(0xff000000);
//        gfx_blockfill(0,0,100,100);
        draw_status_line();
//#ifdef FW_D13_020
//        if( md380_f_4225_operatingmode == SCR_MODE_MENU ) {
//            PRINT("<");
//            OSTimeDly( 1000 );
//        }
//#endif        
    }        
    
//    if ( global_addl_config.experimental == 0 ) {
//        return ;
//    }
    
//#endif
}
