/*
 *  menu.h
 * 
 */

#ifndef MENU_H
#define MENU_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <stdint.h>

extern wchar_t  	md380_menu_edit_buf[];

extern uint8_t		currently_selected_menu_entry2;
extern uint8_t		currently_selected_menu_entry;
extern uint8_t 		top_side_button_pressed_function;
extern uint8_t 		bottom_side_button_pressed_function;
extern uint8_t 		top_side_button_held_function;
extern uint8_t 		bottom_side_button_held_function;

/* mn_editbuffer_poi / md380_menu_0x20001114 */
extern wchar_t *mn_editbuffer_poi;


#ifdef __cplusplus
}
#endif

#endif /* MENU_H */

