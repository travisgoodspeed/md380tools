/*! \file spiflash.c
  \brief spiflash Hook functions.
*/

//#define DEBUG

#include "spiflash.h"

#include <string.h>

#include "printf.h"
#include "dmesg.h"
#include "md380.h"
#include "version.h"
#include "config.h"
#include "printf.h"
#include "debug.h"
#include "codeplug.h"

void spiflash_read_hook(void *dst, long adr, long len)
{
#ifdef DEBUG    
    char *hint = "" ;
#ifdef FW_D13_020    
    if( dst == zone_name ) {
        hint = "zn" ;
    } else 
    if( dst == &contact ) {
        hint = "cont" ;
    } else 
    if( dst == &current_channel_info ) {
        hint = "cci" ;
    } else 
    if( dst == (void*)0x2001de78 ) {
        hint = "ci2" ;
    } else 
    if( dst == (void*)0x2001da7c ) {
        hint = "rxg" ;
    } else 
#endif        
    {
        hint = "?" ;
    }
    PRINTRET();    
    PRINT("0x%06x:%4d 0x%08x (%s)\n", adr, len, dst, hint);
#endif    
    md380_spiflash_read(dst, adr, len);
}

uint32_t get_spi_flash_type(uint8_t *data) {
#ifdef CONFIG_SPIFLASH
  md380_spiflash_enable();
  md380_spi_sendrecv(0x9f);
  data[0]=md380_spi_sendrecv(0x00);
  data[1]=md380_spi_sendrecv(0x00);
  data[2]=md380_spi_sendrecv(0x00);
  md380_spiflash_disable();
  return( (data[0]<<16) | (data[1]<<8) | (data[2]));
#else
  //Return zero if we have no SPI Flash support.
  return 0;
#endif
}


/* 
   So here's the deal:
   
   The SPI Flash is either 1MB or 16MB, but the legit firmware only
   uses the first megabyte and the codeplug always fits in the first
   256K.
   
   spi_flash_addl_config_start and spi_flash_addl_config_size are used
   to place the menu settings at the very end of that image, while
   check_spi_flash_type() is used to verify that the SPI Flash is a
   known modellarger than 1MB.

 */
uint32_t spi_flash_addl_config_start = 0xf0000;
uint32_t spi_flash_addl_config_size  = 0xffff;


int check_spi_flash_size(void) {
  static int size=0;
  if (size) {
    //If the size is known, we return it.
    printf("Size is known to be %08x\n",
	   size);
    return size;
  } else {
    uint32_t ret;
    uint8_t data[4];

    ret=get_spi_flash_type(data);
    if ( ret == 0xef4018  ) {  // Manufacturer and Device Identification for W25Q128FV
      size=0x1000000;//16MB
    } else if ( ret == 0x10dc01 ) { // 2. Manufacturer and Device Identification for W25Q128FV maybe
      size=0x1000000;//16MB
    } else if ( ret == 0xef4014 ) { // 1MB Flash in the VHF models.
      printf("Warning, just 1 MB of Flash.\n");
      size=0x0100000;//1MB
    } else {
      printf("Unknown Flash %02x %02x %02x\n", (ret & 0xff0000)>>16, (ret & 0xff00) >> 8 , (ret & 0xff));
    }
    return size;
  }
}

void spiflash_write_with_type_check(void *dst, long adr, long len)
{
    if( check_spi_flash_size() > adr ) {
#ifdef CONFIG_SPIFLASH
        md380_spiflash_write(dst, adr, len);
#endif
    } else {
        printf("Rejecting write to %x as past the end of SPI Flash.\n",
                adr);
    }
}
