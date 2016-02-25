/*! \file dmr.c
  \brief DMR Hook functions.
  
  This module hooks some of the DMR packet handler functions,
  in order to extend the functionality of the radio.  Ideally,
  we'd like to use just the hooks, but for the time-being some
  direct patches and callbacks are still necessary.
*/


#include <stdio.h>
#include <string.h>

#include "md380.h"
#include "printf.h"
#include "version.h"
#include "tooldfu.h"
#include "config.h"
#include "gfx.h"


void *dmr_call_end_hook(void *pkt){
  /* This hook handles the dmr_contact_check() function, calling
     back to the original function where appropriate.
   */
  
  //Turn on the red LED to know that we're here.
  red_led(1);
  
  //TODO log the call or display something for a jiffy.
  
  //Forward to the original function.
  return dmr_call_end(pkt);
}

void *dmr_call_start_hook(void *pkt){
  /* This hook handles the dmr_contact_check() function, calling
     back to the original function where appropriate.
   */
  
  //Turn on the red LED to know that we're here.
  red_led(1);
  
  //All but the top row is overwritten,
  //so any status has to be logged here.
  drawtext(L"dmr_call_start",
	   160,20);
  
  //Forward to the original function.
  return dmr_call_start(pkt);
}

void *dmr_handle_data_hook(char *pkt, int len){
  /* This hook handles the dmr_contact_check() function, calling
     back to the original function where appropriate.
   */
  
  //Turn on the red LED to know that we're here.
  red_led(1);
  
  printf("Data: ");
  for(int i=0;i<len;i++){
    printf(" %02x",pkt[i]&0xFF);
  }
  printf("\n");
  
  //Forward to the original function.
  return dmr_handle_data(pkt,len);
}


void *dmr_sms_arrive_hook(void *pkt){
  /* This hooks the SMS arrival routine, but as best I can tell,
     dmr_sms_arrive() only handles the header and not the actual
     data payload, which is managed by dmr_handle_data() in each
     fragment chunk.
   */
  
  //Turn on the red LED to know that we're here.
  red_led(1);
  
  printf("SMS header.\n");
  
  //Forward to the original function.
  return dmr_sms_arrive(pkt);
}
