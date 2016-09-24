/*! \file dmr.c
  \brief DMR Hook functions.

  This module hooks some of the DMR packet handler functions,
  in order to extend the functionality of the radio.  Ideally,
  we'd like to use just the hooks, but for the time-being some
  direct patches and callbacks are still necessary.
*/

#define CONFIG_DMR

#define NETMON

#include "dmr.h"

#include <stdio.h>
#include <string.h>

#include "md380.h"
#include "printf.h"
#include "dmesg.h"
#include "version.h"
#include "tooldfu.h"
#include "config.h"
#include "gfx.h"
#include "addl_config.h"
#include "os.h"
#include "debug.h"


/* Used to avoid duplicate call endings. */
int incall=0;

/* global Bufferspace to transfer data*/
//char DebugLine1[30];
//char DebugLine2[160];  // only for debug normal is 80

int g_dst;  // transferbuffer users.csv
int g_src;
    
typedef struct adr {
    uint8_t b16 ;
    uint8_t b8 ;
    uint8_t b0 ;    
} adr_t ;


typedef struct pkt {
    uint32_t unk0 ;
    uint8_t unk1 ;
    adr_t dst ;
    adr_t src ;    
} pkt_t ;

uint32_t adr( adr_t in )
{
    return in.b0 | (in.b8 << 8 ) | (in.b16 << 16 ) ;
}

void dump_pkf( const char *tag, pkt_t *pkt )
{
    NMPRINT("%s(%d,%d) ", tag, adr(pkt->src), adr(pkt->dst) );
}

void *dmr_call_end_hook(char *pkt)
{
    dump_pkf( "ce", (pkt_t*) pkt );
    
  /* This hook handles the dmr_contact_check() function, calling
     back to the original function where appropriate.

     pkt points to something like this:

                    /--dst-\ /--src-\
     08 2a 00 00 00 00 00 63 30 05 54 7c 2c 36

     In a clean, simplex call this only occurs once, but on a
     real-world link, you'll find it called multiple times at the end
     of the packet.
   */

  //Destination adr as Big Endian.
  int dst=(pkt[7]|
	   (pkt[6]<<8)|
	   (pkt[5]<<16));
  //Source comes next.
  int src=(pkt[10]|
	   (pkt[9]<<8)|
	   (pkt[8]<<16));

  //printf("\n");
  //printhex((char*)pkt,14);
  if(incall)
    printf("\nCall from %d to %d ended.\n",
	   src,dst);
  incall=0;

  //Forward to the original function.
  return dmr_call_end((void*)pkt);
}

void *dmr_call_start_hook(uint8_t *pkt)
{
//    PRINTRET();
//    PRINTHEX(pkt,10);
//    PRINT("\n");

    /* This hook handles the dmr_contact_check() function, calling
       back to the original function where appropriate.

       It is called several times per call, presumably when the
       addresses are resent for late entry.  If you need to trigger
       something to happen just once per call, it's better to put that
       in dmr_call_end_hook().

       pkt looks like this:

       overhead
       /    /         /--dst-\ /--src-\
       08 1a 00 00 00 00 00 63 30 05 54 73 e3 ae
       10 00 00 00 00 00 00 63 30 05 54 73 2c 36
     */

    dump_pkf( "cs", (pkt_t*) pkt );
    
  //Destination adr as Big Endian.
  int dst=(pkt[7]|
           (pkt[6]<<8)|
	   (pkt[5]<<16));

  int src=(pkt[10]|
           (pkt[9]<<8)|
           (pkt[8]<<16));



//  OSSemPend(debug_line_sem, 0, &err);
//
  //printf("Call start %d -> %d\n", src,dst);
//  sprintf(DebugLine1, "%d -> %d", src, dst );

//  if( find_dmr_user(DebugLine2, src, (void *) 0x100000, 80) == 0){
//    sprintf(DebugLine2, ",ID not found,in users.csv,see README.md,on Github");   // , is line seperator ;)
//  }

//  OSSemPost(debug_line_sem);

  int primask=OS_ENTER_CRITICAL();
  g_dst=dst;
  g_src=src;
  OS_EXIT_CRITICAL(primask);
    


  //This prints a dot at every resynchronization frame.
  //It can distract AMBE2+ logging.
  //printf(".");

  //Record that we are in a call, for later logging.
  incall=1;

  //Forward to the original function.
  return dmr_call_start(pkt);
}


