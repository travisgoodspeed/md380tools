/*
 *  etsi.c
 * 
 */

#define DEBUG

#include "etsi.h"
#include "debug.h"
#include <string.h>

struct TAContext taContext;
struct TAContext talkerAlias;


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
		//if( global_addl_config.userscsv == 2 && talkerAlias.length != 0) {
		//	draw_ta_screen(0xff8032);
		//}
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

    if (flco == 0 && fid == 0x00 && taContext.src != get_adr(lc->src))
    {
        memset(&taContext, 0, sizeof(taContext)); //Clear TA context
        memset(&talkerAlias, 0, sizeof(talkerAlias)); //Clear TA context
        taContext.src = (uint32_t)get_adr(lc->src);
    }

    //Read TA Header from full LC
    if (flco == 4 && fid == 0x00)
    {
        struct TAHeader* header = (struct TAHeader*)lc; //Copy LC over TA struct
        taContext.format = (header->options >> 6) & 0x03;
        taContext.length = (header->options >> 1) & 0x1f;
        //Read first 6 bytes of TA
        char* destination = taContext.text;
        memcpy(destination, header->text, 6);

        //Calculate amount of TA blocks
        const uint8_t table[] =
        {
            7, 15, 23, 31,
            6, 13, 20, 27,
            6, 13, 20, 27,
            3,  6, 10, 13
        };
        uint8_t length = taContext.length;
        const uint8_t* row = table + taContext.format * 4;
        taContext.blocks = 
            0b0001                   | 
            ((length > row[0]) << 1) |
            ((length > row[1]) << 2) |
            ((length > row[2]) << 3);
        taContext.marked |= 0b0001; 

        //Print TA if no more blocks
        if (taContext.blocks == taContext.marked)
        {
            if (taContext.format == 1 || taContext.format == 2)
            {
                PRINT("TA %d (%d): %s\n", taContext.src, taContext.length, taContext.text);
                if (talkerAlias.src != taContext.src)
                {
                    talkerAlias = taContext;
                }
            }
            else
            {
                PRINT("TA Unsupported format: %s", get_ta_type_str(taContext.format));
                talkerAlias.length = 0;
            }
        }
    }

    //Read TA Block from the full LC header
    if (flco > 4 && flco < 8 && fid == 0x00)
    {
        struct TABlock* block = (struct TABlock*)lc;
        
        //Copy TA block to context
        char* destination = taContext.text;
        destination += 6 + 7 * (flco - 5);
        memcpy(destination, block->text, 7);
        taContext.marked |= 1 << (flco - 4);

        //Print TA if no more blocks
        if (taContext.blocks == taContext.marked)
        {
            if (taContext.format == 1 || taContext.format == 2)
            {
                PRINT("TA %d (%d): %s\n", taContext.src, taContext.length, taContext.text);
                if (talkerAlias.src != taContext.src)
                {
                    talkerAlias = taContext;
                }
            }
            else
            {
                PRINT("TA Unsupported format: %s", get_ta_type_str(taContext.format));
                talkerAlias.length = 0;
            }
        }
    }
}
