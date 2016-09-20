/*
 *  display.h
 * 
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#ifdef __cplusplus
extern "C" {
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
// 156 = channel switch
// 115 = menu start

#define SCR_MODE_16 16
#define SCR_MODE_17 17 // rx/tx in progress.
#define SCR_MODE_18 18
#define SCR_MODE_CHAN_19 19 // when channel RX but other timeslot.
#define SCR_MODE_20 20
#define SCR_MODE_21 21 // initial screen?
#define SCR_MODE_22 22
#define SCR_MODE_MENU 27
#define SCR_MODE_CHAN_IDLE 28
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


#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_H */

