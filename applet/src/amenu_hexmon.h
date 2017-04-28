// File:    md380tools/applet/src/amenu_hexmon.h
// Author:  Wolf (DL4YHF) [initial version]
//
// Date:    2017-04-28
//  Simple 'hex monitor' to inspect blocks of memory.
//  Implemented as a callback function for the 'application menu' (app_menu.c) .

extern uint32_t HexMon_u32StartAddress; 

int am_cbk_HexMon(app_menu_t *pMenu, menu_item_t *pItem, int event, int param );

