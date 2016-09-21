/*! \file os.c
  \brief os Hook functions.
  
*/


#include <stdio.h>
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

OS_EVENT* debug_line_sem;  // not yet used 


INT8U (OSTaskCreateExt_hook)(void (*task)(void *pd), void *pdata, OS_STK *ptos, INT8U prio, INT16U id, OS_STK *pbos, INT32U stk_size, void *pext, INT16U opt) {
  printf("%x %x %x %x %x %x %x %x %x\n", task, pdata, ptos, prio, id, pbos, stk_size, pext, opt);
  return  OSTaskCreateExt(task, pdata, ptos, prio, id, pbos, stk_size, pext, opt);
}


void *(OSTaskNameSet_hook)( INT8U prio, INT8U *name,  INT8U *err) {
  printf("%x %s %x\n", prio, name, *err);
  return OSTaskNameSet(prio, name, err);
}



OS_EVENT * OSSemCreate_hook(uint16_t cnt) {
  OS_EVENT * sem;

  sem  = OSSemCreate(cnt);

  debug_line_sem=OSSemCreate(1);
  if ( debug_line_sem == NULL ) {
    printf("can't create debug_line_sem\n");
    }
  #ifdef DEBUG
  printf("debug_line_sem %x\n", debug_line_sem);
  #endif
  return (sem);
}


void pevent_to_name(OS_EVENT *pevent, void *pmsg) {
  if ( pevent == (void *) 0x20015f0c ) {  // d02.032 FIXME
    printf("to Beep_Process: %x ..", * (uint8_t*)pmsg);
    switch (* (uint8_t*)pmsg  ) {
      case 0x24:
        printf("roger beep ");
        break;
      case 0x27:
        printf("keypad tone ");
        break;
      case 0xe:
        printf("fail to sync with relay ");
        break;
      default:
        printf("not known ");
        break;
    }
  }
    printf("Data:       ");
    printhex(pmsg, 10);
    printf("\n");
}


uint8_t OSMboxPost_hook (OS_EVENT *pevent, void *pmsg) {
  void *return_addr;
  void *sp;
  __asm__("mov %0,r14" : "=r" (return_addr));
  __asm__("mov %0,r13" : "=r" (sp));
  printf("OSMboxPost_hook r: 0x%x s: 0x%x p: 0x%x m: 0x%x ", return_addr, sp, pevent, pmsg);
  pevent_to_name(pevent, pmsg);
  return(md380_OSMboxPost(pevent, pmsg));
}


void * OSMboxPend_hook(OS_EVENT *pevent, uint32_t timeout, int8_t *perr)
{
    void * ret;
    void * return_addr = __builtin_return_address(0);
    //  void * sp;

    //  __asm__("mov %0,r14" : "=r" (return_addr));
    //  __asm__("mov %0,r13" : "=r" (sp));
    ret = md380_OSMboxPend(pevent, timeout, perr);
    
    if( has_console() ) {
        if( ret != NULL ) {
            if( ((uint32_t)pevent) == 0x20017468 ) {
                last_radio_event = *(uint8_t*)ret ;
            }
            if( ((uint32_t)pevent) == 0x20017390 ) {
                // beep events?
                last_event2 = *(uint8_t*)ret ;
            }
            if( ((uint32_t)pevent) == 0x20017348 ) {
                last_event3 = *(uint8_t*)ret ;
            }
        }
    }
    
    if( ret != NULL && global_addl_config.debug == 1 ) {
        printf("OSMboxPend @ 0x%x, 0x%x, 0x%x \n", UNTHUMB_POI(return_addr), pevent, *(uint8_t*)ret);
        switch (* (uint8_t*) ret) {
            case 0x24:
                printf("roger beep ");
                break;
            case 0xe:
                printf("no dmr sync ");
            case 0x11:
                printf("dmr sync ");
                break;
            default:
                printf("not known ");
                break;
        }
        printf("\n");
    }
    return (ret);
}
