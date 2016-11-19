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
#include "beep.h"
#include "codeplug.h"
#include "radiostate.h"
#include "printf.h"

#include <stdint.h>

// 1 = pressed
// 2 = release within timeout
// 1+2 = pressed during rx
// 4+1 = pressed timeout
// 8 = rearm
// 0 = none pressed

#if defined(FW_D13_020) || defined(FW_S13_020)
inline int get_main_mode()
{
    return gui_opmode1 & 0x7F ;
}

void reset_backlight()
{
    // struct @ 0x2001dadc
    backlight_timer = md380_radio_config.backlight_time * 500 ;

#if defined(FW_D13_020)
    // enabling backlight again.
    void (*f)(uint32_t,uint32_t) = (void*)( 0x802b80a + 1 ); // S: ??? 0x0802BAE6
    f(0x40020800,0x40);
#elseif defined(FW_S13_020)
    // enabling backlight again on MD390/G in monitor mode
    void (*f)(uint32_t,uint32_t) = (void*)( 0x802bae6 + 1 ); // S: ??? 0x0802BAE6
    f(0x40020800,0x40);
#warning please consider adding symbols.
#endif
}

int beep_event_probe = 0 ;

void switch_to_screen( int scr )
{
    // cause transient -> switch back to idle screen.
    gui_opmode2 = OPM2_MENU ;
    gui_opmode1 = SCR_MODE_IDLE | 0x80 ;
    
    nm_screen = scr ;
}

void copy_dst_to_contact()
{
#ifdef FW_D13_020
    int dst = rst_dst ;
    
    contact.id_l = dst & 0xFF ;
    contact.id_m = (dst>>8) & 0xFF ;
    contact.id_h = (dst>>16) & 0xFF ;
    
    wchar_t *p = (void*)0x2001e1f4 ;
    
    if( rst_grp ) {
        contact.type = CONTACT_GROUP ;        
        snprintfw( p, 16, "TG %d", dst );
    } else {
        snprintfw( p, 16, "U %d", dst );
        contact.type = CONTACT_USER ;        
    }    
    
    extern void draw_zone_channel(); // TODO.
    
    draw_zone_channel();
#endif
}

//#if defined(FW_D13_020) || defined(FW_S13_020)
#if defined(FW_S13_020)
extern void gui_control( int r0 );
#else
#define gui_control(x)
#endif

void handle_hotkey( int keycode )
{
    PRINT("handle hotkey: %d\n", keycode );
    
    reset_backlight();
    
    switch( keycode ) {
        case 3 :
            copy_dst_to_contact();
            break ;
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
            bp_send_beep(BEEP_TEST_1);
            nm_screen = 0 ;
            // cause transient.
            gui_opmode2 = OPM2_MENU ;
            gui_opmode1 = SCR_MODE_IDLE | 0x80 ;
            break ;
        case 8 :
            bp_send_beep(BEEP_TEST_2);
            switch_to_screen(1);
            break ;
        case 9 :
            bp_send_beep(BEEP_TEST_3);
            switch_to_screen(2);
            break ;
        case 11 :
            //gui_control(1);
            //bp_send_beep(BEEP_9);
            //beep_event_probe++ ;
            //sms_test2(beep_event_probe);
            //mb_send_beep(beep_event_probe);
            break ;
        case 12 :
            //gui_control(241);
            //bp_send_beep(BEEP_25);
            //beep_event_probe-- ;
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
        PRINT("kp: %d %02x -> %02x (%04x) (%d)\n", sw, old_kp, kp, kb_row_col_pressed, kb_keycode );
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
        case 3 :
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
#endif

extern void kb_handler();

void kb_handler_hook()
{
#if defined(FW_D13_020) || defined(FW_S13_020)
    trace_keyb(0);

    kb_handler();

    trace_keyb(1);
    
    // allow calling of menu during qso.
    // not working correctly.
    if( global_addl_config.experimental ) {
        int kp = kb_keypressed ;
        if( (kp & 2) == 2 ) {
            if( gui_opmode2 != OPM2_MENU ) {
                gui_opmode2 = OPM2_MENU ;
                gui_opmode1 = SCR_MODE_IDLE | 0x80 ;
            }
        }
    }

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
