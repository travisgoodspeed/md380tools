// File:    md380tools/applet/src/lcd_driver.h
// Author:  Wolf (DL4YHF) [initial version] 
// Date:    2017-04-14 
//  API for a simple LCD driver for ST7735-compatible controllers,
//             tailored for Retevis RT3 / Tytera MD380 / etc .
//  Details in the implementation; md380tools/applet/src/lcd_driver.h .
//  

// Defines (macro constants, plain old "C"..) 

#define LCD_SCREEN_WIDTH  160
#define LCD_SCREEN_HEIGHT 128

#define LCD_FONT_WIDTH  6  // width  of a character cell in the 'small', fixed font
#define LCD_FONT_HEIGHT 12 // height of a character cell in the 'small', fixed font


// Colour values for the internally used 16-bit "BGR565" format :
// BLUE component in bits 15..11, GREEN in bits 10..5, RED in bits 4..0 :
#define LCD_COLORBIT0_RED   0
#define LCD_COLORBIT0_GREEN 5
#define LCD_COLORBIT0_BLUE  11
#define LCD_COLOR_WHITE 0xFFFF
#define LCD_COLOR_BLACK 0x0000
#define LCD_COLOR_BLUE  0xF800
#define LCD_COLOR_GREEN 0x07E0
#define LCD_COLOR_RED   0x001F
#define LCD_COLOR_YELLOW (LCD_COLOR_RED|LCD_COLOR_GREEN)
#define LCD_COLOR_CYAN   (LCD_COLOR_BLUE|LCD_COLOR_GREEN)
#define LCD_COLOR_PURPLE (LCD_COLOR_RED|LCD_COLOR_BLUE)
#define LCD_COLOR_MD380_BKGND_BLUE 0xFC03 // BGR565-equivalent of Tytera's blue background for the main screen


// Bitwise combineable 'options' for some text drawing functions:
#define LCD_OPT_NORMAL_OUTPUT 0x00 // "nothing special" (use default font, not magnified)
#define LCD_OPT_FONT_6x12     0x00 // use 6x12 pixel font
#define LCD_OPT_FONT_8x8      0x01 // use 8x8 pixel font
#define LCD_OPT_DOUBLE_WIDTH  0x02 // double-width character output
#define LCD_OPT_DOUBLE_HEIGHT 0x04 // double-height character output
#define LCD_OPT_RESERVED_FONT 0x08 // reserved for a future font, possibly "proportional"
#define LCD_OPT_FONT_8x16  (LCD_OPT_FONT_8x8|LCD_OPT_DOUBLE_HEIGHT)
#define LCD_OPT_FONT_16x16 (LCD_OPT_FONT_8x8|LCD_OPT_DOUBLE_WIDTH|LCD_OPT_DOUBLE_HEIGHT)
#define LCD_OPT_FONT_12x12 (LCD_OPT_FONT_6x12|LCD_OPT_DOUBLE_WIDTH)
#define LCD_OPT_FONT_12x24 (LCD_OPT_FONT_6x12|LCD_OPT_DOUBLE_WIDTH|LCD_OPT_DOUBLE_HEIGHT)

//---------------------------------------------------------------------------
// Structures for the 'mid level' LCD drawing functions.
//   Keeping all in a small struct reduces the overhead
//   to pass two colours and a coordinate to LCD_Printf()...
//---------------------------------------------------------------------------

typedef struct tLcdContext
{ 
  int x,y;  // graphic output coord, updated after printing each character .
  uint16_t fg_color, bg_color; // foreground and background colour
  int font; // current font, zoom, and character output options
            // (bitwise combineable, LCD_OPT_FONT_...)
  int x1, y1, x2, y2; // simple clipping and margins for 'printing'.
  // The above range is set for 'full screen' in LCD_InitContext.
} lcd_context_t;


  // Structure of the 32-bit RGB colour type (3 * 8 colour bits),
  // used by LCD_NativeColorToRGB() + LCD_RGBToNativeColor() :
