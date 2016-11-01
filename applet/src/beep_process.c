/*! \file beep_process.c
  \brief beep_process Hook Functions.

  This module contains hooks and replacement functions for the beep_process.

*/

#define DEBUG

//#include <stdio.h>
#include <string.h>

#include "md380.h"
#include "printf.h"
#include "dmesg.h"
#include "addl_config.h"
#include "beep.h"
#include "debug.h"
#include "mbox.h"



// First experiments with beep_tone tunig
// why it's work, i have no idea ;)

void F_294_replacement(uint16_t value) {
#ifdef MD380_d13_020
  uint32_t multiplicand = 0x4a9;
#endif
#ifdef MD380_d02_032
  uint32_t multiplicand = 0x1dd;
#endif
#ifdef MD380_s13_020
  uint32_t multiplicand = 0x49a;
#endif

  if (global_addl_config.rbeep == 1) {
#ifdef MD380_d13_020
    multiplicand= 0x200;
#endif
#ifdef MD380_d02_032
    multiplicand= 0xaa;
#endif
#ifdef MD380_s13_020
    multiplicand= 0x200;
#endif
  }

 *beep_process_unkown=(uint32_t) value * multiplicand;
}

void bp_beep(uint8_t code)
{
    PRINT("bp_beep: %d\n", code);
}

void * beep_OSMboxPend_hook(OS_EVENT *pevent, uint32_t timeout, int8_t *perr)
{
    while(1) {
        void *ret = OSMboxPend_hook(pevent,timeout,perr);
        if( ret == 0 ) {
            return 0 ;
        }
        uint8_t beep = *(uint8_t*)ret ;
        PRINT("beep: %d\n", beep);
        switch( beep ) {
            case BEEP_TEST_1 :
                bp_beep(0);
                break ;
            case BEEP_TEST_2 :
                bp_beep(1);
                break ;
            case BEEP_TEST_3 :
                bp_beep(2);
                break ;
            default:
                return ret ; 
        }
    }
}

static uint8_t beep_msg ; // it cannot live on the stack.

#if defined(FW_D13_020)
void bp_send_beep( uint8_t beep )
{
    beep_msg = beep ;
    md380_OSMboxPost(event2_mbox_poi_beep, &beep_msg);    
}
#else
void bp_send_beep( uint8_t beep )
{
    // dummy. no implementation.
}
#endif
