/*
 *  keyb.c
 * 
 */

#define DEBUG

#include "config.h"

#include "keyb.h"

#include "md380.h"
#include "debug.h"
#include "netmon.h"
#include "mbox.h"
#include "console.h"
#include "gfx.h"
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


// Values for kp ( / kb_keypressed ? )
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
      // ( this was unreliable in certain situations,
      //   see notes in src/app_menu.c : Menu_Close() )
    
    nm_screen = scr ;
}

void netmon_off()
{
            bp_send_beep(BEEP_TEST_1);
            nm_screen = 0 ;
            // cause transient.
            gui_opmode2 = OPM2_MENU ;
            gui_opmode1 = SCR_MODE_IDLE | 0x80 ;
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
    if (global_addl_config.devmode_level != 0) 	
	PRINT("handle hotkey: %d\n", keycode );
    
    reset_backlight();
  
	if ( (keycode) == (kc_netmon_clear) ) {
            syslog_clear();
            lastheard_clear();
            slog_clear();
            clog_clear();
            nm_started = 0;     // reset nm_start flag used for some display handling
            nm_started5 = 0;    // reset nm_start flag used for some display handling
            nm_started6 = 0;    // reset nm_start flag used for some display handling

	} else if ( (keycode) == (kc_sms_test) ) {
		if (global_addl_config.sms_rpt != 0) {          
			sms_rpt();
		}

	} else if ( (keycode) == (kc_talkgroup) ) {
		if (global_addl_config.sms_wx != 0) {          
			sms_wx();
		}
		else {
			create_menu_entry_set_tg_screen();
		}

	} else if ( (keycode) == (kc_copy_contact) ) {
		if (global_addl_config.sms_gps != 0) {          
			sms_gps();
		}
		else {
 			copy_dst_to_contact();
		}

	} else if ( (keycode) == (kc_netmon1) ) {
            bp_send_beep(BEEP_TEST_2);
            switch_to_screen(1);

	} else if ( (keycode) == (kc_netmon2) ) {
            bp_send_beep(BEEP_TEST_3);
            switch_to_screen(2);

	} else if ( (keycode) == (kc_netmon3) ) {
            syslog_redraw();
            switch_to_screen(3);

	} else if ( (keycode) == (kc_netmon4) ) {
            lastheard_redraw();
            switch_to_screen(4);

	} else if ( (keycode) == (kc_netmon5) ) {
            slog_redraw();
            switch_to_screen(5);

	} else if ( (keycode) == (kc_netmon6) ) {
            clog_redraw();
            switch_to_screen(6);

	} else if ( (keycode) == (kc_redback) ) {
	   if (is_netmon_visible() ) {
		netmon_off();
		}

	} else if ( (keycode) == (kc_netmon_off) ) {
	    netmon_off();
	
	} else if ( (keycode) == (kc_cursor_up) ) {
	    return ;	

	} else if ( (keycode) == (kc_cursor_down) ) {
	    return ;	

	} else if ( (keycode) == (kc_syslog_dump) ) {
            static int cnt = 0 ;
            syslog_printf("=dump %d=\n",cnt++);
            syslog_dump_dmesg();
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
    case 0x54 :    // toggle display

	global_addl_config.chan_stat++;

	switch ( global_addl_config.chan_stat ) {  // We are using 0x54 to avoid conflicting with any added functions by Tytera.
	    case 1 : 
		global_addl_config.mode_stat = 2;	// show cc and cts in status
		cfg_save();
		gfx_set_fg_color(0x000000);
		gfx_set_bg_color(0xff8032);
    		gfx_select_font(gfx_font_small); 
		gfx_printf_pos2(35, 63, 120, "                     " ); 
    		gfx_select_font(gfx_font_norm); 
		break;
	    case 2 :  
		global_addl_config.chan_stat = 4;	// show rx/tx
		global_addl_config.mode_stat = 3;	// show cc and cts in status
		cfg_save();
		break;
	    case 5 :  
		global_addl_config.chan_stat = 1;	// TS/TG&Tone display
		global_addl_config.mode_stat = 2;	// show cc and cts in status
		cfg_save();
		gfx_set_fg_color(0x000000);
		gfx_set_bg_color(0xff8032);
    		gfx_select_font(gfx_font_small); 
		gfx_printf_pos2(35, 63, 120, "                     " ); 
    		gfx_select_font(gfx_font_norm); 
		break;
	default:
		global_addl_config.chan_stat = 1;	// show rx/tx
		global_addl_config.mode_stat = 2;	// show cc/ts/tg compact
		cfg_save();
		break;
        }
	break;

    case 0x55 :    // Mic gain off/3dB/6dB - we are using 0x55 to avoid conflicting with any functions by Tytera.
	switch ( global_addl_config.mic_gain ) {  
		case 0 :
		global_addl_config.mic_gain = 1;		// set mic gain 3dB
		cfg_save();
		break;    
		case 1 :
		global_addl_config.mic_gain = 2;		// set mic gain 6dB
		cfg_save();
		break;
		case 2 :
		global_addl_config.mic_gain = 0;		// set mic gain off
		cfg_save();
		break;
	default:
		break;
        }
	break;

    case 0x56 :    // promiscuous mode on/off - we are using 0x56 to avoid conflicting with any functions by Tytera.
	switch ( global_addl_config.promtg ) {
		case 0 :
		global_addl_config.promtg = 1;
		cfg_save();
		break;    
		case 1 :
		global_addl_config.promtg = 0;
		cfg_save();
		break;
	default:
		break;
        }
	break;
  }

  kb_keypressed = 8 ; // Sets the key as handled. The firmware will ignore this button press now.
}

