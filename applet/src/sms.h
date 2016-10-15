/*
 *  sms.h
 * 
 */

#ifndef SMS_H
#define SMS_H

#include "etsi.h"

#ifdef __cplusplus
extern "C" {
#endif
    
// flags

#define SMS_TYPE_MULTICAST 1        // dst=225.x.x.x    
#define SMS_TYPE_SINGLECAST 2       // dst=12.x.x.x
#define SMS_TYPE_BROADCAST 3        // dst=255.x.x.x
// 3 9 11 17 19 25 26 27 33 
    
typedef struct {
    uint8_t dstadr[3] ; // address in reverse (L-M-H)
    uint8_t flags ; // SMS_TYPE
    wchar_t adr[16]; // zeroterm adress string.. (ignored, scratch buffer)
    // sizeof = 0x24 = 36
} sms_hdr_t ;

typedef struct {
    wchar_t txt[144];
    // sizeof = 0x90 * 2  = 144*2
} sms_bdy_t ;

void sms_test();
void sms_test2(int m);

void sms_send( sms_hdr_t *hdr, sms_bdy_t *body );

#ifdef __cplusplus
}
#endif

#endif /* SMS_H */

