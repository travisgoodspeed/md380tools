// File:    md380tools/applet/src/amenu_set_talkgroup.c
// Author:  Wolf (DL4YHF) [initial version] 
//
// Date:    2017-04-17
//  Callback function for the 'application menu' to set
//  the current DMR talkgroup directly (without Tytera menu).
//  Based on Brad's PR #708, which serves the same purpose
//  using the 'MD380Tools' menu (which was too deeply nested
//  in Tytera's original menu).

#include "config.h"

#if (CONFIG_APP_MENU) // this module is only available along with app_menu.c ...

#include <stm32f4xx.h>
#include <string.h>
#include "lcd_driver.h"   // alternative LCD driver (DL4YHF), doesn't depend on 'gfx'
#include "app_menu.h"     // 'simple' alternative menu activated by red BACK-button



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
  // In THIS case (amenu_set_talkgroup.c), the talkgroup shall be
  // modified WITHOUT having to enter (or modify) Tytera's menu.
{
  wchar_t *p

#if(0) // ex: create_menu_entry_set_tg_screen_store()
    uint32_t new_tx_id = 0;
    wchar_t *bf;

    bf = md380_menu_edit_buf;
    while (*bf != 0) {
        new_tx_id *= 10;
        new_tx_id += (*bf++) - '0';
    }

    if ( new_tx_id > 0xffffff ) {
        return;
    }

    contact.id_l = new_tx_id & 0xFF ;
    contact.id_m = (new_tx_id>>8) & 0xFF ;
    contact.id_h = (new_tx_id>>16) & 0xFF ;
    contact.type = CONTACT_GROUP ;

    wchar_t *p = (void*)contact.name; // write entered tg to the contact name 
                             // so that it is dislayed on the monitor1 screen
    snprintfw( p, 16, "TG %d*", new_tx_id ); // (#708)

    extern void draw_zone_channel(); // TODO.
    draw_zone_channel();

    md380_menu_id = md380_menu_id - 1; // exit menu to the proper level (#708) 
    md380_menu_depth = md380_menu_depth - 1;

    md380_create_menu_entry(md380_menu_id, md380_menu_edit_buf, MKTHUMB(md380_menu_entry_back), MKTHUMB(md380_menu_entry_back), 6, 1, 1);
#endif // (0) - ex: create_menu_entry_set_tg_screen_store()

  switch( event ) // what happened, why did the menu framework call us ?
   { case APPMENU_EVT_PAINT: // the menu item is just about to be "painted".
        // This event can be used to update 'dynamic content', custom drawing, etc.
        // None of those are used here... but because the 'current'
        // value (talkgroup) shall immediately be visible as soon as
        // the item is scrolled into view, update it now:
        break;
     case APPMENU_EVT_BEGIN_EDIT: // just beginning to edit the value..
        // For example, we may check the value (in pMenu->iEditValue)
        // and replace it with a meaningful default BEFORE it appears
        // in the edit field .
        break; 
     case APPMENU_EVT_EDIT: // will get here WHILE editing (after each edit value modification)
        // We don't want to 'write back' the value WHILE editing,
        // thus nothing to do here at the moment.
        break;
     case APPMENU_EVT_END_EDIT: // the operator finished or aborted editing,
        // so "how did it end" (ABORTED/CANCELLED or FINISHED/CONFIRMED) ?
        // > param = 1 : "finished, write back the result",
        // > param = 0 : "aborted, discard whatever has been entered".
        if( param ) // write back the result (here: edited talkgroup in pMenu->iEditValue)
         {
         }
        return AM_RESULT_OK; // "successfully processed the event HERE"
  // case APPMENU_EVT_KEY: // keyboard event (only sent to callbacks that occupied the screen)
  //    break; // not used here, let app_menu.c process keyboard events
     default: // all other events are not handled here (let the sender handle them)
        break;
   } // end switch( event )
  return AM_RESULT_NONE; // "proceed as if there was NO callback function"
} // end am_cbk_ColorTest()


#endif // CONFIG_APP_MENU ?
