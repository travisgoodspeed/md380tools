/*
 *  keyb.c
 * 
 */

#include "keyb.h"

#include "debug.h"
#include "netmon.h"
#include "mbox.h"

#include <stdint.h>

uint8_t *keypressed_p = 0x2001e5f8 ;
uint8_t *keycode_p = 0x2001e890 ;
    
inline int get_main_mode()
{
    return md380_f_4225_operatingmode & 0x7F ;
}

int beep_event_probe = 0 ;

void handle_hotkey( int keycode )
{
    switch( keycode ) {
        case 7 :
            global_addl_config.console = 0 ;
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
            global_addl_config.console = 3 ;
            break ;
    }    
}

void trace_keyb()
{
    int keypressed = *keypressed_p ;
    
    if( is_netmon_visible() ) {
        NMPRINT("%02x  ", keypressed );    
    }
    
    switch( get_main_mode() ) {
        case 28 :
        case 19 :
        case 17 :
            break ;
        default:
            return ;
    }
    
    int keycode = *keycode_p ;
    switch( keycode ) {
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

extern void kb_handler();

// kb_poller())
void kb_handler_hook()
{    
#if defined(FW_D13_020)
    kb_handler();
#else
#warning please consider hooking.    
#endif    
    if( global_addl_config.debug ) {
        trace_keyb();
    }
}
