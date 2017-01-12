/*! \file irq_handlers.c
    \brief Own interrupt handler(s), first used for a 'dimmed backlight' .

  Module prefix "IRQ_", except for IRQ handlers (e.g. "SysTick_Handler").

  Drives the MD380's backlight LEDs with a PWMed backlight,
  intensity depending on 'idle' / 'active', configurable in menu.c . 
  
  The MD380's "Lamp"-signal on PC6 is reconfigured as UART6_TX .
  The pulse width modulation is realized by different UART tx patterns,
  and by varying the number of STOP BITS for the lower intensity range.
  
  Details may still be at www.qsl.net/dl4yhf/RT3/md380_fw.html#dimmed_light .
    
 To include the 'dimmed backlight' feature in the patched firmware:
    
  1. Add the following line in applet/Makefile (after SRCS += beep.o) :
      SRCS += irq_handlers.o 
  
  2. #define CONFIG_DIMMED_LIGHT 1  in  ?/md380tools/applet/config.h  .
    
  3. Remove src/stm32f4xx_it.c from the project/make (applet/Makefile) .
        stm32f4xx_it.c is useless, because all 'weak' default handlers
        alreay exist in startup_XYZ.S, for example a *weak* SysTick_Handler().
  
  4. Add a call to IRQ_Init() in main.c : splash_hook_handler() .

  5. Add a patch for SysTick_Handler in merge_d13.020.py (etc) .
  
  6. Issue "make clean image_D13", and carefully watch the output.
       There should be a message indicating SysTick_Handler being patched.
 
*/

#include "config.h"

#include <stm32f4xx.h>
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
#include "keyb.h"      // contains "backlight_timer", which is Tytera's own software-timer

#include "irq_handlers.h" // header for THIS module

// Module-internal options :
#define L_USE_UART6_AS_PWM_GENERATOR 0 /* 0=no (use PC6 as GPIO),  1=yes (use PC6 as USART6_TX to send PWM-like bit patterns) */

volatile uint32_t IRQ_dwSysTickCounter = 0; // Incremented each 1.5 ms. Rolls over from FFFFFFFF to 0 after 74 days


