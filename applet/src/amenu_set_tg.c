// File:    md380tools/applet/src/amenu_set_tg.c
// Author:  Wolf (DL4YHF) [initial version]
//
// Date:    2017-04-23
//  Callback function for the 'application menu' (app_menu.c) to set
//  the current DMR talkgroup directly (without Tytera menu).
//  Based on Brad's PR #708, which serves the same purpose using the
//  'MD380Tools' menu (deeply nested in Tytera's original menu).
//  May even be opened via programmable hotkey / sidebutton .

#include "config.h"

#if (CONFIG_APP_MENU) // this module is only available along with app_menu.c ...

#include <stm32f4xx.h>
#include <string.h>
#include "irq_handlers.h" // green_led_timer, red_led_timer (for debugging)
#include "lcd_driver.h"   // alternative LCD driver (DL4YHF), doesn't depend on 'gfx'
#include "app_menu.h"     // 'simple' alternative menu activated by red BACK-button
#include "codeplug.h"     // struct 'contact' contains the current talkgroup number
#include "printf.h"       // Kustaa Nyholm's tinyprintf (printf.c, snprintfw)
#include "amenu_set_tg.h" // header for THIS module (to check prototypes,etc)

int     ad_hoc_talkgroup = 0; // "temporarily wanted" talkgroup, entered by user in the alternative menu
uint8_t ad_hoc_tg_channel= 0; // current channel number when the above TG had been set

//---------------------------------------------------------------------------
int am_cbk_SetTalkgroup(app_menu_t *pMenu, menu_item_t *pItem, int event, int param )
  // Callback function, invoked from the "app menu" framework
  // to retrieve the currently used talkgroup, 
  // and (after editing the talkgroup number) set the new one.
  // Based on Brad Ramsey's PR #708 which modifies contact.name, 
  // contact.type, etc etc.
  // Valuable info from PR #708 about how the principle:
  //  > using the "manual dial" functionality in the baseline firmware's menu 
  //  > allows only private calls. To join a talkgroup, the destination id 
  //  > must be a group id. (..)
  //  > the entry is placed into the appropriate nibbles of the extern contact 
  //  > structure. Writing to the appropriate tg address is accomplished 
  //  > using a copy of the code from the copy_dst_to_contact() function 
  //  > in keyb.c.
  // In THIS case (amenu_set_tg.c), the talkgroup shall be
  // modified WITHOUT having to enter (or modify) Tytera's menu.
{
  switch( event ) // what happened, why did the menu framework call us ?
   { case APPMENU_EVT_GET_VALUE : // called to retrieve the current value
        // How to retrieve the talkgroup number ? Inspired by Brad's PR #708 :
        return ((int)contact.id_h<<16) | ((int)contact.id_m<<8) | (int)contact.id_l;
     case APPMENU_EVT_END_EDIT: // the operator finished or aborted editing,
        if( param ) // "finished", not "aborted" -> write back the new ("edited") value
         { contact.id_l =  pMenu->iEditValue & 0xFF ;
           contact.id_m = (pMenu->iEditValue>>8) & 0xFF ;
           contact.id_h = (pMenu->iEditValue>>16) & 0xFF ;
           contact.type = CONTACT_GROUP; // now the "contact" is a "talkgroup", not a "user"(-ID) !
           // That's not all yet (after modifying the TG, from PR #708) :
           // > write entered tg to the contact name 
           // > so that it is dislayed on the monitor1 screen
           snprintfw( contact.name, 16, "TG %d*", pMenu->iEditValue ); // (#708)
           // Because the original firmware will overwrite 'contact' when leaving
           // the alternative menu (by setting channel_num = 0 to redraw the idle screen
           // even when tuned to a BUSY FM CHANNEL), also store the "wanted" TG here:
           ad_hoc_talkgroup = pMenu->iEditValue;
           // The above TG shall only be used as long as we're on the same channel.
           // When QSYing via rotary knob, the TG for the new channel shall be taken
           // from the codeplug again. So remember the channel FOR WHICH THE TG WAS SET:
           ad_hoc_tg_channel = channel_num;
           CheckTalkgroupAfterChannelSwitch(); // ad_hoc_talkgroup -> contact.xyz
         } // end if < FINISHED (not ABORTED) editing >
        return AM_RESULT_OK; // "event was processed HERE"
     default: // all other events are not handled here (let the sender handle them)
        break;
   } // end switch( event )
  return AM_RESULT_NONE; // "proceed as if there was NO callback function"
} // end am_cbk_SetTalkgroup()

//---------------------------------------------------------------------------
void CheckTalkgroupAfterChannelSwitch(void) // [in] ad_hoc_tg_channel,ad_hoc_talkgroup; [out] contact.xyz
  // Called from somewhere (display task?) after a channel-switch,
  // including the transition of channel_num = 0 -> channel_num from channel knob.
{
  // red_led_timer = 5; // detected a transition in channel_num ? very short pulse with the red LED !
  if( channel_num==0 )  // still on the "dummy channel" to force redrawing the 'idle' screen ?
   { // Don't modify anything here. Tytera is just going to overwrite the "wanted" talkgroup !
   }
  else if( channel_num == ad_hoc_tg_channel )
   { // When on THIS channel, should we be on the 'ad-hoc entered' talkgroup ? 
     if( ad_hoc_talkgroup <= 0 ) // ... no 'wanted' talkgroup so don't modify 'contact' 
      {
      }
     else // switch back to the "wanted" talkgroup, set by user / alternative menu:
      {
        contact.id_l =  ad_hoc_talkgroup & 0xFF ;
        contact.id_m = (ad_hoc_talkgroup>>8) & 0xFF ;
        contact.id_h = (ad_hoc_talkgroup>>16) & 0xFF ;
        contact.type = CONTACT_GROUP; // now the "contact" is a "talkgroup", not a "user"(-ID) !
        snprintfw( contact.name, 16, "TG %d*", ad_hoc_talkgroup ); // (trick from PR #708)
      }
   }
  else // channel_num != 0,  but *NOT* on the channel for which the "ad-hoc talkgroup" was entered,
   { // e.g. operator switched to different channel. 
     // Keep the ad-hoc talkgroup or "discard" it now ? Consider this:
     //  - Meet on Timeslot 1 (e.g. nationwide), then "QSY" to Timeslot 2, 
     //    and enter an ad-hoc TALKGROUP number for that channel (with TS2).
     //    Something doesn't work as planned so rapidly want the "original" TG (from codeplug) back.
     //    Intuitively switch to A DIFFERENT channel and back, to invoke the "original" TG.
     ad_hoc_tg_channel = 0; // FORGET the *channel* with the ad-hoc TG, but not the ad-hoc TG itself,
     // so we can quickly recall it via app-menu in "up-down"-edit mode.
   }
} // end CheckTalkgroupAfterChannelSwitch()


#endif // CONFIG_APP_MENU ?
