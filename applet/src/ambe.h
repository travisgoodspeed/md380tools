extern int ambe_encode_thing_hook(char *a1, int a2, int *a3, int a4,
		                  short a5, short a6, short a7, int a8);
extern int ambe_unpack_hook(int a1, int a2, char length, int a4);

extern int ambe_decode_wav_hook(int *a1, signed int eighty, char *bitbuffer,
                                int a4, short a5, short a6, int a7);

extern int max_level;
extern uint32_t ambe_encode_frame_cnt;   