typedef union T_RGB_Quad
{ uint32_t u32;  // "all in one DWORD", compatible with 6-digit "hex codes"
  struct
   { uint8_t b; // blue component, 0..255
     // (least significant byte for "hex-code"-compatibility on little endian system.
     //  rgb_quad_t.u32 = 0xFF0000 shall be the same as "#ff0000" = PURE RED, not BLUE) 
     uint8_t g; // green component, 0..255
     uint8_t r; // red component, 0..255
     uint8_t a; // alignment dummy (no "alpha" channel here)
   } s;
  uint8_t ba[4]; // the same as a 4-byte array (for processing in loops)
          // Beware, here: ba[0] = least significant byte = BLUE component !
} rgb_quad_t;




//---------------------------------------------------------------------------
// Global vars - use a few as possible, there's not much RAM to waste !
// The LOW-level LCD functions also do NOT use fancy structs, objects,
//     "graphic contexts", "handles", to keep the footprint low.
// Everything a function needs to know is passed in through the argument list,
//     i.e. only occupies a few bytes on the stack.
//---------------------------------------------------------------------------

extern uint8_t LCD_b12Temp[12];  // small RAM buffer for a single, self-defined character


//---------------------------------------------------------------------------
// Prototypes for LOW-LEVEL LCD driver functions 
//---------------------------------------------------------------------------

void LCD_WritePixels( uint16_t wColor, int nRepeats );
void LimitInteger( int *piValue, int min, int max);
int  LCD_SetOutputRect( int x1, int y1, int x2, int y2 );
void LCD_SetPixelAt( int x, int y, uint16_t wColor ); // inefficient.. avoid if possible 

void LCD_FillRect( // Draws a frame-less, solid, filled rectangle
        int x1, int y1,  // [in] pixel coordinate of upper left corner
        int x2, int y2,  // [in] pixel coordinate of lower right corner
        uint16_t wColor); // [in] filling colour (BGR565)
void LCD_HorzLine( int x1, int y, int x2, uint16_t wColor );

void LCD_ColorGradientTest(void); // Fills the framebuffer with a 
  // 2D color gradient. Used for testing .. details in lcd_driver.c .

uint8_t *LCD_GetFontPixelPtr_8x8( uint8_t c);
  // Retrieves the address of a character's font bitmap, 8 * 8 pixels .
  // Unlike the fonts in Tytera's firmware, the 8*8-pixel font
  // supports all 256 fonts from the ancient 'codepage 437',
  // and can thus be used to draw tables, boxes, etc, as in the old days.

uint8_t *LCD_GetFontPixelPtr_6x12( uint8_t c);
  // Retrieves the address of a character's font bitmap, 6 * 12 pixels .
  // Rarely used by the application, but required by LCD_DrawCharAt() .

uint32_t LCD_NativeColorToRGB( uint16_t native_colour );
uint16_t LCD_RGBToNativeColor( uint32_t u32RGB );

  // Crude measure for the "similarity" of two colour values.
  // First used in app_menu.c to find out if two colours are
  // "different enough" to be used as back- and foreground colours.
int LCD_GetColorDifference( uint16_t color1, uint16_t color2 );
uint16_t LCD_GetGoodContrastTextColor( uint16_t backgnd_color );

//---------------------------------------------------------------------------
// Prototypes for MID-LEVEL LCD driver functions (text output, etc)
//---------------------------------------------------------------------------

int LCD_GetFontHeight(int font_options );
int LCD_GetCharWidth( int font_options, char c );
int LCD_GetTextWidth( int font_options, char *pszText );

int LCD_DrawCharAt( // lowest level of 'text output' into the framebuffer
        char c,            // [in] character code (ASCII)
        int x, int y,      // [in] pixel coordinate
        uint16_t fg_color, // [in] foreground colour (BGR565)
        uint16_t bg_color, // [in] background colour (BGR565)
        int options );     // [in] LCD_OPT_... (bitwise combined)
  // Returns the graphic coordinate (x) to print the next character .

void LCD_InitContext( lcd_context_t *pContext );
  // Clears the struct and sets the output clipping window to 'full screen'.

int LCD_DrawString( lcd_context_t *pContext, char *cp ); 
  // Draws a zero-terminated ASCII string. 
  // Returns the graphic coordinate (x) to print the next character .

int LCD_Printf( lcd_context_t *pContext, char *fmt, ... );
  // Almost the same as LCD_DrawString,
  // but with all goodies supported by tinyprintf .


/* EOF < md380tools/applet/src/lcd_driver.h > */