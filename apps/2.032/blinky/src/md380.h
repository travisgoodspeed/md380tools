/**! \file md380.h
   \brief MD380 callback functions.
*/

#include <stdio.h>

extern int (*spiflash_read)(void *dst, long adr, long len);
extern void (*gfx_drawtext)(wchar_t *str,          //16-bit, little endian.
				  short sx, short sy, //Source coords, maybe?
				  short x, short y,   //X and Y position
				  int maxlen);
extern void (*gfx_drawbmp)(char *bmp,
				 int idx,
				 uint64_t pos);

extern char *welcomebmp;

