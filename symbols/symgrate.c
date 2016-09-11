/* \file symgrate.c
   \author Travis Goodspeed <travis@radiantmachines.com
   \brief Symbol migration tool.
   
   This is a quick and dirty tool for migrating function symbols in
   Thumb(2) machine code on ARM Cortex M4, and more specifically, for
   migrating function symbols between various firmware images of the
   Tytera MD380.  I hope someday to target related radios, such as
   those from Connect Systems.
   
   
   The general theory of operation is pretty simple.  We have a
   known-good firmware images (2.032) with a number of identified
   functions, and we wish to hook, patch, or call those functions on a
   new firmware image.  Since Thumb uses constant pools for its
   immediates, rather than putting them inside the instructions, the
   related function ought to be the closes match for the starting
   code.
   
   Where possible, this tool matches each function in the old image to
   an address in the new image.  This will succeed where the function
   and optimizations haven't changed between revisions, hopefully
   leaving just a few remaining functions to search for manually in
   IDA+Bindiff or Radare.  The goal is not to be sophisticated, but to
   catch as much of a break for free as possible.
   
   
   A separate tool will be needed to find branches and RAM addresses.
   Also, this parser sucks.
   
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Image begins just after the bootloader.
const int BASE=0x0800C000;
const int MAXLEN=1024*1024;

//These refer to the actual buffers.
char *Src;
char *Dst;

//These macros grab a byte from the device.
#define src8(x) (*(Src+x-BASE))
#define dst8(x) (*(Dst+x-BASE))

//These macros grab a word from the device.
#define src32(x) (*((int*)(Src+x-BASE)))
#define dst32(x) (*((int*)(Dst+x-BASE)))

//These macros grab a short from the device.
#define src16(x) (*((short*)(Src+x-BASE)))
#define dst16(x) (*((short*)(Dst+x-BASE)))



//! Loads a disk image into a buffer.
int loadbuffer(char *buf, const char *filename){
  FILE *f;
  int len;
  
  f=fopen(filename,"r");
  if(!f){
    printf("File %s not found!\n", filename);
    exit(1);
  }
  
  //FIXME hardcoded size
  len=fread(buf, 1024, 1024, f);
  
  return 0;
}

//! Allocate the buffers.
int mallocbuffers(){
  Src=(char*) malloc(MAXLEN);
  Dst=(char*) malloc(MAXLEN);
  return 1;//success
}

//! Scores a match between the src and destination buffer by length.
int scorematch(int sadr, int dadr){
  int i=0;
  
  /* Compare up to the first 1024 bytes, one pair at a time.  Usually
     the best score is the same function if that function hasn't been
     modified between versions.
   */
  do{
    i+=2;
  }while(
	 (
	  //Halfwords exactly agree
	  src16(sadr+i)==dst16(dadr+i)

	  //or halfwords partially agree and might be a BL.
	  || ( src16(sadr+i)&0xF000 == 0xF000 &&
	       dst16(dadr+1)&0xF000 == 0xF000)
	  )
	 && i<1024);

  return i;
}

//!Locates a symbol by name and address.
int findsymbol(const char *name, int adr){
  int isfunction=adr&1;
  if(adr<0xffff){
    printf("%s = 0x%x;\n",
           name,adr);
    return adr|isfunction;
  }
  adr=adr&~1;
  if(adr&0x20000000){         //RAM
    printf("/* %s is a RAM address and cannot be converted. */\n",
	   name);
    return adr|isfunction;
  }else if(adr&0x40000000){   //IO
    printf("%s = 0x%08x;\n",
	   name,adr|isfunction);
    return adr|isfunction;
  }
  
  
  /* Here we run through the destination image, trying to find
     anything that matches a few bytes.  The decision of whether to
     accept the match comes later.
   */
  short tofind=src16(adr);
  int dadr=0, dscore=0;
  for(int i=0;i<(MAXLEN);i+=2){
    if(src16(adr)==dst16(BASE+i)){
      int score=scorematch(adr,BASE+i);
      
      if(score>dscore){
	dscore=score;
	dadr=BASE+i;
      }
    }
  }
  
  if(dadr && dscore>8)
    printf("%-20s = 0x%08x; /* %i byte match */\n",
	   name,
	   dadr|isfunction,
	   dscore);
  else if(dadr)
    printf("/* %s has bad match of %i points at 0x%08x*/\n",
	   name, dscore, dadr|isfunction);
  else
    printf("/* %s not found. */\n",
	   name);
  
  return dadr;
}

//! Ugly shotgun parser for one record of the file.
int parseline(char *line,
	      char *retname, int *retadr){
  //Assumes a form like this:
  //md380_spiflash_read                    = 0x802fd83;
  
  
  char *start=line;
  char *eq=strstr(line,"=");
  char *semicolon=strstr(line,";");
  char *name=start;
  char *adr=strstr(line,"0x");
  char *plus=strstr(line,"+");
  
  //We can't handle arithmetic
  if(plus){
    return 0;
  }
  
  //We need all these fields.
  if(name && semicolon && eq && adr){
    sscanf(name,"%s ",retname);
    sscanf(adr, "0x%08x",retadr);
    return 1;
  }
  
  return 0;
}

//! Ugly puntgun parser loop, terribly unsafe.
int parseloop(){
  static char line[10240];
  //FIXME This should really be rewritten with a better parsing library.
  
  static char name[1024];
  int adr;
  memset(name,1024,0);
  
  while(!feof(stdin)){
    //Read the line.
    line[0]='\0';  // remove double last line
    fgets(line,1024,stdin);
    if (line[0]=='\0'){  // better check return from fgets
      return 0;
    }
    if(strlen(line)==1){ //empty line.
      printf("\n");
    }else if(line[0]=='/' && line[1]=='*'){ //commenty line
      printf("%s",line);
    }else if(parseline(line,name,&adr)){
      //Look for a symbol match.
      //This prints its own result.
      findsymbol(name,adr);
    }else{
      printf("/* Unparsed line: %s */\n",
	     line);
    }
  }
  return 0;
}

//! Main method.  Maybe abstract this a bit?
int main(int argc, char **argv){
  //Allocate the buffers.
  mallocbuffers();
  
  if(argc<3){
    printf("Usage: %s D002.032.img D013.014.img "
	   "<symbols_D002.032 >symbols_D013.014\n",
	   argv[0]);
    return 1;
  }
  
  //Load the source and destination buffers.
  loadbuffer(Src,argv[1]);
  loadbuffer(Dst,argv[2]);
  
  printf("/* Symbols for %s imported from %s. */\n",
	 argv[2],
	 argv[1]);
  
  parseloop();
}
