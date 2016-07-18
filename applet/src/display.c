/*! \file display.c
  \brief display Hook functions.
*/

#include <stdio.h>
#include <string.h>

#include "md380.h"

// see issue #178
// workaround flipped display phenomenon

display_init_hook_1(void) {
  if ( md380_radio_config[0x1d] & 1)
   Write_Data_2display(0x48);
  else
   Write_Data_2display(0x8);
}

display_init_hook_2(void) {
  if ( md380_radio_config[0x1d] & 1)
    Write_Data_2display(0x4f);
  else
    Write_Data_2display(0x40);
}





