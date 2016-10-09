/*
 *  etsi.c
 * 
 *  Created on Oct 9, 2016 3:49:45 PM by Simon IJskes
 * 
 */

#define DEBUG

#include "etsi.h"

#include "debug.h"

inline const char* get_flco_str( lc_t *lc )
{
    switch( get_flco(lc) ) {
        case 0 :
            // Group Voice Channel User
            return "grp" ;
        case 3 :
            // Unit to Unit Voice Channel User
            return "u2u" ;
        case 4 :
            return "talker alias hdr" ;
        case 5 :
            return "talker alias blk 1" ;
        case 6 :
            return "talker alias blk 2" ;
        case 7 :
            return "talker alias blk 3" ;
        case 8 :
            return "gpsinfo" ;
        default: 
            return "?" ;
    }
}

// Full Link Control PDU
void dump_full_lc( lc_t *lc )
{
    uint8_t flco = get_flco(lc);
    uint8_t fid = lc->fid ;
    uint8_t opts = lc->svc_opts ;
    
    PRINT("flco=%02x %s fid=%d svc=%d src=%d dst=%d\n",flco,get_flco_str(lc), fid,opts,get_adr(lc->src),get_adr(lc->dst));    
}


