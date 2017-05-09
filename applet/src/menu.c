/*! \file menu.c
  \brief Menu hooks and extensions.
*/

//#define DEBUG
#define TRACE_MENU

#define CONFIG_MENU

#include "menu.h"

#include <string.h>

#include "debug.h"
#include "dmesg.h"
#include "md380.h"
#include "version.h"
#include "config.h"
#include "os.h"
#include "spiflash.h"
#include "addl_config.h"
#include "radio_config.h"
#include "usersdb.h"
#include "util.h"
#include "printf.h"
#include "keyb.h"
#include "codeplug.h"
#include "narrator.h" // optional: reads out channel, zone, menu in Morse code.

const static wchar_t wt_addl_func[]         = L"MD380Tools";
const static wchar_t wt_datef[]             = L"Date format";
const static wchar_t wt_debug[]             = L"USB logging";
//const static wchar_t wt_netmon[]            = L"NetMon";
const static wchar_t wt_netmon[]            = L"DevOnly!!"; // for now, later a true submenu.
const static wchar_t wt_disable[]           = L"Disable";
const static wchar_t wt_enable[]            = L"Enable";
const static wchar_t wt_rbeep[]             = L"M. RogerBeep";

const static wchar_t wt_bootopts[]          = L"Boot Options";
const static wchar_t wt_demoscr[]           = L"Demo Screen";
const static wchar_t wt_demoscr_enable[]    = L"Enable";
const static wchar_t wt_demoscr_disable[]   = L"Disable";
const static wchar_t wt_splash[]            = L"Splash Mode";

const static wchar_t wt_showcall[]          = L"Show Calls";      // was UsersCSV / enable / disable now added Talker Alias
const static wchar_t wt_fromcps[]            = L"CPS only";
const static wchar_t wt_usercsv[]           = L"User DB";
const static wchar_t wt_talkalias[]         = L"Talk Alias";
const static wchar_t wt_ta_user[]           = L"TA & UserDB";

const static wchar_t wt_datef_original[]    = L"YYYY/MM/DD";
const static wchar_t wt_datef_germany[]     = L"DD.MM.YYYY";
const static wchar_t wt_datef_italy[]       = L"DD/MM/YYYY";
const static wchar_t wt_datef_american[]    = L"MM/DD/YYYY";
const static wchar_t wt_datef_iso[]         = L"YYYY-MM-DD";
const static wchar_t wt_datef_alt[]         = L"Lastheard ";
const static wchar_t wt_datef_talias[]      = L"Talker Alias";    // added Talker Alias 

const static wchar_t wt_promtg[]            = L"Promiscuous";
const static wchar_t wt_edit[]              = L"Edit";
const static wchar_t wt_edit_dmr_id[]       = L"Edit DMR-ID";
const static wchar_t wt_no_w25q128[]        = L"No W25Q128";
const static wchar_t wt_set_tg_id[]         = L"Set Talkgroup"; // brad's PR #708 
const static wchar_t wt_experimental[]      = L"Experimental";
const static wchar_t wt_micbargraph[]       = L"Mic bargraph";

const static wchar_t wt_backlight[]         = L"Backlight Tmr";
const static wchar_t wt_blunchanged[]       = L"Unchanged";
const static wchar_t wt_bl5[]               = L"5 sec";
const static wchar_t wt_bl30[]              = L"30 sec";
const static wchar_t wt_bl60[]              = L"60 sec";


const static wchar_t wt_backlight_menu[]   = L"Backlight";

#ifndef  CONFIG_DIMMED_LIGHT   // Dimmed backlight ?
# define CONFIG_DIMMED_LIGHT 0 // only if defined > 0 in config.h
#endif
#ifndef  CONFIG_MORSE_OUTPUT   // Morse output for visually impaired hams ?
# define CONFIG_MORSE_OUTPUT 0 // only if defined > 0 in config.h
#endif

#if( CONFIG_DIMMED_LIGHT )
const static wchar_t wt_bl_intensity_lo[]   = L"Level Low";
const static wchar_t wt_bl_intensity_hi[]   = L"Level High";
#define NUM_BACKLIGHT_INTENSITIES 10 /* number of intensity steps (0..9) for the menu */
const static wchar_t *wt_bl_intensity[NUM_BACKLIGHT_INTENSITIES] = 
 { L"0 (off)", L"1 (lowest)", L"2", L"3 (medium)", L"4", 
   L"5",       L"6",          L"7", L"8",          L"9 (bright)" }; 
#endif // CONFIG_DIMMED_LIGHT ?

const static wchar_t wt_cp_override[]       = L"CoPl Override";
const static wchar_t wt_splash_manual[]     = L"Disabled";
const static wchar_t wt_splash_callid[]     = L"Callsign+DMRID";
const static wchar_t wt_splash_callname[]   = L"Callsign+Name";

const static wchar_t wt_cp_override_dmrid[] = L"ID Override";

const static wchar_t wt_config_reset[] = L"Config Reset";
const static wchar_t wt_config_reset_doit[] = L"Config Reset2";

const static wchar_t wt_sidebutton_menu[]   = L"Side Buttons";
const static wchar_t wt_button_top_press[]  = L"Top Pressed";
const static wchar_t wt_button_bot_press[]  = L"Bottom Pressed";
const static wchar_t wt_button_top_held[]   = L"Top Held";
const static wchar_t wt_button_bot_held[]   = L"Bottom Held";
const static wchar_t wt_button_unassigned[] = L"Unassigned";
const static wchar_t wt_button_alert_tone[] = L"All Tone Tog";
const static wchar_t wt_button_emerg_on[]   = L"Emergency On";
const static wchar_t wt_button_emerg_off[]  = L"Emergency Off";
const static wchar_t wt_button_power[]      = L"High/Low Pwr";
const static wchar_t wt_button_monitor[]    = L"Monitor";
const static wchar_t wt_button_nuisance[]   = L"Nuisance Del";
const static wchar_t wt_button_ot1[]        = L"One Touch 1";
const static wchar_t wt_button_ot2[]        = L"One Touch 2";
const static wchar_t wt_button_ot3[]        = L"One Touch 3";
const static wchar_t wt_button_ot4[]        = L"One Touch 4";
const static wchar_t wt_button_ot5[]        = L"One Touch 5";
const static wchar_t wt_button_ot6[]        = L"One Touch 6";
const static wchar_t wt_button_rep_talk[]   = L"Talkaround";
const static wchar_t wt_button_scan[]       = L"Scan On/Off";
const static wchar_t wt_button_squelch[]    = L"Squelch Tight";
const static wchar_t wt_button_privacy[]    = L"Privacy On/Off";
const static wchar_t wt_button_vox[]        = L"Vox On/Off";
const static wchar_t wt_button_zone[]       = L"Zone Inc.";
const static wchar_t wt_button_zone_tog[]   = L"Zone Toggle";
const static wchar_t wt_button_bat_ind[]    = L"Bat Indicator";
const static wchar_t wt_button_man_dial[]   = L"Manual Dial";
const static wchar_t wt_button_lone_work[]  = L"Lone wk On/Off";
const static wchar_t wt_button_1750_hz[]    = L"1750hz Tone";
const static wchar_t wt_button_bklt_en[]    = L"Toggle bklight";
const static wchar_t wt_button_set_tg[]     = L"Set Talkgroup";
#if( CONFIG_MORSE_OUTPUT )
const static wchar_t wt_button_narrator[]   = L"Morse Narrator";
const static wchar_t wt_button_cw_repeat[]  = L"Morse Repeat";
#endif
const static uint8_t button_functions[]     = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
                                               0x0b, 0x0c, 0x0d, 0x0e, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1e,
                                               0x1f, 0x26, 0x50, 0x51
#                                            if( CONFIG_MORSE_OUTPUT )
                                              ,0x52 // starts the 'Morse narrator' via programmable button
                                              ,0x53 // repeats the last 'Morse announcement'  "  "  "
#                                            endif
                                              };

uint8_t button_selected = 0;
uint8_t button_function = 0;

typedef struct {
    const wchar_t* label ;  // [0]
    void* green ;           // [4]
    void* red ;             // [8]
    uint8_t off12 ;         // [12]  
    uint8_t off13 ;         // [13]
    uint16_t item_count ;   // [14]
    uint8_t off16 ;         // [16]
    uint8_t off17 ;         // [17]
    uint16_t unknown2 ;     // [18]
    // sizeof() == 20 (0x14)
} menu_entry_t ;


typedef struct {
  const wchar_t  *menu_title; // [0]
  menu_entry_t *entries; // [4]
  uint8_t numberof_menu_entries; // [8]
  uint8_t unknown_00;
  uint8_t unknown_01;
  uint8_t filler ;
} menu_t ; // sizeof() == 12

extern menu_t md380_menu_memory[];

extern menu_entry_t md380_menu_mem_base[];

//Old macro, schedule for deletion.
//#define MKTHUMB(adr) ((void(*))(((uint32_t)adr) | 0x1))
//New macro, prints warnings where needed.
#define MKTHUMB(adr) (printf(((int)adr)&1?"":"Warning, 0x%08x function pointer is even.\n",adr),(void(*))(((uint32_t)adr) | 0x1))


