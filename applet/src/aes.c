
#include <stdio.h>
#include <string.h>

#include "md380.h"
#include "printf.h"
#include "dmesg.h"
#include "version.h"
#include "tooldfu.h"
#include "config.h"
#include "gfx.h"


/* This hook intercepts calls to aes_cipher(), which is used to turn
   the 128-bit Enhanced Privacy Key into a 59-bit sequence that gets
   XORed with the audio before transmission and after reception.
   
   By changing the output to match Motorola's Basic Privacy, we can
   patch the MD380 to be compatible with a Motorola network.
 */

int *aes_cipher_hook(int *pkt){
  int *res;
  printf("aes_cipher(0x%08x);\nIN  :",pkt);
  printhex((char*) pkt,16);       //Print the Enhanced Privacy Key
  printf("\nOUT :");
  res=aes_cipher(pkt);
  printhex((char*) res,16);       //Print the keystream it produces. (First 59 bits are XOR'ed with the audio.)
  printf("\n");
  return res;
}
