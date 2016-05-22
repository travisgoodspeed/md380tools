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

void init_global_addl_config_struct() {
  int8_t buf[1];

  md380_spiflash_read(buf, spi_flash_addl_config_start + offset_rbeep, 1);
  if (buf[0] == '1') {
    global_addl_config.rbeep = 1;
  } else {
    global_addl_config.rbeep = 0;
  }

  md380_spiflash_read(buf, spi_flash_addl_config_start + offset_datef, 1);
  if (buf[0] == '1') {
    global_addl_config.datef = 1;
  } else {
    global_addl_config.datef = 0;
  }

  md380_spiflash_read(buf, spi_flash_addl_config_start + offset_userscsv, 1);
  if (buf[0] == '1') {
    global_addl_config.userscsv = 1;
  } else {
    global_addl_config.userscsv = 0;
  }

  md380_spiflash_read(buf, spi_flash_addl_config_start + offset_debug, 1);
  if (buf[0] == '1') {
    global_addl_config.debug = 1;
  } else {
    global_addl_config.debug = 0;
  }

  md380_spiflash_read(buf, spi_flash_addl_config_start + offset_promtg, 1);
  if (buf[0] == '1') {
    global_addl_config.promtg=1;
  } else {
    global_addl_config.promtg=0;
  }
  // global_addl_config.experimental is intentionally not permanent
  global_addl_config.experimental = 0;
}

void init_global_addl_config_hook(void) {
  init_global_addl_config_struct();

  md380_create_main_meny_entry();
}
