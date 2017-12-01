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
	KC_STAR = 14,
	KC_HASH = 15,
        KC_NO_VALID_KEY = 0xFF // kludge for remote control (not used by Tytera)
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
// define keycodes for variable assignment
uint8_t kc_netmon1;
uint8_t kc_netmon2;
uint8_t kc_netmon3;
uint8_t kc_netmon4;
uint8_t kc_netmon5;
uint8_t kc_netmon6;
uint8_t kc_sms_test;
uint8_t kc_talkgroup;
uint8_t kc_copy_contact;
uint8_t kc_netmon_clear;
uint8_t kc_netmon_off;
uint8_t kc_syslog_dump;
uint8_t kc_cursor_up;
uint8_t kc_cursor_down;
uint8_t kc_greenmenu;
uint8_t kc_redback;
uint8_t kc_lastmode;

void evaluate_sidekey(int);
void kb_handle(int);
void set_keyb(int);
void sms_rpt(void);
void sms_wx(void);
void sms_gps(void);

keycode_t kb_ASCIItoTytera(uint8_t ascii);
void kb_OnRemoteKeyEvent(uint8_t key_ascii, uint8_t key_down_flag );

#else
# define CAN_POLL_KEYS 0  /* 0 : cannot poll keys for this firmware yet */
#endif

#ifdef __cplusplus
}
#endif

#endif /* KEYB_H */

