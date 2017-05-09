/*
 *  codeplug.h
 * 
 */

#ifndef CODEPLUG_H
#define CODEPLUG_H

#ifdef __cplusplus
extern "C" {
#endif

// A few address offsets inside SPI-flash, occupied by the codeplug.
// First used by the 'alternative' menu, see amenu_codeplug.c .
// The codeplug seems to live in the first 256 kByte of the SPI-Flash.
// The *.rdt file is *not* an exact image of this part of the Flash !
// Offsets in the RDT file are a bit higher (549 bytes?) than in SPI-flash.

// The first entry in the 'Digital Contacts' lists (e.g. first entry '-All Call-')
// was as 0x00005F84 in SPI flash, but 0x000061A9 in an RDT file (delta=549 bytes): 
#define CODEPLUG_SPIFLASH_ADDR_DIGITAL_CONTACT_LIST   0x0000EC20    
#define CODEPLUG_SIZEOF_DIGITAL_CONTACT_ENTRY  36
#define CODEPLUG_MAX_DIGITAL_CONTACT_ENTIES  1000

// The first entry in the 'Digital RX Group Lists' (e.g. '-WW-', '-EU-', '-DL-')
// was as 0x0000EC20 in SPI flash, but 0x0000EE45 in an RDT file: 
#define CODEPLUG_SPIFLASH_ADDR_DIGITAL_RX_GROUP_LISTS 0x0000EC20    
#define CODEPLUG_SIZEOF_DIGITAL_RX_GROUP_ENTRY 96
#define CODEPLUG_MAX_DIGITAL_RX_GROUP_LISTS   250
   // With 250 entries * 96 byte per entry, the next array (zone list) should begin
   // at 0x0EC20 + 250 * 96 = 0x149E0 . Surprisingly, that's what it does.

// The first entry in the ZONE list (with 64 bytes per entry)
// was at 0x000149E0 in SPI flash, but 0x00014C05 in an RDT file:
#define CODEPLUG_SPIFLASH_ADDR_ZONE_LIST  0x000149E0    
#define CODEPLUG_SIZEOF_ZONE_LIST_ENTRY   64
#define CODEPLUG_MAX_ZONE_LIST_ENTRIES    250
#define CODEPLUG_MAX_CHANNELS_PER_ZONE    16
   // With 250 entries * 64 byte per entry, the next array (zone list) would begin
   // at 0x149E0 + 250 * 64 = 0x189E0 . But it didn't ... must be less than 250 zones.

#define CODEPLUG_SPIFLASH_ADDR_ZONE_NUMBER_STRUCT 0x0002F000 /* details in amenu_codeplug.c */
   // The above seems to be a FIVE-byte-struct, 
   // with the ZONE NUMBER at byte-offset #3. There's a copy of this in RAM:
#if defined(FW_D13_020)
# define CODEPLUG_RAM_ADDR_ZONE_NUMBER_STRUCT 0x2001E57C /* used in amenu_codeplug.c */
#endif // for firmware D13.020 only !
typedef struct
{ // Saved  as a FIVE-byte-thing in SPI-Flash @ 0x2F000, see 0x8022ece in D13.020;
  // Loaded as a FIVE-byte-thing from Flash into RAM @ 0x2001E57C, see 0x8022ebc in D13.020.
  uint8_t unknown_ff[3]; // these bytes always seemed to contain 0xFF
  uint8_t zone_index;    // ONE-based zone index (first zone in the list = index ONE, not ZERO!)
  uint8_t unknown2_ff;   // in an RT3 with D13.020, also 0xFF here
} zone_number_t; 

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

#if defined(FW_D13_020) || defined(FW_S13_020)
# define HAVE_ZONE_NAME 1
extern wchar_t zone_name[16];   // 32 bytes @ 0x2001cddc in D13.020
#endif

#if defined(FW_D13_020) /* no time to investigate this: || defined(FW_S13_020) */
# define HAVE_ZONE_NAME_2 1
extern wchar_t zone_name_2[32]; // 64 bytes @ 0x2001e218 in D13.020 
   // The first half of zone_name_2 @ 0x2001e218 contained the same as zone_name @ 0x2001cddc .
   // zone_name_2 is the destination buffer when reading from SPI-lash - see 0x08022d74 .
   // See also : "how to switch to a different zone" in amenu_codeplug.c .
#endif

#ifndef  HAVE_ZONE_NAME
# define HAVE_ZONE_NAME 0
//#warning "Please try to find the equivalent of 'zone_name' (@ 0x2001cddc in D13.020) !"
#endif

#ifndef  HAVE_ZONE_NAME_2
# define HAVE_ZONE_NAME_2 0
//#warning "Please try to find the equivalent of 'zone_name_2' (@ 0x2001cddc in D13.020) !"
#endif

#if defined(FW_D13_020) || defined(FW_S13_020)
extern contact_t contact ; 
#endif

// from pc = 0x080134dc 
// saved @ 0x1edc0 

// the names are partially from chirp md380.py    
typedef struct {
    // mode: xxxx xxMM 
    // MM: 0=? 1=FM 2=DMR 3=unprog
    uint8_t mode ; // [0] 0x0
    uint8_t cc_slot_flags ; // [0x01] cccc....
    uint8_t priv ; // [2]

    uint8_t off3 ; // [3]
    
    uint8_t power ; // [4] (type verified)
    uint8_t unk5 ; // [5] wasc3
    uint16_t contact ; // [6][7]

    uint8_t unk8 ; // [8]
    uint8_t unk9 ; // [9]
    uint8_t unk10 ; // [10] 0xa (type verified) 1...32
    uint8_t unk11 ; // [11] 0xb (type verified) 1...250  (scanlist?)

    uint8_t off12 ; // [12] 0xc (type verified)          (grouplist?)
    uint8_t gps_tx ; // [13] 0xd (type verified) 0...16  0=gps-no-tx 1=gps-tx
    
    uint8_t unk14 ; // [14] 0xe
    uint8_t unk15 ; // [15] 0xf
    
    uint32_t rxf ;  // [16] 0x10
    uint32_t txf ;  // [20] 0x14
    uint16_t rxtone ; // [24] 0x18
    uint16_t txtone ; // [26] 0x1A

    uint32_t unk28 ;  // [28]
    wchar_t name[16]; // [32]

} channel_info_t ; // sizeof() = 0x40 

#if defined(FW_D13_020) || defined(FW_S13_020)
extern channel_info_t current_channel_info ;
#endif


#ifdef __cplusplus
}
#endif

#endif /* CODEPLUG_H */

