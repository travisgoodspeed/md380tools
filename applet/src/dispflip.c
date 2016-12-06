/*! \file display.c
  \brief display Hook functions.
*/

#include <string.h>

#include "md380.h"

// see issue #178
// workaround flipped display phenomenon

void display_init_hook_1(void) {      // from 0x8033586 @ D003.020
#ifdef CONFIG_GRAPHICS
  md380_copy_spiflash_security_bank2_to_ram();
  if ( md380_radio_config_bank2[0x1d] & 1)  // offset 0x1d from
                                            // 0x08033586 @ D003.020
   md380_Write_Data_2display(0x8);                // MD380
  else
   md380_Write_Data_2display(0x48);               // MD390
#endif
}

void display_init_hook_2(void) {
#ifdef CONFIG_GRAPHICS
  if ( md380_radio_config_bank2[0x1d] & 1)
    md380_Write_Data_2display(0x40);
  else
    md380_Write_Data_2display(0x4f);
#endif
}

