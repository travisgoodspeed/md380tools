/*
 *  keyb.c
 * 
 */

#define DEBUG

#include "config.h"

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
#include "menu.h" // create_menu_entry_set_tg_screen() called from keyb.c !
#if( CONFIG_MORSE_OUTPUT ) 
# include "irq_handlers.h"
# include "narrator.h"
#endif
#if( CONFIG_APP_MENU )
# include "app_menu.h"
#endif
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
    // # warning please consider hooking. // too many warnings - see issue #704 on github
#else //TODO add support for other firmware, e.g. D02.032 (?)

#endif
}

int beep_event_probe = 0 ;

void switch_to_screen( int scr )
{
    // cause transient -> switch back to idle screen.
    gui_opmode2 = OPM2_MENU ;
    gui_opmode1 = SCR_MODE_IDLE | 0x80 ;
      // (DL4YHF: this was unreliable in certain situations,
      //          see notes in src/app_menu.c : Menu_Close() )
    
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
        case KC_0 :
            clog_redraw();
            switch_to_screen(6);
            break ;
        case KC_1 :
            sms_test();
            break ;
        case KC_2 :
            slog_redraw();
            switch_to_screen(5);
            break ;
        case KC_3 :
            copy_dst_to_contact();
            break ;
        case KC_4 :
            lastheard_redraw();
            switch_to_screen(4);
            break ;
        case KC_5 :
            syslog_clear();
            lastheard_clear();
            slog_clear();
            clog_clear();
            nm_started = 0;     // reset nm_start flag used for some display handling
            nm_started5 = 0;    // reset nm_start flag used for some display handling
            nm_started6 = 0;    // reset nm_start flag used for some display handling
            break ;
        case KC_6 :
            {
                static int cnt = 0 ;
                syslog_printf("=dump %d=\n",cnt++);
            }
            syslog_dump_dmesg();
            break ;
        case KC_7 :
            bp_send_beep(BEEP_TEST_1);
            nm_screen = 0 ;
            // cause transient.
            gui_opmode2 = OPM2_MENU ;
            gui_opmode1 = SCR_MODE_IDLE | 0x80 ;
            break ;
        case KC_8 :
            bp_send_beep(BEEP_TEST_2);
            switch_to_screen(1);
            break ;
        case KC_9 :
            bp_send_beep(BEEP_TEST_3);
            switch_to_screen(2);
            break ;
        case KC_UP :
            //gui_control(1);
            //bp_send_beep(BEEP_9);
            //beep_event_probe++ ;
            //sms_test2(beep_event_probe);
            //mb_send_beep(beep_event_probe);
            break ;
        case KC_DOWN :
            //gui_control(241);
            //bp_send_beep(BEEP_25);
            //beep_event_probe-- ;
            //sms_test2(beep_event_probe);
            //mb_send_beep(beep_event_probe);
            break ;
        case KC_OCTOTHORPE :
            syslog_redraw();
            switch_to_screen(3);
            break ;
    }    
}

void handle_sidekey( int keycode, int keypressed )
{
  // red_led_timer = 20; // do we also get here for the 'original' sidekey functions, e.g. "zone inc" ?
     // (obviously NOT, for example the displayed zone changed long BEFORE the 'red LED flash')

    if ( keycode == 18 ) {             //top button
      if ( (keypressed & 2) == 2 && kb_top_side_key_press_time < kb_side_key_max_time) {  //short press
        evaluate_sidekey( top_side_button_pressed_function );
      }
      else if ( keypressed == 5) {     //long press
        evaluate_sidekey( top_side_button_held_function );
      }
    }
    else if ( keycode == 17 ) {        //bottom button
      if ( (keypressed & 2) == 2 && kb_bot_side_key_press_time < kb_side_key_max_time) { //short press
      evaluate_sidekey( bottom_side_button_pressed_function );
    }
    else if ( keypressed == 5 ) {      //long press
      evaluate_sidekey( bottom_side_button_held_function );
    }
    } // what ends here ? Avoid bad indentation by turning off TABS (insert spaces instead)
}

