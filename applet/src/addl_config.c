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

uint8_t spiflash_read_uint8( int offset )
{
    char buf[1];
    md380_spiflash_read(buf, spi_flash_addl_config_start + offset, 1);
    return buf[0] - '0' ;
}

void spiflash_write_uint8( int offset, uint8_t val )
{
    char buf[2];
    buf[0] = '0' + val ;
    buf[1] = 0 ;
    spiflash_write_with_type_check(buf, spi_flash_addl_config_start + offset, 1);
}

uint8_t spiflash_read_uint8_ranged( int offset, uint8_t cnt )
{
    uint8_t r = spiflash_read_uint8( offset );
    if( r >= cnt ) {
        // out of range, reset to default = 0
        r = 0 ;
    }
    return r ;
}

inline void spiflash_write_datef()
{
    spiflash_write_uint8( offset_datef, global_addl_config.datef );    
}

inline void spiflash_read_datef()
{
    global_addl_config.datef = spiflash_read_uint8_ranged( offset_datef, 6 );    
}

inline void spiflash_write_console()
{
    spiflash_write_uint8( offset_console, global_addl_config.console );    
}

inline void spiflash_read_console()
{
#if defined(FW_D13_020)    
    global_addl_config.console = spiflash_read_uint8_ranged( offset_console, 4 );    
#else
    global_addl_config.console = 0 ;
#endif    
}

inline void spiflash_write_promtg()
{
    spiflash_write_uint8( offset_promtg, global_addl_config.promtg );    
}

inline void spiflash_read_promtg()
{
    global_addl_config.promtg = spiflash_read_uint8_ranged( offset_promtg, 2 );    
}

inline void spiflash_write_micbargraph()
{
    spiflash_write_uint8( offset_micbargraph, global_addl_config.micbargraph );    
}

inline void spiflash_write_rbeep()
{
    spiflash_write_uint8( offset_rbeep, global_addl_config.rbeep );    
}

inline void spiflash_read_rbeep()
{
    global_addl_config.rbeep = spiflash_read_uint8_ranged( offset_rbeep, 2 );    
}

inline void spiflash_write_debug()
{
    spiflash_write_uint8( offset_debug, global_addl_config.debug );    
}

inline void spiflash_read_debug()
{
    global_addl_config.debug = spiflash_read_uint8_ranged( offset_debug, 2 );    
}

inline void spiflash_read_micbargraph()
{
    global_addl_config.micbargraph = spiflash_read_uint8_ranged( offset_micbargraph, 2 );    
}

inline void spiflash_write_userscsv()
{
    spiflash_write_uint8( offset_userscsv, global_addl_config.userscsv );    
}

inline void spiflash_read_userscsv()
{
    global_addl_config.userscsv = spiflash_read_uint8_ranged( offset_userscsv, 2 );    
}

void cfg_read_struct( addl_config_t *cfg )
{
    md380_spiflash_read(cfg, spi_flash_addl_config_start, sizeof(addl_config_t));
}

void cfg_write_struct( addl_config_t *cfg )
{
    spiflash_write_with_type_check(cfg, spi_flash_addl_config_start, sizeof(addl_config_t));
}

void read_compat()
{
    spiflash_read_rbeep();
    spiflash_read_datef();
    spiflash_read_userscsv();
    spiflash_read_debug();
    spiflash_read_promtg();
    spiflash_read_micbargraph();
    spiflash_read_console();    
}

void write_compat()
{
    // compat
    spiflash_write_promtg();
    spiflash_write_micbargraph();
    spiflash_write_micbargraph();
    spiflash_write_rbeep();
    spiflash_write_datef();
    spiflash_write_userscsv();
    spiflash_write_debug();
    spiflash_write_console();    
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

void cfg_load()
{
    memset( &global_addl_config, 0, sizeof(addl_config_t) );
    
    addl_config_t tmp ;    
    cfg_read_struct( &tmp );
    
    if( calc_crc(&tmp,tmp.length) != 0 ) {
        // corrupted.
        LOGB("cfg crc fail\n");
        return ;
    }
    
    if( tmp.length > sizeof(addl_config_t) ) {
        LOGB("cfg too big\n");
        tmp.length = sizeof(addl_config_t);
    }
    
    memcpy(&global_addl_config,&tmp,tmp.length);
        
    // restore dmrid
    int dmrid = global_addl_config.dmrid ;
    if( dmrid != 0 ) {
        md380_spiflash_write(&dmrid, 0x2084, 4);            
        md380_radio_config.dmrid = dmrid;
    }
    
    // global_addl_config.experimental is intentionally not permanent
    global_addl_config.experimental = 0;
}

void cfg_save()
{
    global_addl_config.crc = 0 ;
    global_addl_config.length = sizeof(addl_config_t);
    
    global_addl_config.crc = calc_crc(&global_addl_config,sizeof(addl_config_t));
    
    cfg_write_struct( &global_addl_config );
}   

void init_global_addl_config_hook(void)
{
    cfg_load();

    LOGB("booting\n");
    
#ifdef CONFIG_MENU
    md380_create_main_meny_entry();
#endif
    
}

