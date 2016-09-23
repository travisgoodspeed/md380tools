/*! \file dmesg.c
  \brief Kernel logging functions and buffers.

  
  This module implements a kernel log in SRAM, to be accessed over USB
  as a kernel log through hooked USB DFU functions.
*/

#include <stdint.h>

#include "printf.h"
#include "dmesg.h"


//Pointers to the dmesg buffers.
char *dmesg_tx_buf=(char*) DMESG_START;
char dmesg_start[DMESG_SIZE];
int dmesg_wcurs=0;

/* We need to first create our new putc() function
   and then register it in with init_printf(NULL,md380_putc);
*/
void md380_putc ( void* p, char c){
  /*  We'd do this if there were a hardware UART.
  while (!SERIAL_PORT_EMPTY) ;
  SERIAL_PORT_TX_REGISTER = c;
  */
  
  dmesg_start[(dmesg_wcurs++)%DMESG_SIZE]=c;//Write the character.
  dmesg_start[(dmesg_wcurs)%DMESG_SIZE]=0;//Ugly null terminator.
  return;
}

/* This initializes the putc call. */
void dmesg_init(){
  //Wipe out the old buffer.
  for(int i=0;i<DMESG_SIZE;i++)
    dmesg_start[i]=0;
  dmesg_wcurs=0;
  //Register the putc() function.
  init_printf(0,md380_putc);
}

/* This flushes the dmesg buffer.  For now that means moving the
   buffer to the beginning, but we'll add double-buffering when
   we get around to it.
*/
void dmesg_flush(){
  dmesg_init();
}

/* Convenience function to print in hex. */
void printhex(void *buf, int len)
{
    for (int i = 0; i < len; i++) {
        printf(" %02x", ((uint8_t*)buf)[i]&0xFF);
    }
}

/* Convenience function to print in hex. */
void printhex2(const char *buf, int len){
  char strbuf[256];
  int i;
  for(i=0;i<len && len < 255 ;i++){
    printf(" %02x",buf[i]&0xFF);
    if ( (buf[i]&0xFF) > 32 &&  (buf[i]&0xFF) < 127) {
      strbuf[i]=buf[i]&0xFF;
    } else {
      strbuf[i]='.';
    }
  }
  strbuf[++i]='\0';
  printf("   %s",strbuf);
}
