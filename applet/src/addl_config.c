/*! \file addl_config.c
  \brief .
*/

#include <stdio.h>
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

addl_config_t global_addl_config;

void cfg_fix_dmrid()
{
    int dmrid = global_addl_config.dmrid ;
    if( dmrid != 0 ) {
        // store new dmr_id to ram and spi flash (codeplug)
        md380_spiflash_write(&dmrid, FLASH_OFFSET_DMRID, 4);
        md380_radio_config.dmrid = dmrid;
    }
}


void cfg_fix_radioname()
{
    char *rname = global_addl_config.rname;
    if( rname[0] != 0x00 ) {
        md380_spiflash_write(&rname, FLASH_OFFSET_RNAME, 32);
        for (uint8_t ii = 0; ii < 32; ii++) {
            md380_radio_config.radioname[ii] = rname[ii];
        }
    }
}

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
    R(global_addl_config.userscsv,1);
    R(global_addl_config.micbargraph,1);
    R(global_addl_config.debug,1);
    R(global_addl_config.rbeep,1);
    R(global_addl_config.promtg,1);
    R(global_addl_config.boot_demo,1);
    R(global_addl_config.boot_splash,2);
    R(global_addl_config.netmon,3);
    R(global_addl_config.datef,5);

    // restore dmrid
    cfg_fix_dmrid();

    // restore radio name
    if (global_addl_config.userscsv == 1) {
        cfg_fix_radioname();
    }

    // global_addl_config.experimental is intentionally not permanent
    global_addl_config.experimental = 0;

#if defined(FW_D13_020)    
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
    char callsign[10] = {0x00};

    if (get_dmr_user_field(2, callsign, global_addl_config.dmrid, 10) == 0) {
        strncpy(callsign, "UNKNOWNID", 10);
    }

    for (uint8_t ii = 0 ; ii < 20; ii++) {
        toplinetext[ii] = 0x00;
        if (ii%2 == 0) {
            toplinetext[ii] = callsign[ii/2];
        }
    }

    for (uint8_t ii = 0 ; ii < 32; ii++) {
        if (ii%2 == 0 && ii < 20) {
            md380_radio_config.radioname[ii] = callsign[ii/2];
            global_addl_config.rname[ii] = callsign[ii/2];
        } else {
            md380_radio_config.radioname[ii] = 0x00;
            global_addl_config.rname[ii] = 0x00;
        }
    }

    cfg_save();
    md380_spiflash_write(&md380_radio_config.radioname, FLASH_OFFSET_RNAME, 4);
}

void init_global_addl_config_hook(void)
{
    LOGB("booting\n");
    
    cfg_load();

//#ifdef CONFIG_MENU
    md380_create_main_meny_entry();
//#endif    
}

