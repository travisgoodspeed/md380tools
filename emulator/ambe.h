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
