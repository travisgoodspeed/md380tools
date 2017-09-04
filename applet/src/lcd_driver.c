// File:    md380tools/applet/src/lcd_driver.c
// Authors: Wolf (DL4YHF) [initial version],
//          Stephen (K6BSD) [FSMC timing to *read* pixels], ..
// Date:    2017-06-10 
//  Implements a simple LCD driver for ST7735-compatible controllers,
//             tailored for Retevis RT3 / Tytera MD380 / etc .
//  Works much better (!) than the stock driver in the original firmware
//             as far as speed and QRM(!) from the display cable is concerned.
//  Written for the 'alternative setup menu', but no dependcies from that,
//  thus may also be used to replace the 'console' display for Netmon & Co.
// 
// To include this alternative LCD driver in the patched firmware,
//   add the following lines in applet/Makefile :
//      SRCS += lcd_driver.o
//      SRCS += lcd_fonts.o
//      SRCS += font_8_8.o
//  

#include "config.h"

#include <stm32f4xx.h>
#include <stdint.h>
#include <string.h>       // memset(), ...
#include "printf.h"       // Kustaa Nyholm's tinyprintf (printf.c)
#include "md380.h"
#include "irq_handlers.h" // hardware-specific stuff like "LCD_CS_LOW", etc
#include "lcd_driver.h"   // constants + API prototypes for the *alternative* LCD driver (no "gfx")

// Regardless of the command line, let the compiler show ALL warnings from here on:
#pragma GCC diagnostic warning "-Wall"


extern const uint8_t font_8_8[256*8]; // extra font with 256 characters from 'codepage 437'


//---------------------------------------------------------------------------
// Low-level LCD driver, completely bypasses Tytera's "gfx"
//  (but doesn't reconfigure the LCD controller so we can easily switch
//   back from the 'App Menu' into Tytera's menu, or the main screen)
//
// Based on a driver written by DL4YHF for a display with ST7735,
//   which seems to be quite compatible with the MD380 / RT3,
//   except for the 8-bit bus interface ("FSMC") and a few registers
//   in the LCD controller initialized by the original firmware .
//
// Note: Do NOT erase the screen (or portions of it) before drawing
//   into the framebuffer. That would cause an annoying flicker,
//   because this LCD controller doesn't support double buffering.
//   In other words, we always "paint" directly into the CURRENTLY
//   VISIBLE image - and painting isn't spectacularly fast !
//---------------------------------------------------------------------------

  // Internal defines (stuff not required in the header file) :
#define LCD_FSMC_ADDR_COMMAND 0x60000000
#define LCD_FSMC_ADDR_DATA    0x60040000 /* the extra address bit controls "REGISTER SELECT" */

  // Taken from HX8353-E datasheet (tnx K6BSD), actual chip in MD-380 is HX8302-A
#define LCD_CMD_NOP          0x00 // No Operation
#define LCD_CMD_SWRESET      0x01 // Software reset
#define LCD_CMD_RDDIDIF      0x04 // Read Display ID Info
#define LCD_CMD_RDDST        0x09 // Read Display Status
#define LCD_CMD_RDDPM        0x0a // Read Display Power
#define LCD_CMD_RDD_MADCTL   0x0b // Read Display
#define LCD_CMD_RDD_COLMOD   0x0c // Read Display Pixel
#define LCD_CMD_RDDDIM       0x0d // Read Display Image
#define LCD_CMD_RDDSM        0x0e // Read Display Signal
#define LCD_CMD_RDDSDR       0x0f // Read display self-diagnostic resut
#define LCD_CMD_SLPIN        0x10 // Sleep in & booster off
#define LCD_CMD_SLPOUT       0x11 // Sleep out & booster on
#define LCD_CMD_PTLON        0x12 // Partial mode on
#define LCD_CMD_NORON        0x13 // Partial off (Normal)
#define LCD_CMD_INVOFF       0x20 // Display inversion off
#define LCD_CMD_INVON        0x21 // Display inversion on
#define LCD_CMD_GAMSET       0x26 // Gamma curve select
#define LCD_CMD_DISPOFF      0x28 // Display off
#define LCD_CMD_DISPON       0x29 // Display on
#define LCD_CMD_CASET        0x2a // Column address set
#define LCD_CMD_RASET        0x2b // Row address set
#define LCD_CMD_RAMWR        0x2c // Memory write
#define LCD_CMD_RGBSET       0x2d // LUT parameter (16-to-18 color mapping)
#define LCD_CMD_RAMRD        0x2e // Memory read
#define LCD_CMD_PTLAR        0x30 // Partial start/end address set
#define LCD_CMD_VSCRDEF      0x31 // Vertical Scrolling Direction
#define LCD_CMD_TEOFF        0x34 // Tearing effect line off
#define LCD_CMD_TEON         0x35 // Tearing effect mode set & on
#define LCD_CMD_MADCTL       0x36 // Memory data access control
#define LCD_CMD_VSCRSADD     0x37 // Vertical scrolling start address
#define LCD_CMD_IDMOFF       0x38 // Idle mode off
#define LCD_CMD_IDMON        0x39 // Idle mode on
#define LCD_CMD_COLMOD       0x3a // Interface pixel format
#define LCD_CMD_RDID1        0xda // Read ID1
#define LCD_CMD_RDID2        0xdb // Read ID2
#define LCD_CMD_RDID3        0xdc // Read ID3

