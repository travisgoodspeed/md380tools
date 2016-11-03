/*! \file ambe.c
  \brief AMBE2+ Hook Functions.
  
  This module contains hooks and replacement functions for AMBE2+.

*/


#include <string.h>

#include "md380.h"
#include "printf.h"
#include "dmesg.h"
#include "version.h"
#include "tooldfu.h"
#include "config.h"
#include "gfx.h"


int max_level;
uint32_t ambe_encode_frame_cnt;

int ambe_encode_thing_hook(char *a1, int a2, int *a3, int a4,
		      short a5, short a6, short a7, int a8){
#ifdef CONFIG_AMBE
  short *s8;
  int i=0;
  int max=0;
  
  s8=(short *)a3; 
  
  for (i=0; i<80; i++) {
    if ( s8[i] > max ) {
      max=s8[i];
    }  
  }
     
  max_level=max;
  ambe_encode_frame_cnt++;
  return ambe_encode_thing(a1,a2,a3,a4,
			   a5,a6,a7,a8);
#else
  return 0xdeadbeef;
#endif
}


/* This hook intercepts calls to ambe_unpack(), which extracts bits
   from the incoming AMBE frame.  The bit buffer is later re-used to
   error-correct the frame, so if we peek at the buffer before
   ambe_unpack() is run we will get the error-corrected bits of the
   preceding packet, but if we peek after ambe_unpack() is called,
   we'll get the bits of the new frame without error correction.
 */
int ambe_unpack_hook(int a1, int a2, char length, int a4){
  /* Dump the previous, error-corrected AMBE frame.  For some reason,
     these aren't decoding properly in DSD. */

#ifdef AMBECORRECTEDPRINT
  short *bits=(short*) a1;
  static int i;

  printf("AMBE2+ Corr: ");
  for(i=0;i<49;i++){
    md380_putc(NULL,bits[i]?'1':'0');
  }
  md380_putc(NULL,'\n');
#endif //AMBECORRECTEDPRINT

#ifdef CONFIG_AMBE
  ambe_unpack(a1,a2,length,a4);
#endif //CONFIG_AMBE

  /* Dump the new, uncorrected AMBE frame.  Bits won't make sense
     until after decoding. */
#ifdef AMBEUNCORRECTEDPRINT
  printf("AMBE2+:  ");
  for(i=0;i<length;i++){
    md380_putc(NULL,bits[i]?'1':'0');
  }
  md380_putc(NULL,'\n');
#endif //AMBEUNCORRECTEDPRINT
  

  return 0;
}

/* The ambe_decode_wav() function decodes AMBE2+ bits to 80 samples
   (160 bytes) of 8kHz audio as signed shorts.  This hook will
   optionally print that data to the dmesg buffer, where it can be
   extracted and recorded on the host.
*/
int ambe_decode_wav_hook(int *a1, signed int eighty, char *bitbuffer,
			 int a4, short a5, short a6, int a7){

  /* This prints the AMBE2+ structure that is to be decoded.  The
     output is decodable with DSD, but it sounds horrible.
   */
#ifdef AMBEPRINT
  int ambestate=OS_ENTER_CRITICAL();
  
  short *bits=(short*) bitbuffer;
  static int i;

  /* I don't know why, but this output can only be decoded by DSD if
     half the frames are dropped.  The trick is to only decode those
     when the sixth paramter is zero, ignoring all the ones where that
     parameter is a one.
     
     Strangely enough, we do not skip half the frames of the WAV
     ouput below.
  */
  if(!a6){
    printf("AMBE2+ Corr: ");
    for(i=0;i<49;i++){
      md380_putc(NULL,bits[i]?'1':'0');
    }
    md380_putc(NULL,'\n');
  }
  OS_EXIT_CRITICAL(ambestate);
#endif //AMBEPRINT

  int toret=0xdeadbeef;
#ifdef CONFIG_AMBE
  //First we call the original function.
  toret=ambe_decode_wav(a1, eighty, bitbuffer,
			a4, a5, a6, a7);
#endif

  /* Print the parameters
  printf("ambe_decode_wav(0x%08x, %d, 0x%08x,\n"
    "%d, %d, %d, 0x%08x);\n",
    a1, eighty, bitbuffer,
    a4, a5, a6, a7);
  */

  /* This is very noisy, so we don't enable it by default.  It prints
     the WAV as hex pairs, which will quickly flood the buffer if it
     isn't cleared in time.
   */
#ifdef AMBEWAVPRINT
  //Does this really need to be in a critical section?
  int wavstate=OS_ENTER_CRITICAL();
  
  //A1 holds audio as signed LE shorts.
  printf("WAV: ");
  printhex(a1,160);
  printf("\n");
  
  OS_EXIT_CRITICAL(wavstate);
#endif //AMBEWAVPRINT

  
  return toret;
}
