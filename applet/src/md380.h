/**! \file md380.h
   \brief MD380 callback functions.
*/

#include <stdio.h>


//Move these to their own module.
void strhex(char *, long);
void wstrhex(wchar_t *, long);

extern int (*spiflash_read)(void *dst, long adr, long len);
extern void (*gfx_drawtext)(wchar_t *str,          //16-bit, little endian.
				  short sx, short sy, //Source coords, maybe?
				  short x, short y,   //X and Y position
				  int maxlen);
extern void (*gfx_drawbmp)(char *bmp,
				 int idx,
				 uint64_t pos);

//Function that handles checking a DMR contact.
extern void* (*dmr_call_end)(void *pkt);
//Function that handles a DMR call.
extern void* (*dmr_call_start)(void *pkt);
//Function that handles a DMR SMS.
extern void* (*dmr_handle_data)(void *pkt, int len);


//Pointer to the buffer that stores the bottom line of screen text.
char *botlinetext;


//ROM copy of the welcome bitmap.
extern char *welcomebmp;

//! Handle to the original (unhooked) upload handler.
int (*usb_upld_handle)(void*, char*, int, int);
//! This returns a USB packet to the host from the upload handler.
int (*usb_send_packet)(void*, char*, uint16_t);

int (*usb_dnld_handle)();
int *dnld_tohook;
