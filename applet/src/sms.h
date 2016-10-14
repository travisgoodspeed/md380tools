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

typedef struct {
    uint8_t dstadr[3] ; // address in reverse (L-M-H)
    uint8_t unk3 ; // flags. 0x2=?
    wchar_t adr[16]; // zeroterm adress string..
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