void trace_keyb(int sw)
{
	if (global_addl_config.devmode_level > 2)
	{
		static uint8_t old_kp = -1 ;
		uint8_t kp = kb_keypressed ;
    
		if( old_kp != kp ) {
		PRINT("kp: %d %02x -> %02x (%04x) (%d)\n", sw, old_kp, kp, kb_row_col_pressed, kb_keycode );
		old_kp = kp ;
    		}
	}
}

void set_keyb(int keyb_mode)
{
#if defined(FW_D13_020) || defined(FW_S13_020)
//==========================================================================================
//
//			MD380Tools supporting now up to 4 different keyb layouts:
//			0 - legacy (the old standard)
//			1 - modern (based on the suggestions from DL2MF / PR#663)
//			2 - MD-446 (reduced keyboard version support by DL2MF)
//			3 - develop (based on the app-menu layout from DL4YHF)
//
// -----------------------------------------------------------------------------------------
//   Legacy:
//    ___________________________________    			
//   | 'M'ENU | cursor | cursor | 'B'ACK |     \  may have to be used
//   |(green) |  up, U | down,D | (red)  |     /  as 'A'..'D' for DTMF
//   |--------+--------+--------+--------|
//   |  '1'   |  '2'   |  '3'   |  '*'   |
//   | SMS Tst| NetMon5| CopyTG | unused |
//   |--------+--------+--------+--------|
//   |  '4'   |  '5'   |  '6'   |  '0'   |
//   | NetMon4| NetmClr| SysLDmp| NetMon6|
//   |--------+--------+--------+--------|
//   |  '7'   |  '8'   |  '9'   |  '#'   |
//   | NetMOff| NetMon1| NetMon2| NetMon3|
//   |________|________|________|________|
//
//
// ------------------------------------------------------------------------
//   Modern:
//    ___________________________________    			
//   | 'M'ENU | cursor | cursor | 'B'ACK |     \  may have to be used
//   |(green) |  up, U | down,D | (red)  |     /  as 'A'..'D' for DTMF
//   |--------+--------+--------+--------|
//   |  '1'   |  '2'   |  '3'   |  '*'   |
//   | SMS Tst| unused | CopyTG | SysLDmp|
//   |--------+--------+--------+--------|
//   |  '4'   |  '5'   |  '6'   |  '0'   |
//   | NetMon4| NetMon5| NetMon6| NetMClr|
//   |--------+--------+--------+--------|
//   |  '7'   |  '8'   |  '9'   |  '#'   |
//   | NetMon1| NetMon2| NetMon3| NetMOff|
//   |________|________|________|________|
//
// ------------------------------------------------------------------------/   Develop:
//    ___________________________________    			
//   | 'M'ENU | cursor | cursor | 'B'ACK |     \  may have to be used
//   |(green) |  up, U | down,D | (red)  |     /  as 'A'..'D' for DTMF
//   |--------+--------+--------+--------|
//   |  '1'   |  '2'   |  '3'   |  '*'   |
//   | NetMon1| NetMon2| NetMon3| SysLDmp|
//   |--------+--------+--------+--------|
//   |  '4'   |  '5'   |  '6'   |  '0'   |
//   | NetMon4| NetMon5| NetMon6| NetMClr|
//   |--------+--------+--------+--------|
//   |  '7'   |  '8'   |  '9'   |  '#'   |
//   | SMSTst | unused | CopyTG | NetMOff|
//   |________|________|________|________|
//
// ------------------------------------------------------------------------
//   Tytera MD-446 Layout - 20170522 DL2MF
//    ___________________________   __    
//   | 'M'ENU | cursor | 'B'ACK |     \  mirrored to left 3 cols of
//   |(green) |   up   | (red)  |   __/  default MD380/MD390 layout
//   |--------+--------+--------|   __  
//   |  'P1'  | cursor |  'P2'  |     |  only P1 and P2
//   | NetM4  | Dn/Mnu | NetMOff|     |  are free configurable  
//   |________|________|________|   __|
//  
//==========================================================================================

	switch (keyb_mode) {		// configurable keyb layout introduced with MD-446 support
	
	case 0:		// legacy
			kc_sms_test = KC_1;
			kc_copy_contact = KC_3;

			kc_netmon1 = KC_8;
			kc_netmon2 = KC_9;
			kc_netmon3 = KC_HASH;
			kc_netmon4 = KC_4;
			kc_netmon5 = KC_2;
			kc_netmon6 = KC_0;
			kc_netmon_clear = KC_5;
			kc_netmon_off = KC_7;

			kc_syslog_dump = KC_6;
			kc_cursor_up = KC_UP;
			kc_cursor_down = KC_DOWN;
			kc_greenmenu = KC_MENU;
			kc_redback = KC_BACK;
			break ;

	case 1:		// modern
			kc_sms_test = KC_1;
			kc_talkgroup = KC_2;
			kc_copy_contact = KC_3;

			kc_netmon4 = KC_4;
			kc_netmon5 = KC_5;
			kc_netmon6 = KC_6;
			kc_netmon1 = KC_7;
			kc_netmon2 = KC_8;
			kc_netmon3 = KC_9;
			kc_netmon_clear = KC_0;
			kc_netmon_off = KC_HASH;

			kc_syslog_dump = KC_STAR;
			kc_cursor_up = KC_UP;
			kc_cursor_down = KC_DOWN;
			kc_greenmenu = KC_MENU;
			kc_redback = KC_BACK;
			break ;

	case 2:		// MD446
			kc_netmon1 = KC_7;
			kc_netmon2 = KC_8;
			kc_netmon3 = KC_9;
			kc_netmon4 = KC_4;
			kc_netmon5 = KC_5;
			kc_netmon6 = KC_6;
			kc_netmon_off = KC_HASH;

			kc_cursor_up = KC_UP;
			kc_cursor_down = KC_DOWN;
			kc_greenmenu = KC_MENU;
			kc_redback = KC_BACK;
			break ;

	case 3:		// develop
			kc_netmon1 = KC_1;
			kc_netmon2 = KC_2;
			kc_netmon3 = KC_3;
			kc_netmon4 = KC_4;
			kc_netmon5 = KC_5;
			kc_netmon6 = KC_6;
			kc_netmon_clear = KC_0;
			kc_netmon_off = KC_HASH;

			kc_sms_test = KC_7;
			kc_talkgroup = KC_8;
			kc_copy_contact = KC_9;

			kc_syslog_dump = KC_STAR;
			kc_cursor_up = KC_UP;
			kc_cursor_down = KC_DOWN;
			kc_greenmenu = KC_MENU;
			kc_redback = KC_BACK;
			break ;
	}

    LOGB("t=%d: keyb load %d\n", (int)IRQ_dwSysTickCounter, global_addl_config.keyb_mode ); // keyb layout loaded
#else
// TODO: keyb support for old firmware
#endif
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
        case KC_STAR :
        case KC_HASH :
            return 1 ;
        default:
            return 0 ;
    }    
}

