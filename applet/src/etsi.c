/*
 *  etsi.c
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

inline const char* get_ta_type_str(uint8_t taFormat)
{
    switch(taFormat)
    {
        case 0 :
            return "7bit iso";
            break;
        case 1 :
            return "8bit iso";
            break;
        case 2 :
            return "8bit utf";
            break;
        case 3 :
            return "16bit utf";
            break;
        default :
            return "unkown"; 
            break;
    }
}

// Full Link Control PDU
void dump_full_lc( lc_t *lc )
{
    uint8_t flco = get_flco(lc);
    uint8_t fid = lc->fid ;
    uint8_t opts = lc->svc_opts ;
    
    PRINT("flco=%02x %s fid=%d svc=%d src=%d dst=%d\n",flco,get_flco_str(lc), fid,opts,get_adr(lc->src),get_adr(lc->dst));
    if (flco == 4 && fid == 0x00)
    {
        struct TAHeader* header = (struct TAHeader*)lc;
        uint8_t taFormat = (header->options >> 6) & 0x03;
        uint8_t taLength = (header->options >> 1) & 0x1f;
        PRINT("TA Header: %s length: %d %s",get_ta_type_str(taFormat),taLength,header->text);
    }
    if (flco > 4 && flco < 8 && fid == 0x00)
    {
        struct TABlock* block = (struct TABlock*)lc;
        PRINT("TA Block: %s\n", block->text);
    }
}