void create_menu_entry_rev(int menuid, const wchar_t * label , void (*green_key)(), void  (*red_key)(), int e, int f ,int item_count) 
{
//    PRINT("create_menu_entry_rev %x %S %x %x %x\n", menuid, label, e, f, item_count );
//    PRINT("%x\n", green_key);
    
//    green_key = MKTHUMB(green_key);
//    PRINT("%x\n", green_key);
    
//    red_key = MKTHUMB(red_key);
    
#ifdef TRACE_MENU
#ifdef DEBUG    
    char lbl2[10];
    char *lp = (void*)label ;
    for(int i=0;i<10;i++) {
        char c = lp[(i*2)];
        if( c == ' ' ) {
            c = '_' ;
        }
        lbl2[i] = c ;
        if( c == 0 ) {
            break ;
        }
    }
    lbl2[9] = 0 ;

    void *gp = ((uint8_t*)green_key) - 1 ;
    PRINT("f menugreen.%s.%x 0 0x%x\n", lbl2, gp, gp );
#endif
    
#if 0    
    register uint32_t *sp asm("sp");   
    for(int i=15;i<20;i++) {
        printf( "%d : 0x%x\n", i, sp[i] );        
    }
    printf( "f menucall.%s 0 0x%x\n", lbl2, (sp[15] - 1 - 4) );
#endif    
    
#endif    
    
    // e f
    // 6,2 confirmation popup misc.
    // 6,f confirmation popup scanlist.
    // 6,f confirmation popup zone.
    // 6,1 invalid number popup.
    // a,0 ctcss (only editable in FM)
    // 9,0 fullscr msg without timeout (my num,versions)
    // 81,0 enter radio number (new contact,manual dial,edit dmrid,rxf,txf)
    // 85,0 msgbox without timeout (rxf,txf)
    // 8a,0 utilities menu items
    // 8b,0 simple yes no list items.
    // 8c,0 single menu entry for complete contacts list.
    // 93,0 message
    // 98,0 radio settings
    
    // item_count 
    // 0 = not visible
    
    // f
    // 0 = stable
    // 2 = remove after timeout
    
//    if( global_addl_config.experimental == 1 ) {
//        switch( item_count ) {
//            case 0 :
//                item_count = 1 ; // cheating.
//                break ;
//        }
//    }
    
    menu_entry_t *poi = &md380_menu_mem_base[menuid];    
    
    poi->label = label ;
    poi->green = green_key ;
    poi->red = red_key ;
    poi->off12 = e ;
    poi->off13 = f ;
    poi->item_count = item_count ;
    
#ifdef FW_D13_020
    // supress language menu.
    if( green_key == (void*)(0x801ab84 + 1) ) {
        poi->item_count = 0 ;
    }
#else
#warning TODO find language menu on this firmware version    
#endif

}

uint8_t index_of(uint8_t value, uint8_t arr[], uint8_t len)
{
    uint8_t i = 0;
    while(i < len)
    {
      if (arr[i] == value) return i;
      i++;
    }

    return 0;
}

//void md380_create_menu_entry(int menuid, const wchar_t * label , void * green_key, void  * red_key, int e, int f ,int enabled) {
//#ifdef DEBUG
//  printf("0x%x Text: 0x%x GreenKey 0x%x RedKey 0x%x 0x%x 0x%x 0x%x\n", menuid,label,green_key,red_key,e,f,enabled);
//  printf("b: ");
//  printhex2((char *) label,14);
//  printf("\n");
//  printf(" md380_menu_depth: %d\n", md380_menu_depth);
//#endif
//  md380_create_menu_entry(menuid,label,green_key,red_key,e,f,enabled);
//}


menu_t *get_menu_stackpoi()
{
    return &md380_menu_memory[md380_menu_depth+1];
}

void mn_create_single_timed_ack( const wchar_t *title, const wchar_t *label )
{
    menu_t *menu_mem;

    menu_mem = get_menu_stackpoi();
    menu_mem->menu_title = title;

    menu_mem->entries = &md380_menu_mem_base[md380_menu_id];

    menu_mem->numberof_menu_entries = 1;
    menu_mem->unknown_00 = 0;
    menu_mem->unknown_01 = 0;
    
    md380_create_menu_entry(md380_menu_id, label, MKTHUMB(md380_menu_entry_back), MKTHUMB(md380_menu_entry_back), 6, 2, 1);
}

void mn_submenu_init(const wchar_t *title)
{
    menu_t *menu_mem = get_menu_stackpoi();
    menu_mem->menu_title = title;

    menu_mem->entries = &md380_menu_mem_base[md380_menu_id];
    menu_mem->numberof_menu_entries = 0;
    menu_mem->unknown_00 = 0;
    menu_mem->unknown_01 = 0;    
}

void mn_submenu_add(const wchar_t * label, void (*func)())
{
    menu_t *menu_mem = get_menu_stackpoi();
    
    func = MKTHUMB(func);
    
    md380_create_menu_entry(md380_menu_id + menu_mem->numberof_menu_entries, label, func, MKTHUMB(md380_menu_entry_back), 0x8b, 0, 1);

    menu_mem->numberof_menu_entries++ ;
}

void mn_submenu_add_98(const wchar_t * label, void (*func)())
{
    menu_t *menu_mem = get_menu_stackpoi();
    
    func = MKTHUMB(func);
    
    md380_create_menu_entry(md380_menu_id + menu_mem->numberof_menu_entries, label, func, MKTHUMB(md380_menu_entry_back), 0x98, 0, 1);

    menu_mem->numberof_menu_entries++ ;
}

void mn_submenu_add_8a(const wchar_t * label, void (*func)(), int enabled)
{
    menu_t *menu_mem = get_menu_stackpoi();
    
    func = MKTHUMB(func);
    
    md380_create_menu_entry(md380_menu_id + menu_mem->numberof_menu_entries, label, func, MKTHUMB(md380_menu_entry_back), 0x8a, 0, enabled);

    menu_mem->numberof_menu_entries++ ;
}

void mn_submenu_finalize()
{
    menu_t *menu_mem = get_menu_stackpoi();
    
    for (int i = 0; i < menu_mem->numberof_menu_entries; i++) { 
        // conflicts with 'selected' icon.
        // no icons.
        md380_menu_mem_base[md380_menu_id + i].off16 = 0;
    }    
}

void mn_submenu_finalize2()
{
    menu_t *menu_mem = get_menu_stackpoi();
    
    for (int i = 0; i < menu_mem->numberof_menu_entries; i++) { 
        md380_menu_mem_base[md380_menu_id + i].off16 = 2; // numbered icons
    }    
}

void create_menu_entry_promtg_enable_screen(void)
{
    mn_create_single_timed_ack(wt_promtg,wt_enable);
    
    global_addl_config.promtg = 1;
    
    cfg_save();
}

void create_menu_entry_promtg_disable_screen(void)
{
    mn_create_single_timed_ack(wt_promtg,wt_disable);
    
    global_addl_config.promtg = 0;

    cfg_save();
}

void create_menu_entry_micbargraph_enable_screen(void)
{
    mn_create_single_timed_ack(wt_micbargraph,wt_enable);

    global_addl_config.micbargraph = 1;

    cfg_save();
}

void create_menu_entry_micbargraph_disable_screen(void)
{
    mn_create_single_timed_ack(wt_micbargraph,wt_disable);

    global_addl_config.micbargraph = 0;

    cfg_save();
}

void create_menu_entry_rbeep_enable_screen(void)
{
    mn_create_single_timed_ack(wt_rbeep,wt_enable);

    global_addl_config.rbeep = 1;

    cfg_save();
}

void create_menu_entry_rbeep_disable_screen(void)
{
    mn_create_single_timed_ack(wt_rbeep,wt_disable);

    global_addl_config.rbeep = 0;

    cfg_save();
}

void create_menu_entry_demo_enable_screen(void)
{
    mn_create_single_timed_ack(wt_demoscr, wt_enable);

    global_addl_config.boot_demo = 0;

    cfg_save();
}

void create_menu_entry_demo_disable_screen(void)
{
    mn_create_single_timed_ack(wt_demoscr, wt_disable);

    global_addl_config.boot_demo = 1;

    cfg_save();
}

void create_menu_entry_demo_screen(void)
{
    mn_submenu_init(wt_demoscr);

    md380_menu_entry_selected = global_addl_config.boot_demo;

    mn_submenu_add(wt_demoscr_enable, create_menu_entry_demo_enable_screen);
    mn_submenu_add(wt_demoscr_disable, create_menu_entry_demo_disable_screen);

    mn_submenu_finalize();
}

void mn_cp_override_off(void)
{
    mn_create_single_timed_ack(wt_cp_override, wt_splash_manual);

    //global_addl_config.boot_splash = 0;

    global_addl_config.cp_override &= ~CPO_BL1 ;
    global_addl_config.cp_override &= ~CPO_BL2 ;
    
    cfg_save();
}

