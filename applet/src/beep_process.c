/*! \file beep_process.c
  \brief beep_process Hook Functions.

  This module contains hooks and replacement functions for the beep_process.

*/

//#include <stdio.h>
#include <string.h>

#include "md380.h"
#include "printf.h"
#include "dmesg.h"
#include "addl_config.h"



// First experiments with beep_tone tunig
// why it's work, i have no idea ;)

void F_294_replacement(uint16_t value) {
#ifdef MD380_d13_020
  uint32_t multiplicand = 0x4a9;
#endif
#ifdef MD380_d02_032
  uint32_t multiplicand = 0x1dd;
#endif
#ifdef MD380_s13_020
  uint32_t multiplicand = 0x49a;
#endif

  if (global_addl_config.rbeep == 1) {
#ifdef MD380_d13_020
    multiplicand= 0x200;
#endif
#ifdef MD380_d02_032
    multiplicand= 0xaa;
#endif
#ifdef MD380_s13_020
    multiplicand= 0x200;
#endif
  }

 *beep_process_unkown=(uint32_t) value * multiplicand;
}
