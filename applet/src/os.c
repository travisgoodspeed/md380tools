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

OS_EVENT* debug_line_sem;

OS_EVENT  ** OSSemCreate_hook0_event_mem;
OS_EVENT  ** OSSemCreate_hook1_event_mem; 


INT8U (OSTaskCreateExt_hook)(void (*task)(void *pd), void *pdata, OS_STK *ptos, INT8U prio, INT16U id, OS_STK *pbos, INT32U stk_size, void *pext, INT16U opt) {
  printf("%x %x %x %x %x %x %x %x %x\n", task, pdata, ptos, prio, id, pbos, stk_size, pext, opt);
  return  OSTaskCreateExt(task, pdata, ptos, prio, id, pbos, stk_size, pext, opt);
}


void *(OSTaskNameSet_hook)( INT8U prio, INT8U *name,  INT8U *err) {
  printf("%x %s %x\n", prio, name, *err);
  return OSTaskNameSet(prio, name, err);
}



void OSSemCreate_hook(void) {
*OSSemCreate_hook0_event_mem=OSSemCreate(1);  // create the hook events _must_ successful
*OSSemCreate_hook1_event_mem=OSSemCreate(1);  //

debug_line_sem=OSSemCreate(1);
if ( debug_line_sem == NULL ) {
  printf("can't create debug_line_sem\n");
  }
#ifdef DEBUG
 printf("debug_line_sem %x\n", debug_line_sem);
#endif
}