uint32_t get_effective_dmrid()
{
    return global_addl_config.dmrid ;
}

void mn_cp_override_call_dmrid(void)
{
    mn_create_single_timed_ack(wt_cp_override, wt_splash_callid);

    //global_addl_config.boot_splash = 1;
    
    global_addl_config.cp_override |= CPO_BL1 ;
    global_addl_config.cp_override |= CPO_BL2 ;
    
    user_t usr ;
    uint32_t dmrid = get_effective_dmrid();
    int r = usr_find_by_dmrid(&usr,dmrid);
    
    if( r ) {
        snprintf(global_addl_config.bootline1, 10, "%s", usr.callsign);
    } else {
        snprintf(global_addl_config.bootline1, 10, "%s", "unknown");
    }

    snprintf( global_addl_config.bootline2, 10, "%d", (int)dmrid );

    cfg_save();
}

void mn_cp_override_call_name(void)
{
    mn_create_single_timed_ack(wt_cp_override, wt_splash_callname);

    //global_addl_config.boot_splash = 2;

    global_addl_config.cp_override |= CPO_BL1 ;
    global_addl_config.cp_override |= CPO_BL2 ;

    user_t usr ;
    uint32_t dmrid = get_effective_dmrid();
    int r = usr_find_by_dmrid(&usr,dmrid);
    
    if( r ) {
        snprintf(global_addl_config.bootline1, 10, "%s", usr.callsign);
    } else {
        snprintf(global_addl_config.bootline1, 10, "%s", "unknown");
    }

    if( r ) {
        snprintf(global_addl_config.bootline2, 10, "%s", usr.firstname);
    } else {
        snprintf(global_addl_config.bootline2, 10, "%s", "unknown");
    }
    
    cfg_save();
}

void mn_cp_override_dmrid_on(void)
{
    mn_create_single_timed_ack(wt_cp_override_dmrid, wt_enable);

    global_addl_config.cp_override |= CPO_DMR ;

    // set to override dmrid.
    md380_radio_config.dmrid = global_addl_config.dmrid ;

    cfg_save();
}

void mn_cp_override_dmrid_off(void)
{
    mn_create_single_timed_ack(wt_cp_override_dmrid, wt_disable);

    global_addl_config.cp_override &= ~CPO_DMR ;

    // restore codeplug dmrid.
    md380_spiflash_read(&md380_radio_config.dmrid, FLASH_OFFSET_DMRID, 4);
    
    cfg_save();
}

void mn_cp_override_dmrid(void)
{
    mn_submenu_init(wt_cp_override_dmrid);
    mn_submenu_add(wt_demoscr_enable, mn_cp_override_dmrid_on);
    mn_submenu_add(wt_demoscr_disable, mn_cp_override_dmrid_off);
    mn_submenu_finalize();
}

void mn_cp_override(void)
{
    mn_submenu_init(wt_cp_override);
    
    mn_submenu_add(wt_splash_manual, mn_cp_override_off);
    mn_submenu_add(wt_splash_callid, mn_cp_override_call_dmrid);
    mn_submenu_add(wt_splash_callname, mn_cp_override_call_name);

    mn_submenu_add(wt_cp_override_dmrid, mn_cp_override_dmrid);
    
    mn_submenu_finalize();
}

void create_menu_entry_bootopts_screen(void)
{
    mn_submenu_init(wt_bootopts);

    mn_submenu_add_98(wt_demoscr, create_menu_entry_demo_screen);
//    mn_submenu_add_98(wt_splash, create_menu_entry_splash_screen);

    mn_submenu_finalize2();
}

void create_menu_entry_datef_original_screen(void)
{
    mn_create_single_timed_ack(wt_datef,wt_datef_original);

    global_addl_config.datef = 0;

    cfg_save();
}

void create_menu_entry_datef_germany_screen(void)
{
    mn_create_single_timed_ack(wt_datef,wt_datef_germany);

    global_addl_config.datef = 1;

    cfg_save();
}

void create_menu_entry_datef_italy_screen(void)
{
    mn_create_single_timed_ack(wt_datef,wt_datef_italy);

    global_addl_config.datef = 2;

    cfg_save();
}

void create_menu_entry_datef_american_screen(void)
{
    mn_create_single_timed_ack(wt_datef,wt_datef_american);

    global_addl_config.datef = 3;

    cfg_save();
}

void create_menu_entry_datef_iso_screen(void)
{
    mn_create_single_timed_ack(wt_datef,wt_datef_iso);
    
    global_addl_config.datef = 4;

    cfg_save();
}

void create_menu_entry_datef_alt_screen(void)
{
    mn_create_single_timed_ack(wt_datef,wt_datef_alt);
    
    global_addl_config.datef = 5;

    cfg_save();
}

void create_menu_entry_datef_talias_screen(void)
{
    mn_create_single_timed_ack(wt_datef,wt_datef_talias);
    
    global_addl_config.datef = 6;

    cfg_save();
}

//==========================================================================================================//
// submenu: showcall - select options 0-3 callsign display method
//==========================================================================================================//

void create_menu_entry_showcall_disable_screen(void)
{
    mn_create_single_timed_ack(wt_showcall,wt_fromcps);
    global_addl_config.userscsv = 0;
    cfg_save();
}

void create_menu_entry_showcall_userscsv_screen(void)
{
    mn_create_single_timed_ack(wt_showcall,wt_usercsv);
    global_addl_config.userscsv = 1;
    cfg_save();
}

void create_menu_entry_showcall_talkalias_screen(void)
{
    mn_create_single_timed_ack(wt_showcall,wt_talkalias);
    global_addl_config.userscsv = 2;
    cfg_save();
}

void create_menu_entry_showcall_ta_user_screen(void)
{
    mn_create_single_timed_ack(wt_showcall,wt_ta_user);
    global_addl_config.userscsv = 3;
    cfg_save();
}

//==========================================================================================================//

void create_menu_entry_experimental_enable_screen(void)
{
    mn_create_single_timed_ack(wt_experimental,wt_enable);
    
    global_addl_config.experimental = 1;
}

void create_menu_entry_experimental_disable_screen(void)
{
    mn_create_single_timed_ack(wt_experimental,wt_disable);
    
    global_addl_config.experimental = 0;
}

void create_menu_entry_debug_enable_screen(void)
{
    mn_create_single_timed_ack(wt_debug,wt_enable);
    
    global_addl_config.debug = 1;

    cfg_save();
}

void create_menu_entry_debug_disable_screen(void)
{
    mn_create_single_timed_ack(wt_debug,wt_disable);
    
    global_addl_config.debug = 0;

    cfg_save();
}

void create_menu_entry_promtg_screen(void)
{
    mn_submenu_init(wt_promtg);

    if( global_addl_config.promtg == 1 ) {
        md380_menu_entry_selected = 0;
    } else {
        md380_menu_entry_selected = 1;
    }

    mn_submenu_add(wt_enable, create_menu_entry_promtg_enable_screen);
    mn_submenu_add(wt_disable, create_menu_entry_promtg_disable_screen);

    mn_submenu_finalize();
}

void create_menu_entry_micbargraph_screen(void)
{
    mn_submenu_init(wt_micbargraph);

    if( global_addl_config.micbargraph == 1 ) {
        md380_menu_entry_selected = 0;
    } else {
        md380_menu_entry_selected = 1;
    }

    mn_submenu_add(wt_enable, create_menu_entry_micbargraph_enable_screen);
    mn_submenu_add(wt_disable, create_menu_entry_micbargraph_disable_screen);
    
    mn_submenu_finalize();
}

void create_menu_entry_rbeep_screen(void)
{
    mn_submenu_init(wt_rbeep);

    if( global_addl_config.rbeep == 1 ) {
        md380_menu_entry_selected = 0;
    } else {
        md380_menu_entry_selected = 1;
    }

    mn_submenu_add(wt_enable, create_menu_entry_rbeep_enable_screen);
    mn_submenu_add(wt_disable, create_menu_entry_rbeep_disable_screen);

    mn_submenu_finalize();
}

void create_menu_entry_datef_screen(void)
{
    mn_submenu_init(wt_datef);

    md380_menu_entry_selected = global_addl_config.datef;

    mn_submenu_add(wt_datef_original, create_menu_entry_datef_original_screen);
    mn_submenu_add(wt_datef_germany, create_menu_entry_datef_germany_screen);
    mn_submenu_add(wt_datef_italy, create_menu_entry_datef_italy_screen);
    mn_submenu_add(wt_datef_american, create_menu_entry_datef_american_screen);
    mn_submenu_add(wt_datef_iso, create_menu_entry_datef_iso_screen);
    mn_submenu_add(wt_datef_alt, create_menu_entry_datef_alt_screen);
    mn_submenu_add(wt_datef_talias, create_menu_entry_datef_talias_screen);
   
    mn_submenu_finalize();
}


//==========================================================================================================//
// main(?) menu: showcall - select callsign display method
//==========================================================================================================//

