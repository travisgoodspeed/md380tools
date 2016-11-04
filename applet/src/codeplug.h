/*
 *  codeplug.h
 * 
 */

#ifndef CODEPLUG_H
#define CODEPLUG_H

#ifdef __cplusplus
extern "C" {
#endif

// contact_t.type
#define CONTACT_GROUP 0xC1
#define CONTACT_USER 0xC2
    
// 0xfc000000 @ 0x0805031e    
typedef struct {    
    uint8_t id_l ;
    uint8_t id_m ;
    uint8_t id_h ;
    uint8_t type ;
    wchar_t name[16];
} contact_t ; // sizeof() = 36 (0x24)

#ifdef FW_D13_020
extern wchar_t zone_name[16];

extern contact_t contact ; 
#endif

// from pc = 0x080134dc 
// saved @ 0x1edc0 

// the names are partially from chirp md380.py    
typedef struct {
    // mode: xxxx xxMM 
    // MM: 2=DMR 1=FM 0=? 
    uint8_t mode ; // [0] 0x0
    uint8_t cc_slot_flags ; // [0x01] cccc....
    uint8_t priv ; // [2]
//    uint8_t off4[11]; // [0x05] = power&flags? // [0x0A] ?

    uint8_t off3 ; // [3]
    
    uint8_t power ; // [4] (type verified)
    uint8_t unk5 ; // [5] wasc3
    uint16_t contact ; // [6][7]

    uint8_t unk8 ; // [8]
    uint8_t unk9 ; // [9]
    uint8_t unk10 ; // [10] 0xa (type verified) 1...32
    uint8_t unk11 ; // [11] 0xb (type verified) 1...250

    uint8_t off12 ; // [12] 0xc (type verified)
    uint8_t off13 ; // [13] 0xd (type verified) 0...17
    
    uint8_t unk14 ; // [14] 0xe
    uint8_t unk15 ; // [15] 0xf
    
    uint32_t rxf ;  // [16] 0x10
    uint32_t txf ;  // [20] 0x14
    uint16_t rxtone ; // [24] 0x18
    uint16_t txtone ; // [26] 0x1A

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

