/*! \file spiflash.h
  \brief spiflash defines
*/

#include <stdint.h>

//Only writes if the adr is in range.
extern void spiflash_write_with_type_check(void *dst, long adr, long len);

extern int check_spi_flash_size(void);         // Returns greatest unique address.
extern uint32_t get_spi_flash_type(uint8_t *); // place for the id
                                               // ret:0x00aabbcc  aa=MANUFACTURER ID, bb,cc Device Identification

extern uint32_t spi_flash_addl_config_start;
extern uint32_t spi_flash_addl_config_size;

// md380_spiflash
void    md380_spiflash_read(void *dst, long adr, long len);
void    md380_spiflash_write(void *dst, long adr, long len);
int     md380_spiflash_security_registers_read(void *dst, long adr, long len);
void    md380_spiflash_block_erase64k(uint32_t);
void    md380_spiflash_sektor_erase4k(uint32_t);
void    md380_spiflash_enable();
void    md380_spiflash_disable();
void    md380_spiflash_wait();
