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

void *firmware;
void *sram;
void *tcram;

//Maps the firmware image into process memory.
void mapimage(){
  //FIXME, this should be read out of the file.
  size_t length=994304;

  int fd=open("experiment.img",0);
  int fdram=open("ram.bin",0);
  int fdtcram=open("/dev/zero",0);

  /*
  //Map it into the right place, with the right permissions.
  firmware=mmap((void*) 0x0800c000, length,
		//Strictly speaking, it oughtn't be writable, but runtime
		//patching can be handy.
		PROT_EXEC|PROT_READ|PROT_WRITE, //protections
		MAP_PRIVATE,         //flags
		fd,                  //file
		0                    //offset
		);
  */
  //We've already linked it, so just provide the address.
  firmware=(void*) 0x0800C000;


  /*
  //Next we need some RAM.
  sram=mmap((void*) 0x20000000, (size_t) 0x20000,
	    PROT_EXEC|PROT_READ|PROT_WRITE, //protections
	    MAP_PRIVATE,         //flags
	    fdram,               //file
	    0                    //offset
	    );
  */
  sram=(void*) 0x20000000;
  
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

int ambe_decode_wav(signed short *wavbuffer, //1aa8 or 1b48
		    signed int eighty,       //always 80
		    short *bitbuffer,        //always 1c8e
		    int a4,     //0
		    short a5,   //0
		    short a6,   //timeslot, 0 or 1
		    int a7      //0x20011224
		    );
void ambe_init_stuff();



/* Decodes an AMBE2+ frame to verify that the decoding function
   works. Expect to significantly decode this, or maybe give it
   a coredumped RAM image.
*/
void decode_amb_file(char *filename){
  printf("Testing audio decoding from %s\n",
	 filename);

  //Open amb for reading.
  int ambfd=open(filename,0);
  //Open wav for writing.
  int wavfd=open("out.raw",777);
  
  short *ambe=(short*) 0x20011c8e;
  short *outbuf0=(short*) 0x20011aa8;//80 samples
  short *outbuf1=(short*) 0x20011b48;//80 samples
  unsigned char packed[8]; //8 byte frames.

  //ambe_init_stuff();

  if(4!=read(ambfd,packed,4)){
    printf("Unable to read header of %s.\n",filename);
    exit(1);
  }
  packed[5]=0;
  if(!strcmp(packed,".amb")){
    printf("Incorrect magic of %s.\n",filename);
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

int main(int argc, char **argv){
  mapimage();

  printf("Firmware loaded to %08x\n",
	 firmware);
  printf("IVT begins:\n");
  for(int i=0;i<4;i++){
    int *adr=(int*) ((int)firmware)+4*i;
    printf("%08x: %08x\n", adr, *adr);
  }

  //Use the RAM a bit.
  int *inram=sram;
  *inram=0xdeadbeef;
  printf("Wrote 0x%08x to ram at 0x%08x.\n",
	 *inram, sram);

  //Print a string.
  printf("Manufacturer is: '%s'\n",
	 0x080f9e4c);

  //Cast a function pointer to a function in the firmware.
  void (*nullsub)()=(void*) 0x08098e15;
  printf("Calling a nullsub.  If this passes, we can execute the code.\n");
  nullsub();
  printf("Success!\n");

  
  //Either decode a file or just a test routine.
  if(argc>1){
    for(int i=1;i<argc;i++){
      printf("Decoding %s\n",
	     argv[i]);
      decode_amb_file(argv[i]);
      
    }
  }else{
    printf("No files to decode.\n");
  }
  
  return 0;
}
