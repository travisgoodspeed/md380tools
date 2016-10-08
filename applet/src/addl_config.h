/*! \file addl_config.h
  \brief .
*/

#ifndef MD380TOOLS_ADDL_CONFIG_H_INCLUDED
#define MD380TOOLS_ADDL_CONFIG_H_INCLUDED

#include <stdint.h>

enum spi_flash_addl_config {
  offset_rbeep,
  offset_datef,
  offset_userscsv,
  offset_debug,
  offset_promtg,
  offset_prompriv,
  offset_micbargraph,
  offset_console,
};

extern struct addl_config {
    uint8_t rbeep;
    uint8_t datef;
    uint8_t userscsv;
    uint8_t debug;
    uint8_t promtg;
    uint8_t experimental;
    uint8_t micbargraph;
    uint8_t console;
} global_addl_config;

extern void init_global_addl_config_hook(void);

extern void spiflash_write_uint8( int offset, uint8_t val );
extern uint8_t spiflash_read_uint8( int offset );
extern uint8_t spiflash_read_uint8_ranged( int offset, uint8_t cnt );


void cfg_save();

#endif
