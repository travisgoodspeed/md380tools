/*! \file gfx.h
  \brief Graphics function wrappers.
*/

//! Draws wide text at an address by calling back to the MD380 function.
void drawtext(wchar_t *text,
	      int x, int y);
//! Draws ASCII on the screen.
void drawascii(char *ascii,
	       int x, int y);

void drawascii2(char *ascii,
                int x, int y);

void green_led(int on);
void red_led(int on);
void lcd_background_led(int on);


typedef struct gfx_pal {
  long	ncol;
  long  someb; 
  const char * palptr;
  } gfx_pal;


 
typedef struct gfx_bitmap {
  short            width;
  short            height;
  short            bytesperline;
  short            bitsperpixel;
  const char *     pixptr;
  const gfx_pal *  palstruct;
  const gfx_pal *  not_known;
  } gfx_bitmap;
                            
extern const gfx_bitmap bmp_eye;


struct gfx_jmptbl {
    void (*bg_fg_color_func)() ;
    void (*unk_func)() ;
};

typedef struct gfx_font {
    //uint8_t off0[21] ; 
    void *off16 ; // [16]
    uint8_t off21 ;// [21]
    uint8_t off22 ; // 
    uint8_t off23 ;// [23]
} gfx_font_t ;

/* @ 0x2001da1c */
typedef struct gfx_info {
    uint8_t off0[2] ; 
    uint16_t line2 ; // [2] gfx_linefill
    uint8_t off4[10-2];
    uint8_t line1 ; // [12] gfx_linefill
    uint8_t off13[12-1];    
    void *fontpoi ; // [24] // x = fontpoi[21] * fontpoi[23];
    uint8_t off28[32-24-4];
    uint16_t xpos2 ; // [32]
    uint16_t xpos ; // [34]
    uint16_t ypos ; // [36] index (-r6)
    uint8_t off38[44-36-2];   
    uint8_t off44 ; // [44] index // (-r0) // & 0x3
    uint8_t off45[48-44-1];       
    uint32_t fg_color ; // [48]
    uint32_t bg_color ; // [52]
    // ...
    struct gfx_jmptbl *jmptable ; // [72]
    
} gfx_info_t ;

#ifdef FW_D13_020
extern gfx_info_t gfx_info ; 

void gfx_drawchar( uint8_t c );
#else 
#warning TODO: define gfx_info 
#endif

/**
 * 
 * @param p font pointer
 * @return old font pointer
 * 
 * if p == 0, return old pointer without setting new.
 */
void* gfx_select_font(void *p);

// max strlen = 18, if larger end in ".."
void gfx_chars_to_display_hook(wchar_t *str, int x, int y, int xlen);

// if larger than maxstrlen end in ".."
void gfx_drawtext4(wchar_t *str, int x, int y, int xlen, int maxstrlen);
