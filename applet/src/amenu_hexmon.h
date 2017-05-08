// File:    md380tools/applet/src/amenu_hexmon.h
// Author:  Wolf (DL4YHF) [initial version]
//
// Date:    2017-04-28
//  Simple 'hex monitor' to inspect blocks of memory.
//  Implemented as a callback function for the 'application menu' (app_menu.c) .

#define HEXMON_DUMMY_ADDRESS_SPI_FLASH_START 0xC0000000 /* internal dummy address for the SPI Flash */
#define HEXMON_DUMMY_ADDRESS_SPI_FLASH_END   0xC0FFFFFF /* end of the 16-MByte SPI flash dummy area */
  // (the entire 512-MByte block 0xC0000000 .. 0xDFFFFFFF is not used for anything in an STM32F405,
  //  so we can map the MD380's SPI-flash into this area for the hex monitor without losing anything)

extern uint32_t HexMon_u32StartAddress; 

int am_cbk_HexMon(app_menu_t *pMenu, menu_item_t *pItem, int event, int param );