void create_menu_entry_showcall_screen(void)
{
    mn_submenu_init(wt_showcall);

    md380_menu_entry_selected = global_addl_config.userscsv;

    mn_submenu_add(wt_fromcps, create_menu_entry_showcall_disable_screen);
    mn_submenu_add(wt_usercsv, create_menu_entry_showcall_userscsv_screen);
    mn_submenu_add(wt_talkalias, create_menu_entry_showcall_talkalias_screen);
    mn_submenu_add(wt_ta_user, create_menu_entry_showcall_ta_user_screen);
    
    mn_submenu_finalize();
}

void create_menu_entry_debug_screen(void)
{
    mn_submenu_init(wt_debug);

    if( global_addl_config.debug == 1 ) {
        md380_menu_entry_selected = 0;
    } else {
        md380_menu_entry_selected = 1;
    }

    mn_submenu_add(wt_enable, create_menu_entry_debug_enable_screen);
    mn_submenu_add(wt_disable, create_menu_entry_debug_disable_screen);

    mn_submenu_finalize();
}

void create_menu_entry_netmon_disable_screen(void)
{
    mn_create_single_timed_ack(wt_netmon,wt_disable);
    
    global_addl_config.netmon = 0;

    cfg_save();
}

void create_menu_entry_netmon_enable_screen(void)
{
    mn_create_single_timed_ack(wt_netmon,wt_enable);
    
    global_addl_config.netmon = 1;

    cfg_save();
}

#if defined(FW_D13_020) || defined(FW_S13_020)
int enabled = 1;
#else
int enabled = 0;
#endif

void create_menu_entry_netmon_screen(void)
{
    if( !enabled ) {
        return;
    }

    mn_submenu_init(wt_netmon);

    md380_menu_entry_selected = global_addl_config.netmon;

    mn_submenu_add(wt_disable, create_menu_entry_netmon_disable_screen);
    mn_submenu_add(wt_enable, create_menu_entry_netmon_enable_screen);

    mn_submenu_finalize();
}

void create_menu_entry_experimental_screen(void)
{
    mn_submenu_init(wt_experimental);

    if( global_addl_config.experimental == 1 ) {
        md380_menu_entry_selected = 0;
    } else {
        md380_menu_entry_selected = 1;
    }

    mn_submenu_add(wt_enable, create_menu_entry_experimental_enable_screen);
    mn_submenu_add(wt_disable, create_menu_entry_experimental_disable_screen);

    mn_submenu_finalize();
}


//-------------------------------------------------------------
// Backlight configuration: Timer adjustable 5, 30, 60 seconds,
//    besides those in the original firmware.
//    Please don't remove the 5 second option,
//    it's the preferred one in combination with DIMMING .
//-------------------------------------------------------------

void mn_backlight_set(int sec5, const wchar_t *label)
{
    mn_create_single_timed_ack(wt_backlight,label);
    
    md380_radio_config.backlight_time = sec5 ; // in 5 sec incr.

    rc_write_radio_config_to_flash();    
}

void mn_backlight_unchanged()
{
}


void mn_backlight_5sec()
{
    mn_backlight_set(1,wt_bl5);     
}

void mn_backlight_30sec()
{
    mn_backlight_set(6,wt_bl30);     
}

void mn_backlight_60sec()
{
    mn_backlight_set(12,wt_bl60);     
}

void mn_backlight(void)  // menu for the backlight-TIME (longer than Tytera's, but sets the same parameter)
{
    mn_submenu_init(wt_backlight);

    switch( md380_radio_config.backlight_time ) // inspired by stargo0's fix #674 
     { // (fixes the selection of the current backlight-time in the menu)
       case 1 /* times 5sec */ : md380_menu_entry_selected = 1; break;
       case 6 /* times 5sec */ : md380_menu_entry_selected = 2; break;
       case 12/* times 5sec */ : md380_menu_entry_selected = 3; break;
       default/* unchanged  */ : md380_menu_entry_selected = 0; break;
     }
    mn_submenu_add(wt_blunchanged, mn_backlight_unchanged);
    mn_submenu_add(wt_bl5,  mn_backlight_5sec );

    mn_submenu_add(wt_bl30, mn_backlight_30sec);
    mn_submenu_add(wt_bl60, mn_backlight_60sec);

    mn_submenu_finalize();
}


#if( CONFIG_DIMMED_LIGHT ) // Setup for pulse-width modulated backlight ? (DL4YHF 2017-01-08)
typedef void(*tMenuFunctionPtr)(void);
static uint8_t bIntensityMenuIndex; // 0 = modifying "backlight intensity low" (used during idle time),
                                    // 1 = modifying "backlight intensity high" (used when 'radio active').

void mn_backlight_intens( int intensity ) // common 'menu handler' for all <NUM_BACKLIGHT_INTENSITIES> intensity steps
{ // Caller: create_menu_entry_addl_functions_screen() -> mn_backlight_hi() + mn_backlight_lo()
  //          -> mn_submenu_add() -> ?.. 
  //              -> mn_backlight_intens_0/1/../9() -> mn_backlight_intens( intensity=0..9 )
  // 
  switch( bIntensityMenuIndex ) // what's being edited, "low" (idle) or "high" (active) backlight intensity ?
   { case 0 : // selected a new LOW backlight intensity ...
     default:
        mn_create_single_timed_ack( wt_bl_intensity_lo, wt_bl_intensity[intensity]/*label*/ );
        global_addl_config.backlight_intensities &= 0xF0;  // strip old nibble (lower 4 bits for "lower" intensity)
        global_addl_config.backlight_intensities |= (uint8_t)intensity; 
        break;
     case 1 : // selected a new HIGH backlight intensity :
        mn_create_single_timed_ack( wt_bl_intensity_hi, wt_bl_intensity[intensity]/*label*/ );
        global_addl_config.backlight_intensities &= 0x0F;  // strip old nibble (upper 4 bits for "high" intensity)
        global_addl_config.backlight_intensities |= ((uint8_t)intensity << 4);
        break;
   }
  // The upper 4 bits must never be ZERO, to avoid 'complete darkness' of the display when ACTIVE .
  // Because in some radios, the PWM caused audible hum, use max brightness per default:
  // 2017-04-17 : Put this important note back in. Please don't remove this !
  if( !(global_addl_config.backlight_intensities & 0xF0) )
   {    global_addl_config.backlight_intensities |= 0xF0;
   }
  cfg_save();
  
} // end mn_backlight_intens()

// 'menu callback' for backlight intensity steps 0 .. 9 
//  (kludge required because the callback doesn't pass the item-index)
void mn_backlight_intens_0(void) {  mn_backlight_intens(0); }
void mn_backlight_intens_1(void) {  mn_backlight_intens(1); }
void mn_backlight_intens_2(void) {  mn_backlight_intens(2); }
void mn_backlight_intens_3(void) {  mn_backlight_intens(3); }
void mn_backlight_intens_4(void) {  mn_backlight_intens(4); }
void mn_backlight_intens_5(void) {  mn_backlight_intens(5); }
void mn_backlight_intens_6(void) {  mn_backlight_intens(6); }
void mn_backlight_intens_7(void) {  mn_backlight_intens(7); }
void mn_backlight_intens_8(void) {  mn_backlight_intens(8); }
void mn_backlight_intens_9(void) {  mn_backlight_intens(9); }

tMenuFunctionPtr mn_backlight_intensity_funcs[ NUM_BACKLIGHT_INTENSITIES ] =
 { mn_backlight_intens_0, mn_backlight_intens_1, mn_backlight_intens_2, mn_backlight_intens_3, mn_backlight_intens_4,
   mn_backlight_intens_5, mn_backlight_intens_6, mn_backlight_intens_7, mn_backlight_intens_8, mn_backlight_intens_9 
 };
 
void mn_backlight_lo(void)  // configure LOW backlight intensity (used when "idle")
{
  int i;
  bIntensityMenuIndex = 0;  // "now selecting the LOW backlight intensity" ...
  i = (global_addl_config.backlight_intensities & 15) % NUM_BACKLIGHT_INTENSITIES;
  if( i>= NUM_BACKLIGHT_INTENSITIES )
   {  i = NUM_BACKLIGHT_INTENSITIES-1; // valid ITEM indices: 0..9 (with NUM_BACKLIGHT_INTENSITIES=10)
   }
  md380_menu_entry_selected = i;       // <- always a zero-based item index
  mn_submenu_init(wt_bl_intensity_lo);
  for(i=0; i<NUM_BACKLIGHT_INTENSITIES; ++i)
   { mn_submenu_add(wt_bl_intensity[i], mn_backlight_intensity_funcs[i] );
   }
  mn_submenu_finalize();
}

