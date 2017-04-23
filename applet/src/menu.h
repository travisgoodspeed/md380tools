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
#if defined(FW_D13_020) || defined(FW_S13_020)
extern uint8_t      currently_selected_menu_entry;


#endif
/* mn_editbuffer_poi / md380_menu_0x20001114 */
extern wchar_t *mn_editbuffer_poi;


/* Don't call functions without a complete prototype, e.g. from keyb.c . */
/* See also (more menu functions) : md380.h !                            */

void create_menu_entry_set_tg_screen(void);


#ifdef __cplusplus
}
#endif

#endif /* MENU_H */

