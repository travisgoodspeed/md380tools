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

 *beep_process_unkown=(uint32_t) value * multiplicand;
}

#if defined(FW_D13_020)
void F_294(uint16_t value);
#else
#define F_294(x) /*nop*/
#endif

#if 0
|      ||   0x0802fc1e      fff7b9fe       bl 0x802f994                ;[1] ; beep 9    
        
|      ||   0x0802fc22      dff8900d       ldr.w r0, [pc, 0xd90]       ; [0x80309b4:4]=0x2001e8a7                                                                              
|      ||   0x0802fc26      0321           movs r1, 3                  ; 3                                                                                                     
|      ||   0x0802fc28      0170           strb r1, [r0]                                                                                                                       

|      ||   0x0802fc2a      40f2ad30       movw r0, 0x3ad              ; 941                                                                                                   
|      ||   0x0802fc2e      00f053ff       bl F_294                    ;[2]   
        
|      ||   0x0802fc32      00f069ff       bl 0x8030b08                ;[3]   
        
|      ||   0x0802fc36      6420           movs r0, 0x64               ; 'd' ; 100                                                                                             
|      ||   0x0802fc38      04f03cf9       bl OSTimeDly                ;[4]   
        
|      ||   0x0802fc3c      40f2b920       movw r0, 0x2b9              ; 697                                                                                                   
|      ||   0x0802fc40      00f04aff       bl F_294                    ;[2] 
        
|      ||   0x0802fc44      3220           movs r0, 0x32               ; '2' ; 50                                                                                              
|      ||   0x0802fc46      04f035f9       bl OSTimeDly                ;[4]   
        
|      ||   0x0802fc4a      00f085ff       bl 0x8030b58                ;[5]  
        
|      ||   0x0802fc4e      fff7b3fe       bl 0x802f9b8                ;[6]                                                                                                    
|      ||   0x0802fc52      0024           movs r4, 0                                                                                                                          
|      ||   0x0802fc54      0125           movs r5, 1                  ; 1                                                                                                     
#endif


static void start()
{
//#if 0    
//|       |   0x0802fd54      4021           movs r1, 0x40               ; '@' ; 64 ; beginn dmr sync                                                                            
//|       |   0x0802fd56      0e20           movs r0, 0xe                ; 14                                                                                                    
//|       |   0x0802fd58      10f014f9       bl c5000_spi0_writereg      ;[1]                                                                                                    
//#endif
//    c5000_spi0_writereg( 0xe, 0x40 );
//#if 0    
//|       |   0x0802fd5c      fff71afe       bl 0x802f994                ;[2]                                                                                                    
//|       |   0x0802fd60      dff8500c       ldr.w r0, [pc, 0xc50]       ; [0x80309b4:4]=0x2001e8a7                                                                              
//|       |   0x0802fd64      0321           movs r1, 3                  ; 3                                                                                                     
//|       |   0x0802fd66      0170           strb r1, [r0]                                                            
//#endif
}

static void stop()
{
//#if 0    
//|       |   0x0802fdb6      4421           movs r1, 0x44               ; 'D' ; 68                                                                                              
//|       |   0x0802fdb8      0e20           movs r0, 0xe                ; 14                                                                                                    
//|       |   0x0802fdba      10f0e3f8       bl c5000_spi0_writereg      ;[1]                   
//#endif
//    c5000_spi0_writereg( 0xe, 0x44 );
}

static void tone()
{
    F_294(0x64e);
#if 0    
|       |   0x0802fd88      40f24e60       movw r0, 0x64e              ; 1614                                                                                                  
|       |   0x0802fd8c      00f0a4fe       bl F_294                    ;[3]                               
#endif
}

static void silence()
{
}

static void duration(int len)
{
    int time = len * 0x28 ;
    OSTimeDly(time);
}

static void dit_space()
{
    duration(1);
}

static void letter_space()
{
    duration(3);
}

static void word_space()
{
    duration(9);
}

void bp_beep(uint8_t code)
{
    PRINT("bp_beep: %d\n", code);
    
    start();
    
    for(int i=0;i<code;i++) {
        tone();
        dit_space();
        silence();
        dit_space();
    }
    
    stop();
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
    // find: event2_mbox_poi_beep symbol

    // dummy. no implementation.
}
#endif
