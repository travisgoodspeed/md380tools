/*! \file spiflash.h 
  \brief spiflash defines
*/


extern int check_spf_flash_type(void); // 0 Error, 1 Success


extern uint32_t spi_flash_addl_config_start;
extern uint32_t spi_flash_addl_config_size;

enum spi_flash_addl_config {
  offset_rbeep,
  offset_datef,
  offset_userscsv,
  offset_debug
};

