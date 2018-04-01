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

#if defined(FW_S13_020)
// S13 @ 0x2001d950    
extern gps_ring_t gps_ringbuf ;
#endif

typedef struct {
    //00 01 00 01 00 
    //2a 2a 00 c7 26 
    //47 19 a1 02 03 
    //00
    //00 01 00 01 00 
    //2a 2a 00 da 26 
    //47 19 77 02 03 
    //00
    uint8_t off0 ;  // [0] 0x0  S=0 N=1
    uint8_t off1 ;  // [1] 0x1  
    uint8_t off2 ;  // [2] 0x2  W=0 E=1
    uint8_t status ; // [3] 0x3  (0...?)
    // 00 01
    // 00 01

    uint8_t off4 ;   // [4]
    // 00
    
    uint8_t latdeg;   // [5]
    uint8_t latmin;   // [6]
    uint8_t unk1;

    uint16_t latmindec;
    //unsigned int latmindec : 24; //le 
    uint8_t londeg ;
    uint8_t lonmin ;

    uint8_t unk2;
    uint16_t lonmindec ;
    //unsigned int lonmindec : 24; //le
    uint8_t altitude_m;
    uint8_t unk4;
} gps_t ; //want 18 bytes according to original

#if defined(FW_S13_020)
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

void gps_dump_dmesg();


#ifdef __cplusplus
}
#endif

#endif /* GPS_H */