void dmr_apply_squelch_hook(OS_EVENT *event, char * mode){
#ifdef CONFIG_DMR
  /* The *mode byte is 0x09 for an unmuted call and 0x08 for a muted
     call.
  */

  //printf("dmr_apply_squelch_hook for *mode=0x%02x.\n",*mode);

  //Promiscuous mode!
  if(*mode==0x08 && global_addl_config.promtg==1){
    printf("Applying monitor mode to a public call.\n");
    *mode=0x09;
    
    /* I'm not quite sure what this function does, but it must be
       called before dmr_apply_squelch() if the squelch mode is being
       changed. --Travis
     */
    dmr_before_squelch();
  }
  
  /* This is really OSMboxPost().  We should probably change up these
     names now that we're figuring out what the functions really
     do. --Travis
   */
  md380_OSMboxPost(event, mode);
#endif
}

void dmr_apply_privsquelch_hook(OS_EVENT *event, char *mode){
#ifdef CONFIG_DMR
  /* The *mode byte is 0x09 for an unmuted call and 0x08 for a muted
     call.
  */

  //printf("dmr_apply_squelch_hook for *mode=0x%02x.\n",*mode);

  //Promiscuous mode!
  if(*mode==0x08 && global_addl_config.promtg==1){
    printf("Applying monitor mode to a private call.\n");
    *mode=0x09;
    dmr_before_squelch();
  }
  md380_OSMboxPost(event, mode);
#endif
}


void *dmr_handle_data_hook(char *pkt, int len){
#ifdef CONFIG_DMR
  /* This hook handles the dmr_contact_check() function, calling
     back to the original function where appropriate.

     Packes are up to twelve bytes, but they are always preceeded by
     two bytes of C5000 overhead.
   */

    dump_pkf( "da", (pkt_t*) pkt );
    
  //Turn on the red LED to know that we're here.
  red_led(1);

  printf("Data:       ");
  printhex(pkt,len+2);
  printf("\n");

  //Forward to the original function.
  return dmr_handle_data(pkt,len);
#else
  return 0xdeadbeef;
#endif
}


void *dmr_sms_arrive_hook(void *pkt){
#ifdef CONFIG_DMR
  /* This hooks the SMS arrival routine, but as best I can tell,
     dmr_sms_arrive() only handles the header and not the actual
     data payload, which is managed by dmr_handle_data() in each
     fragment chunk.

     *pkt points to a twelve byte header with two bytes of C5000
     overhead.  The body packets will arrive at dmr_handle_data_hook()
     in chunks of up to twelve bytes, varying by data rate.

     A full transaction from 3147092 to 99 looks like this:

             header
             |   / /flg\ /--dst-\ /--src-\ /flg\ /crc\
SMS header:  08 6a 02 40 00 00 63 30 05 54 88 00 83 0c
       Data: 08 7a 45 00 00 5c 00 03 00 00 40 11 5c a8
       Data: 08 7a 0c 30 05 54 0c 00 00 63 0f a7 0f a7
       Data: 08 72 00 48 d1 dc 00 3e e0 00 92 04 0d 00
       Data: 08 72 0a 00 54 00 68 00 69 00 73 00 20 00
       Data: 08 72 69 00 73 00 20 00 61 00 20 00 74 00
       Data: 08 7a 65 00 73 00 74 00 20 00 66 00 72 00
       Data: 08 7a 6f 00 6d 00 20 00 6b 00 6b 00 34 00
       Data: 08 7a 76 00 63 00 7a 00 21 00 9e 21 5a 5c
   */

  //Turn on the red LED to know that we're here.
  red_led(1);

  printf("SMS header: ");
  printhex(pkt, 12+2);
  printf("\n");

  //Forward to the original function.
  return dmr_sms_arrive(pkt);
#else
  return 0xdeadbeef;
#endif
}