//---------------------------------------------------------------------------
void IRQ_Init(void) // initializes OUR interrupt handlers. Called from main.c.
{
  // 2017-01-05 : Tried to reprogram VTOR to avoid having to patch Tytera's 
  //              vector table. But this made the firmware crash somewhere,
  //              so stick to the old method of patching THEIR vector table
  //              to use OUR SysTick_Handler .  
  // 2017-01-07 : Tried to improve the PWM resultion with the lowest possible
  //              interrupt rate (to keep the CPU load low) by abusing
  //              USART6 (with TX on PC6 = "Lamp" = backlight) for PWM generation.
  //

#if( CONFIG_DIMMED_LIGHT ) // initialize I/O registers for dimmable backlight ?


# if( L_USE_UART6_AS_PWM_GENERATOR )
  USART_TypeDef *pUSART = USART6; // load the UART's base address into register (once)

  RCC->APB2ENR |=  RCC_APB2ENR_USART6EN;    // enable internal clock for UART6 (from 'APB2')
  RCC->APB2RSTR &= ~RCC_APB2RSTR_USART6RST; // bring USART6 "out of reset" (RM0090 Rev7 page 176)

  // Turn pin "PC6" from a GPIO into "UART6_TX". No need for a bloated struct ("GPIO_InitTypeDef") for this:
  // Two bits in "MODER" per pin : 00bin for GPI, 01 for GPO, 10 for 'alternate function mode'. RM0090 Rev7 page 278:
  GPIOC->MODER  /*4002800*/ = ( GPIOC->MODER & ~(3 << (6/*pinpos*/ * 2) ) ) |  (2/*ALT*/ << (6/*pinpos*/ * 2) );
  
  // Two bits in "OSPEEDR" per pin : 00bin for the 'lowest speed', to cause the lowest possible RFI                                  
  GPIOC->OSPEEDR/*4002808*/ &= ~(3 << (6/*pinpos*/ * 2) );  // RM0090 Rev7 page 279
  
  // One bit per pin in "OTYPER" to select open drain or push/pull output mode:
  GPIOC->OTYPER /*4002804*/ &= ~(1<<6);  // RM0090 Rev7 page 279 : Low for push-pull
  
  // Two bits in "PUPDR" per pin for the Pull-up / Pull down resistor configuration. 00bin = none
  GPIOC->PUPDR  /*400280C*/ &= ~(3 << (6/*pinpos*/ * 2) );  // RM0090 Rev7 page 280
  
  // Tell "PC6" which of the 16 alternate functions it shall have from: USART6_TX . RM0090 Rev7 page 283.
  // There are FOUR bits per pin in AFR[0..1], thus PC6 is in AFR[0] bits 27..24. USART6_TX = "AF8" (STM32F405/7 DS page 62).
  GPIOC->AFR[0] = (GPIOC->AFR[0] & ~(0x0F << (6/*pinpos*/ * 4 ) ) )  |  (0x08 << (6/*pinpos*/ * 4) );

  // Init the "USART". Page numbers from RM0090 Rev7 (Reference Manual).
  // Some of these registers are overwritten in SysTick_Handler, but let's initialize ALL UART6 registers here.
  pUSART->CR2 /*pg 997*/ = 0;  // "normal" UART, no LIN, no clock pin, 1 stopbit (also possible: 0.5 and 1.5)
  pUSART->CR1 /*pg 994*/ = (1/*enable*/<<13) | (1/*9bit-mode*/<<12) | (1/*TE*/<<3); // 1 startbit, 9 data bits, no parity, no interrupts, "OVER8"=0
  pUSART->CR3 /*pg 998*/ = 0;  // no "onebit", no CTS-IE, no CTS, no RTS, no DMA, no smartcard, no nothing
  pUSART->GTPR/*pg 1001*/= 15; // no "Guard time",  prescaler=1(b7..0), etc
  // Baudrate (pg. 964) = f_clk / ( 8 * (2-"OVER8") * USARTDIV ).   
  // USARTDIV is a FIXED-POINT number in BRR.  Mantissa in bits 15..4, fraction in bits 3..0 (not used here)
  // We'll feed ONE BYTE into the tx register per SysTick-interrupt, 
  // thus the transmission of one 9-bit pattern must take at least 1.5 ms . 
  // The UART peripheral clock (from APB2) was unknown at the time of this writing. 
  // Trial-and-error, a photocell, and an oscilloscope was used to find a value where sending
  // a string of 0xF0 0xF0 0xF0 0xF0 characters, and looking at the FREQUENCY from the photocell.
  // Test result: pUSART->GTPR=15, USARTx->BRR=0x7FFF -> measured PWM frequency = 220 Hz .
  pUSART->BRR /*pg 994*/ = 0x7FFF;

# endif // L_USE_UART6_AS_PWM_GENERATOR ?
#endif // CONFIG_DIMMED_LIGHT ?
  
} // IRQ_Init()



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
  uint8_t intensity = global_addl_config.backlight_intensities; // bits 3..0 for IDLE, bits 7..4 for ACTIVE intensity
# if( L_USE_UART6_AS_PWM_GENERATOR )
  uint16_t wTxData, wCR2Value;
# endif  // L_USE_UART6_AS_PWM_GENERATOR ?
#endif  // CONFIG_DIMMED_LIGHT ?

  ++IRQ_dwSysTickCounter; // counts SysTick_Handler() calls, which seemed to happen every 1.5 ms .

#if( CONFIG_DIMMED_LIGHT ) // Support dimmed backlight (here, via GPIO, or PWM-from-UART) ?

  if( IRQ_dwSysTickCounter < 3000/* x 1.5 ms*/ )
   { dw = IRQ_dwSysTickCounter / 100; // brightness ramps up during init
     intensity = (dw<9) ? dw : 9;
     // while demo runs / config being loaded from SPI-Flash . During this time,
     // global_addl_config.backlight_intensities is invalid, so avoid black screen
   }
  else  // not "shortly after power-on", but during normal operation ...
   {
     if( intensity==0 )   // backlight intensities not configured ? ('0' means take proper default)
      {  intensity= 0x52; // use meaningful default (without overwriting global_addl_config in an interrupt!)
      }          
       
# if(0) // not usable in 2017-01, see gfx.c ... so far just a future plan :
     if( GFX_backlight_on ) intensity >>= 4;  // intensity level for the RADIO-ACTIVE state in the upper 4 nibbles of this BYTE
# else  // as long as gfx.c : isn't called, GFX_backlight_on is a useless dummy, so...
     // Use Tytera's own "backlight timer" instead. Reloaded anywhere, then counts down to zero somewhere, and stops at zero when IDLE.
     if( backlight_timer>0) intensity >>= 4;  // intensity level for the RADIO-ACTIVE state in the upper 4 nibbles of this BYTE   
# endif // < how to find out if the backlight is currently "low" (dimmed) or "high" (more intense) ?
     intensity &= 0x0F;    // 4-bit value, but only steps 0..9 are really used
   }
  
  // MD380 "Lamp" is on "PC6" (port C bit 6), usable also as USART6_TX . Use it as GPIO or U(S)ART ?
