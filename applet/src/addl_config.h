/*! \file addl_config.h
  \brief .
*/

#ifndef MD380TOOLS_ADDL_CONFIG_H_INCLUDED
#define MD380TOOLS_ADDL_CONFIG_H_INCLUDED

#include <stdint.h>

#define FLASH_OFFSET_DMRID 0x2084
#define FLASH_OFFSET_RNAME 0x20B0

typedef struct addl_config {
    uint8_t crc;
    uint8_t length;
    uint8_t datef;
    uint8_t userscsv;
    uint8_t debug;
    uint8_t promtg;
    uint8_t experimental;
    uint8_t micbargraph;
    uint8_t netmon;
    uint8_t rbeep;
    uint8_t bootscr;
    uint32_t dmrid;
    char rname[32];
    uint8_t boot_custom_str;
} addl_config_t ;

extern addl_config_t global_addl_config;

extern void init_global_addl_config_hook(void);

void cfg_fix_dmrid();
void cfg_fix_radioname();

void cfg_save();

#endif
