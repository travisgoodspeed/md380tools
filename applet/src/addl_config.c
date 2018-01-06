/*! \file addl_config.c
  \brief .
*/

#include <string.h>

#include "printf.h"
#include "dmesg.h"
#include "md380.h"
#include "version.h"
#include "config.h"
#include "printf.h"
#include "spiflash.h"
#include "addl_config.h"
#include "radio_config.h"
#include "syslog.h"
#include "usersdb.h"
#include "irq_handlers.h" 	// boot_flags, BOOT_FLAG_LOADED_CONFIG defined here
#include "system_hrc5000.h"	// set HRC5000 FM register during startup to config values
#include "app_menu.h"
//#include "codeplug.h"

#if defined(FW_D13_020) || defined(FW_S13_020)
	#include "amenu_set_tg.h"
#else
#warning old firmware
#endif 

addl_config_t global_addl_config;

//void cfg_fix_radioname()
//{
//    if( global_addl_config.rname[0] != 0x00 ) {
//        md380_spiflash_write(&global_addl_config.rname[0], FLASH_OFFSET_RNAME, 32);
//        for (uint8_t ii = 0; ii < 32; ii++) {
//            md380_radio_config.radioname[ii] = global_addl_config.rname[ii];
//        }
//    }
//}

void cfg_read_struct( addl_config_t *cfg )
{
    md380_spiflash_read(cfg, spi_flash_addl_config_start, sizeof(addl_config_t));
}

void cfg_write_struct( addl_config_t *cfg )
{
    spiflash_write_with_type_check(cfg, spi_flash_addl_config_start, sizeof(addl_config_t));
}

uint8_t calc_crc( void *buf, int size)
{
    uint8_t crc = 0 ;
    uint8_t *p = buf ;
    for(int i=0;i<size;i++) {
        crc ^= p[i] ;
    }
    return crc ;
}

#define R( ii, max ) { if( ii < 0 || ii > max ) { ii = 0 ; } }

void cfg_load()
{
    memset( &global_addl_config, 0, sizeof(addl_config_t) );
    
    addl_config_t tmp ;    
    cfg_read_struct( &tmp );
    
    // the config in flash is bigger than mine.
    if( tmp.length > sizeof(addl_config_t) ) {
        // we cannot crc what we have not read. (for now).
        LOGB("cfg oversized\n");
        return ;        
    }
    
    if( calc_crc(&tmp,tmp.length) != 0 ) {
        // corrupted.
        LOGB("cfg crc fail\n");
        return ;
    }

    // copy the smaller config into our bigger config.
    // leaving the rest 0.
    memcpy(&global_addl_config,&tmp,tmp.length);
    
    // range limit
    R(global_addl_config.userscsv,3);   	// 2017-02-19   0-disable 1-userscsv 2-talkeralias 3-both
    R(global_addl_config.micbargraph,1);
    R(global_addl_config.debug,1);
    R(global_addl_config.rbeep,1);
    R(global_addl_config.promtg,1);
    R(global_addl_config.boot_demo,1);
//    R(global_addl_config.boot_splash,0); 	// unused
    R(global_addl_config.netmon,3);
    R(global_addl_config.datef,6);
    R(global_addl_config.fm_bpf,1);
    R(global_addl_config.fm_comp,1);
    R(global_addl_config.fm_preemp,1);
    R(global_addl_config.fm_bw,1);
    R(global_addl_config.fm_dev,6);
    R(global_addl_config.fm_mode,255);		// 2017-06-07	0x00-0xFF FM Opmode register
    R(global_addl_config.keyb_mode,3);		// 2017-05-25	0-legacy, 1-modern, 2-MD446, 3-develop
    R(global_addl_config.scroll_mode,2);  	// 2017-06-10   0=off, 1=fast, 2=slow
    R(global_addl_config.devmode_level,3);	// 2017-06-06	0-off, 1-show FM options, 2-extended USB logging, 3-hide menus
    R(global_addl_config.sms_mode,1);		// 2017-07-23	SMS Mode 0=off, 1=on
    R(global_addl_config.sms_rpt,1);		// 2017-07-23	RPT SMS 0=off, 1=on
    R(global_addl_config.sms_wx,2);		// 2017-07-28	WX  SMS 0=off, 1=RPT 2=GPS
    R(global_addl_config.sms_gps,2);		// 2017-07-28	GPS SMS 0=off, 1=on
    R(global_addl_config.mode_stat,3);		// 2017-08-08	0=off 1=on 2=CC/gain 3=compact Top statusline with mode/rpt info
    R(global_addl_config.mode_color,1);		// 2017-08-08	0=off 1=on  Top statusline set different color
    R(global_addl_config.chan_stat,4);		// 2017-08-08	0=off 1=on 2=rx_freq 3=tx_freq 4=rx_tx channel statusline with TS / TG info
    R(global_addl_config.chan_color,1);		// 2017-08-08	0=off 1=on  channel statusline set different color
    R(global_addl_config.lh_tsstat,2);		// 2018-01-04	0=off 1=TS on  2=TS/TG on  show TS/TG in lh statusline
    // restore dmrid
    if( ( global_addl_config.cp_override & CPO_DMR ) == CPO_DMR ) {
        md380_radio_config.dmrid = global_addl_config.dmrid ;
    }

//    // restore radio name
//    if (global_addl_config.userscsv == 1) {
//        cfg_fix_radioname();
//    }

    // global_addl_config.experimental is intentionally not permanent
    global_addl_config.experimental = 0;

#if defined(FW_D13_020) || defined(FW_S13_020)
#else
    global_addl_config.netmon = 0 ;
#endif
}

void cfg_save()
{
    global_addl_config.crc = 0 ;
    global_addl_config.length = sizeof(addl_config_t);
    
    global_addl_config.crc = calc_crc(&global_addl_config,sizeof(addl_config_t));
    
    cfg_write_struct( &global_addl_config );
}

void cfg_set_radio_name()
{
//    char callsign[10] = {0x00};
//
//    if (get_dmr_user_field(2, callsign, global_addl_config.dmrid, 10) == 0) {
//        strncpy(callsign, "UNKNOWNID", 10);
//    }
//    
//    // TODO: fix type in addl_config, or convert during boot.
////    wide_sprintf((wchar_t *)&global_addl_config.rname[0], "%s", callsign);
//    snprintfw(global_addl_config.rname, 10, "%s", callsign);
//    global_addl_config.rname[9] = 0x00;
//
//    cfg_save();
//    cfg_fix_radioname();
}

void init_global_addl_config_hook(void)
{
    LOGB("t=%d: booting\n", (int)IRQ_dwSysTickCounter ); // 362 SysTicks after power-on
   
    cfg_load();
#if defined(FW_D13_020) || defined(FW_S13_020)
    set_keyb(global_addl_config.keyb_mode);		// set keyboard to correct mode selected in config menu
    hrc5000_buffer_flush();				
    hrc5000_fm_set();					// set HRC5000 FM register during startup to config settings
    CheckTalkgroupAfterChannelSwitch(); 		// read current channel settings at start for advanced statusline
#endif

//#ifdef CONFIG_MENU
    md380_create_main_menu_entry();
//#endif    

    boot_flags |= BOOT_FLAG_LOADED_CONFIG;
}