// Extended command set
#define LCD_CMD_SETOSC       0xb0 // Set internal oscillator
#define LCD_CMD_SETPWCTR     0xb1 // Set power control
#define LCD_CMD_SETDISPLAY   0xb2 // Set display control
#define LCD_CMD_SETCYC       0xb4 // Set dispaly cycle
#define LCD_CMD_SETBGP       0xb5 // Set BGP voltage
#define LCD_CMD_SETVCOM      0xb6 // Set VCOM voltage
#define LCD_CMD_SETEXTC      0xb9 // Enter extension command
#define LCD_CMD_SETOTP       0xbb // Set OTP
#define LCD_CMD_SETSTBA      0xc0 // Set Source option
#define LCD_CMD_SETID        0xc3 // Set ID
#define LCD_CMD_SETPANEL     0xcc // Set Panel characteristics
#define LCD_CMD_GETHID       0xd0 // Read Himax internal ID
#define LCD_CMD_SETGAMMA     0xe0 // Set Gamma
#define LCD_CMD_SET_SPI_RDEN 0xfe // Set SPI Read address (and enable)
#define LCD_CMD_GET_SPI_RDEN 0xff // Get FE A[7:0] parameter

uint8_t LCD_b12Temp[12];  // small RAM buffer for a self-defined character

uint8_t LCD_busy = 0; // busy from a drawing operation ? 0=no, >0=yes

//---------------------------------------------------------------------------
void LCD_EnterCriticalSection(void) // only call from 'API' !
{ ++LCD_busy;
}

void LCD_LeaveCriticalSection(void) // only call from 'API' !
{ if( LCD_busy )
   { --LCD_busy;
   }
}


//---------------------------------------------------------------------------
__attribute__ ((noinline)) void LCD_Delay(int nLoops) 
  // Waits for a few dozen nanoseconds, for LCD controller chip select, etc
# define DLY_500ns 4
# define DLY_200ns 2
# define DLY_50ns  0
{ // With nLoops=4, ca. 500 us delay, including call+return
  while(nLoops--)
   { asm("NOP"); // don't allow GCC to optimize this away !
   }
}

//---------------------------------------------------------------------------
__attribute__ ((noinline)) void LCD_WriteCommand( uint8_t bCommand )
  // I/O address taken from DL4YHF's dissassembly, @0x803352c in D13.020 .
{ *(volatile uint8_t*)LCD_FSMC_ADDR_COMMAND = bCommand;
}

//---------------------------------------------------------------------------
__attribute__ ((noinline)) void LCD_WriteData( uint8_t bData )
  // I/O address taken from DL4YHF's dissassembly, @0x8033534 in D13.020 .
{ *(volatile uint8_t*)LCD_FSMC_ADDR_DATA = bData;
}

//---------------------------------------------------------------------------
void LCD_ConfigureInterfaceForReading(void)
  // Configures the FSMC for the incredibly slow display READ-access
{
  // For the HX8302-A (used at least in MD380 and MD390-GPS),
  //  no timing parameters were known at the time of this writing.
  //  Thus the following is mainly based on trial-and-error,
  //  because the original firmware doesn't *read* from the display at all.
  // 
  // On the STM32F's side (using FSMC = "Flexible Static Memory Controller"), the
  //  READ-control output (LCD_RD) is on PD4. PD4 is already configured as FSMC_NOE
  //  by the original firmware (see http://www.qsl.net/dl4yhf/RT3/md380_hw.html#display ).
  //  The timing of the low-active 'NOE' ( Not Output Enable) is shown in ST's
  //  "RM0090", chapter 36.5.4, "NOR Flash/PSRAM controller asynchronous transactions".
  //  Sitronix' "TRDLFM" appears to be the sum of the STM32's "ADDSET" + "DATAST" times.
  //   "ADDSET" = Address setup time = bits  3..0 in FSMC_BTRx ,
  //   "DATAST" = Data setup time    = bits 15..8 in FSMC_BTRx .
  FSMC_Bank1->BTCR[1] = 0x101064F0; // aka "FSMC_BTR1" (RM0090 page 1579), here: for LCD READ-access
  //                    0x10100233; // original FSMC timing register (aka "FSMC_BTR1")
  //                      ||||\|||___ address setup time (4 bits)
  //                      |||| ||____ address hold time  (4 bits)
  //                      |||| |___ data setup time time (8 bits) : 0x0B gave about 150 ns
  //                      ||||____ turnaround time (write-to-read, read-to-write)
  //                      |||____ FSMC_CLK clock divider ratio. RM0090 page 1579 says
  //                      ||  "In asynchronous NOR Flash, SRAM or PSRAM accesses, this value is don’t care."
  //                      ||____ "data latency, only for synchronous memory"
  //                      |____ bits 29..28: "ACCMOD", 01bin = "access mode B" (~~ NOR flash)
} // LCD_ConfigureInterfaceForReading()

//---------------------------------------------------------------------------
void LCD_ConfigureInterfaceForWriting(void)
  // Switches back to 'faster' original FSMC settings for WRITING
{
  FSMC_Bank1->BTCR[1] = 0x10100233; // original FSMC timing register (aka "FSMC_BTR1")
} // LCD_ConfigureInterfaceForWriting()


//---------------------------------------------------------------------------
uint8_t LCD_ReadData( void )
  // Read data via FSMC
{ 
  uint8_t bResult;
  LCD_Delay(DLY_200ns); // gap between two READ-cycles must be some hundred nanoseconds !
  bResult = *(volatile uint8_t*)LCD_FSMC_ADDR_DATA;
  // The CPU already executes this code BEFORE the end of the LCD controller's internal READ-cycle.
  // Immediately reading the NEXT byte at this point always failed - see notes below .
  LCD_Delay(DLY_200ns); 
  return  bResult;
} // LCD_ReadData()


