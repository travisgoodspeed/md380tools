/*
 *  codeplug.h
 * 
 */

#ifndef CODEPLUG_H
#define CODEPLUG_H

#ifdef __cplusplus
extern "C" {
#endif


// from pc = 0x080134dc 
// saved @ 0x1edc0 

// the names are partially from chirp md380.py    
typedef struct {
    uint8_t mode ; // if( fm_dmr_mode & 3 ) == 2 { is_dmr(); }
    uint8_t cc_slot_flags ; // [0x01] cccc....
    uint8_t priv ; // [2]
//    uint8_t off4[11]; // [0x05] = power&flags? // [0x0A] ?

    uint8_t off3 ; // [3]
    
    uint8_t power ; // [4]
    uint8_t unk5 ; // [5] wasc3
    uint16_t contact ; // [6][7]

    uint8_t unk8 ; // [8]
    uint8_t unk9 ; // [9]
    uint8_t unk10 ; // [10]
    uint8_t unk11 ; // [11]

    uint8_t off12 ; // [12] 0xc
    uint8_t off13 ; // [13] 0xd
    
    uint8_t unk14 ; 
    uint8_t unk15 ; 
    
    uint32_t rxf ; // [0x10/16]
    uint32_t txf ; // [0x14/20]
    uint16_t rxtone ; // [0x18/24]
    uint16_t txtone ; // [0x1A/26]

    uint32_t unk28 ;
    wchar_t name[16];

} channel_info_t ; // sizeof() = 0x40 

#if defined(FW_D13_020) || defined(FW_S13_020)
extern channel_info_t current_channel_info ;
#endif


#ifdef __cplusplus
}
#endif

#endif /* CODEPLUG_H */

