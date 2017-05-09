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

typedef enum {
	KC_0 = 0,
	KC_1 = 1,
	KC_2 = 2,
	KC_3 = 3,
	KC_4 = 4,
	KC_5 = 5,
	KC_6 = 6,
	KC_7 = 7,
	KC_8 = 8,
	KC_9 = 9,
	KC_MENU = 10,
	KC_UP = 11,
	KC_DOWN = 12,
	KC_BACK = 13,
	KC_ASTERISK = 14,
	KC_OCTOTHORPE = 15
} keycode_t;

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