//---------------------------------------------------------------------------
__attribute__ ((noinline)) void LCD_WritePixels( uint16_t wColor, int nRepeats )
  // Not the same as Tytera's "gfx_write_pixel_to_framebuffer()" (@0x8033728) !
  // Only sends the 16-bit colour value, which requires much less accesses
  // to the external memory interface (which STM calls "FSMC"):
  //  9 accesses per pixel in the Tytera firmware,
  //  2 accesses per pixel in THIS implementation.
{ 
  while( nRepeats-- )
   { // ST7735 datasheet V2.1 page 39, "8-bit data bus for 16-bit/pixel,  
     //                 (RGB 5-6-5-bit input), 65K-Colors, Reg 0x3A = 0x05 " :
     LCD_WriteData( wColor >> 8 );  // 5 "blue" bits + 3 upper "green" bits  
     // __NOP;     // some delay between these two 8-bit transfers ?
     LCD_WriteData( wColor );       // 3 lower green bits + 5 "red" bits
     // Don't de-assert LCD_CS here yet ! In a character, 
     // more pixels will be emitted into the same output rectangle
   }
} // end LCD_WritePixels()


//---------------------------------------------------------------------------
__attribute__ ((noinline)) void LimitInteger( int *piValue, int min, int max)
{ int value = *piValue;
  if( value < min ) 
   {  value = min;
   }
  if( value > max )
   {  value = max;
   }
  *piValue = value;
}


//---------------------------------------------------------------------------
int LCD_SetOutputRect( int x1, int y1, int x2, int y2 )
  // Sets the coordinates before writing a rectangular area of pixels.
  // Returns the NUMBER OF PIXELS which must be sent to the controller
  // after setting the rectangle (used at least in  ) .
  // In the ST7735 datasheet (V2.1 by Sitronix), these are commands
  // "CASET (2Ah): Column Address Set" and "RASET (2Bh): Row Address Set".
  // Similar but inefficient code is @0x8033728 in D13.020,
  // shown in the disassembly as 'gfx_write_pixel_to_framebuffer()' .
{
  int caset_xs, caset_xe, raset_ys, raset_ye;

  // Crude clipping, not bullet-proof but better than nothing:
  LimitInteger( &x1, 0, LCD_SCREEN_WIDTH-1 );
  LimitInteger( &x2, 0, LCD_SCREEN_WIDTH-1 );
  LimitInteger( &y1, 0, LCD_SCREEN_HEIGHT-1 );
  LimitInteger( &y2, 0, LCD_SCREEN_HEIGHT-1 );
  if( x1>x2 || y1>y2 )
   { return 0;
   }

#if(0)  // one would expect this... 
  caset_xs = x1;
  caset_xe = x2;
  raset_ys = y1;
  raset_ye = y2;
#else   // but in an RT3, 'X' and 'Y' had to be swapped, and..
  caset_xs = y1;
  caset_xe = y2;
  raset_ys = 159-x2;  // horizontally mirrored: low  'start' value
  raset_ye = 159-x1;  // high 'end' value  
#endif


  LCD_CS_LOW;   // Activate LCD chip select
  LCD_Delay(DLY_200ns);
  LCD_WriteCommand( LCD_CMD_CASET ); // ST7735 DS V2.1 page 100
    // Beware: 'columns' and 'rows' from the ST7735's 
    //     point of view must be swapped, because the display initialisation
    //     (which still only takes place in TYTERA's part of the firmware),
    //     the possibility of rotating the display by 90° BY THE LCD CONTROLLER
    //     and mirroring coordinates as necessary are not used !
    // To 'compensate' this in software, x- and y-coordinates would have
    // to be swapped, and (much worse and CPU-hogging) the character bitmaps
    // would have to be rotated by 90° before being sent to the controller.
    //     The ST7735 datasheet (V2.1 by Sitronix) explains on pages 60..61 .
    //     how to control "Memory Data Write/Read Direction" via 'MADCTL' .
    //     The use the term "X-Y Exchange" for a rotated display,
    //     and there are EIGHT combinations of 'Mirror' and 'Exchange'.
    //     
    // The original firmware (D13.020 @ 0x08033590) uses different 'MADCTL' 
    //     settings, depending on some setting in SPI-Flash (?):
    //  * 0x08 for one type of display     : MADCTL.MY=0, MX=0, MV=0, ML=0, RGB=1
    //  * 0x48 for another type of display : MADCTL.MY=0, MX=1, MV=0, ML=0, RGB=1
  LCD_WriteData((uint8_t)(caset_xs>>8)); // 1st param: "XS15..XS8" ("X-start", hi)
  LCD_WriteData((uint8_t) caset_xs    ); // 2nd param: "XS7 ..XS0" ("X-start", lo)
  LCD_WriteData((uint8_t)(caset_xe>>8)); // 3rd param: "XE15..XE8" ("X-end",   hi)
  LCD_WriteData((uint8_t) caset_xe    ); // 4th param: "XE7 ..XE0" ("X-end",   lo)

  LCD_WriteCommand( LCD_CMD_RASET );     // ST7735 DS V2.1 page 102: four params..
  LCD_WriteData((uint8_t)(raset_ys>>8)); // 1st param: "YS15..YS8" ("Y-start", hi)
  LCD_WriteData((uint8_t) raset_ys    ); // 2nd param: "YS7 ..YS0" ("Y-start", lo)
  LCD_WriteData((uint8_t)(raset_ye>>8)); // 3rd param: "YE15..YE8" ("Y-end",   hi)
  LCD_WriteData((uint8_t) raset_ye    ); // 4th param: "YE7 ..YE0" ("Y-end",   lo)

  // Do NOT de-assert LCD_CS here yet !
  return (1+x2-x1) * (1+y2-y1);

} // end LCD_SetOutputRect()

