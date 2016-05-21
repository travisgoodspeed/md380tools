/*! \file md380-2.032.c
  \brief MD380 callback definitions for Version 2.032.
*/



#include "md380.h"
#include "os.h"

//So we don't get warnings about integer pointers, for just this header.
#pragma GCC diagnostic ignored "-Wint-conversion"

//Firmware calls to 2.032.


//! Determines the squelching of an incoming call.
char* const dmr_squelch_mode = 0x2001d35f;
//! Unknown function involved in squelching.
char** const dmr_squelch_firstthing = 0x2001d118;


//Function pointer at this address calls DNLD.
int * const dnld_tohook=(int*) 0x20000e9c;//2.032

//Pointer to the buffer that stores the bottom line of screen text.
char * const botlinetext=(char*) 0x2001cee0;

//ROM copy of the welcome bitmap.
char * const welcomebmp=0x080f9ca8;

//! Function that handles uC/OS-II settings
OS_EVENT  ** const OSSemCreate_hook0_event_mem=0x2001d124;
OS_EVENT  ** const OSSemCreate_hook1_event_mem=0x2001d128;


//! Functions and Variabes regarding the menu

uint32_t 	* const md380_menu_0x20001114 = 0x20001114;
uint8_t  	* const md380_menu_0x200011e4 = 0x200011e4;
uint8_t  	* const md380_menu_0x2001d3c1 = 0x2001d3c1;
uint8_t  	* const md380_menu_0x2001d3ed = 0x2001d3ed;
uint8_t  	* const md380_menu_0x2001d3ee = 0x2001d3ee;
uint8_t  	* const md380_menu_0x2001d3ef = 0x2001d3ef;
uint8_t  	* const md380_menu_0x2001d3f0 = 0x2001d3f0;
uint8_t  	* const md380_menu_0x2001d3f1 = 0x2001d3f1;
uint8_t  	* const md380_menu_0x2001d3f4 = 0x2001d3f4;
uint8_t  	* const md380_menu_depth = 0x200011e4;
wchar_t  	* const md380_menu_edit_buf = 0x2001b716;
uint8_t  	* const md380_menu_entry_selected = 0x2001d3b2;
uint8_t  	* const md380_menu_id = 0x2001d3c2;
void     	* const md380_menu_mem_base = 0x20019df0;
void     	* const md380_menu_memory = 0x2001c148;
uint8_t  	* const md380_program_radio_unprohibited = (uint8_t *)(0x2001d030 + 4);
const wchar_t	* const md380_wt_programradio = 0x080d175c;


char * const channelnum=0x2001d376;


//! Functions and Variabes regarding the beep_process
uint32_t * const beep_process_unkown=0x2001d178;

//uint32_t * const md380_dmr_id=0x2001c65c;


uint8_t * const md380_f_4225_operatingmode=0x2001d3f7;
const uint8_t md380_f_4225_operatingmode_menu=0x1b;       // see 0x0801f06a there are a lot of modes
const uint8_t md380_f_4225_operatingmode_menu_exit=0x1c;
