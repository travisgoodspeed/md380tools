#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/mman.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>

#include "md380-emu.h"
#include "ambe.h"


/* Decodes an AMBE2+ frame to verify that the decoding function
   works. Expect to significantly decode this, or maybe give it
   a coredumped RAM image.
*/
void decode_amb_file(char *infilename,
		     char *outfilename){
  if(!infilename || !outfilename){
    printf("Missing input or output filename.\n");
    exit(1);
  }

  //Open amb for reading.
  int ambfd=open(infilename,0);
  //Open wav for writing.
  int wavfd=creat(outfilename,0666);


  //FIXME These are unique to 2.032 firmware; should be symbols instead.
  short *ambe=(short*) 0x20011c8e;
  short *outbuf0=(short*) 0x20011aa8;//80 samples
  short *outbuf1=(short*) 0x20011b48;//80 samples
  unsigned char packed[8]; //8 byte frames.

  //ambe_init_stuff();

  if(4!=read(ambfd,packed,4)){
    fprintf(stderr,"Unable to read header of %s.\n",infilename);
    exit(1);
  }
  packed[5]=0;
  if(!strcmp(packed,".amb")){
    fprintf(stderr,"Incorrect magic of %s.\n",infilename);
    exit(1);
  }
  
  //Eight byte frames.
  int frame=0;
  while(8==read(ambfd,packed,8)){
    frame++;
    if(packed[0]!=0){
      fprintf(stderr,"Warning: Frame %d has a bad status.\b",
	      frame);
    }

    int ambei=0;
    for(int i=1;i<7;i++){//Skip first byte.
      for(int j=0;j<8;j++){
	ambe[ambei++]=(packed[i]>>(7-j))&1; //MSBit first
      }
    }
    ambe[ambei++]=packed[7]&1;//Final bit in its own frame as LSBit.

    if(verbosity>=1){
      for(int i=0;i<49;i++)
	fprintf(stderr,"%d", ambe[i]);
      fprintf(stderr,"\n");
    }

    //This does the decoding
    ambe_decode_wav(outbuf0, 80, ambe,
		    0, 0, 0,
		    0x20011224 //Don't know what structure is at this address.
		    );

    
    ambe_decode_wav(outbuf1, 80, ambe,
		    0, 0, 1,
		    0x20011224 //Don't know what structure is at this address.
		    );
    


    if(verbosity>=2){
      //Dump the audio to stdout.    
      for(int i=0;i<80;i++){
	fprintf(stderr,"%04x ", outbuf0[i] & 0xFFFF);
      }
      fprintf(stderr,"\n");
      for(int i=0;i<80;i++){
	fprintf(stderr,"%04x ", outbuf1[i] & 0xFFFF);
      }
      fprintf(stderr,"\n");
    }
    

    //Dump the audio to the outfile.
    write(wavfd,outbuf0,160);
    write(wavfd,outbuf1,160);
    
  }

  close(wavfd);
  close(ambfd);
  fprintf(stderr,"Done with AMBE test.\n");
}