//---------------------------------------------------------------------------
void LCD_SetPixelAt( int x, int y, uint16_t wColor )
  // Inefficient method to draw anything (except maybe 'thin BRESENHAM lines').
  // Similar to Tytera's 'gfx_write_pixel_to_framebuffer' @0x08033728 in D13.020 .
  // When not disturbed by interrupts, it took 40 ms to fill 160*128 pixels .
{

  // Timing measured with the original firmware (D13.020), 
  //  when setting a single pixel. Similar result with the C code below.
  //           __________   _   _   _   _   _   _   _   _   _________
  // /LCD_WR :           |_| |_| |_| |_| |_| |_| |_| |_| |_|
  //           ____       1   2   3   4   5   6   7   8   9   _____  
  // /LCD_CS :     |_________________________________________|
  // 
  //               |<-a->|<----------- 1.36 us ----------->|b|
  //               |<--------------- 1.93 us --------------->|
  //                   ->| |<- t_WR_low ~ 70ns             
  //             
  LCD_CS_LOW;  // Activate LCD chip select
  asm("NOP");  // (a) pro-forma delay between falling edge on LCD_CS 
               //     and the first low-active LCD_WR .
  LCD_WriteCommand( LCD_CMD_CASET ); // (1) ST7735 DS V2.1 page 100
#if(0) // For a "normally" mounted display, we would use THIS:
  LCD_WriteData((uint8_t)(x>>8)); // 1st CASET param: "XS15..XS8" ("X-start", hi)
  LCD_WriteData((uint8_t) x    ); // 2nd CASET param: "XS7 ..XS0" ("X-start", lo)
#else  // ... but for the unknown LCD controller in an RT3, Tytera only sends this:
  LCD_WriteData( (uint8_t)y    ); // (2) 1st CASET param: "XS15..XS8" ("X-start", hi)
  LCD_WriteData( (uint8_t)y    ); // (3) 2nd CASET param: "XS7 ..XS0" ("X-start", lo)
#endif // ST7735 or whatever-is-used in an MD380 / RT3 ?

  LCD_WriteCommand( LCD_CMD_RASET ); // (4) ST7735 DS V2.1 page 102
#if(0) // For a "normally" mounted display, we would use THIS:
  LCD_WriteData((uint8_t)(y>>8)); // 1st RASET param: "YS15..YS8" ("Y-start", hi)
  LCD_WriteData((uint8_t) y    ); // 2nd RASET param: "YS7 ..YS0" ("Y-start", lo)
#else  // ... but for the unknown LCD controller in an RT3, we need this:
  LCD_WriteData( (uint8_t)(159-x) ); // (5) 1st RASET param: "YS15..YS8" ("Y-start", hi)
  LCD_WriteData( (uint8_t)(159-x) ); // (6) 2nd RASET param: "YS7 ..YS0" ("Y-start", lo)
#endif // ST7735 or whatever-is-used in an MD380 / RT3 ?

  LCD_WriteCommand( LCD_CMD_RAMWR ); // (7) ST7735 DS V2.1 p. 104 : write to framebuffer
  LCD_WritePixels( wColor,1 ); // (8,9) send 16-bit colour in two 8-bit writes
  LCD_Delay(DLY_500ns); // (b) short delay before de-selecting the LCD controller .
  // Without this, there were erroneous red pixels on the screen during update.
  LCD_CS_HIGH; // de-assert LCD chip select


} // end LCD_SetPixelAt()


//---------------------------------------------------------------------------
void LCD_FillRect( // Draws a frame-less, solid, filled rectangle
        int x1, int y1,  // [in] pixel coordinate of upper left corner
        int x2, int y2,  // [in] pixel coordinate of lower right corner
        uint16_t wColor) // [in] filling colour (BGR565)
{
  int nPixels;
  
  LCD_EnterCriticalSection();

  // This function is MUCH faster than Tytera's 'gfx_blockfill' 
  //  (or whatever the original name was), because the rectangle coordinates
  // are only sent to the display controller ONCE, instead of sending 
  // a new coordinate for each stupid pixel (which is what the original FW did):
  nPixels = LCD_SetOutputRect( x1, y1, x2, y2 );  // send rectangle coordinates only ONCE
  if( nPixels<=0 ) // something wrong with the coordinates
   { LCD_LeaveCriticalSection();
     return;
   }

  LCD_WriteCommand( LCD_CMD_RAMWR ); // aka "Memory Write"

  // The ST7735(?) now knows where <n> Pixels shall be written,
  // so bang out the correct number of pixels to fill the rectangle:
  LCD_WritePixels( wColor, nPixels );

  LCD_Delay(DLY_500ns); // short delay before de-selecting the LCD controller .
  // Without this, there were occasional erratic pixels on the screen during update.
  LCD_CS_HIGH; // de-assert LCD chip select (Tytera does this after EVERY pixel. We don't.)
  LCD_LeaveCriticalSection();
} // end LCD_FillRect()


