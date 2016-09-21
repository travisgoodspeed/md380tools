/*  \file rtc_timer.c
  \brief wrapper functions for the "RTC Timer"-Task.
*/

#define DEBUG
#define CONFIG_GRAPHICS

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
#include "console.h"
#include "util.h"
#include "debug.h"
#include "netmon.h"
 
static int flag=0;

/* @ 0x2001e600 */
/*
 *
 * uint8_t 
 * 
 * 0x80 set = no time displayed.
 *
 */

//void (*something_write_to_screen)(wchar_t *str, int x1, int y1, int x2, int y2) = 0x0800ded8 + 1 ;
//void (*gfx_drawtext5)(wchar_t *str, int sx, int sy, int maxlen) = 0x0801dd2c + 1 ;

//extern void mode17_hook()
//{
//    draw_status_line();
//}

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
    netmon_update();
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
    netmon_update();
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
    //PRINT("dt: %d %d %S %x\n", sx, sy, str, str);
    gfx_drawtext(str, sx, sy, x, y, maxlen);
}

// r0 = str, r1 = x, r2 = y, r3 = xlen
void gfx_chars_to_display_hook(wchar_t *str, int x, int y, int xlen)
{
    con_redraw();

    // filter datetime (y=96)
    if( y != 96 ) {
        PRINT("ctd: %d %d %S\n", x, y, str);
    }
    gfx_chars_to_display(str, x, y, xlen);
}

void (*f)(wchar_t *str, int x, int y, int xlen, int ylen) = 0x0801dd1a + 1 ;

void gfx_drawtext4_hook(wchar_t *str, int x, int y, int xlen, int ylen)
{
    void * return_addr = __builtin_return_address(0);
    wchar_t *str2 = str ;
    PRINT("dt4: 0x%x %S %d %d %d %d (%x)\n", return_addr, str, x, y, xlen, ylen, str);
//    if( x == 45 && y == 34 ) {
//        mkascii( tg_buf, sizeof(tg_buf), str );
//        // somehow, if f() is not called, the console is not drawn. 
//        // to fix later.
////        if( !has_gui() ) {
////            str2 = L"" ;
////        }
//    }
//    if( x == 34 && y == 75 ) {
//        mkascii( chan_buf, sizeof(chan_buf), str );
////        if( !has_gui() ) {
////            str2 = L"" ;
////        }
//    }
    
    f(str2,x,y,xlen,ylen);
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

//int old_opmode = 0 ;
//
//void trace_scr_mode()
//{
//    if( old_opmode != md380_f_4225_operatingmode ) {
//        old_opmode = md380_f_4225_operatingmode ;
//        PRINT( "mode: %d\n", md380_f_4225_operatingmode);
//    } else {
////        printf( "%d ", md380_f_4225_operatingmode);
//    }
//    
//    PRINT( "%d %d\n", *mode2, *cntr2 );
//    
//}

#ifdef FW_D13_020
void OSTimeDly(uint32_t delay);
#endif  

//void state_fuzzing()
//{
//    static long cnt = 0 ;
//    
//    cnt++ ;
//    
//    if( cnt > 500 ) {
//        
//        if( cnt < 900 ) {
//            *mode2 = 4 ;            
//        } else {
//            *mode2 = 1 ;
//        }
//        
//    } 
//}

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
    
    netmon_update();
    
    md380_f_4225();
    
    if ( global_addl_config.debug == 1 ) {
//        state_fuzzing();
//        PRINT("%S\n", status_line );
//        static long fg = 0xff8032 ;
//        fg += 0x10 ;
//        gfx_set_fg_color(fg);
//        gfx_set_bg_color(0xff000000);
//        gfx_blockfill(0,0,100,100);
        //draw_status_line();
//#ifdef FW_D13_020
//        if( md380_f_4225_operatingmode == SCR_MODE_MENU ) {
//            PRINT("<");
//            OSTimeDly( 1000 );
//        }
//#endif        
    }        
    
//#endif
}
