/* 
   This is a quick test to try and call functions from the Tytera
   MD380 firmware within a 32-bit ARM Linux machine.
   
   This only runs on AARCH32 Linux, or on other Linux platforms with
   qemu-binfmt.
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
#include<getopt.h>

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

int oldmain(int argc, char **argv){
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

void usage(char *argv0){
  printf("Usage: %s [OPTION]\n"
	 "\t-d                     Decodes AMBE\n"
	 "\t-e                     Encodes AMBE\n"
	 "\t-V                     Version Info\n"
	 "\t-h                     Help!\n"
	 "\n"
	 "\t-v                     Verbose mode.\n"
	 "\t-vv                    Very verbose!\n"
	 "\n"
	 "\t-i foo                 Input File\n"
	 "\t-o bar                 Output File\n",
	 argv0);
}


char *infilename=NULL;
char *outfilename=NULL;
int verbosity=0;

int main(int argc, char **argv){
  int flags, opt;
  char verb; //Main action of this run.

  verb='h';
  while((opt=getopt(argc,argv,"edVvo:i:"))!=-1){
    switch(opt){
      /* For any flag that sets the mode, we simply set the verb char
	 to that mode.
       */
    case 'V'://Version
    case 'd'://Decode AMBE
    case 'e'://Encode AMBE
    case 'h'://usage
      verb=opt;
      break;

      //IO filenames
    case 'o':
      outfilename=strdup(optarg);
      break;
    case 'i':
      infilename=strdup(optarg);
      break;

      //verbosity
    case 'v':
      verbosity++;
      break;
      
    default:
      printf("Unknown flag: %c\n",opt);
      //exit(1);
    }
  }
  
  //Do the main verb.
  switch(verb){
  case 'h'://Usage
    usage(argv[0]);
    exit(1);
    break;
  case 'd'://DECODE
    fprintf(stderr,"Decoding AMBE %s to 8kHz Raw Mono Signed %s.\n",
	    infilename?infilename:"stdin",
	    infilename?outfilename:"stdout");
    decode_amb_file(infilename,
		    outfilename);
    break;
  case 'e'://ENCODE
    printf("TODO: AMBE Encoding doesn't yet work.\n");
    break;
  case 'V'://Version
    printf("TODO: Reading the version doesn't yet work.\n");
    break;
  default:
    printf("Usage error 2.\n");
    exit(1);
  }
  return 0;
}