//---------------------------------------------------------------------------
int LCD_CopyRectFromFramebuffer_RGB( // Reads a rectangular area of pixels, 24 bit RGB.
        int x1, int y1,  // [in] pixel coordinate of upper left corner
        int x2, int y2,  // [in] pixel coordinate of lower right corner
        uint8_t *pbDest, // [out] buffer, must accept two bytes per pixel
        int sizeof_dest) // [in] sizeof(pbDest), for safety checks
  // When successful, returns THE NUMBER OF BYTES (!) actually placed in pbDest.
  //      Returns zero or a negative value when unable to do that,
  //      e.g. sizeof_dest too low, LCD-data-bus is "occupied", or whatever.
  //
  // The "Tytera-compatible" byte sequence is BLUE, GREEN, RED (!) per pixel.
  //  The same format is also used in bitmap files with 24 bits per pixel,
  //  Also compatible with HTML 'hex color' when treated like a 
  //  24-bit integer on little endian machine.
{
  int w,h,nBytes,nPixels;

  // Before sending any command to the LCD controller, check the buffer size:
  w = 1+x2-x1;
  h = 1+y2-y1;
  nBytes = 3 * w * h; // for pre-check..
  if( (nBytes<=0) || (nBytes>sizeof_dest) ) // something wrong, bail out
   { return -1;
   }

  LCD_EnterCriticalSection();

  // Buffer size looks ok, so tell the LCD controller to "start reading".
  // Simply reading pixels in the same 16-bit format as in LCD_WritePixels()
  // didn't work (only produced zeros). Reason possibly in ST7735 DS V2.1,
  // page 104 .... or maybe the HX8302A isn't sufficiently compatible:
  // > The Command 3Ah should be set to 66h when reading pixel data
  // > from frame memory. Please check the LUT in chapter 9.17 ...
  // Let's try this, even though 0x66 sets some undefined bits in "COLMOD" :
  LCD_WriteCommand( LCD_CMD_COLMOD ); // (1) ST7735 DS V2.1 page 115 : Interface Pixel Format
  LCD_WriteData( 0x66 );    // 110bin in bits 2..0 switches to 18-bit/pixel
  LCD_Delay(DLY_200ns);     // ?
  nPixels = LCD_SetOutputRect( x1, y1, x2, y2 ); // send CASET,RASET
  LCD_Delay(DLY_200ns);     // ?
  LCD_WriteCommand( LCD_CMD_RAMRD ); // ST7735 DS V2.1 page 105 : "Memory Read"
  LCD_Delay(DLY_200ns);     // how long to wait after sending "RAMRD" ? 
  LCD_ConfigureInterfaceForReading(); // configure FSMC for an awfully slow 'read'-timing
  LCD_ReadData();           // throw away ONE BYTE after "RAMRD"
  // In an ST7735, "RAMRD" only supports 18-bit-per-pixel (COLMOD set to 0x66) :
  // >     Note 1: The Command 3Ah should be set to 66h when reading pixel data
  // >             from frame memory. Please check the LUT in chapter 9.17 when
  // >             using memory read function.
  // In an HX8353-E (successor to the MD380's HX8302-A), things may be similar :
  //       The datasheet (HX8353-E V0.1 page 141, "Memory Read") only says
  //       'D[7:0] is read back from the frame memory' but doesn't 
  //       specify the pixel format (as set in COLMOD, or always 3 byte/pixel ?).
  //       Page 35, 'MCU Data Colour Coding for RAM data, READ' seems to confirm
  //       -similar to ST7735- an HX8353-E only supports reading 3 bytes/pixel .
  // READING pixels is *much* slower than writing. K6BSD reported 25 times slower.
#if(0) // Without Tytera's strange display rotation and mirroring, we'd use this:
  while( nPixels-- )
   { *pbDest++ = LCD_ReadData(); // 8 bit BLUE  (bits 1..0 unused)
     *pbDest++ = LCD_ReadData(); // 8 bit GREEN (bits 1..0 unused)
     *pbDest++ = LCD_ReadData(); // 8 bit RED   (bits 1..0 unused)
   }
#else // with Tytera's LCD-init, pixels must be rotated and horizontally mirrored
      // by software. Bleah... but that's the way it is: Terrible.
  int x,y;
  uint8_t *pbDest2;
  (void)nPixels; // suppress warning about 'set but not used'
  for( x=w-1; x>=0; --x) // read pixels from LCD controller
   { pbDest2 = pbDest+3*x + 3 * h;
     for( y=0; y<h; ++y) // .. rotated by 90° and horizontally mirrored
      { pbDest2 -= 3;
        pbDest2[0] = LCD_ReadData(); // 8 bit BLUE  (bits 1..0 unused)
        pbDest2[1] = LCD_ReadData(); // 8 bit GREEN (bits 1..0 unused)
        pbDest2[2] = LCD_ReadData(); // 8 bit RED   (bits 1..0 unused)
      }
   }
#endif // MADCTL set by Tytera's LCD-init ?
  LCD_Delay(DLY_500ns);
  LCD_ConfigureInterfaceForWriting(); // back to 'faster' FSMC settings for writing to the display

  // Switch back to 16-bit interface pixel format. ST7735 DS V2.1 page 115 :
  // > The Command 3Ah should be set at 55h when writing 16-bit/pixel data ..
  LCD_WriteCommand( LCD_CMD_COLMOD ); // "COLMOD" aka Interface Pixel Format
  LCD_WriteData( 0x55 );    // back from 18-bit to 16-bit format
    // (In the MD380 the LCD interface is only 8 bit wide.
    //  ST7735 DS V2.1 page 40 shows how "18-bit pixels" would travel
    //  over an 8-bit parallel interface. Too wasteful, 65k colours are enough.
    //  Who wants to waste an extra BYTE to have just "two more bits" of colour.
    //  Thus switching back to "16-bit pixels" here. Also used by Tytera's 'gfx'.)

  LCD_Delay(DLY_500ns); // short delay before de-selecting the LCD controller .
  LCD_CS_HIGH;          // de-assert LCD chip select (never forget !)

  // back to original settings (FSMC,GPIO) for Tytera's original LCD driver:

  LCD_LeaveCriticalSection();

  return nBytes; // <- number of BYTES actually placed in the caller's buffer

} // LCD_CopyRectFromFramebuffer()


//---------------------------------------------------------------------------
void LCD_HorzLine( // Draws a thin horizontal line ..
        int x1, int y, int x2, uint16_t wColor)
{ LCD_FillRect( x1, y, x2, y, wColor ); // .. just a camouflaged 'fill rectangle'
} // end LCD_HorzLine()