void mn_backlight_hi(void)  // configure HIGH backlight intensity (used when "active")
{
  int i;
  bIntensityMenuIndex = 1;  // "now selecting the HIGH backlight intensity" ...
  // intensity value '0' (off) NOT ALLOWED for the 'HIGH' setting, thus SUBTRACT ONE below:
  i = ( (global_addl_config.backlight_intensities>>4) & 15) - 1;
  if( i>= (NUM_BACKLIGHT_INTENSITIES-1) )
   {  i = NUM_BACKLIGHT_INTENSITIES-2; // valid ITEM indices: 0..8 (with NUM_BACKLIGHT_INTENSITIES=10)
   }
  if( i<0 )
   {  i=0;
   }
  md380_menu_entry_selected = i;       // <- always a zero-based item index
  mn_submenu_init(wt_bl_intensity_hi);
  for(i=1/*!!*/; i<NUM_BACKLIGHT_INTENSITIES; ++i)
   { mn_submenu_add(wt_bl_intensity[i], mn_backlight_intensity_funcs[i] );
   }
  mn_submenu_finalize();
}
#endif // CONFIG_DIMMED_LIGHT ?


#if defined(FW_D13_020) || defined(FW_S13_020)
void set_sidebutton_function(const wchar_t *label)
{
   button_function = button_functions[currently_selected_menu_entry];

   switch ( button_selected ) {
      case 0:
         top_side_button_pressed_function = button_function;
         md380_spiflash_write(&button_function, 0x2102, 1);
         mn_create_single_timed_ack(wt_button_top_press,label);
         break;
      case 1:
         bottom_side_button_pressed_function = button_function;
         md380_spiflash_write(&button_function, 0x2104, 1);
         mn_create_single_timed_ack(wt_button_bot_press,label);
         break;
      case 2:
         top_side_button_held_function = button_function;
         md380_spiflash_write(&button_function, 0x2103, 1);
         mn_create_single_timed_ack(wt_button_top_held,label);
         break;
      case 3:
         bottom_side_button_held_function = button_function;
         md380_spiflash_write(&button_function, 0x2105, 1);
         mn_create_single_timed_ack(wt_button_bot_held,label);
         break;
   }
}

void nm_button_unassigned() { set_sidebutton_function(wt_button_unassigned); }
void nm_button_alert_tone() { set_sidebutton_function(wt_button_alert_tone); }
void nm_button_emerg_on() { set_sidebutton_function(wt_button_emerg_on); }
void nm_button_emerg_off() { set_sidebutton_function(wt_button_emerg_off); }
void nm_button_power() { set_sidebutton_function(wt_button_power); }
void nm_button_monitor() { set_sidebutton_function(wt_button_monitor); }
void nm_button_nuisance() { set_sidebutton_function(wt_button_nuisance); }
void nm_button_ot1() { set_sidebutton_function(wt_button_ot1); }
void nm_button_ot2() { set_sidebutton_function(wt_button_ot2); }
void nm_button_ot3() { set_sidebutton_function(wt_button_ot3); }
void nm_button_ot4() { set_sidebutton_function(wt_button_ot4); }
void nm_button_ot5() { set_sidebutton_function(wt_button_ot5); }
void nm_button_ot6() { set_sidebutton_function(wt_button_ot6); }
void nm_button_rep_talk() { set_sidebutton_function(wt_button_rep_talk); }
void nm_button_scan() { set_sidebutton_function(wt_button_scan); }
void nm_button_squelch() { set_sidebutton_function(wt_button_squelch); }
void nm_button_privacy() { set_sidebutton_function(wt_button_privacy); }
void nm_button_vox() { set_sidebutton_function(wt_button_vox); }
void nm_button_zone() { set_sidebutton_function(wt_button_zone); }
void nm_button_zone_tog() { set_sidebutton_function(wt_button_zone_tog); }
void nm_button_bat_ind() { set_sidebutton_function(wt_button_bat_ind); }
void nm_button_man_dial() { set_sidebutton_function(wt_button_man_dial); }
void nm_button_lone_work() { set_sidebutton_function(wt_button_lone_work); }
void nm_button_1750_hz() { set_sidebutton_function(wt_button_1750_hz); }
void nm_button_bklt_en() { set_sidebutton_function(wt_button_bklt_en); }
void nm_button_set_tg()  { set_sidebutton_function(wt_button_set_tg);  }
#if( CONFIG_MORSE_OUTPUT )
 void nm_button_narrator() { set_sidebutton_function(wt_button_narrator); }
 void nm_button_cw_repeat(){ set_sidebutton_function(wt_button_cw_repeat); }
#endif

void select_sidebutton_function_screen(void)
{
   button_selected = currently_selected_menu_entry;

   switch ( button_selected ) {
      case 0:
         md380_menu_entry_selected = index_of(top_side_button_pressed_function, (uint8_t*)button_functions,  sizeof(button_functions));
         // cast to (uint8_t*) to eliminate several 'const' warnings
         mn_submenu_init(wt_button_top_press);
         break;
      case 1:
         md380_menu_entry_selected = index_of(bottom_side_button_pressed_function, (uint8_t*)button_functions, sizeof(button_functions));
         mn_submenu_init(wt_button_bot_press);
         break;
      case 2:
         md380_menu_entry_selected = index_of(top_side_button_held_function, (uint8_t*)button_functions, sizeof(button_functions));
         mn_submenu_init(wt_button_top_held);
         break;
      case 3:
         md380_menu_entry_selected = index_of(bottom_side_button_held_function, (uint8_t*)button_functions, sizeof(button_functions));
         mn_submenu_init(wt_button_bot_held);
         break;
   }


   mn_submenu_add(wt_button_unassigned, nm_button_unassigned);
   mn_submenu_add(wt_button_alert_tone, nm_button_alert_tone);
   mn_submenu_add(wt_button_emerg_on, nm_button_emerg_on);
   mn_submenu_add(wt_button_emerg_off, nm_button_emerg_off);
   mn_submenu_add(wt_button_power, nm_button_power);
   mn_submenu_add(wt_button_monitor, nm_button_monitor);
   mn_submenu_add(wt_button_nuisance, nm_button_nuisance);
   mn_submenu_add(wt_button_ot1, nm_button_ot1);
   mn_submenu_add(wt_button_ot2, nm_button_ot2);
   mn_submenu_add(wt_button_ot3, nm_button_ot3);
   mn_submenu_add(wt_button_ot4, nm_button_ot4);
   mn_submenu_add(wt_button_ot5, nm_button_ot5);
   mn_submenu_add(wt_button_ot6, nm_button_ot6);
   mn_submenu_add(wt_button_rep_talk, nm_button_rep_talk);
   mn_submenu_add(wt_button_scan, nm_button_scan);
   mn_submenu_add(wt_button_squelch, nm_button_squelch);
   mn_submenu_add(wt_button_privacy, nm_button_privacy);
   mn_submenu_add(wt_button_vox, nm_button_vox);
   mn_submenu_add(wt_button_zone, nm_button_zone);
   mn_submenu_add(wt_button_zone_tog, nm_button_zone_tog);
   mn_submenu_add(wt_button_bat_ind, nm_button_bat_ind);
   mn_submenu_add(wt_button_man_dial, nm_button_man_dial);
   mn_submenu_add(wt_button_lone_work, nm_button_lone_work);
   mn_submenu_add(wt_button_1750_hz, nm_button_1750_hz);
   mn_submenu_add(wt_button_bklt_en, nm_button_bklt_en);
   mn_submenu_add(wt_button_set_tg,  nm_button_set_tg );
# if( CONFIG_MORSE_OUTPUT )
   mn_submenu_add(wt_button_narrator,nm_button_narrator);
   mn_submenu_add(wt_button_cw_repeat,nm_button_cw_repeat);
# endif

   mn_submenu_finalize();
}
#else
#warning Side Buttons not supported on D02 firmware
#endif

void create_menu_entry_sidebutton_screen(void)
{
#if defined(FW_D13_020) || defined(FW_S13_020)

   md380_menu_entry_selected = 0;
    mn_submenu_init(wt_sidebutton_menu);

    mn_submenu_add_98(wt_button_top_press, select_sidebutton_function_screen);
    mn_submenu_add_98(wt_button_bot_press, select_sidebutton_function_screen);
    mn_submenu_add_98(wt_button_top_held, select_sidebutton_function_screen);
    mn_submenu_add_98(wt_button_bot_held, select_sidebutton_function_screen);

    mn_submenu_finalize2();
#endif
}


void create_menu_entry_backlight_screen(void)
{
#if defined(FW_D13_020) || defined(FW_S13_020)
   md380_menu_entry_selected = 0;
   mn_submenu_init(wt_backlight_menu);


#  if( CONFIG_DIMMED_LIGHT )    // *optional* feature since 2017-01-08 - see config.h
    mn_submenu_add_98(wt_bl_intensity_lo/*item text*/, mn_backlight_lo/*menu handler*/ ); // backlight intensity "low" (used when idle)
    mn_submenu_add_98(wt_bl_intensity_hi/*item text*/, mn_backlight_hi/*menu handler*/ ); // backlight intensity "high" (used when active)
#  endif   
    mn_submenu_add_98(wt_backlight, mn_backlight); // backlight TIMER (longer than Tytera's 5/10/15 seconds)
    mn_submenu_finalize2();
#endif
}

