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
    uint8_t fsn ;    
    uint16_t crc ;    
} data_hdr_t ;

inline uint8_t get_sap( data_hdr_t *data )
{
    return ( data->sap_poc >> 4 ) & 0xF ;
}

inline uint8_t get_dpf( data_hdr_t *data )
{
    return data->g_a_hc_poc_dpf & 0xF ;
}

inline const char* dpf_to_str( uint8_t dpf ) 
{
    switch( dpf ) {
        case 0 : 
            return "udt" ;
        case 1 :
            return "response packet" ;
        case 2 :
            return "dpkt-unc" ;
        default:
            return "?" ;
    }
}

inline uint8_t get_blocks( data_hdr_t *data )
{
    return data->f_blocks & 0x7F ;
}

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


#ifdef __cplusplus
}
#endif

#endif /* ETSI_H */