//---------------------------------------------------------------------------
void LCD_ColorGradientTest(void)
  // Fills the framebuffer with a 2D colour gradient for testing .
  // If the colour-bit-fiddling below is ok, the display
  // should be filled with colour gradients :
  //       ,----------------, - y=0
  //       |Green ...... Red|
  //       | .            . |
  //       | .            . |
  //       |Cyan Blue Violet|
  //       '----------------' - y=127
  //       |                |
  //      x=0              x=159
  // 
{ int x,y;
  for(y=0; y<128; ++y)
   { for(x=0; x<160; ++x)
      { 
        // 'Native' colour format, found by trial-and-error:
        //   BGR565 = 32 levels of BLUE  (in bits 15..11), 
        //            64 levels of GREEN (in bits 10..5),
        //            32 levels of RED   (in bits 4..0).
        LCD_SetPixelAt( x,y, 
           (x/5) |         // increasing from left to right: RED
         ((63-(2*x)/5)<<5) // increasing from right to left: GREEN
         |((y/4)<<11) );   // increasing from top to bottom: BLUE
      }
   }
} // end LCD_ColorGradientTest()

//---------------------------------------------------------------------------
uint32_t LCD_NativeColorToRGB( uint16_t native_color )
  // Converts a 'native' colour (in the display's hardware-specific format)
  // into red, green, and blue component; each ranging from 0 to ~~255 .
{
  rgb_quad_t rgb;
  int i;
  i = (native_color & LCD_COLOR_RED) >> LCD_COLORBIT0_RED; // -> 0..31 (5 'red bits' only)
  i = (255*i) / 31;
  rgb.s.r = (uint8_t)i; 
  i = (native_color & LCD_COLOR_GREEN)>> LCD_COLORBIT0_GREEN; // -> 0..63 (6 'green bits')
  i = (255*i) / 63;
  rgb.s.g = (uint8_t)i;
  i = (native_color & LCD_COLOR_BLUE) >> LCD_COLORBIT0_BLUE; // -> 0..31 (5 'blue bits')
  i = (255*i) / 31;
  rgb.s.b = (uint8_t)i;
  rgb.s.a = 0; // clear bits 31..24 in the returned DWORD (nowadays known as uint32_t)
  return rgb.u32;
} // end LCD_NativeColorToRGB()

//---------------------------------------------------------------------------
uint16_t LCD_RGBToNativeColor( uint32_t u32RGB )
  // Converts an RGB mix (red,green,blue ranging from 0 to 255)
  // into the LCD controller's hardware specific format (here: BGR565).
{
  rgb_quad_t rgb;
  rgb.u32 = u32RGB;
  return ((uint16_t)(rgb.s.r & 0xF8) >> 3)  // red  : move bits 7..3 into b 4..0 
       | ((uint16_t)(rgb.s.g & 0xFC) << 3)  // green: move bits 7..2 into b 10..5
       | ((uint16_t)(rgb.s.b & 0xF8) << 8); // blue : move bits 7..3 into b 15..11
} // end LCD_RGBToNativeColor()

//---------------------------------------------------------------------------
int LCD_GetColorDifference( uint16_t color1, uint16_t color2 )
  // Crude measure for the "similarity" of two colours:
  // Colours are split into R,G,B (range 0..255 each), 
  // then absolute differences added. 
  // Theoretic maximum 3*255, here slightly less (doesn't matter).
{ rgb_quad_t rgb[2];
  int delta_r, delta_g, delta_b;
  rgb[0].u32 = LCD_NativeColorToRGB( color1 );
  rgb[1].u32 = LCD_NativeColorToRGB( color2 );
  delta_r = (int)rgb[0].s.r - (int)rgb[1].s.r;
  delta_g = (int)rgb[0].s.g - (int)rgb[1].s.g;
  delta_b = (int)rgb[0].s.b - (int)rgb[1].s.b;
  if( delta_r<0) delta_r = -delta_r;  // abs() may be a code-space-hogging macro..
  if( delta_g<0) delta_g = -delta_g;  // .. and there are already TOO MANY dependencies here
  if( delta_b<0) delta_b = -delta_b;
  return delta_r + delta_g + delta_b;
} // end LCD_GetColorDifference()

//---------------------------------------------------------------------------
uint16_t LCD_GetGoodContrastTextColor( uint16_t backgnd_color )
  // Returns either LCD_COLOR_BLACK or LCD_COLOR_WHITE,
  // whichever gives the best contrast for text output
  // using given background colour. First used in color_picker.c .
{
  rgb_quad_t rgb;
  rgb.u32 = LCD_NativeColorToRGB( backgnd_color );
  if( ((int)rgb.s.r + (int)rgb.s.g + (int)rgb.s.b/2 ) > 333 )
       return LCD_COLOR_BLACK;
  else return LCD_COLOR_WHITE;
} 

//---------------------------------------------------------------------------
uint8_t *LCD_GetFontPixelPtr_8x8( uint8_t c)
  // Retrieves the address of a character's font bitmap, 8 * 8 pixels .
  //  [in] 8-bit character, most likely 'codepage 437'
  //       (details on this ancient 'VGA'-font in applet/src/font_8_8.c) .
  // Hint: Print out the image applet/src/font_8_8.bmp to find the hex code
  //       of any of CP437's "line drawing" character easily :
  //       table row = upper hex digit, table column = lower hex digit.
  //       Welcome back to the stoneage with fixed font 'text-only' screens :o)
{
  return (uint8_t*)(font_8_8 + 8*c);
} // end LCD_GetFontPixelPtr_8x8()

