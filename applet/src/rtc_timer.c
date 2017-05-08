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
#include "irq_handlers.h"
#include "narrator.h"
#include "app_menu.h"

 
//static int flag=0;

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
#  if( CONFIG_MORSE_OUTPUT ) 
    narrate(); // may "tell a story" in Morse code (for visually impaired hams).
               // don't seem get here while in the alternative menu (app_menu.c).
#  endif

#  if( CONFIG_APP_MENU ) 
    if( Menu_DrawIfVisible(AM_CALLER_F_4315_HOOK) )  
     { return; // the menu covers the entire screen, so don't draw anything else
     }
#  endif // CONFIG_APP_MENU ?

    netmon_update();
    con_redraw();


    if( is_netmon_visible() ) {
        return ;
    }
    F_4315(); // this seems to be Tytera's own "painter" for update_scr_17.
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
  // green_led_timer = 5; // rx_screen_blue_hook() called ? very short pulse with the green LED !

    netmon_update();

#if( CONFIG_MORSE_OUTPUT ) 
    narrate(); // continue "telling a story" in Morse code
#endif

#  if( CONFIG_APP_MENU ) 
    if( Menu_DrawIfVisible(AM_CALLER_RX_SCREEN_BLUE_HOOK) )  
     { return; // the menu covers the entire screen, so don't draw anything else
     }
#  endif // CONFIG_APP_MENU ?

#ifdef CONFIG_GRAPHICS
    if( global_addl_config.userscsv > 0 && !is_menu_visible() ) {
      if( global_addl_config.userscsv == 2) {
           draw_rx_screen(0xff8032);      // ta
      } else {
           draw_rx_screen(0xff8032);
      }
    } else {
        gfx_drawbmp(bmp, x, y);
    }
#endif //CONFIG_GRAPHICS

}

void rx_screen_gray_hook(void *bmp, int x, int y)
{
    netmon_update();

#  if( CONFIG_MORSE_OUTPUT ) 
    narrate(); // continue "telling a story" in Morse code
#  endif

#  if( CONFIG_APP_MENU ) 
    if( Menu_DrawIfVisible(AM_CALLER_RX_SCREEN_GRAY_HOOK) )  
     { return; // the menu covers the entire screen, so don't draw anything else
     }
#  endif // CONFIG_APP_MENU ?

#ifdef CONFIG_GRAPHICS
    if( global_addl_config.userscsv > 0 && !is_menu_visible() ) {
       if( global_addl_config.userscsv == 2) {
           draw_rx_screen(0x888888);      // ta
       } else {
           draw_rx_screen(0x888888);
       }
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

#ifdef FW_S13_020
extern void gui_control( int r0 );
extern void gui_control_hook( int r0 );
    
void f_4520_hook()
{
   {
      uint32_t *p = (uint32_t*)0x2001e6bc;
//    *p = 0x00080001 ;
      *p = 0x0 ;
   }
//    void *return_addr;
//    void *sp;
//    __asm__("mov %0,r14" : "=r" (return_addr));
//    __asm__("mov %0,r13" : "=r" (sp));
//    printf("Call md380_f_4520 from r: %x s: %x\n", return_addr, sp);
    md380_f_4520();
    
    static int once = 1 ;   
    
    if( once ) {
        once = 0 ;
        gui_control_hook(241);
    }
}
#endif

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


#ifdef FW_D13_020
/**
 * write centered horizontally / vertically
 */
void gfx_drawtext10_hook(wchar_t *str, int x1, int y1, int x2, int y2)
{
    PRINT("swts: %S %d %d %d %d\n", str, x1, y1, x2, y2);
    gfx_drawtext10(str,x1,y1,x2,y2);
}
#endif  

void trace_gui_opmode1()
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

void trace_gui_opmode2()
{
    static int old = -1 ;
    int new = gui_opmode2 ;
    if( old != new ) {
        PRINT( "mode2: %d -> %d\n", old, new );
        LOGG( "mode2: %d -> %d\n", old, new );
        old = new ;
#      if( CONFIG_MORSE_OUTPUT ) 
        narrate(); // may begin to "tell a different story" in Morse code (?)
#      endif
    }
}

#if defined(FW_D13_020) || defined(FW_S13_020)
void trace_gui_opmode3()
{
    static int old = -1 ;
    int new = gui_opmode3 ;
    if( old != new ) {
        PRINT( "mode3: %d -> %d\n", old, new );
        LOGG( "mode3: %d -> %d\n", old, new );
        old = new ;
#      if( CONFIG_MORSE_OUTPUT ) 
        narrate(); // "say what's going on" (gui_opmode3 related ?)
#      endif
    }
}
#else
void trace_gui_opmode3()
{
}
#endif

void f_4225_hook()
{
    
#ifdef DEBUG    
    trace_gui_opmode1();
    trace_gui_opmode2();
    trace_gui_opmode3();
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

#  if( CONFIG_MORSE_OUTPUT ) 
    narrate(); // "tell what's going on" (after change in gui_opmode*)
#  endif

#  if( CONFIG_APP_MENU ) 
    if( Menu_DrawIfVisible(AM_CALLER_F_4225_HOOK) )  
     { return; // THIS call made the 'App Menu' work on a busy FM channel
       // (when Tytera doesn't paint anything to camouflage the QRM
       //  caused by traffic on the 8-bit LCD interface / connector cable) 
     }
#  endif // CONFIG_APP_MENU ?


#if 0
    int upd = gui_opmode1 > 0x7F ;
    if( upd == 0 ) {
        if( is_netmon_visible() ) {
            con_redraw();            
        } else {
            f_4225();
        }
    } else {
        f_4225();        
    }
#else     
    f_4225();        
#endif    
    
    if( is_netmon_visible() ) {
        
        // steer back to idle screen, because that's the most intercepted.
        if( gui_opmode2 == OPM2_VOICE ) {
            gui_opmode2 = OPM2_IDLE;
        }
    }
    
}

#ifdef FW_S13_020

void gui_control_hook( int r0 ) 
{
    PRINTRET();
    PRINT("gc %d\n", r0 );
    gui_control( r0 );
//    PRINT("-> %d\n", r0 );
//    return r0 ;

#  if( CONFIG_MORSE_OUTPUT ) 
    narrate(); // not sure what gui_control_hook() actually does, but
               // if won't hurt to let the "CW storyteller" take a look
               // at the current values in gui_opmode_x again. (?)
#  endif

}
#endif
