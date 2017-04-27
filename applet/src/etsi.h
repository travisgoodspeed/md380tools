/*
 *  etsi.h
 * 
 */

#ifndef ETSI_H
#define ETSI_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//////////////
// adr 
    
typedef struct adr {
    uint8_t b16 ;
    uint8_t b8 ;
    uint8_t b0 ;    
} adr_t ;

inline uint32_t get_adr(adr_t in)
{
    return in.b0 | (in.b8 << 8) | (in.b16 << 16);
}

//////////////
// data

// Unconfirmed data Header
typedef struct data {
    uint8_t g_a_hc_poc_dpf ;    
    uint8_t sap_poc ;
    adr_t dst ;
    adr_t src ;                
    uint8_t f_blocks ;
    union {
        uint8_t s_ns_fsn ;    
        uint8_t cls_type_status ;
    } ;
    uint16_t crc ;    
} data_hdr_t ;

//inline uint8_t get_dpf( data_hdr_t *data )
//{
//    return data->g_a_hc_poc_dpf & 0xF ;
//}

#define DPF_UDT 0

#define DPF_RESPONSE 1
#define DPF_UNCONFIRMED 2
#define DPF_CONFIRMED 3

#define DPF_DEFINED_SHORT 13
#define DPF_RAW_STATUS_SHORT 14 
#define DPF_PROPRIETARY 15

inline const char* dpf_to_str( uint8_t dpf ) 
{
    switch( dpf ) {
        case DPF_UDT : 
            return "udt" ;
        case DPF_RESPONSE :
            return "response_pkt" ;
        case DPF_UNCONFIRMED :
            return "datapkt_unconf" ;
        case DPF_CONFIRMED :
            return "datapkt_conf" ;
        case DPF_DEFINED_SHORT :
            return "shrtdata_defined" ;
        case DPF_RAW_STATUS_SHORT :
            return "shrtdata_raw" ;
        case DPF_PROPRIETARY :
            return "prop_dpkt" ;
        default:
            return "?" ;
    }
}

//inline uint8_t get_blocks( data_hdr_t *data )
//{
//    return data->f_blocks & 0x7F ;
//}

inline const char* sap_to_str( uint8_t sap ) 
{
    switch( sap ) {
        case 0 :
            return "UDT" ;
        case 1 :
            return "(1?)" ;
        case 2 :
            return "TCP" ;
        case 3 :
            return "UDP" ;
        case 4 :
            return "IP" ;
        case 5 :
            return "ARP" ;
        case 9 :
            return "prop" ;
        case 10 :
            return "shrtdata" ;
        default:
            return "?" ;
    }
}

typedef struct {
    uint8_t filler ;
} data_blk_t ;


typedef struct lc {
    uint8_t pf_flco ;    
    uint8_t fid ;
    uint8_t svc_opts ;
    adr_t dst ;
    adr_t src ;    
} lc_t ;

void dump_full_lc( lc_t *lc );

inline uint8_t get_flco( lc_t *lc )
{
    return lc->pf_flco & 0x3f ;
}

struct TAHeader
{
  uint8_t code;
  uint8_t feature;
  uint8_t options;
  char text[6]; 
};

struct TABlock
{
  uint8_t code;
  uint8_t feature;
  char text[7]; 
};

struct TAContext
{
  uint8_t format;
  uint8_t length;
  uint8_t blocks;
  uint8_t marked;
  uint8_t displayed;
  uint32_t src;
  char text[28];
};

struct TAContext taContext;
extern struct TAContext talkerAlias;

#ifdef __cplusplus
}
#endif

#endif /* ETSI_H */
