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
#include "radio_config.h"
#include "sms.h"

#include <stdint.h>

// 1 = pressed
// 2 = release within timeout
// 1+2 = pressed during rx
// 4+1 = pressed timeout
// 8 = rearm
// 0 = none pressed

#ifndef FW_D02_032
inline int get_main_mode()
{
    return md380_f_4225_operatingmode & 0x7F ;
}

void reset_backlight()
{
    // struct @ 0x2001dadc
    backlight_timer = md380_radio_config.backlight_time * 500 ;

#if defined(FW_D13_020)
    // enabling backlight again.
    void (*f)(uint32_t,uint32_t) = (void*)( 0x802b80a + 1 ); // S: ??? 0x0802BAE6
    f(0x40020800,0x40);
#else
#warning please consider adding symbols.
#endif
}

int beep_event_probe = 0 ;

void switch_to_screen( int scr )
{
    // cause transient -> switch back to idle screen.
    gui_opmode2 = OPM2_MENU ;
    md380_f_4225_operatingmode = SCR_MODE_IDLE | 0x80 ;
    
    nm_screen = scr ;
}

void handle_hotkey( int keycode )
{
    PRINT("handle hotkey: %d\n", keycode );
    
    reset_backlight();
    
    switch( keycode ) {
        case 4 :
            sms_test();
            break ;
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
            nm_screen = 0 ;
            // cause transient.
            gui_opmode2 = OPM2_MENU ;
            md380_f_4225_operatingmode = SCR_MODE_IDLE | 0x80 ;
            break ;
        case 8 :
            switch_to_screen(1);
            break ;
        case 9 :
            switch_to_screen(2);
            break ;
        case 11 :
            beep_event_probe++ ;
            //sms_test2(beep_event_probe);
            //mb_send_beep(beep_event_probe);
            break ;
        case 12 :
            beep_event_probe-- ;
            //sms_test2(beep_event_probe);
            //mb_send_beep(beep_event_probe);
            break ;
        case 15 :
            syslog_redraw();
            switch_to_screen(3);
            break ;
    }    
}

void trace_keyb(int sw)
{
    static uint8_t old_kp = -1 ;
    uint8_t kp = kb_keypressed ;
    
    if( old_kp != kp ) {
        PRINT("kp: %d %02x -> %02x (%04x) (%d)\n", sw, old_kp, kp, kb_key_row_col, kb_keycode );
        old_kp = kp ;
    }
}

inline int is_intercept_allowed()
{
    if( !is_netmon_enabled() ) {
        return 0 ;
    }
    
//    switch( get_main_mode() ) {
//        case 28 :
//            return 1 ;
//        default:
//            return 0 ;
//    }
    
    switch( gui_opmode2 ) {
        case OPM2_MENU :
            return 0 ;
        //case 2 : // voice
        //case 4 : // terminate voice
        //    return 1 ;
        default:
            return 1 ;
    }
}

inline int is_intercepted_keycode( int kc )
{
    switch( kc ) {
        case 4 :            
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
#endif //ndef FW_D02_032

extern void kb_handler();

void kb_handler_hook()
{
#ifndef FW_D02_032
    trace_keyb(0);

    kb_handler();

    trace_keyb(1);

    if( is_intercept_allowed() ) {
        int kc = kb_keycode ;
        if( is_intercepted_keycode(kc) ) {
            int kp = kb_keypressed ;

            if( (kp & 2) == 2 ) {
                kb_keypressed = 8 ;
                handle_hotkey(kc);
                return ;
            }
        }
    }
#else
#warning please consider hooking.
    return;
#endif
}
