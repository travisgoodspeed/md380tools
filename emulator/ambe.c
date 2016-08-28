#include<stdio.h>
#include<stdlib.h>


/* Decodes an AMBE2+ frame to verify that the decoding function
   works. Expect to significantly decode this, or maybe give it
   a coredumped RAM image.
*/
void decode_amb_file(char *infilename,
		     char *outfilename){
  printf("Testing audio decoding from %s\n",
	 infilename);

  //Open amb for reading.
  int ambfd=open(infilename,0);
  //Open wav for writing.
  int wavfd=open(outfilename,777);


  //FIXME These are unique to 2.032 firmware; should be symbols instead.
  short *ambe=(short*) 0x20011c8e;
  short *outbuf0=(short*) 0x20011aa8;//80 samples
  short *outbuf1=(short*) 0x20011b48;//80 samples
  unsigned char packed[8]; //8 byte frames.

  //ambe_init_stuff();

  if(4!=read(ambfd,packed,4)){
    printf("Unable to read header of %s.\n",infilename);
    exit(1);
  }
  packed[5]=0;
  if(!strcmp(packed,".amb")){
    printf("Incorrect magic of %s.\n",infilename);
  }
  
  //Eight byte frames.
  int frame=0;
  while(8==read(ambfd,packed,8)){
    frame++;
    if(packed[0]!=0){
      printf("Warning: Frame %d has a bad status.\b",
	     frame);
    }

    int ambei=0;
    for(int i=1;i<7;i++){//Skip first byte.
      for(int j=0;j<8;j++){
	ambe[ambei++]=(packed[i]>>(7-j))&1; //MSBit first
      }
    }
    ambe[ambei++]=packed[7]&1;//Final bit in its own frame as LSBit.

    
    for(int i=0;i<49;i++)
      printf("%d", ambe[i]);
    printf("\n");


    //This does the decoding
    ambe_decode_wav(outbuf0, 80, ambe,
		    0, 0, 0,
		    0x20011224 //Don't know what structure is at this address.
		    );

    
    ambe_decode_wav(outbuf1, 80, ambe,
		    0, 0, 1,
		    0x20011224 //Don't know what structure is at this address.
		    );
    

    
    //Dump the audio to stdout.    
    for(int i=0;i<80;i++){
      printf("%04x ", outbuf0[i] & 0xFFFF);
    }
    printf("\n");

    
    for(int i=0;i<80;i++){
      printf("%04x ", outbuf1[i] & 0xFFFF);
    }
    printf("\n");
    

    //Dump the audio to the outfile.
    write(wavfd,outbuf0,160);
    write(wavfd,outbuf1,160);
    
  }

  close(wavfd);
  close(ambfd);
  printf("Done with AMBE test.\n");
}
