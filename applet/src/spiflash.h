/*! \file spiflash.h
  \brief spiflash defines
*/

//Only writes if the adr is in range.
extern void spiflash_write_with_type_check(void *dst, long adr, long len);

extern int check_spi_flash_size(void);         // Returns greatest unique address.
extern uint32_t get_spi_flash_type(uint8_t *); // place for the id
                                               // ret:0x00aabbcc  aa=MANUFACTURER ID, bb,cc Device Identification

extern uint32_t spi_flash_addl_config_start;
extern uint32_t spi_flash_addl_config_size;

enum spi_flash_addl_config {
  offset_rbeep,
  offset_datef,
  offset_userscsv,
  offset_debug,
  offset_promtg, offset_prompriv
};

