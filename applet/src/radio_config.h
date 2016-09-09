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

extern radio_config_t md380_radio_config;
    
// lucky guess, written as documentation.
// lives around md380_radio_fm_dmr = 0x2001deb8;  
// from pc = 0x080134dc 
// it would be nice if this correlates with chirp md380.py struct memory.
struct channel_mode_unknown {
    uint8_t fm_dmr_mode ; // if( fm_dmr_mode & 3 ) == 2 { is_dmr(); }
};

#ifdef __cplusplus
}
#endif

#endif /* RADIO_CONFIG_H */
