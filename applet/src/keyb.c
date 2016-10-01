/*
 *  keyb.c
 * 
 */

#include "keyb.h"

//#include "netmon.h"
#include "debug.h"
#include "netmon.h"

#include <stdint.h>

uint8_t *keypressed_p = 0x2001e5f8 ;
uint8_t *keycode_p = 0x2001e890 ;
    
//void intercept_keyb()
//{    
//    int keycode = *keycode_p ;
//    int keypressed = *keypressed_p & 1 ;
//    
//    if( keycode == 0xA ) {
//        // menu
//        return ;
//    }
//    
//    if( !keypressed ) {
//        return ;
//    }
//    switch( keycode ) {
//        case 7 :
//        case 8 :
//        case 9 :
//            break ;
//        default:
//            // early escape.
//            return ;
//    }
//    
//    // clear
//    *keypressed_p &= ~1 ;
//    
//    if( is_netmon_visible() ) {
////        netmon_printf("%02x  ", *keypress_flag);    
//        netmon_printf("%02x:", keycode);    
//    }
//        
//}

inline int get_main_mode()
{
    return md380_f_4225_operatingmode & 0x7F ;
}

void trace_keyb()
{
    int keypressed = *keypressed_p ;
    
    if( is_netmon_visible() ) {
        netmon_printf("%02x  ", keypressed );    
//        netmon_printf("%02x:", keycode);    
    }
    
    if( get_main_mode() != 28 ) {
        return ;
    }
    
    int keycode = *keycode_p ;
    
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
        default:
            // early exit
            return ;
    }
    
    *keypressed_p = 0 ;
}

extern void kb_handler();

void kb_handler_hook()
{    
#if defined(FW_D13_020)
    kb_handler();
#else
#warning please consider hooking.    
#endif    
    trace_keyb();
}
