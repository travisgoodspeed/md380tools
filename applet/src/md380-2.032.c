/*! \file md380-2.032.c
  \brief MD380 callback definitions for Version 2.032.
*/



#include "md380.h"


//So we don't get warnings about integer pointers, for just this header.
#pragma GCC diagnostic ignored "-Wint-conversion"

//Firmware calls to 2.032.

//! Functions that handles spi flash .. handles semaphore internal
int (*spiflash_read)(void *dst, long adr, long len) = 0x0802fd83;
void (*spiflash_write)(void *dst, long adr, long len) =0x0802fe6b;

int (*spiflash_security_registers_read)(void *dst, long adr, long len) = 0x080301bd;

void (*spiflash_enable)() = 0x0802fe37;
void (*spiflash_disable)() = 0x0802fe53;
void (*spiflash_wait)()=0x0802fe15;

void (*spiflash_block_erase64k)(uint32_t adr)=0x0802fbb7;
void (*spiflash_sektor_erase4k)(uint32_t adr)=0x0802fb83;

INT8U (*spi_sendrecv)(INT8U data) = 0x0802fdc9; // SPI1 




void (*gfx_drawtext)(wchar_t *str,    //16-bit, little endian.
		     short sx, short sy, //Source coords, maybe?
		     short x, short y,   //X and Y position
		     int maxlen) = 0x0800D88B;
void (*gfx_drawbmp)(char *bmp,
		    int idx,
		    uint64_t pos) = 0x08022887;

void (*gfx_drawtext2)(wchar_t *str,    //16-bit, little endian.
                     int x, int y,   //X and Y position, Unit unknown
                     int unknown) = 0x0801cf1d; // max 19 char ???

void (*gfx_select_font)(void *p)=0x8020975;
void (*gfx_set_bg_color)(int color)=0x801c5e1;
void (*gfx_set_fg_color)(int color)=0x801c5e9;


//! Function that handles the end of a DMR call.
void* (*dmr_call_end)(void *pkt) = 0x0803f33d;
//! Function that handles a DMR call.
void* (*dmr_call_start)(void *pkt) = 0x0803ec87;
//! Function that handles a DMR SMS.
void* (*dmr_handle_data)(void *pkt, int len) = 0x0804b66d;
//! Function that handles an incoming SMS.
void* (*dmr_sms_arrive)(void *pkt)=0x0803f03d;

void (*MD380_RTC_GetTime)(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct)=0x0802634b;


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

//! Function that handles uC/OS-II settings

INT8U (*OSTaskCreateExt)(void (*task)(void *pd), void *pdata, OS_STK *ptos, INT8U prio, INT16U id, OS_STK *pbos, INT32U stk_size, void *pext, INT16U opt)=0x804bbf5;
void* (*OSTaskNameSet)(INT8U prio, INT8U *pname, INT8U *perr)=0x804bcc1;

void* (*main_menu)(void *)=0x08039c23;

char* channelnum=0x2001d376;
int (*read_channel_switch)()=0x0804d269;


int (*OS_ENTER_CRITICAL)()    = 0x08041df9;
void (*OS_EXIT_CRITICAL)(int) = 0x08041e01;


void (*c5000_spi0_readreg)(int reg, char*buf)=0x0803e2f5;
void (*c5000_spi0_writereg)(int reg, int val)=0x0803e2a9;


char* (*aes_cipher)(char *pkt)=0x080356b1;

//! Unknown AMBE2+ thing.
int (*ambe_encode_thing)(char *a1, int a2, int *a3, int a4,
			 short a5, short a6, short a7, int a8)=0x08050d91;
//! Decodes an AMBE2+ frame into bits.
int (*ambe_unpack)(int a1, int a2, char length, int a4)=0x08048c9d;

//! Populates the audio buffer.
int (*ambe_decode_wav)(int *a1, signed int eighty, char *bitbuffer,
		       int a4, short a5, short a6, int a7)=0x08051249;
