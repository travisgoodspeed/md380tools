/*! \file config.h
  \brief Configuration options.
  
  This file configures our patches, either to temporarily enable
  unstable features or to disable standard features when porting to a
  new target application version.
  
  The default version of this file, as found in the repository, is to
  be used for all external releases.
*/


/* Basic features that ought to be working in 2.032 but might be
   missing in later revisions. */
#define CONFIG_SPIFLASH
#define CONFIG_GRAPHICS
#define CONFIG_SPIC5000
#define CONFIG_AMBE
#define CONFIG_DMR
#define CONFIG_MENU
#define CONFIG_AES
#ifdef FW_D13_020 // so far, the 'dimmable backlight' was only tested with FW D13.020 (non-GPS unit)
# define CONFIG_DIMMED_LIGHT 1 // PWMed 'dimmable backlight' ? 0 or undefined = no, 1 = yes
#endif


/* Uncomment this to print AMBE frames for decoding with DSD.  You
   probable want this instead of AMBECORRECTEDPRINT or
   AMBEUNCORRECTEDPRINT below.
*/
//#define AMBEPRINT

/* This one prints all incoming audio as WAV.  Very verbose, but it
   allows for nice clean recordings of DMR audio.
*/
//#define AMBEWAVPRINT

/* Uncomment these to enable printing raw frames, either before or
   after error correction is applied. */
//#define AMBECORRECTEDPRINT
//#define AMBEUNCORRECTEDPRINT

//#define I2CPRINT
