/**! \file md380.h
   \brief MD380 callback functions.
*/

#include <stdio.h>
#include <stdint.h>

#include "os.h"
#include "peripherals/stm32f4xx_rtc.h"


//Move these to their own module.
void strhex(char *, long);
void wstrhex(wchar_t *, long);

extern int (*spiflash_read)(void *dst, long adr, long len);
extern void (*spiflash_write)(void *dst, long adr, long len);

extern int (*spiflash_security_registers_read)(void *dst, long adr, long len);

extern void (*spiflash_enable)();
extern void (*spiflash_disable)();   
extern void (*spiflash_wait)();   

void (*spiflash_block_erase64k)(uint32_t);
void (*spiflash_sektor_erase4k)(uint32_t);


extern INT8U (*spi_sendrecv)(INT8U data); // SPI1              



extern void (*gfx_drawtext)(wchar_t *str,          //16-bit, little endian.
			    short sx, short sy, //Source coords, maybe?
			    short x, short y,   //X and Y position
			    int maxlen);
extern void (*gfx_drawbmp)(char *bmp,
			   int idx,
			   uint64_t pos);

void (*gfx_drawtext2)(wchar_t *str,    //16-bit, little endian.
                      int x, int y,   //X and Y position
                      int maxlen);


void (*gfx_select_font)(void *p);

void (*gfx_set_bg_color)(int color);
void (*gfx_set_fg_color)(int color);

                                          

//! Function that handles checking a DMR contact.
extern void* (*dmr_call_end)(void *pkt);
//! Function that handles a DMR call.
extern void* (*dmr_call_start)(void *pkt);
//! Function that handles a data packet.  (Also used in SMS.)
extern void* (*dmr_handle_data)(void *pkt, int len);
//! Function that handles an incoming SMS.
extern void* (*dmr_sms_arrive)(void *pkt);


extern void (*MD380_RTC_GetTime)(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct);


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

//! Function that handles uC/OS-II settings
extern INT8U (*OSTaskCreateExt)(void (*task)(void *pd), void *pdata, OS_STK *ptos, INT8U prio, INT16U id, OS_STK *pbos, INT32U stk_size, void *pext, INT16U opt);
extern void* (*OSTaskNameSet)(INT8U prio, INT8U *pname, INT8U *perr);


//! Still figuring this out; it is involved in rendering menus.
extern void* (*main_menu)(void *);


//! This points to the byte of the current channel.
extern char* channelnum;

//! Reads the current channel number from the rotary switch.
extern int (*read_channel_switch)();


//! Halts all threads.
extern int (*OS_ENTER_CRITICAL)();
//! Resumes threads.
extern void (*OS_EXIT_CRITICAL)(int);



//! Reads a register from the C5000.
extern void (*c5000_spi0_readreg)(int reg, char *buf);

//! Writes a register in the C5000.
extern void (*c5000_spi0_writereg)(int reg, int val);


//! Unknown AES function.
extern char* (*aes_cipher)(char *pkt);

//! Unknown AMBE2+ thing.
extern int (*ambe_encode_thing)(char *a1, int a2, int *a3, int a4,
				short a5, short a6, short a7, int a8);
//! Decodes an AMBE2+ frame into bits.
extern int (*ambe_unpack)(int a1, int a2, char length, int a4);

//! Populates the audio buffer.
extern int (*ambe_decode_wav)(int *a1, signed int eighty, char *bitbuffer,
			      int a4, short a5, short a6, int a7);
