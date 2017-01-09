/*! \file irq_handlers.c
    \brief Own interrupt handler(s), first used for a 'dimmed backlight' .

  Module prefix : "IRQ_", but the patched IRQ handlers begin with 'our',
                  followed by the official handler name (e.g. "SysTick_Handler").

  Started 2016-12 by DL4YHF as a 'proof of concept' .
  Tries to drives the 'backlight' LEDs (for the TFT and keyboard)
  with a pulse-width modulated driving signal.
  
  Contains a few (!) hijacked interrrupt handlers,
   depending on which options are selected in md380tools/applet/config.h .
  
  Preliminary details / documentation / hardware details at 
     www.qsl.net/dl4yhf/RT3/md380_fw.html#dimmed_light .
     
  To include the 'dimmed backlight' feature in YOUR patched firmware, 
    add the following line in ?/md380tools/applet/Makefile (after SRCS += beep.o) :
    SRCS += irq_handlers.o 
  Furthermore,
    #define CONFIG_DIMMED_LIGHT 1 // in  ?/md380tools/applet/config.h  .
    
  Also, remove src/stm32f4xx_it.c from the project/make (applet/Makefile) .
   (stm32f4xx_it.c is useless, because there are already 
    all necessary 'weak' default handlers 
    in startup_XYZ.S, for example a *weak* SysTick_Handler().
    Only those *weak* default handlers can easily be overwritten 
    by implementing it HERE (in *this* sourcecode module. 
    The purpose of stm32f4xx_it.c was unclear 
    (why "occpy" all handlers without implementing anything there ?)
    
  Then, with the CWD set to md380tools, issue command
    make clean image_D13
    (at the time of this writing, only the "D13" firmware patch was supported here).
    
  Watch for warnings in the messages from the C compiler 
    (at least when compiling THIS module, there shouldn't be any warning at all).
  When our SysTick_Handler() replaced Tytera's SysTick_Handler() (as in the 1st attempt),
   the following patch was necessary in merge_d13.020.py :
   
    # DL4YHF 2017-01-05 (quite prosaic, since this was the first 'vector table patch'):
    # IF   the applet's symbol table contains a function named 'SysTick_Handler',
    # THEN patch its address, MADE ODD to indicate Thumb-code, into the
    # interrupt-vector-table as explained in applet/src/irq_handlers.c :
    # ex: new_adr = sapplet.getadr("SysTick_Handler"); # threw an exception when "not found".
    new_adr = sapplet.try_getadr("SysTick_Handler");
    if new_adr != None:
        vect_adr = 0x800C03C;  # address within Tytera's VT for SysTick_Handler
        exp_adr  = 0x8093F1D;  # expected 'old' content of the above VT entry (LSBit set for THUMB)
        old_adr  = merger.getword(vect_adr); # original content of the VT entry
        new_adr |= 0x0000001;  # new content in this VT entry must also be an ODD address !
        if( old_adr == exp_adr ) :
           print "Patching SysTick_Handler in VT addr 0x%08x," % vect_adr;
           print "  old value in vector table = 0x%08x," % old_adr;
           print "   expected in vector table = 0x%08x," % exp_adr;
           print "  new value in vector table = 0x%08x." % new_adr;
           merger.setword( vect_adr, new_adr, old_adr);
           print "  SysTick_Handler successfully patched.";
        else:
           print "Cannot patch SysTick_Handler() !";
    else:
           print "No SysTick_Handler() found in the symbol table. Building firmware without.";    

           
   Insert the above Python fragment before ' print "Merging %s into %s at %08x" % ... ',
   and watch the output when invoking 
    > make image_D13
   Depending on where the linker has located our SysTick_Handler(), 
   the output will look similar to this :
   
         DEBUG: reading "patched.img"
         make[2]: Leaving directory '/c/tools/md380tools/patches/d13.020'
         cp ../patches/d13.020/patched.img ./merged.img
         python2 merge_d13.020.py merged.img applet.img 0x0809b000
         Merging an applet.
         Loading symbols from applet.img.sym
         Inserting a stub hook at 0800c72e to 0809c635.
         Patching SysTick_Handler in VT addr 0x0800c03c,
           old value in vector table = 0x08093f1d,
            expected in vector table = 0x08093f1d,
           new value in vector table = 0x080a04fd.
           SysTick_Handler successfully patched.
         Merging applet.img into merged.img at 0809b000
         ../md380-fw --wrap merged.img wrapped.bin
         DEBUG: reading "merged.img"
         INFO: base address 0x800c000
         INFO: length 0xf3000
         DEBUG: writing "wrapped.bin"
         cp wrapped.bin experiment.bin
         make[1]: Leaving directory '/c/tools/md380tools/applet'   
  
  Finally, test the patch by uploading it into the radio:
  
  > $ python md380_dfu.py upgrade applet/experiment.bin
  
  
*/

