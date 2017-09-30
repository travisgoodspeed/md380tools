/*
 *  sms.c
 * 
 */

#include <string.h>

#include "sms.h"

#include "addl_config.h"
#include "md380.h"
#include "menu.h"
#include "syslog.h"
#include "printf.h"
#include "codeplug.h"

#if defined(FW_D13_020) || defined(FW_S13_020)
    extern uint32_t msg_sms_hdr ;
    extern uint32_t msg_sms_bdy ;
#endif

OS_EVENT *mbox_msg = (OS_EVENT *)0x20017438 ;

inline int is_fm()
{
#if defined(FW_D13_020) || defined(FW_S13_020) 
    int m = current_channel_info.mode & 0x3 ;
    if( m == 1 ) {
        return 1 ;
    } else {
        return 0 ;
    }
#else
    return 0 ;
#endif    
}

void sms_rpt()
{
    sms_hdr_t hdr ;    
    sms_bdy_t bdy ;
 
    // 262994
    hdr.dstadr[0] = 0x52 ;
    hdr.dstadr[1] = 0x03 ;
    hdr.dstadr[2] = 0x04 ;
    
    hdr.flags = SMS_TYPE_SINGLECAST ;
    //hdr.flags = SMS_TYPE_MULTICAST ;
    
    snprintfw(bdy.txt,4,"RPT");
    sms_send( &hdr, &bdy );
}

void sms_wx()
{
    sms_hdr_t hdr ;    
    sms_bdy_t bdy ;
 
    // 262993
    hdr.dstadr[0] = 0x51 ;
    hdr.dstadr[1] = 0x03 ;
    hdr.dstadr[2] = 0x04 ;
    
    hdr.flags = SMS_TYPE_SINGLECAST ;
   
    if(global_addl_config.sms_wx == 1) {			// WX report repeater based location
	snprintfw(bdy.txt, 3, "WX");
    } 
    else if(global_addl_config.sms_wx == 2) {			// WX report gps based location
	snprintfw(bdy.txt, 7, "WX GPS");
    } 

    sms_send( &hdr, &bdy );
}

void sms_gps()
{
    sms_hdr_t hdr ;    
    sms_bdy_t bdy ;
 
    // 262993
    hdr.dstadr[0] = 0x51 ;
    hdr.dstadr[1] = 0x03 ;
    hdr.dstadr[2] = 0x04 ;
    
    hdr.flags = SMS_TYPE_SINGLECAST ;
   
    snprintfw(bdy.txt,144,"GPS");
    sms_send( &hdr, &bdy );
}

int msg_event ; // must be global.

void sms_send( sms_hdr_t *hdr, sms_bdy_t *body )
{
#if defined(FW_D13_020) || defined(FW_S13_020) 
    if( is_fm() ) {
        // TODO: beep
        return ;
    }
    
    sms_hdr_t *hp = (void*)&msg_sms_hdr ;
    sms_bdy_t *bp = (void*)&msg_sms_bdy ;

    memcpy(hp,hdr,sizeof(sms_hdr_t));
    memcpy(bp,body,sizeof(sms_bdy_t));
    
    msg_event = 3 ;
    md380_OSMboxPost(mbox_msg, &msg_event);    
#endif    
}
