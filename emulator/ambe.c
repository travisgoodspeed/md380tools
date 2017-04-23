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


extern int ambe_inbuffer, ambe_outbuffer0, ambe_outbuffer1, ambe_mystery;
extern int ambe_outbuffer, wav_inbuffer0, wav_inbuffer1, ambe_en_mystery;

/* Decodes an AMBE2+ frame to verify that the decoding function
   works. Expect to significantly decode this, or maybe give it
   a coredumped RAM image.
*/
void decode_amb_file(char *infilename,
		     char *outfilename){

  int ambfd=STDIN_FILENO;
  int wavfd=STDOUT_FILENO;
    
  if (infilename)
    ambfd=open(infilename,0);
  
  if (outfilename)
    wavfd=creat(outfilename,0666);

  //FIXME These are unique to 2.032 firmware; should be symbols instead.
  short *ambe=(short*) &ambe_inbuffer; //0x20011c8e;
  short *outbuf0=(short*) &ambe_outbuffer0; //0x20011aa8;//80 samples
  short *outbuf1=(short*) &ambe_outbuffer1; //0x20011b48;//80 samples
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
      fprintf(stderr,"Warning: Frame %d has a bad status.\n",
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
		    (int) &ambe_mystery //0x20011224 //Don't know what structure is at this address.
		    );
    
    ambe_decode_wav(outbuf1, 80, ambe,
		    0, 0, 1,
		    (int) &ambe_mystery //0x20011224 //Don't know what structure is at this address.
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


int ambe_encode_thing2(short *bitbuffer,
                       int a2,
                       signed short *wavbuffer,
                       signed int eighty,
                       int a5,
                       short a6, //timeslot, 0 or 1
                       short a7, //2000
                       unsigned int a8){  // 2000c730
    if(verbosity>=3){
      printf("%x %x %x %x %x %x %x %x\n", bitbuffer , a2, wavbuffer, eighty, a5, a6, a7, a8);
    }
    ambe_encode_thing(bitbuffer,a2,wavbuffer, eighty, a5,a6,a7,a8);
}



void encode_wav_file(char *infilename,
		     char *outfilename){

  int wavfd=STDIN_FILENO;
  int ambfd=STDOUT_FILENO;

  if (infilename)
    wavfd=open(infilename,0);
  if (outfilename)
    ambfd=creat(outfilename,0666);

  unsigned char packed[8]; //8 byte frames.
  short ambe_test[50];

  short *inbuf0 = (short*) &wav_inbuffer0;
  short *inbuf1 = (short*) &wav_inbuffer1;
  short *ambe   = (short*) &ambe_outbuffer;

  write(ambfd,".amb",4); // write dsd header

  short inbuf[160];
  int frame=0;
  memset(ambe,0,50); // init ambe memory

  while((160*2)==read(wavfd,inbuf,160*2)){
    for (int i=0;i<80;i++) inbuf0[i] = inbuf[i];
    for (int i=0;i<80;i++) inbuf1[i] = inbuf[i+80];

    //This does the decoding
    ambe_encode_thing2(ambe, 0, inbuf0,
		    0x50, 0x1840, 0, 0x2000,
                    (int) &ambe_en_mystery
		    );

    ambe_encode_thing2(ambe, 0, inbuf1,
		    0x50, 0x1840, 0x1, 0x2000,
		    (int) &ambe_en_mystery
		    );



    if(verbosity>=2){
      //Dump the audio to stdout.    
      for(int i=0;i<80;i++){
	fprintf(stderr,"%04x ", inbuf0[i] & 0xFFFF);
      }
      fprintf(stderr,"\n");
      for(int i=0;i<80;i++){
	fprintf(stderr,"%04x ", inbuf1[i] & 0xFFFF);
      }
      fprintf(stderr,"\n");
    }

    if(verbosity>=1){
      for(int i=0;i<49;i++)
        fprintf(stderr,"%d", ambe[i]);
      fprintf(stderr,"\n");
    }

    packed[0] = 0;
    packed[1] = ambe[7] | ambe[6] << 1 | ambe[5] << 2 | ambe[4] << 3 | ambe[3] << 4 | ambe[2] << 5 | ambe[1] << 6 | ambe[0] << 7 ;
    packed[2] = ambe[15] | ambe[14] << 1 | ambe[13] << 2 | ambe[12] << 3 | ambe[11] << 4 | ambe[10] << 5 | ambe[9] << 6 | ambe[8] << 7 ;
    packed[3] = ambe[23] | ambe[22] << 1 | ambe[21] << 2 | ambe[20] << 3 | ambe[19] << 4 | ambe[18] << 5 | ambe[17] << 6 | ambe[16] << 7 ;
    packed[4] = ambe[31] | ambe[30] << 1 | ambe[29] << 2 | ambe[28] << 3 | ambe[27] << 4 | ambe[26] << 5 | ambe[25] << 6 | ambe[24] << 7 ;
    packed[5] = ambe[39] | ambe[38] << 1 | ambe[37] << 2 | ambe[36] << 3 | ambe[35] << 4 | ambe[34] << 5 | ambe[33] << 6 | ambe[32] << 7 ;
    packed[6] = ambe[47] | ambe[46] << 1 | ambe[45] << 2 | ambe[44] << 3 | ambe[43] << 4 | ambe[42] << 5 | ambe[41] << 6 | ambe[40] << 7 ;
    packed[7] = ambe[48];

    if(verbosity>=2){
      int ambei=0;
      ambei=0;
        for(int i=1;i<7;i++){//Skip first byte.
          for(int j=0;j<8;j++){
	    ambe_test[ambei++]=(packed[i]>>(7-j))&1; //MSBit first
          }
        }
      ambe_test[ambei++]=packed[7]&1;//Final bit in its own frame as LSBit.

      for(int i=0;i<49;i++)
        fprintf(stderr,"%d", ambe_test[i]);
      fprintf(stderr,"\n\n");
    }
    if (frame > 25) // skip the first ambe frames ... experiment to ignore start noise 
      write(ambfd,packed,8);
    frame++;
  }

  close(wavfd);
  close(ambfd);
  fprintf(stderr,"Done with AMBE test.\n");
}