#if( CONFIG_MORSE_OUTPUT )  // *optional* feature since 2017 - see config.h
//-------------------------------------------------------------
// CW output for visually impaired hams ?
// Morse code generator in irq_handlers.c, 'narrator' in narrator.c .
// Configuration are 4 bytes in md380_radio_config, NOT in the codeplug.
const static wchar_t wt_morse_menu[] = L"Morse output";
const static wchar_t wt_morse_mode[] = L"Mode";   // ex: "Mode/verbosity" (too verbose :)
const static wchar_t wt_off[]      = L"off";
const static wchar_t wt_short[]    = L"short";
const static wchar_t wt_verbose[]  = L"verbose";
const static wchar_t wt_test[]     = L"test/DevOnly";
const static wchar_t wt_cw_speed[] = L"Speed [WPM]";
const static wchar_t wt_15[]       = L"15";
const static wchar_t wt_18[]       = L"18";
const static wchar_t wt_22[]       = L"22";
const static wchar_t wt_30[]       = L"30";
const static wchar_t wt_35[]       = L"35";
const static wchar_t wt_40[]       = L"40";
const static wchar_t wt_cw_pitch[] = L"CW pitch [Hz]";
const static wchar_t wt_400[]      = L"400";
const static wchar_t wt_650[]      = L"650";
const static wchar_t wt_800[]      = L"800";
const static wchar_t wt_cw_volume[]= L"CW volume";
const static wchar_t wt_low[]      = L"low";
const static wchar_t wt_medium[]   = L"medium";
const static wchar_t wt_high[]     = L"high";
const static wchar_t wt_auto[]     = L"auto";

void mn_morse_mode_off(void)
{  
   mn_create_single_timed_ack(wt_morse_mode, wt_off);
   // ex: global_addl_config.narrator_mode = NARRATOR_MODE_OFF; 
   // Since Mr Narrator can be started via prog'able sidekey,
   // leave the 'verbosity' flag unchanged, clear TEST mode,
   // disable the AUTOMATIC start of the narrator (e.g. channel knob),
   // so the *requested* announcement can be short or verbose.
   global_addl_config.narrator_mode &= ~(NARRATOR_MODE_ENABLED|NARRATOR_MODE_TEST);
   cfg_save();
}

void mn_morse_mode_short(void)
{  
   mn_create_single_timed_ack(wt_morse_mode, wt_short);
   global_addl_config.narrator_mode = NARRATOR_MODE_ENABLED;
   cfg_save();
}

void mn_morse_mode_verbose(void)
{  
   mn_create_single_timed_ack(wt_morse_mode, wt_verbose);
   global_addl_config.narrator_mode = NARRATOR_MODE_ENABLED | NARRATOR_MODE_VERBOSE; 
   cfg_save();
}

void mn_morse_mode_test(void)
{  
   mn_create_single_timed_ack(wt_morse_mode, wt_test);
   global_addl_config.narrator_mode |= NARRATOR_MODE_ENABLED | NARRATOR_MODE_TEST;
   // Don't modify the "VERBOSE"-flag here. To have both 'test' AND 
   //  'verbose' mode, first select 'verbose',  then select 'test' mode.
   // Details about the narrator and his configuration in narrator.c .
   cfg_save();
}

void mn_morse_mode(void)  // CW output mode : off / short / verbose / test ?
{
    if( global_addl_config.narrator_mode & NARRATOR_MODE_TEST )
     { md380_menu_entry_selected = 3;
     }
    else if( (global_addl_config.narrator_mode & (NARRATOR_MODE_VERBOSE|NARRATOR_MODE_ENABLED) )
                                              == (NARRATOR_MODE_VERBOSE|NARRATOR_MODE_ENABLED) )
     { md380_menu_entry_selected = 2;
     }
    else if( global_addl_config.narrator_mode & NARRATOR_MODE_ENABLED )
     { md380_menu_entry_selected = 1;
     }
    else // neither "TEST", "VERBOSE", "NORMAL", or any combination...
     { md380_menu_entry_selected = 0; // .. so *automatic* Morse output is OFF
     }
    mn_submenu_init(wt_morse_mode);
    mn_submenu_add(wt_off,    mn_morse_mode_off );      // no morse output at all
    mn_submenu_add(wt_short,  mn_morse_mode_short );    // channel NUMBERS, short output
    mn_submenu_add(wt_verbose,mn_morse_mode_verbose );  // channel NAMES, longer output
    mn_submenu_add(wt_test,   mn_morse_mode_test );     // signal any state transition in CW
    mn_submenu_finalize();
}

void mn_cw_pitch_400(void)
{  
   mn_create_single_timed_ack(wt_cw_pitch, wt_400);
   global_addl_config.cw_pitch_10Hz = 40; // 400 Hz
   cfg_save();
}

void mn_cw_pitch_650(void)
{  
   mn_create_single_timed_ack(wt_cw_pitch, wt_650);
   global_addl_config.cw_pitch_10Hz = 65; // 650 Hz
   cfg_save();
}

void mn_cw_pitch_800(void)
{  
   mn_create_single_timed_ack(wt_cw_pitch, wt_800);
   global_addl_config.cw_pitch_10Hz = 80; // 800 Hz
   cfg_save();
}

void mn_cw_pitch(void)  // CW pitch : stored in 10-Hz unit in global_addl_config
{  // A DECIMAL input field would be too clumsy with this dreadful API,
   // so for the moment, only offer a few 'tone frequencies' here.
   // A wider choice of values (entered directly, without this clumsiness)
   // is possible through DL4YHF's "alternative" menu - see app_menu.c .
   // The PWM'ed rectangular wave is rich in harmonics,
   // thus even 400 Hz is well audible in the speaker.
   // Note: To fit in a byte, the unit for storage is 10 Hz.
    if( global_addl_config.cw_pitch_10Hz < 50 ) 
     { md380_menu_entry_selected = 0; // 400 Hz
     }
    else if( global_addl_config.cw_pitch_10Hz < 70 ) 
     { md380_menu_entry_selected = 1; // 650 Hz
     }
    else
     { md380_menu_entry_selected = 2; // 800 Hz
     }
    mn_submenu_init(wt_cw_pitch);
    mn_submenu_add(wt_400, mn_cw_pitch_400 );
    mn_submenu_add(wt_650, mn_cw_pitch_650 );
    mn_submenu_add(wt_800, mn_cw_pitch_800 );
    mn_submenu_finalize();
}

void mn_cw_volume_low(void)
{  
   mn_create_single_timed_ack(wt_cw_volume, wt_low);
   global_addl_config.cw_volume = 5; // unit: "percent of maximum"
   cfg_save();
}

void mn_cw_volume_medium(void)
{  
   mn_create_single_timed_ack(wt_cw_volume, wt_medium);
   global_addl_config.cw_volume = 10; 
   cfg_save();
}

void mn_cw_volume_high(void)
{  
   mn_create_single_timed_ack(wt_cw_volume, wt_high);
   global_addl_config.cw_volume = 20; // %, NOT affected by the pot !
   cfg_save();
}

void mn_cw_volume_auto(void)
{  
   mn_create_single_timed_ack(wt_cw_volume, wt_auto);
   global_addl_config.cw_volume = BEEP_VOLUME_AUTO; // "try to follow the audio volume pot"
   cfg_save();
}

void mn_cw_volume(void) 
{  // Also here, a simple DECIMAL input field would be nice to have...
   // since 2017-04, that's possible through applet/src/app_menu.c .
    if( global_addl_config.cw_volume <= 5 ) 
     { md380_menu_entry_selected = 0; // "low" (on a very unscientific scale)
     }
    else if( global_addl_config.cw_volume < 20 ) 
     { md380_menu_entry_selected = 1; // "medium"
     }
    else if( global_addl_config.cw_volume == BEEP_VOLUME_AUTO ) 
     { md380_menu_entry_selected = 3; // "auto[matic]"
     }
    else
     { md380_menu_entry_selected = 2; // "high"
     }
    mn_submenu_init(wt_cw_volume);
    mn_submenu_add(wt_low,    mn_cw_volume_low    );
    mn_submenu_add(wt_medium, mn_cw_volume_medium );
    mn_submenu_add(wt_high,   mn_cw_volume_high   );
    mn_submenu_add(wt_auto,   mn_cw_volume_auto   );
    mn_submenu_finalize();
}

void mn_cw_speed_15WPM(void)
{  
   mn_create_single_timed_ack(wt_cw_speed, wt_15 );
   global_addl_config.cw_speed_WPM = 15;
   cfg_save();
}

void mn_cw_speed_18WPM(void)
{  
   mn_create_single_timed_ack(wt_cw_speed, wt_18 );
   global_addl_config.cw_speed_WPM = 18;
   cfg_save();
}

void mn_cw_speed_22WPM(void)
{  
   mn_create_single_timed_ack(wt_cw_speed, wt_22 );
   global_addl_config.cw_speed_WPM = 22;
   cfg_save();
}

void mn_cw_speed_30WPM(void)
{  
   mn_create_single_timed_ack(wt_cw_speed, wt_30 );
   global_addl_config.cw_speed_WPM = 30;
   cfg_save();
}