#include "config.h"

#include <stm32f4xx.h>   // only need this single header (not the fancy bulky stuff from the "STMCubeMX" ! )
#include <string.h>
#include "printf.h"
#include "dmesg.h"
#include "md380.h"
#include "version.h"
#include "printf.h"
#include "spiflash.h"
#include "addl_config.h"
#include "radio_config.h"
#include "syslog.h"
#include "usersdb.h"
#include "keyb.h"         // contains "backlight_timer", which seems to be Tytera's own software-timer. Does it COUNT DOWN TO ZERO ?

#include "irq_handlers.h" // header for THIS module

// Module-internal options :
#define L_USE_UART6_AS_PWM_GENERATOR 1 /* 0=no (use PC6 as GPIO),  1=yes (use PC6 as USART6_TX to send PWM-like bit patterns) */


typedef void(*T_VoidFunctionPtr)(void);


#if( CONFIG_DIMMED_LIGHT )
volatile uint32_t IRQ_dwSysTickCounter = 0; // Incremented each 1.5 ms. Rolls over from FFFFFFFF to 0 after 74 days
#endif // CONFIG_DIMMED_LIGHT ?



#if( CONFIG_DIMMED_LIGHT /* || .. */ )

//---------------------------------------------------------------------------
void IRQ_Init(void)
{
  // 2017-01-05 : Tried to reprogram VTOR to avoid having to patch Tytera's 
  //              vector table. But this made the firmware crash somewhere,
  //              so stick to the old method of patching THEIR vector table
  //              to use OUR SysTick_Handler .  
  // 2017-01-07 : Tried to improve the PWM resultion with the lowest possible
  //              interrupt rate (to keep the CPU load low) by abusing
  //              USART6 (with TX on PC6 = "Lamp" = backlight) for PWM generation.
  //   
#if( L_USE_UART6_AS_PWM_GENERATOR )
  USART_TypeDef *pUSART = USART6; // load the UART's base address into register (once)

  RCC->APB2ENR |=  RCC_APB2ENR_USART6EN;   // enable internal clock for UART6 (from 'APB2')
  RCC->APB2RSTR &= ~RCC_APB2RSTR_USART6RST; // bring USART6 "out of reset" (RM0090 Rev7 page 176)

  // Turn pin "PC6" from a GPIO into "UART6_TX". No need for a bloated struct ("GPIO_InitTypeDef") for this:
  // Two bits in "MODER" per pin : 00bin for GPIO, 10bin for 'alternate function mode'. RM0090 Rev7 page 278 :  
  GPIOC->MODER  /*4002800*/ = ( GPIOC->MODER & ~(3 << (6/*pinpos*/ * 2) ) ) |  (2 << (6/*pinpos*/ * 2) );
  // GPIOC->MODER &= ~(3 << (6/*pinpos*/ * 2) ); // MODER it.. to use PC6 as GPIO
  // GPIOC->BSRRL = (1<<6);  // turn BL on (don't affect any other register)
  
  // Two bits in "OSPEEDR" per pin : 00bin for the 'lowest speed', to cause the lowest possible RFI                                  
  GPIOC->OSPEEDR/*4002808*/ &= ~(3 << (6/*pinpos*/ * 2) );  // RM0090 Rev7 page 279
  
  // One bit per pin in "OTYPER" to select open drain or push/pull output mode:
  GPIOC->OTYPER /*4002804*/ &= ~(1<<6);  // RM0090 Rev7 page 279 : Low for push-pull
  
  // Two bits in "PUPDR" per pin for the Pull-up / Pull down resistor configuration. 00bin = none
  GPIOC->PUPDR  /*400280C*/ &= ~(3 << (6/*pinpos*/ * 2) );  // RM0090 Rev7 page 280
  
  // Tell "PC6" which of the 16 alternate functions it shall have from: USART6_TX . RM0090 Rev7 page 283.
  // There are FOUR bits per pin in AFR[0..1], thus PC6 is in AFR[0] bits 27..24. USART6_TX = "AF8" (STM32F405/7 DS page 62).
  GPIOC->AFR[0] = (GPIOC->AFR[0] & ~(0x0F << (6/*pinpos*/ * 4 ) ) )  |  (0x08 << (6/*pinpos*/ * 4) );

  // To check if any of these settings get "overwritten" by Tytera's part of the firmware,
  //    many registers can be inspected (a bit like Keil's PERIPHERAL VIEW) via Python:
  //  > python tool2.py hexdump32 GPIOC 64
  //                  |"MODER6" (bits 13,12) : 10bin for "not GPIO but alternate function mode"
  //                 \|/               
  //  > 40020800: 06A565C0 00000080 0AAA8A00 00000000   ("MODER", "OTYPER", "OSPEEDER", "PUPDR")
  //  > 40020810: 00000207 00000280 00000000 00000000   ("IDR",   "ODR",    "BSRR",     "LCKR" )
  //  > 40020820: 08000000 00065603 00000000 00000000   ("AFRL",  "AFRH",    -            -    )
  //               |__ AFR[0] bits 27..24 : Alternate Function #8, here: "UART6_TX" on PC6

  
  // Init the "USART" itself. Use ST's 'library' (lib/in/peripherals/stm32f4xx_usart)
  // or follow the manual, step-by-step, to initialize the UART ("TX only", no DMA, no buffers) ?
  // To keep the precious code size low, WB decided to init the UART "by hand". Page numbers from RM0090 Rev7 .
  // Since we don't really care for the precise baudrate, etc etc, this is almost trivial ...
  pUSART->CR2 /*pg 997*/ = 0; /* "normal" UART, no LIN, no clock pin, 1 stopbit (also possible: 0.5 !!) */
  pUSART->CR1 /*pg 997*/ = (1/*enable*/<<13) | (1/*9bit-mode*/<<12) | (1/*TE*/<<3); /* 1 startbit, 9 data bits, no parity, no interrupts, "OVER8"=0 */
  pUSART->CR3 /*pg 997*/ = 0; /* no "onebit", no CTS-IE, no CTS, no RTS, no DMA, no smartcard, no nothing */ 
  pUSART->GTPR/*pg 1001*/= 15; /* no "Guard time",  prescaler=1(b7..0), etc */
  // Baudrate (pg. 964) = f_clk / ( 8 * (2-"OVER8") * USARTDIV ).   
  // USARTDIV is a FIXED-POINT number in BRR.  Mantissa in bits 15..4, fraction in bits 3..0 (not used here)
  // We'll feed ONE BYTE into the tx register in every 4th to 8th SysTick-interrupt, 
  // thus maximum = 666 Hz / 4 = 166 Hz PWM base freq; a character must be sent within 1/(166Hz) = 6 ms . 
  // That's 10 bit / 6 ms = 1666 baud, PWM resolution = 0.6 ms.
  // The UART's peripheral clock (from APB2) was unknown at the time of this writing. We cannot change it anyway.
  // Trial-and-error, a photocell, and an oscilloscope(!) was used to find a value where sending
  // a string of 0xF0 0xF0 0xF0 0xF0 characters, and looking at the FREQUENCY from the photocell.
  // Test results : 
  //   pUSART->GTPR=15,  USARTx->BRR=0x7FFF -> measured 'byte'-frequency = 220 Hz .
  pUSART->BRR /*pg 994*/ = 0x7FFF;
  
  // To check if any of these settings get "overwritten" by Tytera's part of the firmware,
  //    many registers can be also inspected via Python:
  //                                              _"UE" (bit13 set : UART enabled)
  //  > $ python tool2.py hexdump32 USART6 32    |   __ "TE" (bit 3 set : transmit enabled)
  //                                             |  |
  //  > 40011400: 00000000 00000000 0000FFFF 00002008  ("USART_SR", "USART_DR", "USART_BRR", "USART_CR1")
  //  > 40011410: 00000000 00000000 00000001 00000000  ("USART_CR2","USART_CR3","USART_GTPR",  ---      )

# endif // L_USE_UART6_AS_PWM_GENERATOR ?
    
  
} // IRQ_Init()
#endif // ( CONFIG_DIMMED_LIGHT /* || .. */ ) ?


