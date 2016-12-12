int ambe_decode_wav(signed short *wavbuffer, //1aa8 or 1b48
		    signed int eighty,       //always 80
		    short *bitbuffer,        //always 1c8e
		    int a4,     //0
		    short a5,   //0
		    short a6,   //timeslot, 0 or 1
		    int a7      //0x20011224
		    );
void ambe_init_stuff();


void decode_amb_file(char *infilename,
		     char *outfilename);


void encode_wav_file(char *infilename,
                     char *outfilename);

int ambe_encode_thing(short *bitbuffer,
                      int a2,
                      signed short *wavbuffer,
                      signed int eighty,
                      int,
                      short a6, //timeslot, 0 or 1
                      short a7, //2000
                      int);  // 2000c730
