/*
 *  keyb.c
 * 
 */

#define DEBUG

#include "keyb.h"

#include "debug.h"
#include "netmon.h"
#include "mbox.h"
#include "console.h"
#include "syslog.h"
#include "lastheard.h"
#include "radio_config.h"
#include "sms.h"
#include "beep.h"
#include "codeplug.h"
#include "radiostate.h"
#include "printf.h"

#include <stdint.h>

uint8_t kb_backlight=0; // flag to disable backlight via sidekey.
// Other keyboard-related variables belong to the original firmware,
// e.g. kb_keypressed, address defined in symbols_d13.020 (etc).


// Values for kp
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
    void (*f)(uint32_t,uint32_t) = (void*)( 0x0802b80a + 1 ); // S: ??? 0x0802BAE6
    f(0x40020800,0x40);
#elif defined(FW_S13_020)
    // enabling backlight again on MD390/G in monitor mode
    void (*f)(uint32_t,uint32_t) = (void*)( 0x0802bae6 + 1 ); // S: ??? 0x0802BAE6
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
#if defined(FW_D13_020) || defined(FW_S13_020)
    int dst = rst_dst ;
    extern wchar_t channel_name[] ;
    
    contact.id_l = dst & 0xFF ;
    contact.id_m = (dst>>8) & 0xFF ;
    contact.id_h = (dst>>16) & 0xFF ;
    
    //wchar_t *p = (void*)0x2001e1f4 ;
    wchar_t *p = (void*)channel_name ;
    
    if( rst_grp ) {
        contact.type = CONTACT_GROUP ;        
        snprintfw( p, 16, "TG %d", dst );
    } else {
        snprintfw( p, 16, "U %d", dst );
        contact.type = CONTACT_USER ;        
    }

    /* I can't see how this doesn't crash, as draw_zone_channel() is
       an even address. --Travis
    */
    extern void draw_zone_channel(); // TODO.
    
    draw_zone_channel();
#else
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
        case 0 :
	    clog_redraw();
            switch_to_screen(6);
            break ;
        case 1 :
            sms_test();
            break ;
        case 2 :
	    slog_redraw();
            switch_to_screen(5);
            break ;
        case 3 :
            copy_dst_to_contact();
            break ;
        case 4 :
	    lastheard_redraw();
            switch_to_screen(4);
            break ;
        case 5 :
            syslog_clear();
	    lastheard_clear();
	    slog_clear();
	    clog_clear();
	    nm_started = 0;	// reset nm_start flag used for some display handling
	    nm_started5 = 0;	// reset nm_start flag used for some display handling
	    nm_started6 = 0;	// reset nm_start flag used for some display handling
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

void handle_sidekey( int keycode, int keypressed )
{
    if ( keycode == 18 ) {												//top button
    	if ( (keypressed & 2) == 2 && kb_top_side_key_press_time < kb_side_key_max_time) {									//short press
    		evaluate_sidekey( top_side_button_pressed_function );
    	}
    	else if ( keypressed == 5) {									//long press
    		evaluate_sidekey( top_side_button_held_function );
    	}
    }
    else if ( keycode == 17 ) {											//bottom button
    	if ( (keypressed & 2) == 2 && kb_bot_side_key_press_time < kb_side_key_max_time) {									//short press
			evaluate_sidekey( bottom_side_button_pressed_function );
		}
		else if ( keypressed == 5 ) {									//long press
			evaluate_sidekey( bottom_side_button_held_function );
		}
    }
}

void evaluate_sidekey ( int button_function)							//This is where new functions for side buttons can be added
{
	switch ( button_function ) {										//We will start at 0x50 to avoid conflicting with any added functions by Tytera.
		case 0x50 :														//Toggle backlight enable pin to input/output. Disables backlight completely.
		{
			#if (CONFIG_DIMMED_LIGHT) // If backlight dimmer is enabled, we will use that instead.
				kb_backlight ^= 0x01; // flag for SysTick_Handler() to turn backlight off completely
			#else
				GPIOC->MODER = GPIOC->MODER ^ (((uint32_t)0x01) << 12);
            #endif
			reset_backlight();
			break;
		}
		default:
			return;
	}

	kb_keypressed = 8 ;											//Sets the key as handled. The firmware will ignore this button press now.
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
        case 0 :
        case 1 :
        case 2 :
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
    
    int kp = kb_keypressed ;
    int kc = kb_keycode ;
    // allow calling of menu during qso.
    // not working correctly.
    if( global_addl_config.experimental ) {
        if( (kp & 2) == 2 ) {
            if( gui_opmode2 != OPM2_MENU ) {
                gui_opmode2 = OPM2_MENU ;
                gui_opmode1 = SCR_MODE_IDLE | 0x80 ;
            }
        }
    }

    if( is_intercept_allowed() ) {
        if( is_intercepted_keycode(kc) ) {
            if( (kp & 2) == 2 ) {
                kb_keypressed = 8 ;
                handle_hotkey(kc);
                return ;
            }
        }
    }

    if ( kc == 17 || kc == 18 ) {
    	if ( (kp & 2) == 2 || kp == 5 ) {					//The reason for the bitwise AND is that kp can be 2 or 3
    		handle_sidekey(kc, kp);							//A 2 means it was pressed while radio is idle, 3 means the radio was receiving
    		return;
    	}
    }

#else
#warning please consider hooking.
    return;
#endif
}
