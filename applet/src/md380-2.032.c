/*! \file md380-2.032.c
  \brief MD380 callback definitions for Version 2.032.
*/



#include "md380.h"
#include "os.h"

//So we don't get warnings about integer pointers, for just this header.
#pragma GCC diagnostic ignored "-Wint-conversion"

//Firmware calls to 2.032.

//! Functions that handles spi flash .. handles semaphore internal
int (*md380_spiflash_read)(void *dst, long adr, long len) = 0x0802fd83;
void (*md380_spiflash_write)(void *dst, long adr, long len) =0x0802fe6b;

int (*md380_spiflash_security_registers_read)(void *dst, long adr, long len) = 0x080301bd;

void (*md380_spiflash_enable)() = 0x0802fe37;
void (*md380_spiflash_disable)() = 0x0802fe53;
void (*md380_spiflash_wait)()=0x0802fe15;

void (*md380_spiflash_block_erase64k)(uint32_t adr)=0x0802fbb7;
void (*md380_spiflash_sektor_erase4k)(uint32_t adr)=0x0802fb83;

INT8U (*md380_spi_sendrecv)(INT8U data) = 0x0802fdc9; // SPI1




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

void (*gfx_chars_to_display)(wchar_t *str, int x, int y, int unknown) = 0x0801cf1d;

void (*gfx_select_font)(void *p)=0x8020975;
void (*gfx_set_bg_color)(int color)=0x801c5e1;
void (*gfx_set_fg_color)(int color)=0x801c5e9;

void (*gfx_blockfill)(int xmin, int ymin, int xmax, int ymax)=0x0801cb05;

//! Function that handles the end of a DMR call.
void* (*dmr_call_end)(void *pkt) = 0x0803f33d;
//! Function that handles a DMR call.
void* (*dmr_call_start)(void *pkt) = 0x0803ec87;
//! Function that handles a DMR SMS.
void* (*dmr_handle_data)(void *pkt, int len) = 0x0804b66d;
//! Function that handles an incoming SMS.
void* (*dmr_sms_arrive)(void *pkt)=0x0803f03d;
//! Function that applies a squelch.
void (*dmr_apply_squelch)(char *dmr_squelch_firsthing, int dmr_squelch_mode)=0x080303b5;
//! Called before unsquelching.
int (*dmr_before_squelch)()=0x0803ef6d;

//! Determines the squelching of an incoming call.
char* dmr_squelch_mode = 0x2001d35f;
//! Unknown function involved in squelching.
char** dmr_squelch_firstthing = 0x2001d118;



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

int (*OS_ENTER_CRITICAL)()    = 0x08041df9;
void (*OS_EXIT_CRITICAL)(int) = 0x08041e01;

//! OSSem Create Hook used the place from 2 OSSemCreate Calls
OS_EVENT *(*OSSemCreate)(uint16_t) = 0x803da2c+1;
OS_EVENT  ** OSSemCreate_hook0_event_mem=0x2001d124;
OS_EVENT  ** OSSemCreate_hook1_event_mem=0x2001d128;

void (*OSSemPend)(OS_EVENT *pevent, uint32_t timeout,  uint8_t *perr)=0x803da78+1;
uint8_t (*OSSemPost)(OS_EVENT *pevent)=0x803db68+1;

uint8_t (*md380_OSMboxPost)(OS_EVENT *pevent, void *pmsg)=0x080303b4+1;

//! Functions and Variabes regarding the menu
void     *(*main_menu)(void *)=0x08039c23;
void     *(*md380_create_main_meny_entry)(void)=0x0800c189;
void     *(*md380_menu_numerical_input)(void) = 0x801a2d6;
void     *(*md380_create_menu_entry)(int a, void *b , void *c, void  *d, int e, int f ,int g)=0x0800c731;
void     *(*md380_menu_entry_back)(void)=0x800f452;
void     *(*md380_menu_entry_programradio)(void)=0x80127d0;

uint32_t *md380_menu_0x20001114 = 0x20001114;
uint8_t  *md380_menu_0x200011e4 = 0x200011e4;
uint8_t  *md380_menu_0x2001d3c1 = 0x2001d3c1;
uint8_t  *md380_menu_0x2001d3ed = 0x2001d3ed;
uint8_t  *md380_menu_0x2001d3ee = 0x2001d3ee;
uint8_t  *md380_menu_0x2001d3ef = 0x2001d3ef;
uint8_t  *md380_menu_0x2001d3f0 = 0x2001d3f0;
uint8_t  *md380_menu_0x2001d3f1 = 0x2001d3f1;
uint8_t  *md380_menu_0x2001d3f4 = 0x2001d3f4;
uint8_t  *md380_menu_depth = 0x200011e4;
wchar_t  *md380_menu_edit_buf = 0x2001b716;
uint8_t  *md380_menu_entry_selected = 0x2001d3b2;
uint8_t  *md380_menu_id = 0x2001d3c2;
void     *md380_menu_mem_base = 0x20019df0;
void     *md380_menu_memory = 0x2001c148;
uint8_t  *md380_program_radio_unprohibited = (uint8_t *)(0x2001d030 + 4);
uint8_t  *md380_wt_programradio = 0x080d175c;


char* channelnum=0x2001d376;
int (*read_channel_switch)()=0x0804d269;




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


//! Functions and Variabes regarding the beep_process
uint32_t *beep_process_unkown=0x2001d178;


//! useful md380 firmware functions
void (*md380_itow)(wchar_t *, int value)=0x080172ed;
void (*md380_RTC_GetDate)(uint32_t RTC_Format, RTC_DateTypeDef *RTC_DateStruct)=0x08026461;
void (*md380_RTC_GetTime)(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct)=0x0802634b;

uint32_t *md380_dmr_id=0x2001c65c;

//! Functions and Variabes from the "RTC Timer" - task

void (*md380_f_4137)()=0x080290c8+1;
void (*md380_f_4520)()=0x08027ae8+1;
void (*md380_f_4098)()=0x0804c1d0+1;
void (*md380_f_4102)()=0x0804c2aa+1;

uint8_t *md380_f_4225_operatingmode=0x2001d3f7;
uint8_t md380_f_4225_operatingmode_menu=0x1b;       // see 0x0801f06a there are a lot of modes
uint8_t md380_f_4225_operatingmode_menu_exit=0x1c;
