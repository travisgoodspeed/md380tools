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
#include "syslog.h"
 
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

extern void f_4315_hook()
{
    netmon_update();
    con_redraw();
    if( is_netmon_visible() ) {
        return ;
    }
    F_4315();
}

//// this hook switcht of the exit from the menu in case of RX
//
//static int new_anti = 1 ;
//
//void * f_4225_internel_hook()
//{
////    if( new_anti || global_addl_config.experimental == 1 ) {
//    if( new_anti ) {
//        return &gui_opmode1;
//    }
//
//#ifdef DEBUG
//    //    printf("<%d> \n", gui_opmode1);
//#endif
//    if( gui_opmode1 == SCR_MODE_MENU ) {
//        flag = 1;
//    }
//    if( gui_opmode1 == SCR_MODE_CHAN_IDLE_INIT ) {
//        flag = 0;
//    }
//    if( flag == 1 ) {
//        gui_opmode1 = SCR_MODE_MENU;
//    }
//    return &gui_opmode1;
//}

// 0x2001e895 != 32
// 0x2001e895 == 64 -> rx_screen_blue_hook

void rx_screen_blue_hook(char *bmp, int x, int y)
{
    netmon_update();
#ifdef CONFIG_GRAPHICS
    if( global_addl_config.userscsv == 1 && !is_menu_visible() ) {
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
    if( global_addl_config.userscsv == 1 && !is_menu_visible() ) {
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

//#if defined(FW_D13_020)
//void gfx_drawtext8_hook(uint8_t *r0)
//{
//    gfx_info_t *g = &gfx_info ;
//    
////    if( g->xpos == 10 && g->ypos == 55 ) {
////        // filter out status.
////        return ;
////    }
//    
//    PRINT("%s \n",r0);
//    PRINT("Dt8: %d %d %08x %08x %x\n", g->xpos, g->ypos, g->fg_color, g->bg_color, g->off44 );
//    
////    PRINT("%s\n",r0);
////    printhex(g,72);
////    PRINT("\n");
//    //gfx_drawtext(str, sx, sy, x, y, maxlen);
//}
//#endif

void gfx_drawtext_hook(wchar_t *str, short sx, short sy, short ex, short ey, int maxlen)
{
    PRINT("dt: %d %d %d %d %d %S %x\n", sx, sy, ex, ey, maxlen, str, str);
    gfx_drawtext(str, sx, sy, ex, ey, maxlen);
}

//void (*f)(wchar_t *str, int x, int y, int xlen, int ylen) = 0x0801dd1a + 1 ;


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

#ifdef FW_D13_020
void OSTimeDly(uint32_t delay);
#endif  

void trace_scr_mode()
{
    static int old = -1 ;
    int new = gui_opmode1 & 0x7F ;
    int upd = gui_opmode1 > 0x7F ;
    if( upd == 0 ) {
        // to prevent flooding. 
        // still interesting flooding. it keeps on setting mode 19. when rx in other timeslot.
        return ;
    }
    if( old != new ) {
        PRINT( "mode1: %d -> %d (%d)\n", old, new, upd );
        old = new ;
    }
}

void trace_scr_mode2()
{
    static int old = -1 ;
    int new = gui_opmode2 ;
    if( old != new ) {
        PRINT( "mode2: %d -> %d\n", old, new );
        LOGG( "mode2: %d -> %d\n", old, new );
        old = new ;
    }
}

void f_4225_hook()
{
    
#ifdef DEBUG    
    trace_scr_mode();
    trace_scr_mode2();
#endif   
    
    static int old = -1 ;
    int new = gui_opmode1 & 0x7F ;
    if( old != new ) {
        if( gui_opmode2 == OPM2_MENU ) {
            // menu is showing.
            if( new == SCR_MODE_IDLE || new == SCR_MODE_RX_VOICE || new == SCR_MODE_RX_TERMINATOR ) {
                // new mode tries to deviate from menu to popup.
                
                // reset.
                gui_opmode1 = SCR_MODE_MENU ;
            }
        } else {
            old = new ;
        }
    }
    
    if ( global_addl_config.micbargraph == 1 ) {
        if( !is_netmon_visible() ) {
            draw_micbargraph();
        }
    }
    
    netmon_update();

#if 0
    int upd = gui_opmode1 > 0x7F ;
    if( upd == 0 ) {
        if( is_netmon_visible() ) {
            con_redraw();            
        } else {
            md380_f_4225();
        }
    } else {
        md380_f_4225();        
    }
#else     
    md380_f_4225();        
#endif    
    
    if( is_netmon_visible() ) {
        
        // steer back to idle screen, because thata the most intercepted.
        if( gui_opmode2 == OPM2_VOICE ) {
            gui_opmode2 = OPM2_IDLE;
        }
    }
    
}
