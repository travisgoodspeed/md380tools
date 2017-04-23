/*! \file beep_process.c
  \brief beep_process Hook Functions.

  This module contains hooks and replacement functions for the beep_process.

*/

#define DEBUG

#include <string.h>

#include "md380.h"
#include "printf.h"
#include "dmesg.h"
#include "addl_config.h"
#include "beep.h"
#include "debug.h"
#include "mbox.h"
#include "os.h"



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

 *bp_freq=(uint32_t) value * multiplicand;
}

#if defined(FW_D13_020)
# define CAN_BEEP 1
void bp_set_freq(uint16_t value);

void bp_sempost();
void bp_sempost2();
void bp_tone_on();
void bp_tone_off();

extern uint8_t bp_2001e8a7 ;

#else
# define CAN_BEEP 0
#define bp_set_freq(x) /*nop*/
#define bp_sempost()
#define bp_sempost2()
#define bp_tone_on()
#define bp_tone_off()

uint8_t bp_2001e8a7 ;

#endif

// unsused for now.
void beep9()
{
    bp_sempost();
    bp_2001e8a7 = 3 ;
    bp_set_freq(0x3ad);
    bp_tone_on();
    OSTimeDly(0x64);
    bp_set_freq(0x2b9);
    OSTimeDly(0x32);
    bp_tone_off();    
    bp_sempost2();
}

#if (CAN_BEEP) // only implement when called, to avoid 'blabla defined but never used' 
static void start()
{
    bp_sempost();
    bp_2001e8a7 = 3 ;
}

#define DITFREQ 0x3ad
#define DITLEN 0x32

static void dit()
{
    bp_set_freq(DITFREQ);
    bp_tone_on();
    OSTimeDly(DITLEN);
//    bp_tone_off();        
    
//    bp_set_freq(0x2b9);
//    OSTimeDly(0x32);
//    bp_tone_off();        
//
//    OSTimeDly(0x100);
}

static void dit_space()
{
    bp_tone_off();        
    OSTimeDly(DITLEN);
}

static void stop()
{
    bp_sempost2();
}
#endif // CAN_BEEP ?


void bp_beep(uint8_t code)
{
    PRINT("bp_beep: %d\n", code);
    
#if (CAN_BEEP) 
    start();
    
    for(int i=0;i<code;i++) {
        dit();
        dit_space();
    }
    
    stop();
#endif // can beep ?    
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
                bp_beep(1);
                break ;
            case BEEP_TEST_2 :
                bp_beep(2);
                break ;
            case BEEP_TEST_3 :
                bp_beep(3);
                break ;
            default:
                return ret ; 
        }
    }
}


#if defined(FW_D13_020) || defined(FW_S13_020)
static uint8_t beep_msg ; // it cannot live on the stack.

void bp_send_beep( uint8_t beep )
{
    beep_msg = beep ;
    md380_OSMboxPost((OS_EVENT*)event2_mbox_poi_beep, &beep_msg);
}

#else

void bp_send_beep( uint8_t beep )
{
    // find: event2_mbox_poi_beep symbol

    // dummy. no implementation for now.
}

#endif
