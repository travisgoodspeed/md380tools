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

struct addl_config global_addl_config;

void init_global_addl_config_struct() 
{
    spiflash_read_rbeep();
    spiflash_read_datef();
    spiflash_read_userscsv();
    spiflash_read_debug();
    spiflash_read_promtg();
    spiflash_read_micbargraph();
    spiflash_read_console();
    
    // global_addl_config.experimental is intentionally not permanent
    global_addl_config.experimental = 0;
}

void init_global_addl_config_hook(void) {
  init_global_addl_config_struct();

#ifdef CONFIG_MENU
  md380_create_main_meny_entry();
#endif
}