void mn_cw_speed_35WPM(void)
{  
   mn_create_single_timed_ack(wt_cw_speed, wt_35 );
   global_addl_config.cw_speed_WPM = 35;
   cfg_save();
}

void mn_cw_speed_40WPM(void)
{  
   mn_create_single_timed_ack(wt_cw_speed, wt_40 );
   global_addl_config.cw_speed_WPM = 40;
   cfg_save();
}

void mn_cw_speed(void) 
{
   // An easier method, without this nested "submenu-madness" is possible
   // via applet/src/app_menu.c (direct decimal input of the value in WPM).
   switch( global_addl_config.cw_speed_WPM )
    { case 15: md380_menu_entry_selected = 0; break; // for beginners
      case 18: md380_menu_entry_selected = 1; break;
      case 22: md380_menu_entry_selected = 2; break; // for advanced
      case 30: md380_menu_entry_selected = 3; break;
      case 35: md380_menu_entry_selected = 4; break; // for freaks
      case 40: md380_menu_entry_selected = 5; break; // for complete nuts 
      default: md380_menu_entry_selected = 2; break; // meaningful default ?
      // wrong md380_menu_entry_selected makes the menu crash !
    }
   mn_submenu_init(wt_cw_speed);
   mn_submenu_add(wt_15, mn_cw_speed_15WPM );
   mn_submenu_add(wt_18, mn_cw_speed_18WPM );
   mn_submenu_add(wt_22, mn_cw_speed_22WPM );
   mn_submenu_add(wt_30, mn_cw_speed_30WPM );
   mn_submenu_add(wt_35, mn_cw_speed_35WPM );
   mn_submenu_add(wt_40, mn_cw_speed_40WPM );
   mn_submenu_finalize();
}

void create_menu_entry_morse_screen(void)
{
    md380_menu_entry_selected = 0;
    mn_submenu_init(wt_morse_menu);
    mn_submenu_add_98(wt_morse_mode,mn_morse_mode); // disable, short, verbose output ?
    mn_submenu_add_98(wt_cw_speed,  mn_cw_speed  ); // morse output speed in WPM
    mn_submenu_add_98(wt_cw_pitch,  mn_cw_pitch  ); // audio frequency in Hertz
    mn_submenu_add_98(wt_cw_volume, mn_cw_volume ); // speaker output volume (pot has no effect)
    mn_submenu_finalize2();
}
#endif // CONFIG_MORSE_OUTPUT ?


void mn_config_reset2()
{
    mn_create_single_timed_ack(wt_config_reset,wt_config_reset_doit);

    memset( &global_addl_config, 0, sizeof(global_addl_config) );
    
    cfg_save();
}

void mn_config_reset(void)
{
    mn_submenu_init(wt_config_reset);
    
    mn_submenu_add(wt_config_reset_doit, mn_config_reset2);

    mn_submenu_finalize();
}

void create_menu_entry_edit_screen_store(void)
{
#if 0
    printf("your enter: ");
    printhex2((char *) md380_menu_edit_buf, 14);
    printf("\n");
#endif
    md380_menu_id = md380_menu_id - 1;
    md380_menu_depth = md380_menu_depth - 1;

#ifdef CONFIG_MENU
    md380_create_menu_entry(md380_menu_id, md380_menu_edit_buf, MKTHUMB(md380_menu_entry_back), MKTHUMB(md380_menu_entry_back), 6, 1, 1);
#endif
}

void create_menu_entry_edit_screen(void)
{
    menu_t *menu_mem;
    uint8_t i;
    uint8_t *p;

    md380_menu_0x2001d3c1 = md380_menu_0x200011e4;
    mn_editbuffer_poi = md380_menu_edit_buf;


    /*
      0x08012a8e      dff8200b       ldr.w r0, [pc, 0xb20]       ; [0x80135b0:4]=0x20001114
      0x08012a92      dff8201b       ldr.w r1, [pc, 0xb20]       ; [0x80135b4:4]=0x2001b716
      0x08012a96      0160           str r1, [r0]
      0x08012a98      0020           movs r0, 0
      0x08012a9a      07e0           b 0x8012aac
      0x08012a9c      c0b2           uxtb r0, r0
      0x08012a9e      dff8101b       ldr.w r1, [pc, 0xb10]       ; [0x80135b0:4]=0x20001114
      0x08012aa2      0968           ldr r1, [r1]
      0x08012aa4      0022           movs r2, 0
      0x08012aa6      21f81020       strh.w r2, [r1, r0, lsl 1]
      0x08012aaa      401c           adds r0, r0, 1
      0x08012aac      c0b2           uxtb r0, r0
      0x08012aae      1128           cmp r0, 0x11
      0x08012ab0      f4d3           blo 0x8012a9c
     */

    // clear return buffer //  see 0x08012a98
    // TODO: is wchar_t (16 bits))
    for (i = 0; i < 0x11; i++) {
        p = (uint8_t *) mn_editbuffer_poi;
        p = p + i;
        *p = 0;
    }


    md380_menu_0x2001d3ed = 8;
    md380_menu_0x2001d3ee = 0;
    md380_menu_0x2001d3ef = 0;
    md380_menu_0x2001d3f0 = 3;
    md380_menu_0x2001d3f1 = 0;
    md380_menu_0x2001d3f4 = 0;

    menu_mem = get_menu_stackpoi();
    menu_mem->menu_title = wt_edit;
    menu_mem->entries = &md380_menu_mem_base[md380_menu_id];
    menu_mem->numberof_menu_entries = 1;
    menu_mem->unknown_00 = 0;
    menu_mem->unknown_01 = 0;

#ifdef CONFIG_MENU
    md380_create_menu_entry(md380_menu_id, wt_edit, MKTHUMB(create_menu_entry_edit_screen_store), MKTHUMB(md380_menu_numerical_input), 0x81, 0, 1);
#endif
}

void create_menu_entry_edit_dmr_id_screen_store(void)
{
    uint32_t new_dmr_id = 0;
    wchar_t *bf;

#if 0
    printf("your enter: ");
    printhex2((char *) md380_menu_edit_buf, 14);
    printf("\n");
#endif

    bf = md380_menu_edit_buf;
    while (*bf != 0) {
        new_dmr_id *= 10;
        new_dmr_id += (*bf++) - '0';
    }
    
    if ( new_dmr_id > 0xffffff ) {
        return;
    }
    
#if 0
    printf("\n%d\n", new_dmr_id);
#endif
    
    // save in addl cfg.
    global_addl_config.dmrid = new_dmr_id ;
    cfg_save();

    // save in codeplug.
    md380_radio_config.dmrid = new_dmr_id ;
    md380_spiflash_write(&new_dmr_id, FLASH_OFFSET_DMRID, 4);
    
    md380_menu_id = md380_menu_id - 1;
    md380_menu_depth = md380_menu_depth - 1;

    if (global_addl_config.userscsv == 1) {
        cfg_set_radio_name();
    }

#ifdef CONFIG_MENU
    md380_create_menu_entry(md380_menu_id, md380_menu_edit_buf, MKTHUMB(md380_menu_entry_back), MKTHUMB(md380_menu_entry_back), 6, 1, 1);
#endif
}

void create_menu_entry_edit_dmr_id_screen(void)
{
    menu_t *menu_mem;
    uint8_t i;
    uint8_t *p;
    uint32_t nchars;

    md380_menu_0x2001d3c1 = md380_menu_0x200011e4;
    mn_editbuffer_poi = md380_menu_edit_buf;



    // clear return buffer //  see 0x08012a98
    // TODO: is wchar_t (16 bits))
    for (i = 0; i < 0x11; i++) {
        p = (uint8_t *) mn_editbuffer_poi;
        p = p + i;
        *p = 0;
    }

    nchars = uli2w(md380_radio_config.dmrid, md380_menu_edit_buf);

#if 0
    printf("\ncreate_menu_entry_edit_dmr_id_screen %x %d \n", md380_menu_edit_buf, nchars);
    printhex2((char *) md380_menu_edit_buf, 14);
    printf("\n");
#endif

    md380_menu_0x2001d3ed = 8; // max char
    md380_menu_0x2001d3ee = nchars; //  startpos cursor
    md380_menu_0x2001d3ef = nchars; //  startpos cursor
    md380_menu_0x2001d3f0 = 3; // 3 = numerical input
    md380_menu_0x2001d3f1 = 0;
    md380_menu_0x2001d3f4 = 0;
    menu_mem = get_menu_stackpoi();
    menu_mem->menu_title = wt_edit_dmr_id;
    menu_mem->entries = &md380_menu_mem_base[md380_menu_id];
    menu_mem->numberof_menu_entries = 1;
    menu_mem->unknown_00 = 0;
    menu_mem->unknown_01 = 0;

#ifdef CONFIG_MENU
    md380_create_menu_entry(md380_menu_id, wt_edit_dmr_id, MKTHUMB(create_menu_entry_edit_dmr_id_screen_store), MKTHUMB(md380_menu_numerical_input), 0x81, 0, 1);
#endif
}