void evaluate_sidekey( int button_function) // This is where new functions for side buttons can be added
{
  switch ( button_function ) {  // We will start at 0x50 to avoid conflicting with any added functions by Tytera.
    case 0x50 :                 // Toggle backlight enable pin to input/output. Disables backlight completely.
      #if (CONFIG_DIMMED_LIGHT) // If backlight dimmer is enabled, we will use that instead.
        kb_backlight ^= 0x01;   // flag for SysTick_Handler() to turn backlight off completely
      #else
        GPIOC->MODER = GPIOC->MODER ^ (((uint32_t)0x01) << 12);
      #endif
      reset_backlight();
      break;
    case 0x51 :    // "Set Talkgroup"
      create_menu_entry_set_tg_screen(); 
      // Creating the menu entry seems ok here, 
      // but it's impossible (or at least unsafe) to ENTER / invoke the menu from here.
      // Call stack: kb_handler_hook() -> handle_sidekey() -> evaluate_sidekey()
      //               |__ in D13.020, patched to address 0x0804ebd2, thus
      //                   called from task 'biglist_pollsubsys_maybe', when the
      //                   shared keyboard/LCD interface is configured to poll the keyboard,
      //                   not to 'drive the display'. See the monster-disassembly.
      // If available, open the 'alternative menu' (aka 'app menu'), and tell it
      // to jump to the item labelled 'TkGrp' (short for Set Talkgroup):
#    if (CONFIG_APP_MENU)       // optional feature - see config.h 
      Menu_Open( NULL/*default instance*/, NULL/*main items*/, "TkGrp"/*cpJumpToItem*/, APPMENU_EDIT_OVERWRT );   
#    endif

      break;
#  if( CONFIG_MORSE_OUTPUT )    // optional feature - see config.h 
    case 0x52 : // starts the 'Morse narrator' via programmable button ("on request")
      narrator_start_talking(); // doesn't call anything in the original firmware
      break;
    case 0x53 : // repeats the last 'Morse anouncement' (short, not the full story)
      narrator_repeat();
      break;
#  endif
    default:
      return;
  }

  kb_keypressed = 8 ; // Sets the key as handled. The firmware will ignore this button press now.
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

inline int is_intercepted_keycode( uint8_t kc )
{
    switch( kc ) {
        case KC_0 :
        case KC_1 :
        case KC_2 :
        case KC_3 :
        case KC_4 :
        case KC_5 :
        case KC_6 :
        case KC_7 :
        case KC_8 :
        case KC_9 :
        case KC_UP :
        case KC_DOWN :
        case KC_OCTOTHORPE :
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
        if( (kp & 3) == 3 ) {
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
      if ( (kp & 2) == 2 || kp == 5 ) { // The reason for the bitwise AND is that kp can be 2 or 3
        handle_sidekey(kc, kp);         // A 2 means it was pressed while radio is idle, 3 means the radio was receiving
        return;
      }
    }

#  if( CONFIG_APP_MENU )  // prevent opening "green key menu" as long as our "alternative menu" is open
    if( Menu_IsVisible() )  
     { 
       if( (kp & 2) == 2 )   // 'speaking' macro constants would be nice to have here.
        { kb_keypressed = 8; // Sets the key as handled. The firmware will ignore this button press now.
          // (would be nice if it did. But Tytera's original menu still flashed up
          //  when leaving our ALTERNATIVE menu (aka 'app menu') via GREEN key)
        }
       return; // "return early", don't let kb_handler() process 'kb_row_col_pressed' at all .
     }
#  endif // CONFIG_APP_MENU ?


#else //TODO add support for other firmware, e.g. D02.032 (?)
    // # warning please consider hooking. // too many warnings - see issue #704 on github
    return;
#endif
}
