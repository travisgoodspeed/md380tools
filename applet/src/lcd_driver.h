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
// (don't waste space and time converting from 'true colour')
#define LCD_COLOUR_WHITE 0xFFFF
#define LCD_COLOUR_BLACK 0x0000
#define LCD_COLOUR_BLUE  0xF800
#define LCD_COLOUR_GREEN 0x07E0
#define LCD_COLOUR_RED   0x001F
#define LCD_COLOUR_MD380_BKGND_BLUE 0xFC03 // BGR565-equivalent of Tytera's blue background for the main screen


// Bitwise combineable 'options' for some text drawing functions:
#define LCD_OPT_NORMAL_OUTPUT 0x00 // "nothing special" (opaque, default font)
#define LCD_OPT_FONT_6x12     0x00 // use 6x12 pixel font
#define LCD_OPT_FONT_8x8      0x01 // use 8x8 pixel font
#define LCD_OPT_DOUBLE_WIDTH  0x02 // double-width character output
#define LCD_OPT_DOUBLE_HEIGHT 0x04 // double-height character output
#define LCD_OPT_FONT_8x16  (LCD_OPT_FONT_8x8|LCD_OPT_DOUBLE_HEIGHT)
#define LCD_OPT_FONT_16x16 (LCD_OPT_FONT_8x8|LCD_OPT_DOUBLE_WIDTH|LCD_OPT_DOUBLE_HEIGHT)
#define LCD_OPT_FONT_12x24 (LCD_OPT_FONT_6x12|LCD_OPT_DOUBLE_WIDTH|LCD_OPT_DOUBLE_HEIGHT)
                           // other combinations are possible but not very useful


//---------------------------------------------------------------------------
// Global vars - the ultimate minimum, there's not much RAM to waste !
// The low-level LCD functions also do NOT use fancy structs, objects,
//     "graphic contexts", "handles", to keep the footprint low.
// Everything a function needs to know is passed in through the argument list,
//     i.e. only occupies a few bytes on the stack.
//---------------------------------------------------------------------------

extern uint8_t LCD_b12Temp[12]; // small RAM buffer for a single, self-defined character


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
void LCD_ColourGradientTest(void); // Fills the framebuffer with a 
  // 2D colour gradient. Used for testing .. details in lcd_driver.c .

uint8_t *LCD_GetFontPixelPtr_8x8( uint8_t c);
  // Retrieves the address of a character's font bitmap, 8 * 8 pixels .
  // Unlike the fonts in Tytera's firmware, the 8*8-pixel font
  // supports all 256 fonts from the ancient 'codepage 437',
  // and can thus be used to draw tables, boxes, etc, as in the old days.

uint8_t *LCD_GetFontPixelPtr_6x12( uint8_t c);
  // Retrieves the address of a character's font bitmap, 6 * 12 pixels .
  // Rarely used by the application, but required by LCD_DrawCharAt() .

//---------------------------------------------------------------------------
// Prototypes for MID-LEVEL LCD driver functions (text output, etc)
//---------------------------------------------------------------------------

int LCD_GetTextWidth( int font_options, char *pszText );

int LCD_DrawCharAt( // lowest level of 'text output' into the framebuffer
        char c,            // [in] character code (ASCII)
        int x, int y,      // [in] pixel coordinate
        uint16_t fg_color, // [in] foreground colour (BGR565)
        uint16_t bg_color, // [in] background colour (BGR565)
        int options );     // [in] LCD_OPT_... (bitwise combined)
  // Returns the graphic coordinate (x) to print the next character .

int LCD_DrawStringAt(char *cp, int x, int y, 
        uint16_t fg_color, uint16_t bg_color,
        int options ); // [in] LCD_OPT_xyz (bitwise combined)
  // Draws a zero-terminated ASCII string. 
  // Returns the graphic coordinate (x) to print the next character .


/* EOF < md380tools/applet/src/lcd_driver.h > */