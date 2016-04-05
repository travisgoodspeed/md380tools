/*! \file ambe.c
  \brief AMBE2+ Hook Functions.
  
  This module contains hooks and replacement functions for AMBE2+.

*/


#include <stdio.h>
#include <string.h>

#include "md380.h"
#include "printf.h"
#include "dmesg.h"
#include "version.h"
#include "tooldfu.h"
#include "config.h"
#include "gfx.h"




int ambe_encode_thing_hook(char *a1, int a2, int *a3, int a4,
		      short a5, short a6, short a7, int a8){
  printf("AMBE2+ function is hooked!\n");
  
  //Call back to the original function.
  return ambe_encode_thing(a1,a2,a3,a4,
			   a5,a6,a7,a8);
}


/* This hook intercepts calls to ambe_unpack(), which extracts bits
   from the incoming AMBE frame.  The bit buffer is later re-used to
   error-correct the frame, so if we peek at the buffer before
   ambe_unpack() is run we will get the error-corrected bits of the
   preceding packet, but if we peek after ambe_unpack() is called,
   we'll get the bits of the new frame without error correction.
 */
int ambe_unpack_hook(int a1, int a2, char length, int a4){
  short *bits=(short*) a1;
  static int i;

  /* Dump the previous, error-corrected AMBE frame. */

#ifdef AMBECORRECTEDPRINT
  printf("AMBE2+ Corr: ");
  for(i=0;i<49;i++){
    md380_putc(NULL,bits[i]?'1':'0');
  }
  md380_putc(NULL,'\n');
#endif //AMBECORRECTEDPRINT

  
  ambe_unpack(a1,a2,length,a4);

  /* Dump the new, uncorrected AMBE frame.  Bits won't
     make sense until after decoding. */
#ifdef AMBEUNCORRECTEDPRINT
  printf("AMBE2+:  ");
  for(i=0;i<length;i++){
    md380_putc(NULL,bits[i]?'1':'0');
  }
  md380_putc(NULL,'\n');
#endif //AMBEUNCORRECTEDPRINT
  

  return 0;
}
