/*! \file beep_process.c
  \brief beep_process Hook Functions.

  This module contains hooks and replacement functions for the beep_process.

*/

#include <stdio.h>
#include <string.h>

#include "md380.h"
#include "printf.h"
#include "dmesg.h"
#include "addl_config.h"



// First experiments with beep_tone tunig
// why it's work, i have no idea ;)

void F_294_replacement(uint16_t value) {
  uint32_t multiplicand = 0x1dd;  // original value  - original val @ d13.020 0x4a9

  if (global_addl_config.rbeep == 1) {
    multiplicand= 0xaa;
  }

 *beep_process_unkown=(uint32_t) value * multiplicand;
}