#if( CONFIG_DIMMED_LIGHT ) // do we need 'our' SysTick_Handler() ?
//---------------------------------------------------------------------------
void SysTick_Handler(void)
  // ISR to generate the PWM'ed output for the backlight (and maybe more) .
  // Called instead of the original SysTick_Handler(), but only if we ..
  //   (a) "patched" SysTick_Handler() in the original firmware (bleah..)
  //         - see md380tools/applet/merge_d13.020.py
  //   (b) use OUR OWN vector table (-> VTOR) and override the "weak" 
  //       default handler in src/startup_stm32f4xx_D13_020.S (preferred).
  //
  // Note: Simply implementing SysTick_Handler() here isn't enough !
  // Trying to swap interrupt vector tables via VTOR didn't seem to work,
  // so the address of *OUR* SysTick_Handler must be patched into the ORIGINAL vector table.
  // Check the modified Python script's output during 'make', e.g.:
  // > Patching SysTick_Handler in VT addr 0x0800c03c,
  // >   old value in vector table = 0x08093f1d,
  // >    expected in vector table = 0x08093f1d,
  // >   new value in vector table = 0x080a02b9.  <-- address of "our handler" PLUS ONE because it's Thumb !
  // >   SysTick_Handler successfully patched.    <-- lengthy output from DL4YHF's modified merge_d13.020.py
  
