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

//! Handle to the original (unhooked) upload handler.
int (*usb_upld_handle)(void*, char*, int, int)=0x0808d3d9;
//! This returns a USB packet to the host from the upload handler.
int (*usb_send_packet)(void*, char*, uint16_t)=0x080577af;


//Original handler for the DFU DNLD event.
int (*usb_dnld_handle)()=0x0808ccbf;//2.032
//Function pointer at this address calls DNLD.
int *dnld_tohook=(int*) 0x20000e9c;//2.032

//Pointer to the buffer that stores the bottom line of screen text.
char *botlinetext=(char*) 0x2001cee0;

//ROM copy of the welcome bitmap.
char *welcomebmp=0x080f9ca8;
