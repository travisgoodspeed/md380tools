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

    // ringbuffer maybe?
typedef struct {
    uint8_t buf[100];
    uint16_t off100 ; // [100] 0x64
    uint16_t inbuf ; // [102] 0x66 (0...100-1)
    uint16_t wr_idx ; // [104] 0x68 (0...100-1)
} gps_ring_t ;

#if 0
// S13 @ 0x2001d950    
extern gps_ring_t gps_ringbuf ;
#endif

#ifdef __cplusplus
}
#endif

#endif /* GPS_H */