{
  uint32_t dw;

#if( CONFIG_DIMMED_LIGHT ) // simple GPIO "bit banging", min PWM pulse with = one 'SysTick' period. 
  // [in] global_addl_config.backlight_intensities : can be edited (sort of..) in applet/src/menu.c
  uint8_t curr_intensity = global_addl_config.backlight_intensities; // bits 3..0 for IDLE, bits 7..4 for ACTIVE intensity
# if( L_USE_UART6_AS_PWM_GENERATOR )
  uint32_t dwCR2value;     // value to be written into USART_CR2 to send 0.5, 1.0, or 1.5 STOP BITS. RM0090 Rev7 page 997. 
# endif 
#endif  // CONFIG_DIMMED_LIGHT ?

  ++IRQ_dwSysTickCounter; // counts SysTick_Handler() calls, which seemed to happen every 1.5 milliseconds 
   
#if( CONFIG_DIMMED_LIGHT ) // Support dimmed backlight (here, via GPIO, or PWM-from-UART) ?

  if( IRQ_dwSysTickCounter < 3000/* x 1.5 ms*/ )
   { dw = IRQ_dwSysTickCounter / 100; // brightness ramps up during init
     curr_intensity = (dw<9) ? dw : 9;
     // while demo runs / config being loaded from SPI-Flash . During this time,
     // global_addl_config.backlight_intensities is invalid, so avoid black screen
   }
  else  // not "shortly after power-on", but during normal operation ...
   {
# if(0) // one fine day, the patched firmware will call gfx.c : lcd_background_led() 
        //     to inform us about the current "wanted backlight intensity" ..
     // In 2017-01-08, there didn't seem to be any caller for lcd_background_led(),
     //    so GFX_backlight_on (set/cleared in the above function) is useless .  
     if( GFX_backlight_on ) curr_intensity >>= 4;  // intensity level for the RADIO-ACTIVE state in the upper 4 nibbles of this BYTE
# else  // as long as gfx.c : isn't called, GFX_backlight_on is a useless dummy, so...
     // Use Tytera's own "backlight timer" instead. Reloaded anywhere, then counts down to zero somewhere, and stops at zero when IDLE.
     if( backlight_timer>0) curr_intensity >>= 4;  // intensity level for the RADIO-ACTIVE state in the upper 4 nibbles of this BYTE   
# endif // < how to find out if the backlight is currently "low" (dimmed) or "high" (more intense) ?
     curr_intensity &= 0x0F;    // 4-bit value, but only steps 0..9 are really used
   }
  
  // The backlight ("Lamp") is on "PC6" (port C bit 6), usable also as USART6_TX . Use it as GPIO or U(S)ART ?
# if( L_USE_UART6_AS_PWM_GENERATOR )
  // Who knows what the Tytera firmware is doing.. maybe it enables SysTick_Handler() before WE called IRQ_Init().
  // To avoid trouble when accessing a hardware register in an "on-chip peripheral without clock" :
  if( RCC->APB2ENR & RCC_APB2ENR_USART6EN ) // only if USART6 is still 'fed with a system clock' ...
   { // Keep feeding bytes into the UART's transmit data register 
     // (regardless if it can accept data or not, don't care if tx-bytes get lost here)
     // Unfortunately, the UART's TX-output has the wrong polarity :
     // > When the transmitter is enabled and nothing is to be transmitted, 
     // > the TX pin is at high level.   (YHF: .. which would turn the backlight ON)
     // In contrast to other controllers, the TXD OUTPUT POLARTIY cannot be
     //    inverted (to send LOW on idle) through a bit in the UART control registers.
     // It's NOT possible to transmit a long break with this UART (RM0090 Rev7 page 956).
     // Thus, when sending NOTHING, the backlight would have MAXIMUM brightness,
     //    and we need to handle a few special cases below .
     if( curr_intensity == 0 )  // backlight shall be COMPLETELY DARK ->
      { // Reconfigure PC6 as 'GPIO' to turn the backlight COMPLETELY off .
        // Two bits in "MODER" per pin : 00bin for GPIO, 10bin for 'alternate function mode'. RM0090 Rev7 page 278 :  
        GPIOC->MODER &= ~( 3 << (6/*pinpos*/ * 2) ); // hope we didn't interrupt another read-modify-write on this
        // (if it happens, no big problem, the NEXT SysTick-interrupt will put things right again)
        GPIOC->BSRRH = (1<<6);  // turn BL off (completely, as GPIO, no PWM)
      }
     else // backlight not one of the 'very dark' states, so turn off the 'strong' driver and send PWM pattern..
      { GPIOC->MODER |= ( 2 << (6/*pinpos*/ * 2) ); // PC6 now configured as UART6_TX again
        // Ideally, each of the following steps should increase the intensity 
        //   by a certain FACTOR : Intensity(N) = 0.7 * Intensity(N+1)
        //
        //  --------------------------+--------------------------------
        //   'intensity' value        |  0  1  2  3  4  5  6  7  8  9
        //  --------------------------+--------------------------------
        //   IDEAL PWM duty cycle (%) |  0  5  8  12 17 24 34 49 70 100
        //  --------------------------+--------------------------------
        //
        dwCR2value = 0; // default value for USART_CR2 to send 0.5, 1.0, or 1.5 STOP BITS. RM0090 Rev7 page 997. 
                        // 0 = "normal" UART, no LIN, no clock pin, 1 stopbit
        dw = 0x00;  // send as many 'off'-bits as we can, which results in the..
           //  minimum possible duty cycle. The backlight is now only
           //  "driven by the STOP BIT, between two 'characters'.
        switch( curr_intensity ) // which UART-data to send as PWM ?
         { case 1 : // minimum possible intensity : circa 5% duty cycle.
              // Fortunately STM32F4's USART can be configured to send A HALF stopbit.
              //  which results in the following theoretic duty cycle:
              //  100% * 0.5 / (1+9+0.5) = 4.76 % duty cycle. Nice !
              dwCR2value |= (1<<12); // CR2 bits 13..12 = 01bin : send A HALF STOP BIT (only)
              break;
           case 2 : // the next 'brighter' step, ideally ~8 % duty cycle:
              // use default: BL driven during 1.0 startbits -> DC = 1 / (1+9+1.0) = 9 %
              break;
           case 3 : 
              dwCR2value |= (3<<12); // CR2 bits 13..12 = 11bin : send 1.5 stop bits
              break;                 // DC = 100% * 1.5 / (1+9+1.5) = 13 %
           case 4 : dw = 0x100; // one "active" data bit + 1 stop bit
              // A UART(!) send the LSBit first. For the lowest possible RFI,
              // we only want ONE PULSE in each cycle, thus the "active" data bit
              // must be sent immediately before the STOP BIT. Thus dw=0x100, not 0x001 .
              break;                 // DC = 100% * (1+1) / (1+9+1) = 18 %
           case 5 : dw = 0x100; // one "active" data bit + 1.5 stop bits
              dwCR2value |= (3<<12); // CR2 bits 13..12 = 11bin : send 1.5 stop bits
              break;                 // DC = 100% * (1+1.5) / (1+9+1.5) = 21.7 %
           case 6 : dw = 0x180; break; // 2+1 "on" bits / 11 -> 27 % DC
           case 7 : dw = 0x1E0; break; // 4+1 "on" bits / 11 -> 45 % DC
           case 8 : dw = 0x1F8; break; // 6+1 "on" bits / 11 -> 64 % DC
           default: // maximum possible intensity 
              dwCR2value |= (2<<12); // CR2 bits 13..12 = 10bin : send TWO stop bits
              dw = 0x1FF;  // 100 % * (9+2) / (1+9+2) = 11  "on" bits ->  91.6 % duty cycle 
              // (because there's still ONE unavoidable GAP caused by the START(!)-bit) 
              break;
         } // end switch( curr_intensity )
        USART6->CR2 = dwCR2value;
        USART6->DR  = dw;
      }   // end else < backlight not completely dark > 
   }     // end if( RCC->APB2ENR & RCC_APB2ENR_USART6EN )
   
# else // L_USE_UART6_AS_PWM_GENERATOR : first experiment, simply drive PC6 as GPIO (purely soft-PWM) :
  
  // According to RM0090 page 281, the STM's GPIO 'port bit set/reset register'
  // can be used to modify a single port pin (without a costly read/modify/write).
  // The datasheet only mentions "GPIOx_BSRR" as a 32-bit register at offset 0x18 in chapter 8.4.7,
  // but applet/lib/inc/stm32f4xx.h : GPIO_TypeDef has TWO 16-bit components: BSRRL and BSRRH .
  dw = (IRQ_dwPWMCounter++) & 15;
  if( dw > curr_intensity )   
   { GPIOC->BSRRL = (1<<6);  // turn BL on  (what a strange name: a one written into BSRRL *SETS* a bit in GPIOx_ODR !)
     // Test result: SysTick_Handler() was called 666.6 times per second.
     //  With a 16-phase PWM, the 'base frequency' was 41.7 Hz (too low).
   }
  else  // (most of the time) -> PWM off 
   { GPIOC->BSRRH = (1<<6);  // turn BL off (what a strange name: a one written into BSRRH *CLEARS* a bit in GPIOx_ODR !!)
   }
# endif // L_USE_UART6_AS_PWM_GENERATOR ?
#endif // CONFIG_DIMMED_LIGHT ?


  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Next: "Jump into the original handler", at the following address:
#ifdef IRQ_ORIGINAL_SYSTICK_HDLR  // Do we know the address of SysTick_Handler in the *original* firmware ?
  dw = IRQ_ORIGINAL_SYSTICK_HDLR; // for the "D013.020" firmware, see www.qsl.net/dl4yhf/RT3/listing.txt
#else
#  error "The firmware you're trying to patch is not supported here yet !"
#endif
  // Don't "return" from here (no 'BX LR') but "jump directly into the ORIGINAL handler" .
  // What we WANT to have here (.. would be easy if GCC inline-asm wouldn't be so clumsy) :
  //  > mov pc, dw  ; dw = 32-bit variable with the original handler address (even)
  // 
  // GCC syntax: asm(  code : output operand list : input operand list : clobber list  );
  //                    |     |_________________|   |________________|   |__________|
  //    Aaalmost looks like     |                     |    ________________|
  //    'normal' assembly,      |                     |   |
  //    except for operands,    |    _________________|   Optional list of clobbered registers   
  //    e.g. "B %[handler]"     |   |
  //      ______________________|   comma separated list of input operands,
  //     |                          same syntax as the list of output operands
  //     |
  //     list of output operands, separated by commas. Each entry consists of  
  //     a symbolic name enclosed in square brackets, followed by a constraint string, 
  //     followed by a C expression enclosed in parentheses. 
  // 
  //     __"keep your fingers off MY code" ("volatile" ~~ "don't optimize me away" ??)
  //    |
  asm volatile(  "MOV pc, %0\n\t" : /*no outputs*/ : "r" (dw) );
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  
} // end SysTick_Handler()
#endif // ( CONFIG_DIMMED_LIGHT /* || .. */ ) ?



/* EOF < irq_handlers.c >  Leave an empty line for stupid compilers after this. */
