/*
 *  radiostate.c
 * 
 */

#define DEBUG

#include "radiostate.h"

#include "debug.h"
#include "syslog.h"

int rst_voice_active = 0 ;
int rst_src = 0 ;
int rst_dst = 0 ;
int rst_mycall = 0 ;

int rst_hdr_sap ;
int rst_hdr_src ;
int rst_hdr_dst ;

// TODO locking. because 1 writer locking no prio. readers only visualize.

void rst_voice_lc_header(int src, int dst, int groupcall)
{
    if( !rst_voice_active || rst_src != src || rst_dst != dst) {
        rst_src = src ;
        rst_dst = dst ;
        PRINT("\n* Call from %d to %s%d started.\n", src, groupcall ? "group ":"", dst);
        LOGR("cs %d->%s%d\n", src, groupcall ? "group ":"", dst );
    }
    rst_voice_active = 1 ;    
}

void rst_term_with_lc( int src, int dst, int groupcall )
{
    if( rst_voice_active || rst_src != src || rst_dst != dst) {
        rst_src = src ;
        rst_dst = dst ;
        PRINT("\n* Call from %d to %s%d ended.\n", src, groupcall ? "group ":"", dst);
        LOGR("ce %d->%s%d\n", src, groupcall ? "group ":"", dst );
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

    LOGR("dh %d:%d->%d\n", rst_hdr_sap, rst_hdr_src, rst_hdr_dst );
}

void rst_data_block(data_blk_t *data)
{
}
