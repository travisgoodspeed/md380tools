/*
 *  radiostate.c
 * 
 */

#define DEBUG

#include "radiostate.h"

#include "debug.h"

int rst_voice_active = 0 ;
int rst_src = 0 ;
int rst_dst = 0 ;
int rst_mycall = 0 ;

int rst_hdr_sap ;
int rst_hdr_src ;
int rst_hdr_dst ;

// TODO locking. because 1 writer locking no prio. readers only visualize.

void rst_voice_lc_header(int src, int dst)
{
    //TODO: also signal takeover due to missed packets.
    rst_src = src ;
    rst_dst = dst ;
    if( !rst_voice_active ) {
        PRINT("\n* Call from %d to %d started.\n", src, dst);        
    }
    rst_voice_active = 1 ;    
}

void rst_term_with_lc( int src, int dst )
{
    if( rst_voice_active ) {
        PRINT("\n* Call from %d to %d ended.\n", src, dst);
    }
    rst_voice_active = 0 ;
}

void rst_signal_other_call()
{
    rst_mycall = 0 ;    
}

void rst_signal_my_call()
{
    rst_mycall = 1 ;
}

void rst_data_header(data_hdr_t *data)
{
    rst_hdr_sap = get_sap(data);
    rst_hdr_src = get_adr(data->src);
    rst_hdr_dst = get_adr(data->dst);
}

void rst_data_block(data_blk_t *data)
{
}