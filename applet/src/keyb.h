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

#if defined(FW_D13_020) || defined(FW_S13_020)
extern uint8_t kb_keycode;
extern uint8_t kb_keypressed;
extern uint8_t kb_key_press_time;
extern uint16_t kb_row_col_pressed;
extern uint16_t backlight_timer;
#endif

#ifdef __cplusplus
}
#endif

#endif /* KEYB_H */

