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

#ifndef FW_D02_032
extern uint8_t kb_keycode;
extern uint8_t kb_keypressed;
extern uint8_t kb_key_press_time;
extern uint16_t kb_key_row_col;
extern uint16_t backlight_timer;
#endif

#ifdef __cplusplus
}
#endif

#endif /* KEYB_H */

