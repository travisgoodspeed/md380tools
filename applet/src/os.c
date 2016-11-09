/*! \file os.c
  \brief os Hook functions.
  
*/

//#define DEBUG
#define NETMON

#include <string.h>

#include "printf.h"
#include "dmesg.h"
#include "md380.h"
#include "version.h"
#include "config.h"
#include "os.h"
#include "addl_config.h"
#include "debug.h"
#include "console.h"
#include "netmon.h"
#include "mbox.h"
#include "beep.h"

OS_EVENT* debug_line_sem;  // not yet used 


INT8U (OSTaskCreateExt_hook)(void (*task)(void *pd), void *pdata, OS_STK *ptos, INT8U prio, INT16U id, OS_STK *pbos, INT32U stk_size, void *pext, INT16U opt) {
  printf("%x %x %x %x %x %x %x %x %x\n", task, pdata, ptos, prio, id, pbos, stk_size, pext, opt);
  return  OSTaskCreateExt(task, pdata, ptos, prio, id, pbos, stk_size, pext, opt);
}


void *(OSTaskNameSet_hook)( INT8U prio, INT8U *name,  INT8U *err) {
  printf("%x %s %x\n", prio, name, *err);
  return OSTaskNameSet(prio, name, err);
}

OS_EVENT * OSSemCreate_hook(uint16_t cnt)
{
    OS_EVENT * sem;

    sem = OSSemCreate(cnt);

    debug_line_sem = OSSemCreate(1);
    if( debug_line_sem == NULL ) {
        printf("can't create debug_line_sem\n");
    }
//#ifdef DEBUG
//    printf("debug_line_sem %x\n", debug_line_sem);
//#endif
    return (sem);
}

//#if defined(FW_D13_020)
//OS_EVENT *mbox_radio = (OS_EVENT *)0x20017468 ;
//OS_EVENT *mbox_beep = (OS_EVENT *)0x20017390 ;
//#elif defined(FW_D02_032)
//OS_EVENT *mbox_radio = (OS_EVENT *)0x20017468 ; // wrong
//OS_EVENT *mbox_beep = (OS_EVENT *)0x20015f0c ;
//#warning please consider finding mbox pointers for this firmware version                
//#else
//#warning please consider finding mbox pointers for this firmware version                
//#endif


//void pevent_to_name(OS_EVENT *pevent, void *pmsg)
//{
//    if( pevent == mbox_beep ) { 
//        printf("to Beep_Process: %x ..", * (uint8_t*) pmsg);
//        switch (* (uint8_t*) pmsg) {
//            case 0x24:
//                printf("roger beep ");
//                break;
//            case 0x27:
//                printf("keypad tone ");
//                break;
//            case 0xe:
//                printf("fail to sync with relay ");
//                break;
//            default:
//                printf("not known ");
//                break;
//        }
//    }
//    printf("Data:       ");
//    printhex(pmsg, 10);
//    printf("\n");
//}

uint8_t OSMboxPost_hook(OS_EVENT *pevent, void *pmsg)
{
    void *return_addr;
    void *sp;
    __asm__("mov %0,r14" : "=r" (return_addr));
    __asm__("mov %0,r13" : "=r" (sp));

//    if( pevent == mbox_radio ) {
//        uint8_t event = *(uint8_t*)pmsg ;
//        if( event == 0x4 ) {
////            NMPRINTRET();
////            NMPRINT("%02x\n", event );
//        }
//    }
    
    //  printf("OSMboxPost_hook r: 0x%x s: 0x%x p: 0x%x m: 0x%x ", return_addr, sp, pevent, pmsg);
    //  pevent_to_name(pevent, pmsg);

    return md380_OSMboxPost(pevent, pmsg);
}

void * OSMboxPend_hook(OS_EVENT *pevent, uint32_t timeout, int8_t *perr)
{
    void * ret;
    //void * return_addr = __builtin_return_address(0);
    //  void * sp;

    //  __asm__("mov %0,r14" : "=r" (return_addr));
    //  __asm__("mov %0,r13" : "=r" (sp));
    ret = md380_OSMboxPend(pevent, timeout, perr);
    
    if( is_netmon_enabled() ) {
        if( ret != NULL ) {
#if defined(FW_D13_020) || defined(FW_S13_020)
            if( pevent == (OS_EVENT *)event1_mbox_poi_radio ) {
                last_radio_event = *(uint8_t*)ret ;
            } else 
#endif                
#if defined(FW_D13_020) || defined(FW_S13_020)
            if( pevent == (OS_EVENT *)event2_mbox_poi_beep ) {
                // beep events
                last_event2 = *(uint8_t*)ret ;
            } else 
#endif                
#if defined(FW_D13_020)
            if( ((uint32_t)pevent) == 0x20017348 ) {
                last_event3 = *(uint8_t*)ret ;
            } else if( ((uint32_t)pevent) == 0x20017450 ) {
                last_event4 = *(uint8_t*)ret ;
            } else 
#endif                
#if defined(FW_D13_020) || defined(FW_S13_020)
            if( pevent == (OS_EVENT *)event5_mbox_poi ) {
                last_event5 = *(uint8_t*)ret ;
            } else 
#endif
            {
                PRINT( "unknown mbox 0x%x\n", pevent );
            }            
        }
    }

    if( ret != NULL && global_addl_config.debug == 1 ) {
        PRINTRET();
        PRINT("OMp 0x%x, 0x%x ", pevent, *(uint8_t*) ret);
//        if( pevent == mbox_beep ) {
//            switch (* (uint8_t*) ret) {
//                case BEEP_ROGER:
//                    PRINT("roger beep ");
//                    break;
//                case BEEP_DENY_TX:
//                    PRINT("no dmr sync ");
//                case 0x11:
//                    PRINT("dmr sync ");
//                    break;
//                default:
//                    PRINT("not known ");
//                    break;
//            }
//        }
        PRINT("\n");
    }
    return ret ;
}
