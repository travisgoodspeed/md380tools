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
  return spiflash_read(dst, adr, len);
}

int check_spf_flash_type(void) {
    static int ok=0;

    if (ok==1) {
      return 1;
    } else {
      char data[3];
      
      spiflash_enable();
      spi_sendrecv(0x9f);
      data[0]=spi_sendrecv(0x00);
      data[1]=spi_sendrecv(0x00);
      data[2]=spi_sendrecv(0x00);
      spiflash_disable();
      if (data[0]==0xef && data[1]==0x40 && data[2]==0x18) {
        ok=1;
        return 1;
      } else {
        printf("no W25Q128FV %x %x %x\n", data[0],data[1],data[2]);
        return 0;
      }
    }
}

uint32_t spi_flash_addl_config_start = 0xf0000;
uint32_t spi_flash_addl_config_size  = 0xffff;
