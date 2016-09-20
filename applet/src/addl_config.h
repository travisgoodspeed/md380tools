/*! \file addl_config.h
  \brief .
*/

#ifndef MD380TOOLS_ADDL_CONFIG_H_INCLUDED
#define MD380TOOLS_ADDL_CONFIG_H_INCLUDED

enum spi_flash_addl_config {
  offset_rbeep,
  offset_datef,
  offset_userscsv,
  offset_debug,
  offset_promtg,
  offset_prompriv,
  offset_micbargraph
};


extern struct addl_config {
  uint8_t  rbeep;
  uint8_t  datef;
  uint8_t  userscsv;
  uint8_t  debug;
  uint8_t  promtg;
  uint8_t  experimental;
  uint8_t  micbargraph;
  } global_addl_config;

extern void init_global_addl_config_hook(void);

extern void spiflash_write_uint8( int offset, uint8_t val );
extern uint8_t spiflash_read_uint8( int offset );
extern uint8_t spiflash_read_uint8_ranged( int offset, uint8_t cnt );

inline void spiflash_write_datef()
{
    spiflash_write_uint8( offset_datef, global_addl_config.datef );    
}

inline void spiflash_read_datef()
{
    global_addl_config.datef = spiflash_read_uint8_ranged( offset_datef, 5 );    
}

inline void spiflash_write_promtg()
{
    spiflash_write_uint8( offset_promtg, global_addl_config.promtg );    
}

inline void spiflash_read_promtg()
{
    global_addl_config.promtg = spiflash_read_uint8_ranged( offset_promtg, 2 );    
}

inline void spiflash_write_micbargraph()
{
    spiflash_write_uint8( offset_micbargraph, global_addl_config.micbargraph );    
}

inline void spiflash_write_rbeep()
{
    spiflash_write_uint8( offset_rbeep, global_addl_config.rbeep );    
}

inline void spiflash_read_rbeep()
{
    global_addl_config.rbeep = spiflash_read_uint8_ranged( offset_rbeep, 2 );    
}

extern int con_enabled ; 

inline void spiflash_write_debug()
{
    spiflash_write_uint8( offset_debug, global_addl_config.debug );    
    con_enabled = global_addl_config.debug ;
}

inline void spiflash_read_debug()
{
    global_addl_config.debug = spiflash_read_uint8_ranged( offset_debug, 2 );    
    con_enabled = global_addl_config.debug ;
}

inline void spiflash_read_micbargraph()
{
    global_addl_config.micbargraph = spiflash_read_uint8_ranged( offset_micbargraph, 2 );    
}

inline void spiflash_write_userscsv()
{
    spiflash_write_uint8( offset_userscsv, global_addl_config.userscsv );    
}

inline void spiflash_read_userscsv()
{
    global_addl_config.userscsv = spiflash_read_uint8_ranged( offset_userscsv, 2 );    
}

#endif
