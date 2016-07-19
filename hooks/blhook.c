/* \file blhook.c
   \author Travis Goodspeed <travis@radiantmachines.com
   \brief Function hooking tool for Thumb binaries.
   
   This is a quick and dirty tool for hooking Thumb function calls in
   static function calls without the help of interactive tooks like
   IDA or Radare2.  (Those tools are great for reverse engineering,
   but not very nice as dependencies in a build chain.)
   
   The tool is to be run from the Unix commandline, with an input file
   as argv[1] and an output file as argv[2].  Provide a series of
   symbols to stdin, and it will patch them while logging the changes
   to stdout.
   
   The textfile is formatted as lines of "name old new", where the
   name is only used for logging.

   
   This is hardwired for a load point of 0x0800C000.  You'll probably
   want something different, like 0x08000000, for your own
   application.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Image begins just after the bootloader.
const int BASE=0x0800C000;
const int MAXLEN=1024*1024;

//These refer to the actual buffers.
char *Src;
int SrcLen;

//These macros grab a byte from the device.
#define src8(x) (*(Src+x-BASE))

//These macros grab a word from the device.
#define src32(x) (*((int*)(Src+x-BASE)))

//These macros grab a short from the device.
#define src16(x) (*((short*)(Src+x-BASE)))



//! Loads a firmware image into a buffer.
int loadbuffer(char *buf, const char *filename){
  FILE *f;
  int len;
  
  f=fopen(filename,"rb");
  if(!f){
    printf("File %s not found!\n", filename);
    exit(1);
  }
  
  //FIXME hardcoded size
  len=fread(buf, 1024, 1024, f)*1024;
  fclose(f);
  
  return len;
}

//! Writes a buffer into an image.
int writebuffer(char *buf, const char *filename){
  FILE *f;
  f=fopen(filename,"wb");
  if(!f){
    printf("Unable to open %s.\n", filename);
    exit(1);
  }
  
  fwrite(buf, 1024, SrcLen/1024, f);
  fclose(f);
  
  return SrcLen;
}

//! Allocate the buffers.
int mallocbuffers(){
  Src=(char*) malloc(MAXLEN);
  return 1;//success
}



//! Ugly shotgun parser for one record of the file.
int parseline(char *line,
	      char *retname,
	      int *retold, int *retnew){
  //Assumes a form like this:
  //OS_ENTER_CRITICAL 0x08044925   0x080c0000
  
  char *start=line;
  char *name=start;
  char *old=strstr(line,"0x");
  char *new=strstr(old+1,"0x");
  
  //We need all these fields.
  if(name && old && new){
    sscanf(name,"%s ",retname);
    sscanf(old, "0x%08x",retold);
    sscanf(new, "0x%08x",retnew);
    
    return 1;
  }
  
  return 0;
}

//! Calculates Thumb code to branch to a target.
int calcbl(int adr, int target){
  /* Begin with the difference of the target and the PC, which points
     to just after the instruction that is being executed.*/
  int offset=target-adr-4;
  //This offset is always relative to a half-word, so we right shift it.
  offset=(offset>>1);
  
  /* The BL instruction is actually two Thumb instructions, with one
     setting the high part of the LR and the other setting the lo part
     while swapping LR and PC. */
  int hi=0xF000 | ((offset&0xFFF800)>>11);
  int lo=0xF800 | (offset&0x7FF);
  
  //Return the pair as a single 32-bit word.
  return (lo<<16)|hi;
}

//! Hooks all calls to a Thumb function.
int patchsymbol(char *name,
		int old, int new){
  int count=0;
  
  /* Thumb functions are all at odd addresses, and the Cortex M4 can't
     run ARM instructions, so we know that something went terribly
     wrong if either the old or the new function address is even.
  */
     
  if(((old&1)==0) ||
     ((new&1)==0)){
    printf("ERROR: %s (0x%08x->0x%08x) must be Thumb functions for hooking.\n",
	   name,old,new);
    exit(1);
  }
  
  /* First we need to identify all potential patch-points, then we
     need to redirect them to the new address.
  */
  for(int adr=BASE;
      adr<BASE+SrcLen;
      adr+=2){
    //Fetch the word.
    int word=src32(adr);
    //Is it a call to our old target?
    if(calcbl(adr,old)==word){
      printf("Patching %s call at 0x%08x from 0x%08x to 0x%08x.\n",
	     name, adr, old, new);
      src32(adr)=calcbl(adr,new);
      count++;
    }
  }
  
  printf("Found %d calls in %d bytes.\n\n", count, SrcLen);
  return count;
}

//! Ugly puntgun parser loop, terribly unsafe.
int parseloop(){
  //FIXME This should really be rewritten with a better parsing library.
  static char line[10240];
  static char name[1024];
  
  int old, new;
  memset(name,1024,0);
  
  while(!feof(stdin)){
    //Read the line.
    fgets(line,1024,stdin);
    
    if(strlen(line)==1){ //empty line.
      printf("\n");
    }else if(line[0]=='/' && line[1]=='*'){ //comment line
      printf("%s",line);
    }else if(parseline(line,name,
		       &old, &new)){
      printf("Patching %s from 0x%08x to 0x%08x.\n",
	     name, old, new);
      patchsymbol(name,old,new);
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
    printf("Usage: %s S013.020.img hooked.img "
	   "<hooks.txt\n",
	   argv[0]);
    return 1;
  }
  
  //Load the source and destination buffers.
  SrcLen=loadbuffer(Src,argv[1]);
  
  printf("/* Hooked symbols from from %s. */\n",
	 argv[1]);
  
  
  parseloop();
  
  //Dump the output.
  writebuffer(Src,argv[2]);
  printf("Done.\n");
}
