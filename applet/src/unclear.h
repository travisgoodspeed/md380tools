/*
 *  unclear.h
 * 
 */

#ifndef UNCLEAR_H
#define UNCLEAR_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(FW_D13_020) || defined(FW_S13_020)

typedef struct {
    uint8_t m0 ;
    uint8_t m1 ; // .... ..RT : T=tx_flag R=rx_flag
    uint8_t m2 ;
    uint8_t m3 ;
} radio_status_1_t ; // sizeof = 4

extern radio_status_1_t radio_status_1;

#endif


#ifdef __cplusplus
}
#endif

#endif /* UNCLEAR_H */

