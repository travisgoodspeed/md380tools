/*! \file md380-2.032.c
  \brief MD380 callback definitions for Version 2.032.
*/



#include "md380.h"

//So we don't get warnings about integer pointers, for just this header.
#pragma GCC diagnostic ignored "-Wint-conversion"

//Firmware calls to 2.032.
int (*spiflash_read)(void *dst, long adr, long len) = 0x0802fd83;
void (*gfx_drawtext)(wchar_t *str,    //16-bit, little endian.
		     short sx, short sy, //Source coords, maybe?
		     short x, short y,   //X and Y position
		     int maxlen) = 0x0800D88B;
void (*gfx_drawbmp)(char *bmp,
			  int idx,
			  uint64_t pos) = 0x08022887;

char *welcomebmp=0x080f9ca8;
