/*
 *  sms.c
 * 
 *  Created on Oct 14, 2016 8:41:57 PM by Simon IJskes
 */

#include <string.h>

#include "sms.h"

#include "md380.h"
#include "syslog.h"
#include "printf.h"

OS_EVENT *mbox_msg = (OS_EVENT *)0x20017438 ;

void sms_test()
{
    sms_hdr_t hdr ;    
    sms_bdy_t bdy ;
  
#if 1    
    // 204
    hdr.dstadr[0] = 0xCC ;
    hdr.dstadr[1] = 0x00 ;
    hdr.dstadr[2] = 0x00 ;
#endif    
#if 0      
    // all
    hdr.dstadr[0] = 0xFF ;
    hdr.dstadr[1] = 0xFF ;
    hdr.dstadr[2] = 0xFF ;
#endif    
#if 0      
    // Null
    hdr.dstadr[0] = 0x0 ;
    hdr.dstadr[1] = 0x0 ;
    hdr.dstadr[2] = 0x0 ;
#endif    
    
    hdr.flags = SMS_TYPE_SINGLECAST ;
//    hdr.flags = SMS_TYPE_MULTICAST ;
    
//    wide_sprintf(hdr.adr,"");
            
    wide_sprintf(bdy.txt,"md380tools rulez");
    
    sms_send( &hdr, &bdy );
}

void sms_test2(int m)
{
    LOGR("sms_test2 %d\n", m);
    sms_hdr_t hdr ;    
    sms_bdy_t bdy ;
  
#if 0    
    // 204
    hdr.dstadr[0] = 0xCC ;
    hdr.dstadr[1] = 0x00 ;
    hdr.dstadr[2] = 0x00 ;
#endif    
#if 1      
    // all
    hdr.dstadr[0] = 0xFF ;
    hdr.dstadr[1] = 0xFF ;
    hdr.dstadr[2] = 0xFF ;
#endif    
    
    hdr.flags = m ; 
    
    wide_sprintf(bdy.txt,"md380tools rulez");
    
    sms_send( &hdr, &bdy );
}

int msg_event ; // must be global.

void sms_send( sms_hdr_t *hdr, sms_bdy_t *body )
{
    sms_hdr_t *hp = (void*)0x2001e1d0 ;
    sms_bdy_t *bp = (void*)0x2001cefc ;
    
    memcpy(hp,hdr,sizeof(sms_hdr_t));
    memcpy(bp,body,sizeof(sms_bdy_t));
    
    msg_event = 3 ;
    md380_OSMboxPost(mbox_msg, &msg_event);    
}
