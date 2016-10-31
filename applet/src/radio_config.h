/* 
 * File:   radio_config.h
 */

#ifndef RADIO_CONFIG_H
#define RADIO_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t unk1[4];        // [0]
    uint32_t dmrid;         // [4]
    uint8_t unk2[11-4-4];   // [8] 
    uint8_t vox ;           // [11] VOX
    uint8_t unk3[21-11-1];  //  
    uint8_t backlight_time ;// [21] // times 5 seconds.
    uint8_t off22 ;         // [22]
    uint8_t mode_ch_mr ;    // [23] 255 = CH / 0 = MR
    uint8_t unk4[48-23-1];
    char radioname[32];     // [48]
    // [156] led ind?
} radio_config_t ;

extern radio_config_t md380_radio_config;
    
// @ 0x2100 len=0x40 in spi fw:D13
struct radio_config2 {
    uint8_t button1_short ;
    uint8_t button1_long ;
    uint8_t button2_short ;
    uint8_t button2_long ;
    // [0x11] long press time in 4 msec units
};

#if defined(FW_D13_020)
void rc_write_radio_config_to_flash();
#else
#warning please consider adding symbols.    
#define rc_write_radio_config_to_flash() /*nop*/
#endif

// 
struct long_press_struct {
    // unk0: b1=ptt key
    uint8_t unk0 ;
    uint8_t unk1 ;
    uint8_t longpress_flags ;
};

// probably one big struct, or loose vars.
struct keyboard_data {
    // pressed=b0
    uint8_t pressed ;
    // flags1: locked=b1 
    uint8_t flags1 ;  // config from radio_config2 0x6,0x7 (also locked flag)
    uint8_t flags2 ;  // config from radio_config2 0x8,0x9,0xa,0xb
    uint8_t flags3 ;  // config from radio_config2 0xc,0xd,    
};
struct keyboard_data2 {
    uint8_t flags0 ;  // config from radio_config2 0xe,0xf
};

// lucky guess, written as documentation.
// from pc = 0x080134dc 
// it would be nice if this correlates with chirp md380.py struct memory.
// saved @ 0x1edc0 
typedef struct {
    uint8_t fm_dmr_mode ; // if( fm_dmr_mode & 3 ) == 2 { is_dmr(); }
    uint8_t off1 ; // [1] 
    uint8_t unk2 ; // [2] 
    uint8_t off3 ; // [3]
    
    uint8_t off4 ; // [4]
    uint8_t unk5 ; // [5]
    uint16_t off6 ; // [6][7]

    uint8_t unk8 ; // [8]
    uint8_t unk9 ; // [9]
    uint8_t unk10 ; // [10]
    uint8_t unk11 ; // [11]
    
    uint8_t off12 ; // [12] 0xc
    uint8_t off13 ; // [13] 0xd
    
    uint8_t unk14 ; 
    uint8_t unk15 ; 
    
    uint32_t off16 ; // [16] 0x10
    uint32_t off20 ; // [20] 0x14
} channel_info2_t ; // sizeof() = 0x40 

// chirp memory struct?
typedef struct {
    uint8_t off0 ;
    uint8_t cc_slot_flags ; // [0x01] cccc....
    uint8_t off4[12]; // [0x05] = power&flags? // [0x0A] ?
    uint32_t rxf ; // [0x10]
    uint32_t txf ; // [0x14]
    uint16_t rxtone ; // [0x18]
    uint16_t txtone ; // [0x16]
    uint32_t unk1 ;
    wchar_t name[16];
} channel_info_t ; // sizeof() = 0x40 

#ifdef FW_D13_020
extern channel_info_t current_channel_info ;
#endif

#ifdef __cplusplus
}
#endif

#endif /* RADIO_CONFIG_H */
