/*! \file display.c
  \brief display Hook functions.
*/

#include <stdio.h>
#include <string.h>

#include "md380.h"

// see issue #178
// workaround flipped display phenomenon

void display_init_hook_1(void) {      // from 0x8033586 @ D003.020
#ifdef CONFIG_GRAPHICS
  md380_read_config_3000();
  if ( md380_radio_config[0x1d] & 1)  // offset 0x1d from
                                      // 0x08033586 @ D003.020
   Write_Data_2display(0x8);         //  orginal MD380
  else
   Write_Data_2display(0x8);          // orginal MD380
#endif
}

void display_init_hook_2(void) {
#ifdef CONFIG_GRAPHICS
  if ( md380_radio_config[0x1d] & 1)
    Write_Data_2display(0x40);
  else
    Write_Data_2display(0x4f);
}