//---------------------------------------------------------------------------
uint8_t *LCD_GetFontPixelPtr_6x12( uint8_t c)
  // Retrieves the address of a character's font bitmap, 6 * 12 pixels .
  //  [in]     7-bit ASCII character, codes #32 to #127 .
  //           Codes <= 31 are reserved for special purposes,
  //           for example to create user-defined chars 'on the fly'.
  //  [return] pointer to the first byte of the character's bitmap.
  //           Never returns NULL (but a 'replacement' if c is invalid).
{
  if( c<32 || c>127 )
   {  c=32;
   }

#if defined(FW_D02_032)
# define FONT_MATRIX_FIRST_ADDR 0x080FB68C /* see long explanation above */ 
#endif
#if defined(FW_D13_020)
# define FONT_MATRIX_FIRST_ADDR 0x080F9F8C /* 'small' font matrix in D13.020 */ 
  // In the disassembled FW D13.020, "font_small_table_at_offset_0x18" was:
  // > [0x080faa48 0% 448 ../../firmware/unwrapped/D013.020.img]
  // > pxw @ font_small_table_at_offset_0x18
  // > 0x080faa48:  0x000a000a 0x080633f8 0x080faa3c 0xf7fef7a1
  //                           |________|
  //       ,--------<<<<------------'
  //       |       points to a table with 8 bytes per entry:
  //      \|/
  // > 0x080633f8: 0x00010606 0x080f9f68 0x00010606 0x080f9f80 (<- not sure what these are)
  // > 0x08063408: 0x00010606 0x080f9f8c 0x00010606 0x080f9f98
  //                          |___' '__|            |__'!'___|
  //                                    \_delta=12_/
#endif // FW_D13_020 ?
#if defined(FW_S13_020)
# define FONT_MATRIX_FIRST_ADDR 0x080FAE4C 
#endif // FW_S13_020 ?
#ifdef FONT_MATRIX_FIRST_ADDR
  return (uint8_t*)(FONT_MATRIX_FIRST_ADDR + 12 * (int)(c-32) );
#else
# error "Please add support for the new firmware here !"
  // (see explanation above how to 'find' the address of the font's first bitmap)
#endif

} // end LCD_GetFontPixelPtr_6x12()

//---------------------------------------------------------------------------
int LCD_GetFontHeight( int font_options )
{
  int font_height = ( font_options & LCD_OPT_FONT_8x8 ) ? 8 : 12;
  if( font_options & LCD_OPT_DOUBLE_HEIGHT )
   {  font_height *= 2;
   }
  return font_height;
} // end LCD_GetFontHeight()

//---------------------------------------------------------------------------
int LCD_GetCharWidth( int font_options, char c )
{
  int width = ( font_options & LCD_OPT_FONT_8x8 ) ? 8 : 6;
  // As long as all fonts supported HERE are fixed-width, 
  // ignore the character code ('c')  without a warning :
  (void)c;

  if( font_options & LCD_OPT_DOUBLE_WIDTH )
   {  width *= 2;
   }
  return width;
} // end LCD_GetCharWidth()

//---------------------------------------------------------------------------
int LCD_GetTextWidth( int font_options, char *pszText )
{ 
  int text_width = 0;
  if( pszText != NULL )
   { while(*pszText)
      { text_width += LCD_GetCharWidth(font_options, *(pszText++) );
      }
   }
  else // special service for lazy callers: NULL = "average width of ONE character"
   { text_width = LCD_GetCharWidth(font_options, 'A' );
   }
  return text_width;
} // end LCD_GetTextWidth()

//---------------------------------------------------------------------------
int LCD_DrawCharAt( // lowest level of 'text output' into the framebuffer
        char c,            // [in] character code (ASCII)
        int x, int y,      // [in] pixel coordinate
        uint16_t fg_color, // [in] foreground colour (BGR565)
        uint16_t bg_color, // [in] background colour (BGR565)
        int options )      // [in] LCD_OPT_... (bitwise combined)
  // Returns the graphic coordinate (x) to print the next character .
  // 
  // Speed: *MUCH* higher than with Tytera's original code (aka 'gfx'),
  //        for reasons explained in LCD_SetOutputRect() .
  // 
  // Drawing a zoomed character with 12 * 24 pixels took about 160 microseconds.
  // A screen with 5*13 'large' characters was filled in less than 11 ms.
  // Despite that, only redraw the screen when necessary because the QRM
  // from the display connector cable was still audible in an SSB receiver.
{
  uint8_t *pbFontMatrix;
  int x_zoom = ( options & LCD_OPT_DOUBLE_WIDTH ) ? 2 : 1;
  int y_zoom = ( options & LCD_OPT_DOUBLE_HEIGHT) ? 2 : 1;
  int font_width,font_height,x2,y2;
  if( options & LCD_OPT_FONT_8x8 )  // use the 8*8 pixel font ?
   { pbFontMatrix = LCD_GetFontPixelPtr_8x8(c);
     font_width = font_height = 8;
   }
  else // use Tytera's 6 * 12 pixel font (with characters #32 .. 127 only)
   { pbFontMatrix = LCD_GetFontPixelPtr_6x12(c);
     font_width  = 6;
     font_height = 12;
   }
  x2 = x + x_zoom*font_width-1;
  y2 = y + y_zoom*font_height-1;
  if(x2 >=LCD_SCREEN_WIDTH )
   { x2 = LCD_SCREEN_WIDTH-1;
     // Kludge to avoid an extra check in the loops further below:
     font_width = (1+x2-x) / x_zoom;
     x2 = x + x_zoom*font_width-1; // <- because the number of pixels
     // sent in the loops below must exactly match the rectangle sent
     // to the LCD controller via LCD_SetOutputRect()  !
     // This way, characters are truncated at the right edge.
   }
  if(y2 >=LCD_SCREEN_HEIGHT ) // similar kludge to truncate at the bottom
   { y2 = LCD_SCREEN_HEIGHT-1;
     font_height = (1+y2-y) / y_zoom;
     y2 = y + y_zoom*font_height-1;
   }

  LCD_EnterCriticalSection();  


  // Instead of Tytera's 'gfx_drawtext' (or whatever the original name was),
  // use an important feature of the LCD controller (ST7735 or similar) :
  // Only set the drawing rectangle ONCE, instead of sending a new coordinate
  // to the display for each pixel (which wasted time and caused avoidable QRM):
  if( LCD_SetOutputRect( x, y, x2, y2 ) <= 0 ) 
   { LCD_LeaveCriticalSection();
     return x; // something wrong with the graphic coordinates 
   }
  LCD_WriteCommand( LCD_CMD_RAMWR ); // begin writing to framebuffer

  // Without Tytera's strange display rotation and mirroring, we'd use this:
  // for( y=0; y<font_height; ++y)
  //  { for( x=0; x<font_width; ++x)
  //     {  ...
  // But in an RT3/MD380 (with D13.020), the pixels must be rotated and mirrored.
  for( x=font_width-1; x>=0; --x)  // read 'monochrome pixel' from font bitmap..
   { x_zoom = ( options & LCD_OPT_DOUBLE_WIDTH ) ? 2 : 1;
     while(x_zoom--)
      { for( y=0; y<font_height; ++y) // .. rotated by 90° and horizontally mirrored
         { if( pbFontMatrix[y] & (0x80>>x) ) 
            { LCD_WritePixels( fg_color, y_zoom ); // approx. 1us per double-width pixel
            }
           else
            { LCD_WritePixels( bg_color, y_zoom );
            }
         }
      }
   }
  LCD_Delay(DLY_500ns); // short delay before de-selecting the LCD controller (see LCD_FillRect)
  LCD_CS_HIGH; // de-assert LCD chip select (Tytera does this after EVERY pixel. We don't.)
  LCD_LeaveCriticalSection();
  return x2+1; // pixel coord for printing the NEXT character
} // end LCD_DrawCharAt()

