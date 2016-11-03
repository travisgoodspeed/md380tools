/*
 *  dmr.h
 * 
 */

#ifndef DMR_H
#define DMR_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//extern int g_dst;  // transferbuffer users.csv
//extern int g_dst_is_group;
//extern int g_src;

void dmr_CSBK_handler(uint8_t *pkt);



#ifdef __cplusplus
}
#endif

#endif /* DMR_H */

