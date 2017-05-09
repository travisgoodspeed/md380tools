/**! \file md380.h
   \brief MD380 callback functions.
*/

#include <stddef.h>
#include <stdint.h>

#include "config.h"
#include "os.h"
#include "peripherals/stm32f4xx_rtc.h"


//Move these to their own module.
void strhex(char *, long);
void wstrhex(wchar_t *, long);




// md380 dmr

//! Function that handles checking a DMR contact.
void* dmr_call_end(void *pkt);
//! Function that handles a DMR call.
void* dmr_call_start(void *pkt);
//! Function that handles a data packet.  (Also used in SMS.)
void* dmr_handle_data(void *pkt, int len);
//! Function that handles an incoming SMS.
void* dmr_sms_arrive(void *pkt);

//! Called before unsquelching.
int dmr_before_squelch();


//! Determines the squelching of an incoming call.
extern char dmr_squelch_mode[];
//! Unknown function involved in squelching.
extern char* const dmr_squelch_firstthing[];


//Pointer to the buffer that stores the top and bottom line of the boot screen text.
extern wchar_t toplinetext[10]; // 0-term
extern wchar_t botlinetext[10]; // 0-term

//ROM copy of the welcome bitmap.
extern char welcomebmp[];

// md380 usb

//! Handle to the original (unhooked) upload handler.
int usb_upld_handle(void*, char*, int, int);
//! This returns a USB packet to the host from the upload handler.
int usb_send_packet(void*, char*, uint16_t);

int usb_dnld_handle();


// This is the target address of the Application's DFU engine.
extern char *   md380_dfutargetadr;
extern char *   md380_dfu_target_adr[]; // same as md380_dfutargetadr TODO: fix
extern char     md380_packet[];
extern int      md380_packetlen[];
extern int      md380_blockadr[];
extern char     md380_dfu_state[];
extern char     md380_thingy2[];
extern char     md380_usbstring[];

// title for menus, version in info screen
extern wchar_t  print_buffer[];

uint8_t md380_spi_sendrecv(INT8U data); // SPI1

// md380_i2c // stolen from ../../lib/src/peripherals/stm32f4xx_i2c.c
void    md380_I2C_GenerateSTART(I2C_TypeDef* I2Cx, FunctionalState NewState);
void    md380_I2C_GenerateSTOP(I2C_TypeDef* I2Cx, FunctionalState NewState);
uint8_t md380_I2C_ReceiveData(I2C_TypeDef* I2Cx);
void    md380_I2C_Send7bitAddress(I2C_TypeDef* I2Cx, uint8_t Address, uint8_t I2C_Direction);
void    md380_I2C_SendData(I2C_TypeDef* I2Cx, uint8_t Data);


//! Function that handles uC/OS-II settings
//Task
INT8U       OSTaskCreateExt(void (*task)(void *pd), void *pdata, OS_STK *ptos, INT8U prio, INT16U id, OS_STK *pbos, INT32U stk_size, void *pext, INT16U opt);
void*       OSTaskNameSet(INT8U prio, INT8U *pname, INT8U *perr);

//Semaphore
OS_EVENT *  OSSemCreate(uint16_t);
void        OSSemPend(OS_EVENT *pevent, uint32_t timeout,  int8_t *perr);
uint8_t     OSSemPost(OS_EVENT *pevent);

//Mbox
uint8_t     md380_OSMboxPost(OS_EVENT *pevent, void *pmsg);
void *      md380_OSMboxPend(OS_EVENT *pevent, uint32_t timeout, int8_t *perr);

//! Halts all threads.
int         OS_ENTER_CRITICAL();
//! Resumes threads.
void        OS_EXIT_CRITICAL(int);

//! Functions and Variabes regarding the menu
void*   main_menu(void *);     // menu exec

void*   md380_menu_entry_back(void);
void    md380_create_main_menu_entry(void); // doesn't return anything (no 'void*')

// create one new menu entry
// menu_id (count from mainmenu 0), wt_menu_text, *()green key, *() red key, ?, ?, enabled
void  md380_create_menu_entry(int, const wchar_t *, void (*green_key)(), void  (*red_key)(), int , int, int);

// for the hook funktion (hook used the space from this entry)
void*  md380_menu_entry_programradio(void);
void*  md380_menu_numerical_input(void);

uint8_t   md380_menu_0x200011e4;
uint8_t   md380_menu_0x2001d3c1;
uint8_t   md380_menu_0x2001d3ed;
uint8_t   md380_menu_0x2001d3ee;
uint8_t   md380_menu_0x2001d3ef;
uint8_t   md380_menu_0x2001d3f0;
uint8_t   md380_menu_0x2001d3f1;
uint8_t   md380_menu_0x2001d3f4;
uint8_t   md380_menu_depth;
uint8_t   md380_menu_entry_selected;
uint8_t   md380_menu_id;

extern wchar_t          md380_wt_programradio[];  // menutext <- menu_entry_programradio

//! program_radio_unprohibited (menu entry) ... bulding site is an struct
extern uint8_t md380_program_radio_unprohibited[];

//! This points to the byte of the current channel.
extern char  channelnum[];

//! Reads the current channel number from the rotary switch.
int read_channel_switch();

//! Reads a register from the C5000.
void c5000_spi0_readreg(int reg, char *buf);

//! Writes a register in the C5000.
void c5000_spi0_writereg(int reg, int val);


// md380 aes
int * aes_startup_check(void);
char * aes_loadkey(char *);

//! Unknown AES function.
char* aes_cipher(char *pkt);

// md380 ambe2+
//! Unknown AMBE2+ thing.
int ambe_encode_thing(char *a1, int a2, int *a3, int a4,
                      short a5, short a6, short a7, int a8);
//! Decodes an AMBE2+ frame into bits.
int ambe_unpack(int a1, int a2, char length, int a4);

//! Populates the audio buffer.
int ambe_decode_wav(int *a1, signed int eighty, char *bitbuffer,
                    int a4, short a5, short a6, int a7);


void md380_Write_Command_2display(uint8_t data);
void md380_Write_Data_2display(uint8_t data);

void md380_GPIO_SetBits(int addr, uint16_t GPIO_Pin); 
void md380_GPIO_ResetBits(int addr, uint16_t GPIO_Pin);



//! Functions and Variabes regarding the beep_
// not yet known ;)
extern uint32_t  bp_freq[];

//! useful firmware functions
wchar_t * md380_itow(wchar_t *, int value);
void      md380_RTC_GetDate(uint32_t RTC_Format, RTC_DateTypeDef *RTC_DateStruct);
void      md380_RTC_GetTime(uint32_t RTC_Format, RTC_TimeTypeDef *RTC_TimeStruct);

// stuff to handle different display (flip (380/390) type
extern uint8_t  const md380_radio_config_bank2[]; // from spiflash Security Registers
                                                  // tunig parameter
void md380_copy_spiflash_security_bank2_to_ram(void);

// rtc_timer process stuff ( user interface task)
// menu no exit ....
uint8_t gui_opmode1;


// debug and training stuff
void md380_f_4137();
void md380_f_4520();
void md380_f_4098();
void md380_f_4102();
void f_4225();

// major display driver for popup during RX/TX
void F_4315();