# if( L_USE_UART6_AS_PWM_GENERATOR )
  // Who knows what the Tytera firmware is doing.. maybe it enables SysTick_Handler() before WE called IRQ_Init().
  // To avoid trouble when accessing a hardware register in an "on-chip peripheral without clock" 
  //    (caused a 'hard fault' exception, or simply crashed in a similar microcontroller) : 
  if( RCC->APB2ENR & RCC_APB2ENR_USART6EN ) // only if USART6 is still 'fed with a system clock':
   { // Keep feeding bytes into the UART's transmit data register 
     // (regardless if it can accept data or not, don't care if tx-bytes get lost here)
     // Unfortunately, the UART's TX-output has the wrong polarity :
     // > When the transmitter is enabled and nothing is to be transmitted, 
     // > the TX pin is at high level.   (YHF: .. which would turn the backlight ON)
     // Thus, when sending NOTHING, the backlight would have MAXIMUM brightness,
     //  -> UART transmit data register must be continuously 'flooded' with data.
     if( intensity == 0 )  // backlight shall be COMPLETELY DARK ->
      { // Reconfigure PC6 as 'GPIO' to turn the backlight COMPLETELY off .
        // Two bits in "MODER" per pin : 00bin for GPI,  01bin for GPO, 10bin for 'alternate function mode'.
        GPIOC->MODER = (GPIOC->MODER & ~( 3 << (6/*pin*/ * 2))) |  ( 1 << (6/*pin*/ * 2) ) ;
        // hope we didn't interrupt another read-modify-write on this register.
        // (if it happens, no big problem, the NEXT SysTick-interrupt will put things right again)
        GPIOC->BSRRH = (1<<6);  // turn BL off (completely, as GPIO, no PWM)
      }
     else // backlight not one of the 'very dark' states, so turn off the 'strong' driver and send PWM pattern..
      { GPIOC->MODER = (GPIOC->MODER & ~( 3 << (6/*pin*/ * 2))) |  ( 2 << (6/*pin*/ * 2) ); // PC6 now configured as UART6_TX again
        // Ideally, each of the following steps should increase the intensity 
        //   by a certain FACTOR : Intensity(N) = 0.7 * Intensity(N+1)
        //
        //  --------------------------+--------------------------------
        //   'intensity' value        |  0  1  2  3  4  5  6  7  8  9
        //  --------------------------+--------------------------------
        //   IDEAL PWM duty cycle (%) |  0  5  8  12 17 24 34 49 70 100
        //  --------------------------+--------------------------------
        //
        wCR2Value = 0; // default value for USART_CR2 to send 0.5, 1.0, or 1.5 STOP BITS. RM0090 Rev7 page 997. 
                       // 0 = "normal" UART, no LIN, no clock pin, 1 stopbit
        wTxData = 0x00;  // send as many 'off'-bits as we can, defauls for low intensity,
           //  when backlight is only driven by the STOP bit, between two 'characters'
        switch( intensity ) // which UART-data to send as PWM ?
         { case 1 : // minimum possible intensity : circa 5% duty cycle.
              // Fortunately STM32F4's USART can be configured to send A HALF stopbit.
              //  which results in the following theoretic duty cycle:
              //  100% * 0.5 / (1+9+0.5) = 4.76 % duty cycle. Nice !
              wCR2Value |= (1<<12); // CR2 bits 13..12 = 01bin : send A HALF stopbit (only)
              break;
           case 2 : // the next 'brighter' step, ideally ~8 % duty cycle:
              // use default: BL driven during 1.0 startbits -> DC = 1 / (1+9+1.0) = 9 %
              break;
           case 3 : 
              wCR2Value |= (3<<12);  // CR2 bits 13..12 = 11bin : send 1.5 stop bits
              break;                 // DC = 100% * 1.5 / (1+9+1.5) = 13 %
           case 4 : wTxData = 0x100; // one "active" data bit + 1 stop bit
              // A UART(!) send the LSBit first. For the lowest possible interference,
              // we only want ONE PULSE in each cycle, thus the "active" data bit
              // must be sent immediately before the STOP BIT. Thus dw=0x100, not 0x001 .
              break;                 // DC = 100% * (1+1) / (1+9+1) = 18 %
           case 5 : wTxData = 0x100; // one "active" data bit + 1.5 stop bits
              wCR2Value |= (3<<12);  // CR2 bits 13..12 = 11bin : send 1.5 stop bits
              break;                 // DC = 100% * (1+1.5) / (1+9+1.5) = 21.7 %
           case 6 : wTxData = 0x180; break; // 2+1 "active" bits / 11 -> 27 % DC
           case 7 : wTxData = 0x1E0; break; // 4+1 "active" bits / 11 -> 45 % DC
           case 8 : wTxData = 0x1F8; break; // 6+1 "active" bits / 11 -> 64 % DC
           default: // maximum brightness (PWM duty cycle)
              wCR2Value |= (2<<12); // CR2 bits 13..12 = 10bin : send TWO stop bits
              wTxData = 0x1FF;  // 100 % * (9+2) / (1+9+2) =  91.6 % duty cycle 
              // (there's still ONE unavoidable gap, caused by the START(!)-bit.
              //  this "loss" is neglectable and doesn't justify reconfiguring
              //  PC6 as GPIO, to turn the backlight on continuously)              
              break;
         } // end switch( curr_intensity )
        USART6->CR2 = wCR2Value; // new(?) number of stopbits
        USART6->DR  = wTxData;   // keep transmit data register filled, all the time
      }   // end else < backlight not completely dark >
     
   } // end if( RCC->APB2ENR & RCC_APB2ENR_USART6EN )
   
