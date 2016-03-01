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

void *(OSTaskCreate_hook)( void *task_function, int something1, void *something2, int id) {
  printf("%x %x %x %x\n", task_function, something1, something2, id);
  return OSTaskCreate((void *)task_function, something1, (void *)something2, id);
}


void *(os_set_task_hook)( int id, char *name,  int something1) {
  printf("%x %s %x\n", id, name, something1);
  return os_set_task(id, (char *)name, (void *)something1); 
}  




