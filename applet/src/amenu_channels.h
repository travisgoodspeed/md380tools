// File:    md380tools/applet/src/amenu_codeplug.h
// Purpose:
//  Helper functions to show parts of the codeplug in the 'application menu'
//         (app_menu.c), for example:
//  - a fast-scrolling ZONE list,
//  - a fast-scrolling CONTACTS list,
//  - and maybe more in future .
// Details in the implementation - see amenu_codeplug.c .

extern int 	ZoneList_nEntries;		// number of entries in the zone list.
extern int 	ZoneList_iCurrent;		// index of the currently selected (active) zone.

extern uint8_t	channel_num;		// <- belongs to THE ORIGINAL FIRMWARE (1..16, 0 forces "reload")

extern int	am_cbk_ChannelList(app_menu_t *pMenu, menu_item_t *pItem, int event, int param );
extern void	ChannelList_Draw(app_menu_t *pMenu, menu_item_t *pItem);
extern void	ChannelList_OnEnter(app_menu_t *pMenu, menu_item_t *pItem);
extern BOOL	ChannelList_ReadNameByIndex(int index, channel_t *tChannel);
extern int	SaveChannel(channel_t* chan, channel_easy* chanE);
extern int	ParseChannel(channel_t* chan, channel_easy* chanE);
extern int	readTone(channel_t* chan, tone_t* tone, char fEnc);
