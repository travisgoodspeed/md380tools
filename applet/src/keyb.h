/*
 *  keyb.h
 * 
 */

#ifndef KEYB_H
#define KEYB_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


void f_4101();

extern uint8_t kb_backlight; // flag to disable backlight via sidekey

#if defined(FW_D13_020) || defined(FW_S13_020)
# define CAN_POLL_KEYS 1
extern uint8_t kb_keycode;
extern uint8_t kb_keypressed;
extern uint8_t kb_key_press_time;
extern uint8_t top_side_button_pressed_function;
extern uint8_t bottom_side_button_pressed_function;
extern uint8_t top_side_button_held_function;
extern uint8_t bottom_side_button_held_function;
extern uint16_t kb_row_col_pressed;
extern uint16_t backlight_timer;   // seems to be a COUNTDOWN, decremented every 10 (?) ms
extern uint8_t kb_top_side_key_press_time;
extern uint8_t kb_bot_side_key_press_time;
extern uint8_t kb_side_key_max_time;

void evaluate_sidekey(int);
#else
# define CAN_POLL_KEYS 0  /* 0 : cannot poll keys for this firmware yet */
#endif

#ifdef __cplusplus
}
#endif

#endif /* KEYB_H */