void create_menu_entry_set_tg_screen_store(void)
{
#if defined(FW_D13_020) || defined(FW_S13_020)
    uint32_t new_tx_id = 0;
    wchar_t *bf;

# if 0
    printf("your enter: ");
    printhex2((char *) md380_menu_edit_buf, 14);
    printf("\n");
# endif

    bf = md380_menu_edit_buf;
    while (*bf != 0) {
        new_tx_id *= 10;
        new_tx_id += (*bf++) - '0';
    }

    if ( new_tx_id > 0xffffff ) {
        return;
    }

# if 0
    printf("\n%d\n", new_tx_id);
# endif

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

# ifdef CONFIG_MENU
    md380_create_menu_entry(md380_menu_id, md380_menu_edit_buf, MKTHUMB(md380_menu_entry_back), MKTHUMB(md380_menu_entry_back), 6, 1, 1);
# endif
#endif // D13_020, S13_020, ..(?)
}

void create_menu_entry_set_tg_screen(void)
{
#if defined(FW_D13_020) || defined(FW_S13_020)
   menu_t *menu_mem;
   uint8_t i;
   uint8_t *p;
   uint32_t nchars;
   int current_tg = 0;

   md380_menu_0x2001d3c1 = md380_menu_0x200011e4;
   mn_editbuffer_poi = md380_menu_edit_buf;

   // clear return buffer //  see 0x08012a98
   // TODO: is wchar_t (16 bits))
   for (i = 0; i < 0x11; i++) {
      p = (uint8_t *) mn_editbuffer_poi;
      p = p + i;
      *p = 0;
   }

   // load current tg into edit buffer (#708) :
   current_tg = (int) contact.id_h ;
   current_tg = (current_tg<<8) + (int) contact.id_m;
   current_tg = (current_tg<<8) + (int) contact.id_l;

   nchars = uli2w(current_tg, md380_menu_edit_buf);
#  if 0
    printf("\ncreate_menu_entry_set_tg_screen %x %d \n", md380_menu_edit_buf, nchars);
    printhex2((char *) md380_menu_edit_buf, 14);
    printf("\n");
#  endif

    md380_menu_0x2001d3ed = 8; // max char
    md380_menu_0x2001d3ee = nchars; //  startpos cursor
    md380_menu_0x2001d3ef = nchars; //  startpos cursor
    md380_menu_0x2001d3f0 = 3; // 3 = numerical input
    md380_menu_0x2001d3f1 = 0;
    md380_menu_0x2001d3f4 = 0;
    menu_mem = get_menu_stackpoi();
    menu_mem->menu_title = wt_set_tg_id;
    menu_mem->entries = &md380_menu_mem_base[md380_menu_id];
    menu_mem->numberof_menu_entries = 1;
    menu_mem->unknown_00 = 0;
    menu_mem->unknown_01 = 0;

#  ifdef CONFIG_MENU
    md380_create_menu_entry(md380_menu_id, wt_set_tg_id, MKTHUMB(create_menu_entry_set_tg_screen_store), MKTHUMB(md380_menu_numerical_input), 0x81, 0, 1);
#  endif
#endif // D13_020, S13_020, ..(?)
}

void create_menu_entry_addl_functions_screen(void)
{
    mn_submenu_init(wt_addl_func);
    
#  if 0
    register uint32_t * sp asm("sp");
    for (int i = 0; i < 20; i++) {
        printf("%d : 0x%x\n", i, sp[i]);
    }
    //printf( "f menucall.%s 0 0x%x\n", lbl2, (sp[15] - 1 - 4) );
#  endif    
    PRINTRET();
    PRINT("create_menu_entry_addl_functions_screen\n");

    mn_submenu_add_98(wt_rbeep, create_menu_entry_rbeep_screen);
    mn_submenu_add(wt_bootopts, create_menu_entry_bootopts_screen);
    mn_submenu_add_98(wt_datef, create_menu_entry_datef_screen);
    mn_submenu_add_98(wt_showcall, create_menu_entry_showcall_screen);
    mn_submenu_add_98(wt_debug, create_menu_entry_debug_screen);
    mn_submenu_add_98(wt_promtg, create_menu_entry_promtg_screen);
    mn_submenu_add_8a(wt_edit, create_menu_entry_edit_screen, 0); // disable this menu entry - no function jet
    mn_submenu_add_8a(wt_edit_dmr_id, create_menu_entry_edit_dmr_id_screen, 1);
    mn_submenu_add_8a(wt_set_tg_id, create_menu_entry_set_tg_screen, 1); // Brad's PR#708 already in use here (DL4YHF, since 2017-03)
    mn_submenu_add_98(wt_micbargraph, create_menu_entry_micbargraph_screen);
    mn_submenu_add_8a(wt_experimental, create_menu_entry_experimental_screen, 1);
    mn_submenu_add(wt_sidebutton_menu, create_menu_entry_sidebutton_screen);
    
    mn_submenu_add_98(wt_config_reset, mn_config_reset);

    mn_submenu_add(wt_backlight_menu, create_menu_entry_backlight_screen);
#  if( CONFIG_MORSE_OUTPUT )
    mn_submenu_add(wt_morse_menu, create_menu_entry_morse_screen);
#  endif   
    mn_submenu_add_98(wt_cp_override, mn_cp_override);    
    mn_submenu_add_98(wt_netmon, create_menu_entry_netmon_screen);
    
    mn_submenu_finalize2();
}

void create_menu_utilies_hook(void)
{
    menu_t *menu_mem;
    int enabled;

    if( (md380_program_radio_unprohibited[4] & 0x4) == 0x4 ) {
        enabled = 0;
    } else {
        enabled = 1;
    }

    menu_mem = get_menu_stackpoi();
    menu_mem->entries = &md380_menu_mem_base[md380_menu_id];
    //  menu_mem->numberof_menu_entries++;
    menu_mem->numberof_menu_entries = 6;


#ifdef CONFIG_MENU
    md380_create_menu_entry(8, md380_wt_programradio, MKTHUMB(md380_menu_entry_programradio), MKTHUMB(md380_menu_entry_back), 0x8a, 0, enabled);

#  ifdef FW_D13_020
    md380_create_menu_entry(11, wt_addl_func, MKTHUMB(create_menu_entry_addl_functions_screen), MKTHUMB(md380_menu_entry_back), 0x8a, 0, 1);
#  else
    if( menu_mem->numberof_menu_entries == 6 ) { // d13.020 has hidden gps entrys on this menu
        md380_create_menu_entry(11, wt_addl_func, MKTHUMB(create_menu_entry_addl_functions_screen), MKTHUMB(md380_menu_entry_back), 0x8a, 0, 1);
    } else {
        md380_create_menu_entry(9, wt_addl_func, MKTHUMB(create_menu_entry_addl_functions_screen), MKTHUMB(md380_menu_entry_back), 0x8a, 0, 1);
    }
#  endif

#endif // CONFIG_MENU ?

}

/* This hooks a function that is called a lot during menu processing.
   Its exact purpose is unknown, but I'm working on that.
 */
void *main_menu_hook(void *menu){
#if 0
  void *menustruct;

//  printf("main_menu() ");
//  printhex(menu,32);
//  printf("\n");


  switch(* ((int*)menu)){
  case 0x0b:
    //printf("Exiting menu.\n");
    break;
  case 0x24:
    //Third word of the parameter is a structure with
    //more entries.
    menustruct=(void*) *((int*)menu + 2);

    printf("Menu struct: @0x%08x\n",
      menustruct);
    printf("Item %5d/%5d selected. %s\n",
      (int) *((unsigned short*) (menustruct+0x42)),
      (int) *((unsigned short*)menustruct),
      "test");


    //printhex(*((int*) menu+2),128);
    //printf("\n");

    /*

Main menu:
Menu struct: @0x20001398
06000000 Total Entries
02000000 Selected Page Index
04000000 4a000000 00000000 91000000 8c0f0d08 00000000 3280ff00
1414ff00 c0c0c000 c0c0c000 00000000 ffffff00 ffffff00 80808000 00000500
                                                                   \--/
                                                                Selected item
00000000 4a001600 00436f6e 74616374 73006361 27001600 00536361 6e001600
                    \--Contacts begins here.
0a000c00 27001600 005a6f6e 65006c20 0a000c00 0b006573 49001600

Contacts Menu, last Entry:
Menu struct: @0x20001390
e4020000 Total Entries
e0020000 Selected Page Index
         04000000 9c000000 00000000 91000000 8c0f0d08 00000000 3280ff00
1414ff00 c0c0c000 c0c0c000 00000000 ffffff00 ffffff00 80808000 0000e302
                                                                   \--/
                                                                Selected item
00000000 7d001600 00547269 2d537461 74652028 4c322900 41001600 004c6f63
                    \--First contact entry starts here.
616c2039 00436f6e 09000b00 3b001600 00444d52 204e4100 09000b00
     */
    break;
  default:
    //do nothing
    break;
  }
#endif
  return main_menu(menu);
}
