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
uint8_t *keycode_p = (void*)0x2001e890 ;
    
inline int get_main_mode()
{
    return md380_f_4225_operatingmode & 0x7F ;
}

int beep_event_probe = 0 ;

void handle_hotkey( int keycode )
{
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
            gui_opmode2 = OPM2_IDLE ;
            md380_f_4225_operatingmode = SCR_MODE_IDLE | 0x80 ;
//            md380_f_4225_operatingmode = md380_f_4225_operatingmode | 0x80 ;
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

void intercept_keyb()
{
    int keypressed = *keypressed_p ;
    
//    if( is_netmon_visible() ) {
//        NMPRINT("%02x  ", keypressed );    
//    }
    
    switch( get_main_mode() ) {
        case 28 :
        case 19 :
        case 17 :
            break ;
        default:
            return ;
    }
    
    if( is_menu_visible() ) {
        return ;
    }
    
    //PRINT("%d %d\n", get_main_mode(), *mode2 );
    
    int keycode = *keycode_p ;
    switch( keycode ) {
        case 5 :            
        case 6 :
        case 7 :
        case 8 :
        case 9 :
        case 11 :
        case 12 :
        case 15 :
            break ;
        default:
            // don't process
            return ;
    }

    // detect transition
    static int oldstate = -1 ;
    
    int key_is_pressed = keypressed != 0 ;
    
    if( oldstate != key_is_pressed && key_is_pressed ) {
        handle_hotkey(keycode);
    }
    
    oldstate = key_is_pressed ;
    
    // eat key event.
    *keypressed_p = 0 ;
}

void trace_keyb()
{
    static uint8_t old_kp = -1 ;
    uint8_t kp = *keypressed_p ;
    
    if( old_kp != kp ) {
        PRINT("kp: %02x -> %02x\n", old_kp, kp );
        old_kp = kp ;
    }
}

extern void kb_handler();

uint32_t kb_handler_count = 0;

void kb_handler_hook()
{
    kb_handler_count++;

#if defined(FW_D13_020)
    kb_handler();
#else
#warning please consider hooking.    
#endif    
    if( global_addl_config.experimental ) {
        trace_keyb();
        return;
    }
    if( global_addl_config.debug ) {
        intercept_keyb();
    }

}
