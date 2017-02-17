/*
 *  radiostate.h
 * 
 */

#ifndef RADIOSTATE_H
#define RADIOSTATE_H

#include "etsi.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int rst_voice_active ;    
extern int rst_src ;
extern int rst_dst ;
extern int rst_grp ;
extern int rst_mycall ;
extern uint8_t rst_flco ;

extern uint8_t rx_voice ;
    
extern int rst_hdr_sap ;
extern int rst_hdr_src ;
extern int rst_hdr_dst ;

void rst_voice_lc_header(lc_t *data);

void rst_term_with_lc(lc_t *data);

void rst_data_header(void *data);        
void rst_data_block(void *data, int len);

void rst_signal_other_call();
void rst_signal_my_call();

#ifdef __cplusplus
}
#endif

#endif /* RADIOSTATE_H */

