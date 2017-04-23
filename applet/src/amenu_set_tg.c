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
#include "lcd_driver.h"   // alternative LCD driver (DL4YHF), doesn't depend on 'gfx'
#include "app_menu.h"     // 'simple' alternative menu activated by red BACK-button
#include "codeplug.h"     // struct 'contact' contains the current talkgroup number
#include "printf.h"       // Kustaa Nyholm's tinyprintf (printf.c, snprintfw)


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
         } // end if < FINISHED (not ABORTED) editing >
        return AM_RESULT_OK; // "event was processed HERE"
     default: // all other events are not handled here (let the sender handle them)
        break;
   } // end switch( event )
  return AM_RESULT_NONE; // "proceed as if there was NO callback function"
} // end am_cbk_SetTalkgroup()


#endif // CONFIG_APP_MENU ?