//---------------------------------------------------------------------------
void LCD_InitContext( lcd_context_t *pContext )
  // Clears the struct and sets the output clipping window to 'full screen'.
{
  memset( pContext, 0, sizeof( lcd_context_t ) );
  pContext->x2 = LCD_SCREEN_WIDTH-1;
  pContext->y2 = LCD_SCREEN_HEIGHT-1;
} // end LCD_InitContext()

//---------------------------------------------------------------------------
int LCD_DrawString( lcd_context_t *pContext, char *cp )
  // Draws a zero-terminated ASCII string. Should be simple but versatile. 
  //  [in]  pContext, especially pContext->x,y = graphic output cursor .
  //        pContext->x1,x1,x2,y2 = clipping area (planned) .
  //  [out] pContext->x,y = graphic coordinate for the NEXT output .
  //        Return value : horizontal position for the next character (x).
  // For multi-line output (with '\r' or '\n' in the string),
  //     the NEXT line for printing is stored in pContext->y .
  // ASCII control characters with special functions :
  //   \n (new line) : wrap into the next line, without clearing the rest
  //   \r (carriage return) : similar, but if the text before the '\r'
  //                   didn't fill a line on the screen, the rest will
  //                   be filled with the background colour 
  //      (can eliminate the need for "clear screen" before printing, etc)
  //   \t (horizontal tab) : doesn't print a 'tab' but horizontally 
  //                   CENTERS the remaining text in the current line
{
  int fh = LCD_GetFontHeight(pContext->font);
  int w;
  int left_margin = pContext->x; // for '\r' or '\n', not pContext->x1 !
  int x = pContext->x;
  int y = pContext->y;
  unsigned char c;
  unsigned char *cp2;
  while( (c=*cp++) != 0 )
   { switch(c)
      { case '\r' :  // carriage return : almost the same as 'new line', but..
           // as a service for flicker-free output, CLEARS ALL UNTIL THE END
           // OF THE CURRENT LINE, so clearing the screen is unnecessary.
           LCD_FillRect( x, y, pContext->x2, y+fh-1, pContext->bg_color );
           // NO BREAK HERE !           
        case '\n' :  // new line WITHOUT clearing the rest of the current line
           x = left_margin;
           y += fh;
           break;
        case '\t' :  // horizontally CENTER the text in the rest of the line
           w = 0; // measure the width UNTIL THE NEXT CONTROL CHARACTER:
           cp2 = (unsigned char*)cp;
           while( (c=*cp2++) >= 32 )
            { w += LCD_GetCharWidth( pContext->font, c ); // suited for proportional fonts (which we don't have here yet?)
            }
           w = (pContext->x2 - x - w) / 2; // "-> half remaining width"
           if( w>0 )
            { LCD_FillRect( x, y, x+w-1, y+fh-1, pContext->bg_color );
              x += w;
            }
           break;
        default   :  // anything should be 'printable' :
           x = LCD_DrawCharAt( c, x, y, pContext->fg_color, pContext->bg_color, pContext->font );
           break;
      }
   }
  // Store the new "output cursor position" (graphic coord) for the NEXT string:
  pContext->x = x;
  pContext->y = y;
  return x;
} // end LCD_DrawString()

//---------------------------------------------------------------------------
int LCD_Printf( lcd_context_t *pContext, char *fmt, ... )
  // Almost the same as LCD_DrawString,
  // but with all goodies supported by tinyprintf .
  // Total length of the output should not exceed 80 characters.
{
  char szTemp[84]; // how large is the stack ? Dare to use more ?
  va_list va;
  va_start(va, fmt);
  va_snprintf(szTemp, sizeof(szTemp)-1, fmt, va );    
  va_end(va); 
  szTemp[sizeof(szTemp)-1] = '\0';     
  return LCD_DrawString( pContext, szTemp );
} // end LCD_Printf()



/* EOF < md380tools/applet/src/lcd_driver.c > */