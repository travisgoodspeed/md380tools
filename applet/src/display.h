/*
 *  display.h
 * 
 * for high-level drawing functions.
 * 
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
    
#define D_ICON_ANT_X 0     
#define D_ICON_ANT_Y 0     

#define D_DATETIME_Y 96
    
// status text coordinates    
#define STATUS_X 10
#define STATUS_Y 55    

// second line in idle screen
    
#define D_ICON1_X 2
#define D_ICON1_Y 16
    
#define D_ICON2_X 2
#define D_ICON2_Y 57
    
#define D_TEXT_CHANNAME_X 45    
#define D_TEXT_CHANNAME_Y 34
    
#define D_TEXT_ZONENAME_X 34    
#define D_TEXT_ZONENAME_Y 75
    
#if 0    
= draw channel =
        
gfx_drawtext4( cn, 45, 34, 157 );        
    
= idle screen =
    
gfx_drawbmp( x, 2, 57 );
gfx_drawtext2( x, 10, 110, 157 );

// line 75
gfx_drawtext4( zonename, 34, 75, 157 );
gfx_drawtext2( x, 105, 75, 157 );

// @ 0x0802d712  
gfx_drawchar_pos( '1', 139, 75 );
gfx_drawchar_pos( n,  146, 75 );

//        
gfx_drawtex6( sp+4, 25, 32, 18 );        
#endif    
    
/*  see 0x0801f06a there are a lot of modes */

// 16  = ? not seen
// 17  = public call rx
// 18  = ? not seen
// 19  = ? after rx call?
// 27  = menu refresh
// 28  = idle channel screen
// 35  = volume screen
//
// high bit (0x80) signals transition
// 156(28) = channel switch
// 115 = menu start

#define SCR_MODE_16 16
#define SCR_MODE_RX_VOICE 17 // rx/tx in progress.
#define SCR_MODE_RX_TERMINATOR 18 // rx call end LC
#define SCR_MODE_IDLE 19 // when channel RX but other timeslot.
#define SCR_MODE_20 20
#define SCR_MODE_21 21 // initial screen?
#define SCR_MODE_22 22
#define SCR_MODE_MENU 27
#define SCR_MODE_CHAN_IDLE_INIT 28
#define SCR_MODE_29 29
#define SCR_MODE_30 30
#define SCR_MODE_31 31
#define SCR_MODE_32 32
#define SCR_MODE_33 33
#define SCR_MODE_VOLUME 35
#define SCR_MODE_36 36


void draw_eye_opt();
void draw_micbargraph();
void draw_rx_screen(unsigned int bg_color);
void draw_ta_screen(unsigned int bg_color);

void draw_statusline( uint32_t r0 ); // in md380

void draw_datetime_row(); // in md380

#define OPM2_IDLE 1
#define OPM2_VOICE 2
#define OPM2_TERM 4
#define OPM2_MSG_POPUP 5
#define OPM2_ALARM_RECV 7

#define OPM2_MENU 10

extern uint8_t gui_opmode2 ;
extern uint8_t gui_opmode3 ;

void display_credits();

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_H */
