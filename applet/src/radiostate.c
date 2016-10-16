/*
 *  radiostate.c
 * 
 */

#define DEBUG

#include "radiostate.h"

#include "debug.h"
#include "syslog.h"
#include "usersdb.h"

int rst_voice_active = 0 ;
int rst_src = 0 ;
int rst_dst = 0 ;
int rst_mycall = 0 ;

int rst_hdr_sap ;
int rst_hdr_src ;
int rst_hdr_dst ;

// TODO locking. because 1 writer locking no prio. readers only visualize.

void rst_voice_lc_header(lc_t *lc)
{
//    #define BSIZE 100
//    char src_buf[BSIZE];
//    char dst_buf[BSIZE];

    int src = get_adr( lc->src );
    int dst = get_adr( lc->dst );
    int flco = get_flco( lc );
    
    int groupcall = flco == 0;

    if( !rst_voice_active || rst_src != src || rst_dst != dst) {
        rst_src = src ;
        rst_dst = dst ;

        PRINT("\n* Call from %d to %s%d started.\n", src, groupcall ? "group ":"", dst);

        PRINT("cs " );
        dump_full_lc(lc);
        
        char grp_c = 'U' ;        
        if( flco == 0 ) {
            grp_c = 'G' ;
        }
        
        LOGR("cs %c %d->%d\n", grp_c, src, dst );

        // insert trigger here
#if 0
        // move this to display thread, and run on trigger.
        if( find_dmr_user(src_buf, src, (void *) 0x100000, BSIZE) ) {
            LOGR("src: %s\n", src_buf);
        }
        if( find_dmr_user(dst_buf, dst, (void *) 0x100000, BSIZE) ) {
            LOGR("dst: %s\n", dst_buf);
        }
#endif

        rst_voice_active = 1 ;    
    }
}

void rst_term_with_lc(lc_t *lc)
{
    int src = get_adr( lc->src );
    int dst = get_adr( lc->dst );
    int flco = get_flco( lc );
    
    int groupcall = flco == 0;
    
    if( rst_voice_active ) {
        rst_src = src ;
        rst_dst = dst ;
        PRINT("\n* Call from %d to %s%d ended.\n", src, groupcall ? "group ":"", dst);
        
        PRINT("ce " );
        dump_full_lc(lc);
        
        char grp_c = 'U' ;        
        if( flco == 0 ) {
            grp_c = 'G' ;
        }

        LOGR("ce %c %d->%d\n", grp_c, src, dst );

        rst_voice_active = 0 ;
    }
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
    int sap = get_sap(data);
    int blocks = get_blocks(data);
    int dpf = get_dpf(data);
    
    rst_hdr_sap = sap ;
    rst_hdr_src = get_adr(data->src);
    rst_hdr_dst = get_adr(data->dst);

    LOGR("dh %d:%d->%d\n", rst_hdr_sap, rst_hdr_src, rst_hdr_dst );

    //TODO: print DPF (6.1.1))
    // 9.3.17 from part 1
    PRINT("sap=%d %s dpf=%d %s src=%d dst=%d %d\n", sap, sap_to_str(sap), dpf, dpf_to_str(dpf), get_adr(data->src),get_adr(data->dst), blocks);
}

void rst_data_block(data_blk_t *data)
{
}
