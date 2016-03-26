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


int* (*aes_cipher)(int *pkt)=0x080356b1;