inline int is_intercepted_keycode2(uint8_t kc)
{
	switch (kc) {
	
	case 20:
	case 21:
	case 13: //end call
		return 1;
	default:
		return 0;
	}
}
#endif

extern void kb_handler();


#if defined(FW_D13_020) || defined(FW_S13_020)
static int nextKey = -1;

void kb_handle(int key) {

	int kc = key;

	if (is_intercept_allowed()) {
		if (is_intercepted_keycode2(kc)) {
				handle_hotkey(kc);
				return;
		}
	}


	if ( (key == kc_cursor_up) || (key == kc_cursor_down) ) {
		kb_keycode = key;
		kb_keypressed = 2;
	}
}
#endif

void kb_handler_hook()
{

#if defined(FW_D13_020) || defined(FW_S13_020)

    trace_keyb(0);

    kb_handler();

    trace_keyb(1);

    if (nextKey > 0) {
	kb_keypressed = 2;
	kb_keycode = nextKey;
	nextKey = -1;
    }
    
    int kp = kb_keypressed ;
    int kc = kb_keycode ;

    if ( global_addl_config.keyb_mode == 1) {	// keyb layout = Modern

    	switch( kc ) {
        	case KC_BACK :
		   if (is_netmon_visible() ) {
			netmon_off();
			}
		break ;	
	}
    } 

    if ( global_addl_config.keyb_mode == 2) {	// keyb layout = MD-446 (select in setup)
   /* ================================================================================= */
   /*     Tytera MD-446 Layout - supported since 20170524                               */
   /* ================================================================================= */
   //    ___________________________    
   //   | 'M'ENU | cursor | 'B'ACK |   __
   //   |(green) |  up, U | (red)  |     \  mirrored to left 3 cols of
   //   | 0x0022 | 0x0012 | 0x000A |   __/  default MD380/MD390 layout
   //   |--------+--------+--------|   __  
   //   |  'P1'  | cursor |  'P2'  |     |
   //   |    3   |  dn, D |    1   |     |  only P1 up/DN P2
   //   | 0x0024 | 0x0014 | 0x000C |     |  
   //   |________|________|________|   --
   //  

    switch( kc ) {
        case 10 :
		if (kb_row_col_pressed == 0x000A)	// the MD-446 layout is something weird, so it needs some rewrites of keycodes!!!
		{
			kb_keycode = kc_redback;	// if the RedKey (key-10) was pressed, we need rewrite to RedKey function (kc=13)
			kc = kc_redback;		// RedKey - NEVER change this!!
		}
		else
		{
			kb_keycode = kc_greenmenu;	// if we got kc=10 not from 0x000A, this was rewrite result of GreenKey (key-12)
			kc = kc_greenmenu;		// NEVER change this!!
		}
		break;
        case 12 :					// if GreenKey (keycode=12) was pressed, we need to rewrite it to GreenKey function (kc=10)
		kb_keycode = kc_greenmenu;		// GreenKey - NEVER change this!!
		kc = kc_greenmenu;			// GreenKey - NEVER change this!!
        	break ;
        case 11 :
		kc = kc_cursor_up;			// Cursor up - NEVER change this!!
        	break ;
        case 3 :
		if( !is_netmon_visible() || kc_lastmode == 0x000C ) {
			kc = kc_netmon4;		// define your preferred P1 function: 0=Netmon6 2=Netmon5 4=Netmon4 8=Netmon1 9=Netmon2 15=Netmon3
			kc_lastmode = kb_row_col_pressed;// remember current mode for P1/P2 toggle keys
		} else {
			kc = kc_netmon_off;		// exit from Netmon screens
		}
        	break ;
        case 2 :
		kb_keycode = kc_cursor_down;		// Cursor down - also used for quick menu access!
		kc = kc_cursor_down;			// Cursor down - NEVER change this!!
        	break ;
        case 1 :
		if( !is_netmon_visible() || kc_lastmode == 0x0024 ) {
			kc = kc_netmon1;		// define your preferred P1 function: 0=Netmon6 2=Netmon5 4=Netmon4 8=Netmon1 9=Netmon2 15=Netmon3
			kc_lastmode = kb_row_col_pressed;// remember current mode for P1/P2 toggle keys
		} else {
			kc = kc_netmon_off;		// exit from Netmon screens
		}
        	break ;	
   	} 
    }	// if keyb_mode MD-446
/* ================================================================================= */

	if (kc == 20 || kc == 21) {
		kb_keypressed = 8;
		return;
	}

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

#if (CONFIG_APP_MENU)
    if( is_intercept_allowed() && !Menu_IsVisible() ) {
#else
    if( is_intercept_allowed() ) {
#endif
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

#if( CAN_POLL_KEYS )
//---------------------------------------------------------------------------
keycode_t kb_ASCIItoTytera(uint8_t ascii)
  // Converts a given 'ASCII' key ('M','U','D','B','0'..'9', '*','#')
  //   into Tytera's own key code (30, 11, 12, 13,  0(!)..9, 14, 15 ).
  // Used for REMOTE CONTROL via USB (which uses ASCII characters,
  //   not the strange 'Tytera' keyboard codes),
  //   and maybe for auto-repeat in the original menu.
{ 
  if( ascii>='0' && ascii<='9' )
   { return (keycode_t)(ascii-'0');
     // Not a bug but an annoying feature of Tytera's firmware:
     // code ZERO doesn't mean 'no key pressed' but 'digit zero' !
     // (that's why the 'app menu' and the remote control doesn't
     //  use any of these codes, but upper case ASCII )
   }
  switch( ascii )
   { case 'M' : return KC_MENU;
     case 'B' : return KC_BACK;
     case 'U' : return KC_UP;
     case 'D' : return KC_DOWN;
     case '*' : return KC_STAR; // ex: KC_ASTERISK (?)
     case '#' : return KC_HASH; // ex: KC_OCTOTHORPE (?)
     default  : return KC_NO_VALID_KEY; // bleah.. cannot use 0x00 for this
   }
} // end kb_ASCIItoTytera()

//---------------------------------------------------------------------------
void kb_OnRemoteKeyEvent( uint8_t key_ascii, uint8_t key_down_flag )
  // Called on reception of a 'remote keyboard event' from USB.C .
  // [in]  key_ascii : 'M','U','D','B','0'..'9','*','#' (keys on MD380 keyboard),
  //       key_down_flag : 0 = key not pressed but released, 1 = pressed (down) .
  // [out] keypress_ascii_remote : for the ALTERNATIVE menu, processed in irq_handlers.c;
  //       kb_keycode, kb_keypressed : set here to operate TYTERA's menu remotely .
{
  // For a simple start, only ONE key may be pressed at a time:
  if( key_down_flag )
   { // key has just been PRESSED (on the remote keyboard) :
     keypress_ascii_remote = key_ascii;
     // To pass on the 'remote' keyboard event to the original firmware, 
     // mimick the behaviour of Tytera's own keyboard handler, but 
     // DO NOT invoke anything in the original firmware to avoid multitasking issues
     // (the caller may even be an interrupt handler, so be very careful)
     kb_keycode = kb_ASCIItoTytera(keypress_ascii_remote);
     // The flags in kb_keypressed are strange. Only setting bit 0 here,
     // and clearing it on release opened the Tytera menu but nothing else worked.
     // So, instead of simply setting kb_keypressed = 1 :
     //     bit 0 : pressed
     //     bit 1 : release(d?) within timeout
     //     bit 0+1 : pressed during rx (does this really matter for Tytera's menu ?)
     //     bit 1+2 : pressed timeout   ?
     //     bit 3 : rearm
     kb_keypressed |= 3;
   }
  else  // key was RELEASED (remotely) ?
   { keypress_ascii_remote = 0;
     kb_keypressed = (kb_keypressed & ~3) | 8/*rearm*/;
   }
} // kb_OnRemoteKeyEvent()


#endif // CAN_POLL_KEYS ?

