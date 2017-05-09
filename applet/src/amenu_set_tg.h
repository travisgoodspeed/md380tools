// File:    md380tools/applet/src/amenu_set_tg.h
// Purpose:
//  Helper for the 'application menu' (app_menu.c) to set
//  the current DMR talkgroup directly (without Tytera's menu),
//  and KEEP IT SET even when returning from the application menu
//  to the 'idle screen' by setting channel_num = 0 
//  (which redraw the idle screen even when on a busy FM channel,
//   but also reloads struct contact (which contains the TG) 
//   from the codeplug / SPI-Flash or whatever .
// Details in amenu_set_tg.c .

extern int     ad_hoc_talkgroup;  // "wanted" talkgroup, set by user / alternative menu
extern uint8_t ad_hoc_tg_channel; // current channel number when the above TG had been entered .
                                  // 0 : "don't want to modify the TG on ANY channel" .
extern uint8_t channel_num; // this variable belongs to THE ORIGINAL FIRMWARE (1..16, 0 forces "reload")

extern int  am_cbk_SetTalkgroup(app_menu_t *pMenu, menu_item_t *pItem, int event, int param); // in amenu_set_tg.c
extern void CheckTalkgroupAfterChannelSwitch(void); // also in amenu_set_tg.c


