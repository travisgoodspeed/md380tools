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
    uint8_t unk0 ;
    uint8_t unk1 ;
    uint8_t unk2 ;
    uint8_t unk3 ; // flags. 0x2=?
    wchar_t adr[32]; // zeroterm adress string..
    // sizeof = 0x24 = 36
} sms_hdr_t ;

typedef struct {
    wchar_t txt[144];
    // sizeof = 0x90 * 2  = 144*2
} sms_body_t ;

#ifdef __cplusplus
}
#endif

#endif /* SMS_H */

