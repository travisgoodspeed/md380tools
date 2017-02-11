/* \file localize.c
   
   This is a quick and dirty hack in C for patching the MD380
   firmware's strings from English to other Roman-alphabet languages.
   The code is rather ugly, and all of this will be rewritten lest the
   technical debt collector come calling.
   
   For adding new locales, add lines of text to strings.txt.

*/

//Needed for memmem()
#define _GNU_SOURCE

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<wchar.h>
#include<assert.h>

//! Maximum string length.
#define MAXLEN 1024
//! Buffer for the current line of text.
char buf[MAXLEN];
//! Buffer for the string to be found
short en[MAXLEN];
//! Buffer length
int enlen;
//! Buffer for the target to replace it with.
short target[MAXLEN];

//! Target language.
char targetlang[2];
//! Megabyte firmware buffer, just a bit large.
char firmware[0x100000];
//! Size of the firmware.
size_t firmwaresize=0;

//! Ends the wide string with a newline.
void trimstring(short *target, int len){
  for(int i=0; i<len && target[i]; i++){
    if(target[i]=='\r' || target[i]=='\n')
      target[i]=0;
  }
}

//! Copies from a char array to a short array.
void copystring(short *dst, char *src){
  while(*src){
    *dst=*src;
    printf("%c",*dst);
    dst++; src++;
  }
  printf("\n");
}

//! Handles a line of input.
int handle(char *line){
  //Simple state machine for parsing.
  static enum {IDLE, EN, NEW} handlerstate=IDLE;
  int len = strlen(line);
  

  if(len<=2){
    handlerstate=IDLE;
  }else if(*line=='#'){
    //Just a comment.
  }else if(line[0]=='e' && line[1]=='n'){
    printf("\n");
    printf("English: %s",line+3);
    handlerstate=EN;
    enlen=strlen(line+3);
    copystring(en,line+3);
    trimstring(en,MAXLEN);
  }else if(line[0]==targetlang[0] && line[1]==targetlang[1]){
    printf("Target:  %s",line+3);
    copystring(target,line+3);
    trimstring(target,MAXLEN);
    handlerstate=NEW;
  }
  
  if(handlerstate==NEW){
    short *toreplace;
    if((toreplace=memmem(firmware,firmwaresize*1024,
			 en,enlen))){
      printf("Found it at %08x.\n",
	     (int) toreplace - (int) firmware + 0x0800C000);
      memcpy(toreplace,target,enlen*2+2);
    }
    handlerstate=IDLE;
  }

  return handlerstate;
}



//! Open the firmware
void openfirmware(char *filename){
  FILE *input = fopen(filename,"rb");
  if(input){
    firmwaresize=fread(firmware,1024,1024,input);
    printf("Read %d blocks of %s\n",(int) firmwaresize,filename);
    fclose(input);
  }else{
    printf("Error reading: %s\n",filename);
  }

}
//! Write out the firmware.
void closefirmware(char *filename){
  FILE *output = fopen(filename,"wb");
  size_t outsize;
  if(output){
    outsize=fwrite(firmware,1024,firmwaresize,output);
    printf("Wrote %d blocks of %s\n",(int) outsize,filename);
    fclose(output);
  }else{
    printf("Error writing: %s\n",filename);
  }
}


//! Prints usage info.
void usage(char *filename){
  printf("USAGE: \n\t%s language infile outfile < strings.txt\n",
	 filename);
}


//! Main method.
int main(int argc, char **argv){
  
  if(argc<4){
    usage(argv[0]);
    exit(1);
  }

  //Set the global language
  targetlang[0]=argv[1][0];
  targetlang[1]=argv[1][1];

  //Load the input file.
  openfirmware(argv[2]);
  
  //Perform the conversion.
  while(!feof(stdin)){
    fgets(buf,MAXLEN,stdin);
    handle(buf);
  }

  //Write the output file.
  closefirmware(argv[3]);
}