# else // L_USE_UART6_AS_PWM_GENERATOR : first experiment, simply drive PC6 as GPIO (purely soft-PWM) :
  // Revived 2017-01-12 when some users reported the UART method didn't work for their radios.
  // Maybe the UART-generated PWM frequency was too high, and there are low-pass RC filters
  // populated on some of those boards ? Trying this with f_pwm = ~~ 70 Hz may tell.
  // Downside: Tytera firmware drives PC6 via GPIO too, and this interferes
  //           occasionally (for example when turning the volume pot,
  //           the intensity flickers momentarily because Tytera turns
  //           the backlight on 'outside our nice PWM cycle' )
  //
  // According to RM0090 page 281, the STM's GPIO 'port bit set/reset register'
  // can be used to modify a single port pin (without a costly read/modify/write).
  dw = IRQ_dwSysTickCounter % 9; // dw = sawtooth ranging from 0..8. One step = 1.5 ms. f_pwm = 1 / (9*1.5ms) = 74 Hz .
  if( intensity > dw )           // intensity from config, ranging from 0..9 (!)
   { GPIOC->BSRRL = (1<<6);  // turn BL on  (strange name: a one written into BSRRL *SETS* a bit in GPIOx_ODR !)
   }
  else // e.g. intensity=0, can never be > dw; PC6 must bw LOW (bit in GPIOx_ODR *cleared* :
   { GPIOC->BSRRH = (1<<6);  // turn BL off (strange name: a one written into BSRRH *CLEARS* a bit in GPIOx_ODR !!)
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
  // What we WANT to have here (.. would be easy it it wasn't GCC inline-asm ..) :
  //  > mov pc, dw  ; dw = 32-bit variable with the original handler address
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
  //     __"volatile" : here ~~ "don't optimize me away" (??)
  //    |
  asm volatile(  "MOV pc, %0\n\t" : /*no outputs*/ : "r" (dw) );
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  
} // end SysTick_Handler()


/* EOF < irq_handlers.c > .  Leave an empty line after this. */
