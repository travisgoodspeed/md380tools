/* 
 * File:   radio_config.h
 * Author: Simon IJskes
 *
 * Created on September 7, 2016, 9:59 PM
 */

#ifndef RADIO_CONFIG_H
#define RADIO_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

struct radio_config {
    uint8_t unk1[4];   // [0]
    uint32_t dmrid;    // [4]
    uint8_t unk2[13];   // [8]
    // [11] VOX
    uint8_t backlight_time ; // [21] // times 5 seconds.
    uint8_t off22 ; // [22]
    uint8_t mode_ch_mr ; // [23] 255 = CH / 0 = MR
    // [156] led ind?
};

typedef struct radio_config radio_config_t ;



#ifdef __cplusplus
}
#endif

#endif /* RADIO_CONFIG_H */
