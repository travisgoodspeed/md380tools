/*! \file addl_config.h
  \brief .
*/

#ifndef MD380TOOLS_ADDL_CONFIG_H_INCLUDED
#define MD380TOOLS_ADDL_CONFIG_H_INCLUDED

extern struct addl_config {
  uint8_t  rbeep;
  uint8_t  datef;
  uint8_t  userscsv;
  uint8_t  debug;
  uint8_t  promtg;
  uint8_t  experimental;
  } global_addl_config;

extern void init_global_addl_config_hook(void);

#endif
