/*
 *  keyb.c
 * 
 */

#define DEBUG

#include "keyb.h"

#include "debug.h"
#include "netmon.h"
#include "mbox.h"
#include "debug.h"
#include "console.h"
#include "syslog.h"

#include <stdint.h>

uint8_t *keypressed_p = (void*)0x2001e5f8 ;
uint8_t *kb_keycode = (void*)0x2001e890 ;
uint16_t *kb_row_col_pressed = (void*)0x2001e7ba ;
    
inline int get_main_mode()
{
    return md380_f_4225_operatingmode & 0x7F ;
}

int beep_event_probe = 0 ;

void handle_hotkey( int keycode )
{
    PRINT("handle hotkey: %d\n", keycode );
    
    switch( keycode ) {
        case 5 :
            syslog_clear();
            break ;
        case 6 :
        {
            static int cnt = 0 ;
            syslog_printf("=dump %d=\n",cnt++);
        }
            syslog_dump_dmesg();
            break ;
        case 7 :
            global_addl_config.console = 0 ;
            // cause transient.
            gui_opmode2 = OPM2_MENU ;
            md380_f_4225_operatingmode = SCR_MODE_IDLE | 0x80 ;
            break ;
        case 8 :
            global_addl_config.console = 1 ;
            break ;
        case 9 :
            global_addl_config.console = 2 ;
            break ;
        case 11 :
            beep_event_probe++ ;
            mb_send_beep(beep_event_probe);
            break ;
        case 12 :
            beep_event_probe-- ;
            mb_send_beep(beep_event_probe);
            break ;
        case 15 :
            syslog_redraw();
            global_addl_config.console = 3 ;
            break ;
    }    
}

void trace_keyb(int sw)
{
    static uint8_t old_kp = -1 ;
    uint8_t kp = *keypressed_p ;
    
    if( old_kp != kp ) {        
        PRINT("kp: %d %02x -> %02x (%04x) (%d)\n", sw, old_kp, kp, *kb_row_col_pressed, *kb_keycode );
        old_kp = kp ;
    }
}

inline int is_intercept_allowed()
{
//    switch( get_main_mode() ) {
//        case 28 :
//            return 1 ;
//        default:
//            return 0 ;
//    }
    switch( gui_opmode2 ) {
        case OPM2_MENU :
            return 0 ;
        default:
            return 1 ;
    }
}

inline int is_intercepted_keycode( int kc )
{
    switch( kc ) {
        case 5 :            
        case 6 :
        case 7 :
        case 8 :
        case 9 :
        case 11 :
        case 12 :
        case 15 :
            return 1 ;
        default:
            return 0 ;
    }    
}

extern void kb_handler();

void kb_handler_hook()
{
    trace_keyb(0);
    
#if defined(FW_D13_020)
    kb_handler();
#else
#warning please consider hooking.    
#endif    
    
    trace_keyb(1);

    if( is_intercept_allowed() ) {
        int kc = *kb_keycode ;
        if( is_intercepted_keycode(kc) ) {
            int kp = *keypressed_p ;
            
            if( kp == 2 ) {
                *keypressed_p = 0 ;
                handle_hotkey(kc);
                return ;
            }
        }        
    }
    
//    if( global_addl_config.experimental ) {
//        static uint16_t old = 0 ;
//        uint16_t new = *kb_row_col_pressed ;
//        if( old != new ) {
//            LOGR("rc: %04x -> %04x \n", old, new );
//            old = new ;
//        }
//        *kb_row_col_pressed = 0 ;
//    }

}
