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

extern int  (*md380_spiflash_read)(void *dst, long adr, long len);
extern void (*md380_spiflash_write)(void *dst, long adr, long len);
extern int  (*md380_spiflash_security_registers_read)(void *dst, long adr, long len);
extern void (*md380_spiflash_block_erase64k)(uint32_t);
extern void (*md380_spiflash_sektor_erase4k)(uint32_t);

extern void (*md380_spiflash_enable)();
extern void (*md380_spiflash_disable)();
extern void (*md380_spiflash_wait)();


extern INT8U (*md380_spi_sendrecv)(INT8U data); // SPI1



extern void (*gfx_drawtext)(wchar_t *str,          //16-bit, little endian.
			    short sx, short sy, //Source coords, maybe?
			    short x, short y,   //X and Y position
			    int maxlen);
extern void (*gfx_drawbmp)(char *bmp,
			   int idx,
			   uint64_t pos);

extern void (*gfx_drawtext2)(wchar_t *str,    //16-bit, little endian.
                      int x, int y,   //X and Y position
                      int maxlen);

extern void (*gfx_chars_to_display)(wchar_t *str, int x, int y, int unknown);


extern void (*gfx_select_font)(void *p);

extern void (*gfx_set_bg_color)(int color);
extern void (*gfx_set_fg_color)(int color);

extern void (*gfx_blockfill)(int xmin, int ymin, int xmax, int ymax);

//! Function that handles checking a DMR contact.
extern void* (*dmr_call_end)(void *pkt);
//! Function that handles a DMR call.
extern void* (*dmr_call_start)(void *pkt);
//! Function that handles a data packet.  (Also used in SMS.)
extern void* (*dmr_handle_data)(void *pkt, int len);
//! Function that handles an incoming SMS.
extern void* (*dmr_sms_arrive)(void *pkt);

//! Function that applies a squelch.
extern void (*dmr_apply_squelch)(char *dmr_squelch_firsthing, int dmr_squelch_mode);
//! Called before unsquelching.
extern int (*dmr_before_squelch)();


//! Determines the squelching of an incoming call.
extern char* dmr_squelch_mode;
//! Unknown function involved in squelching.
extern char** dmr_squelch_firstthing;



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

extern OS_EVENT *(*OSSemCreate)(uint16_t);
extern void (*OSSemPend)(OS_EVENT *pevent, uint32_t timeout,  uint8_t *perr);
extern uint8_t (*OSSemPost)(OS_EVENT *pevent);

extern uint8_t (*md380_OSMboxPost)(OS_EVENT *pevent, void *pmsg);


//! Functions and Variabes regarding the menu
extern void*   (*main_menu)(void *);     // menu exec

extern void*   (*md380_menu_entry_back)(void);
extern void*   (*md380_create_main_meny_entry)(void);

// create one new menu entry
// menu_id (count from mainmenu 0), wt_menu_text, *()green key, *() red key, ?, ?, enabled
extern void*    (*md380_create_menu_entry)(int, void *, void *, void  *,
                                           int , int, int);

// for the hook funktion (hook used the space from this entry)
extern void*    (*md380_menu_entry_programradio)(void);
extern void*    (*md380_menu_numerical_input)(void);

extern uint32_t *md380_menu_0x20001114;
extern uint8_t  *md380_menu_0x200011e4;
extern uint8_t  *md380_menu_0x2001d3c1;
extern uint8_t  *md380_menu_0x2001d3ed;
extern uint8_t  *md380_menu_0x2001d3ee;
extern uint8_t  *md380_menu_0x2001d3ef;
extern uint8_t  *md380_menu_0x2001d3f0;
extern uint8_t  *md380_menu_0x2001d3f1;
extern uint8_t  *md380_menu_0x2001d3f4;
extern uint8_t  *md380_menu_depth;
extern wchar_t  *md380_menu_edit_buf;
extern uint8_t  *md380_menu_entry_selected;
extern uint8_t  *md380_menu_id;
extern void     *md380_menu_mem_base;
extern void     *md380_menu_memory;
extern uint8_t  *md380_wt_programradio;  // menutext <- menu_entry_programradio


//! program_radio_unprohibited ... bulding site is an struct
extern uint8_t *md380_program_radio_unprohibited;

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


//! Functions and Variabes regarding the beep_
// not yet known ;)
extern uint32_t *beep_process_unkown;

//! useful firmware functions
extern void  (*md380_itow)(wchar_t *, int value);
extern void  (*md380_RTC_GetDate)(uint32_t RTC_Format, RTC_DateTypeDef *RTC_DateStruct);
extern void  (*md380_RTC_GetTime)(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct);

extern uint32_t *md380_dmr_id;

extern void (*md380_f_4137)();
extern void (*md380_f_4520)();
extern void (*md380_f_4098)();
extern void (*md380_f_4102)();

extern uint8_t *md380_f_4225_operatingmode;
extern uint8_t md380_f_4225_operatingmode_menu;
extern uint8_t md380_f_4225_operatingmode_menu_exit;