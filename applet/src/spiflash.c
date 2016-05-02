/*! \file spiflash.c
  \brief spiflash Hook functions.
*/

#include <stdio.h>
#include <string.h>

#include "printf.h"
#include "dmesg.h"
#include "md380.h"
#include "version.h"
#include "config.h"
#include "printf.h"


int (spiflash_read_hook)(void *dst, long adr, long len) {
  printf("%x %x %d\n", dst, adr, len);
  return md380_spiflash_read(dst, adr, len);
}

uint32_t get_spi_flash_type(uint8_t *data) {
  md380_spiflash_enable();
  md380_spi_sendrecv(0x9f);
  data[0]=md380_spi_sendrecv(0x00);
  data[1]=md380_spi_sendrecv(0x00);
  data[2]=md380_spi_sendrecv(0x00);
  md380_spiflash_disable();
  return( (data[0]<<16) | (data[1]<<8) | (data[2]));
}

int check_spi_flash_type(void) {
  static int ok=0;
  if (ok==1) {
    return 1;
  } else {
    uint32_t ret;
    uint8_t data[4];

    ret=get_spi_flash_type(data);
    if ( ret == 0xef4018  ) {  // Manufacturer and Device Identification for W25Q128FV
      ok=1;
      return 1;
    } else {
      if ( ret == 0x10dc01 ) { // 2. Manufacturer and Device Identification for W25Q128FV maybe
        ok=1;
        return 1;
      } else {
        printf("no W25Q128FV %x %x %x\n", (ret & 0xff0000)>>16, (ret & 0xff00) >> 8 , (ret & 0xff));
        return 0;
      }
    }
  }
}


void spiflash_write_with_type_check(void *dst, long adr, long len) {
  if (check_spi_flash_type() == 1) {
    md380_spiflash_write(dst, adr, len);
    }
}

uint32_t spi_flash_addl_config_start = 0xf0000;
uint32_t spi_flash_addl_config_size  = 0xffff;
