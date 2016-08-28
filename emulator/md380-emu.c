/* 
   This is a quick test to try and call functions from the Tytera
   MD380 firmware within a 32-bit ARM Linux machine.
   
   This only runs on AARCH32 Linux, or on other Linux platforms with a
   porper emulator.
*/

#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/mman.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>


#include "ambe.h"

void *firmware;
void *sram;
void *tcram;

//Maps the firmware image into process memory.
void mapimage(){

  //We've already linked it, so just provide the address.
  firmware=(void*) 0x0800C000;
  sram=(void*) 0x20000000;

  int fdtcram=open("/dev/zero",0);
  //Next we need some TCRAM.  (This is hard to dump.)
  tcram=mmap((void*) 0x10000000,
	     (size_t) 0x20000,
	     PROT_READ|PROT_WRITE, //protections
	     MAP_PRIVATE,          //flags
	     fdtcram,              //file
	     0                     //offset
	     );
  

  if(firmware==(void*) -1 ||
     sram==(void*) -1 ||
     tcram==(void*) -1){
    printf("mmap() error %i.\n", errno);
    exit(1);
  }
}



int main(int argc, char **argv){
  mapimage();

  //printf("Firmware loaded to %08x\n", firmware);
  
  //Either decode a file or just a test routine.
  if(argc>1){
    for(int i=1;i<argc;i++){
      printf("Decoding %s\n",
	     argv[i]);
      decode_amb_file(argv[i],"out.raw");
    }
  }else{
    printf("No files to decode.\n");
  }
  
  return 0;
}
