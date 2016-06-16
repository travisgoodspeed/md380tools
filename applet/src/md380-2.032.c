/*! \file md380-2.032.c
  \brief MD380 callback definitions for Version 2.032.
*/

#include "md380.h"
#include "os.h"

//So we don't get warnings about integer pointers, for just this header.
#pragma GCC diagnostic ignored "-Wint-conversion"

//Firmware calls to 2.032.


//! Determines the squelching of an incoming call.
char            * const dmr_squelch_mode = 0x2001d35f;
//! Unknown function involved in squelching.
char           ** const dmr_squelch_firstthing = 0x2001d118;

//Function pointer at this address calls DNLD.
int             * const dnld_tohook=(int*) 0x20000e9c;//2.032

//Pointer to the buffer that stores the bottom line of screen text.
char            * const botlinetext=(char*) 0x2001cee0;

//ROM copy of the welcome bitmap.
char            * const welcomebmp=0x080f9ca8;

//! Variabes regarding uC/OS-II
OS_EVENT       ** const OSSemCreate_hook0_event_mem=0x2001d124;
OS_EVENT       ** const OSSemCreate_hook1_event_mem=0x2001d128;

//! Variabes regarding menu.c
void     	* const md380_menu_mem_base = 0x20019df0;
void     	* const md380_menu_memory = 0x2001c148;
wchar_t  	* const md380_menu_edit_buf = 0x2001b716;

char            * const channelnum = 0x2001d376;

//! Variabes regarding the beep_process
uint32_t        * const beep_process_unkown=0x2001d178;

//! Variabes regarding rtc_timer process
uint8_t         * const md380_f_4225_operatingmode=0x2001d3f7;
const uint8_t md380_f_4225_operatingmode_menu=0x1b;       // see 0x0801f06a there are a lot of modes
const uint8_t md380_f_4225_operatingmode_menu_exit=0x1c;
