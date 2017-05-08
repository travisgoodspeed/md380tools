/*! \file gfx.h
  \brief Graphics function wrappers.
*/

// 160 pixels wide, 128 pixels high
#define MAX_X 159
#define MAX_Y 127    
    

#define GFX_FONT_SMALL_HEIGHT 12 
#define GFX_FONT_NORML_HEIGHT 16

// max string that spans from left to right one single line.
#define MAX_SCR_STR_LEN 30 


// md380 gfx
/**
 * Draw text, centered in area
 * @param str 16-bit, little endian.
 * @param sx area start x 
 * @param sy area start y 
 * @param ex area end x
 * @param ey area end y
 * @param maxlen chars
 */
void gfx_drawtext(wchar_t *str, uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t maxlen);

void gfx_drawbmp(char *bmp, int x, int y);

void gfx_set_bg_color(int color);
void gfx_set_fg_color(int color);


/**
 * 
 * @param x_from 0...159
 * @param y_from 0...127
 * @param x_to   0...159
 * @param y_to   0...127
 */
void gfx_blockfill(int x_from, int y_from, int x_to, int y_to);


// xlen: if curpos > xlen print ".." instead.
// assume clear ylen = 18 pixels?
void gfx_drawtext2(wchar_t *str, int x, int y, int xlen); // firmware

void gfx_drawtext6(wchar_t *str, int x, int y, int ylen); // firmware

void gfx_clear3( int xlen ); // firmware


// if larger than maxstrlen end in ".."
void gfx_drawtext4(const wchar_t *str, int x, int y, int xlen, int maxstrlen); // firmware

//! Draws wide text at an address by calling back to the MD380 function.
void drawtext(wchar_t *text, int x, int y);

//! Draws ASCII on the screen.
void drawascii(char *ascii, int x, int y);

//// TODO: how does this differ from drawascii?
//void drawascii2(char *ascii, int x, int y);

void gfx_printf_pos(int x, int y, const char *fmt, ... );
void gfx_printf_pos2(int x, int y, int ylen, const char*fmt, ... );
void gfx_puts_pos(int x, int y, const char *str);

void green_led(int on);
void red_led(int on);
void lcd_background_led(int on);

void print_time_hook(const char log);   

typedef struct gfx_pal {
  long   ncol;
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

#if defined(FW_D13_020) || defined(FW_S13_020)
extern gfx_info_t gfx_info ; 

void gfx_drawchar( uint8_t c );
#else 
# define _WARN_ONCE_ // ex: #warning TODO: define gfx_info  (ONE such warning is enough)
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
void gfx_drawtext2_hook(wchar_t *str, int x, int y, int xlen);


#if defined(FW_D13_020) || defined(FW_S13_020)
void gfx_drawtext7(const char *str, int x, int y); // firmware
#else
#define gfx_drawtext7(p1,p2,p3) /* nop */
# define _WARN_ONCE_ // ex: #warning TODO: please consider finding symbol (ONE such warning is enough)
#endif    

#if defined(FW_D13_020) 
void gfx_drawtext10(wchar_t *str, int x1, int y1, int x2, int y2); // firmware
#else
#define gfx_drawtext10(p1,p2,p3,p4,p5) /* nop */
# define _WARN_ONCE_ // ex: #warning TODO: please consider finding symbol (ONE such warning is enough)
#endif    

extern uint32_t gfx_font_small[];
extern uint32_t gfx_font_norm[];

uint32_t gfx_get_fg_color(void);

#ifdef _WARN_ONCE_ // Cleanup Superfluous Warnings, issue #704 . Here: only show ONE warning, if any:
# ifdef COMPILING_MAIN_C
#  warning TODO: please consider finding symbol(s); there may be more than one of these...
   // (still this warning was displayed dozens of times when making 'image_D02' ..)
# endif // compiling MAIN.C ?
#endif // WARN ?
