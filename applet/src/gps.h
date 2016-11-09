/*
 *  gps.h
 * 
 */

#ifndef GPS_H
#define GPS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// ringbuffer 
typedef struct {
    uint8_t buf[100];
    uint16_t rd_idx ; // [100] 0x64
    uint16_t inbuf ; // [102] 0x66 (0...100-1)
    uint16_t wr_idx ; // [104] 0x68 (0...100-1)
} gps_ring_t ;

#if 0 
// defined(FW_S13_020)
// S13 @ 0x2001d950    
extern gps_ring_t gps_ringbuf ;
#endif

typedef struct {
    uint8_t off0 ;  // [0] 0x0  S=0 N=1
    uint8_t off1 ;  // [1] 0x1  
    uint8_t off2 ;  // [2] 0x2  W=0 E=1
    uint8_t status ; // [3] 0x3  (0...?)

    uint8_t off4 ;   // [4]
    uint8_t off5 ;   // [5]
    uint8_t off6 ;   // [6]
    uint8_t off7 ;   // [7] unsused?
    uint16_t off8 ;  // [8]
    uint8_t off10 ;  // [10] 0xa
    uint8_t off11 ;  // [11] 0xb
    uint16_t off12 ; // [12] 0xc
    uint16_t off14 ; // [14] 0xe coord lat?
    uint16_t off16 ; // [16] 0x10 coord lon?        
} gps_t ;

#if 0 
// defined(FW_S13_020)
// S13 @ 0x2001e4a4    
extern gps_t gps_data ;
#endif

// relevant in md380_f_4520
// q_status_2 5= 6= 1= 2= 4= 0= 7= 8= 3=  / gui_opmode3
// q_status_3 beepcode
// q_status_4 flags? (1000b=flag)
// q_status_5 .... ..xx xx=?
// q_status_5[3] flags?
// q_struct_1[0] flags?



#ifdef __cplusplus
}
#endif

#endif /* GPS_H */

