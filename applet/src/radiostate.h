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
extern int rst_mycall ;
    
extern int rst_hdr_sap ;
extern int rst_hdr_src ;
extern int rst_hdr_dst ;

void rst_voice_lc_header(int src, int dst);

void rst_term_with_lc( int src, int dst );

void rst_data_header(data_hdr_t *data);
        
void rst_data_block(data_blk_t *data);

void rst_signal_other_call();
void rst_signal_my_call();

#ifdef __cplusplus
}
#endif

#endif /* RADIOSTATE_H */

