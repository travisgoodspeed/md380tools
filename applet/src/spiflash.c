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



int (spiflash_read_hook)(void *dst, long adr, long len) {
  printf("%x %x %d\n", dst, adr, len);
  return spiflash_read(dst, adr, len);
}


