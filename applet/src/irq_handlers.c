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
  
  2. #define CONFIG_DIMMED_LIGHT 1  in  md380tools/applet/config.h  .
    
  3. Remove src/stm32f4xx_it.c from the project/make (applet/Makefile) .
        stm32f4xx_it.c is useless, because all 'weak' default handlers
        alreay exist in startup_stm32f4xx_asm.S (ex startup_stm32f4xx.s) .
  
  4. Add calls to IRQ_Init() in main.c : splash_hook_handler() .

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

#ifndef IRQ_ORIGINAL_SYSTICK_HDLR  // Do we know the address of SysTick_Handler in the *original* firmware ?
#  error "Missing address of the original SysTick_Handler in 'irq_handlers.h' !"
#endif

typedef void (*void_func_ptr)(void);

  // How to drive the LEDs (here, mostly for testing) ? From gfx.c :
  // > The RED LED is supposed to be on pin A0 by the schematic, but in
  // > point of fact it's on E1.  Expect more hassles like this.
#define PINPOS_C_BL 6 /* pin position of backlight output within GPIO_C */
#define PINPOS_E_TX 1 /* pin position of the red   TX LED within GPIO_E */
#define PINPOS_E_RX 0 /* pin position of the green RX LED within GPIO_E */
#define LED_GREEN_ON  GPIOE->BSRRL=(1<<PINPOS_E_RX) /* green LED on (w/o ST-bulk) */
#define LED_GREEN_OFF GPIOE->BSRRH=(1<<PINPOS_E_RX) /* green LED off */
#define LED_RED_ON    GPIOE->BSRRL=(1<<PINPOS_E_TX) /* red LED on  */
#define LED_RED_OFF   GPIOE->BSRRH=(1<<PINPOS_E_TX) /* red LED off */

  // How to poll a few keys 'directly' after power-on ? 
  // The schematic shows "K3" from the PTT pad to the STM32, 
  // but it doesn't look like an INPUT (may be a multiplexer
  // to poll the two "side buttons" shared with LCD_D6 & D7 .. eeek)
  // -> leave that for later... something like
  // "keep 'M' pressed on power-on for Morse output" will be difficult.

volatile uint32_t IRQ_dwSysTickCounter = 0; // Incremented each 1.5 ms. Rolls over from FFFFFFFF to 0 after 74 days
#if( CONFIG_DIMMED_LIGHT )
 static uint8_t may_turn_on_backlight = 0; // ok to turn on the backlight ? 0=no, 1=yes
#endif


#if( CONFIG_DIMMED_LIGHT )
static void InitDimming(void)
{ // Since 2017-02-16, called from SysTick_Handler() when Tytera's firmware(!)
  // has initialized the hardware, waited until the power supply is stable, 
  // and turned on the backlight via GPIO port register for the first time.
  // Only THEN, it's definitely safe to draw the current for the backlight !
  // (when turning on the backlight "too early", the CPU seemed to reset itself
  //  in some radios. The CPU possibly starts to run before the power supply
  //  can provide the full current. By "waiting" until the original firmware 
  //  turns on the backlight for the first time (polled in SysTick_Handler),
  //  there should be no risk of drawing too much current too early.
  
  USART_TypeDef *pUSART = USART6; // load the UART's base address into a register (once)

  RCC->APB2ENR |=  RCC_APB2ENR_USART6EN;    // enable internal clock for UART6 (from 'APB2')
  RCC->APB2RSTR &= ~RCC_APB2RSTR_USART6RST; // bring USART6 "out of reset" (RM0090 Rev13 page 240)
    
  // Turn pin "PC6" from a GPIO into "UART6_TX". No need for a bloated struct ("GPIO_InitTypeDef") for this:
  // Two bits in "MODER" per pin : 00bin for GPI, 01 for GPO, 10 for 'alternate function mode'. RM0090 Rev13 page 283:
  GPIOC->MODER  /*4002800*/ = ( GPIOC->MODER & ~(3 << (PINPOS_C_BL * 2) ) ) |  (2/*ALT*/ << (PINPOS_C_BL * 2) );
  
  // Two bits in "OSPEEDR" per pin : 00bin for the 'lowest speed', to cause the lowest possible RFI                                  
  GPIOC->OSPEEDR/*4002808*/ &= ~(3 << (PINPOS_C_BL * 2) );  // RM0090 Rev7 page 283
  
  // One bit per pin in "OTYPER" to select open drain or push/pull output mode:
  GPIOC->OTYPER /*4002804*/ &= ~(1<<PINPOS_C_BL);  // RM0090 Rev13 page 279 : Low for push-pull
  
  // Two bits in "PUPDR" per pin for the Pull-up / Pull down resistor configuration. 00bin = none
  GPIOC->PUPDR  /*400280C*/ &= ~(3 << (PINPOS_C_BL * 2) );
  
  // BEEN_HERE__STOP; // 2017-02-16 : No.

  // Tell "PC6" which of the 16 alternate functions it shall have: USART6_TX . RM0090 Rev13 page 287.
  // Only has an effect if the two MODER-bits for this pin are 10bin = "alternate function mode". 
  // There are FOUR bits per pin in AFR[0..1], thus PC6 is in AFR[0] bits 27..24. USART6_TX = "AF8" (STM32F405/7 DS page 62).
  GPIOC->AFR[0] = (GPIOC->AFR[0] & ~(0x0F << (PINPOS_C_BL * 4 ) ) )  |  (0x08 << (PINPOS_C_BL * 4) );

  // Init the "USART". Page numbers from RM0090 Rev13 (Reference Manual).
  pUSART->CR2 /*pg 1017*/ = (1<<12);  // "normal" UART, no LIN, no clock pin, 0.5 stopbits (!)
  pUSART->CR1 /*pg 1014*/ = (1/*enable*/<<13) | (1/*9bit-mode*/<<12) | (1/*TE*/<<3); // 1 startbit, 9 data bits, no parity, no interrupts, "OVER8"=0
  pUSART->CR3 /*pg 1018*/ = 0;  // no "onebit", no CTS-IE, no CTS, no RTS, no DMA, no smartcard, no nothing
  pUSART->GTPR/*pg 1021*/ = 32; // no "Guard time",  prescaler=32 (b7..0, only for IRDA ?), etc
  // RM0090 Rev13 page 982: Baudrate = f_CK / ( 8 * (2-"OVER8") * USARTDIV ),
  //   where
  //      USARTDIV is a FIXED-POINT number in BRR.  Mantissa in bits 15..4, fraction in bits 3..0 .
  //      f_CK = APB2 clock (which must not be changed, here: 72 MHz from the main PLL).
  // We'll feed ONE 9-bit 'word' into the tx register per SysTick-interrupt, 
  // thus a 9-bit UART transmission must take longer than the SysTick-period. 
  pUSART->BRR /*pg 1014*/ = 0xFFF0;  
  // PWM frequency = UART Frame rate. With BRR=0xFFF0, 'OVER8'=0, and 1+9+0.5 bits per frame:
  //   f_pwm = 72 MHz [f_APB2] / ( 10.5 [bits per frame] * 4095.0 [BRR] * 16 [samples per bit] ) = 104.65 Hz .
  // Confirmed with a photocell on the display, and an oscilloscope.
 
} // InitDimming()
#endif // CONFIG_DIMMED_LIGHT ?


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
  // [in] global_addl_config.backlight_intensities : configurable in applet/src/menu.c
  uint8_t intensity = global_addl_config.backlight_intensities; // bits 3..0 for IDLE, bits 7..4 for ACTIVE intensity
#endif  // CONFIG_DIMMED_LIGHT ?

  // Will any of "tytera's" interrupts have a higher priority, and/or will it
  // be able to interrupt THIS interrupt handler ? Instead of finding out,
  // play safe, assume anything can happen, and DISABLE interrupts in the core
  // before fiddling around with GPIO-, UART-, RCC-, and who knows what else :
  int irq_was_disabled = __get_PRIMASK();  // ugly, non-portable, but .. ?
  __disable_irq(); // -> cpsid I (disable IRQ by setting PRIMASK on Cortex-M)

  ++IRQ_dwSysTickCounter; // counts SysTick_Handler() calls, every 1.5 ms

#if( CONFIG_DIMMED_LIGHT ) // Support dimmed backlight (here, via GPIO, or PWM-from-UART) ?

  // "Wait" until the original firmware turns on the backlight:
  if( ! may_turn_on_backlight ) 
   { // Did the original firmware turn on the backlight ? 
     // First of all, did it turn on the peripheral clocks for GPIO A..E ?
#    define RCC_AHB1ENR_GPIO_A_to_E (RCC_AHB1ENR_GPIOAEN|RCC_AHB1ENR_GPIOBEN|RCC_AHB1ENR_GPIOCEN|RCC_AHB1ENR_GPIODEN|RCC_AHB1ENR_GPIOEEN)
     if( (RCC->AHB1ENR & RCC_AHB1ENR_GPIO_A_to_E) == RCC_AHB1ENR_GPIO_A_to_E )
      { // Did the firmware configure backlight and RX,TX-LEDs as output ?
        if(   ( (GPIOC->MODER & (3<<(PINPOS_C_BL*2)) ) == (1<<(PINPOS_C_BL*2)) ) 
           && ( (GPIOE->MODER & (3<<(PINPOS_E_TX*2)) ) == (1<<(PINPOS_E_TX*2)) )
           && ( (GPIOE->MODER & (3<<(PINPOS_E_RX*2)) ) == (1<<(PINPOS_E_RX*2)) )
          )
         { // Ok, from here on, we can safely (?) drive the GPIOs .
#         if(0) // TEST: How long does it take until Tytera turns the BL on ?
           // Turn on the GREEN 'RX' LED to find out if/when we get here .
           // Test result: The first time Tytera turns the backlight on
           // was ~~ 1 second after power-on. Turn it on too early -> CRASH.
           dw = IRQ_dwSysTickCounter & 0x1F; // flickering, dimmed green LED
           if( dw==0x00 )   // Note: MOST of the time, Tytera drives this LED,
            { LED_GREEN_ON; // so it could still act as a squelch indicator !
            }
           if( dw==0x01 )    
            { LED_GREEN_OFF;  
            }
#         endif // TEST ?

           // Did the firmware turn the backlight on (for the 1st time) ?  
           if( GPIOC->ODR & (1<<6) ) // backlight-bit in Output Data Register set ?
            { // Yes; guess it's ok to "take over" backlight control now .
              InitDimming();  // switch from GPIO- to UART-generated PWM
              IRQ_dwSysTickCounter  = 0; // restart timer (to ramp up brightness)
              may_turn_on_backlight = 1; // start dimming in the next interrupt
            } // end if < backlight turned ON (by original firmware) >
         }   // end if < GPIO_C.6 configured as OUTPUT >
      }     // end if < GPIO_C supplied with a peripheral clock >
   }       // end if < may NOT turn on the backlight yet >   
  else    // may control the backlight now ...
   { 
     if( IRQ_dwSysTickCounter < 3000/* x 1.5 ms*/ )
      { dw = IRQ_dwSysTickCounter / 100; // brightness ramps up during init
        intensity = (dw<9) ? dw : 9;     // ... from 0 to 9 (=max brightness)
      }
     else  // not "shortly after power-on", but during normal operation ...
      {
        if( intensity==0 )   // backlight intensities not configured ? ('0' means take proper default)
         {  intensity= 0x90; // 'hum-free' default (without overwriting global_addl_config in an interrupt!)
                             // lower nibble = brightness when idle, upper nibble = brightness when active.          
         }          
       
#    if(0) // not usable in 2017-01, see gfx.c ... so far just a future plan :
        if( GFX_backlight_on ) 
#    else  // as long as gfx.c:lcd_background_led() isn't called, GFX_backlight_on is useless, 
           // so use Tytera's "backlight_timer" instead:
        if( backlight_timer>0)
#    endif // < how to find out if the backlight is currently "low" (dimmed) or "high" (more intense) ?
         { intensity >>= 4;  // intensity level for the RADIO-ACTIVE state in the upper 4 nibbles of this BYTE
           intensity |=  1;
         } // end if < backlight should be "on" (active state) > 
        intensity &= 0x0F;   // 4-bit value, but only steps 0..9 are really used
      } // <normal operation>

     if( ! (RCC->APB2ENR & RCC_APB2ENR_USART6EN) ) // oops.. USART6 has been de-initialized ?!
      { InitDimming();   
      }
      
     // Except for min and max brightness, keep feeding bytes into the UART's 
     //     transmit data register to generate the PWM .
     // Don't care if it can accept data or not, don't care if tx-bytes get lost here.
     // Unfortunately, the UART's TX-output has the wrong polarity :
     // > When the transmitter is enabled and nothing is to be transmitted, 
     // > the TX pin is at high level.   (YHF: .. which would turn the backlight ON)
     // Thus, when sending NOTHING, the backlight will have MAXIMUM brightness,
     //  -> UART transmit data register must be continuously 'flooded' with data.
     if( intensity == 0 )  // backlight shall be COMPLETELY DARK ->
      { // Reconfigure PC6 as 'GPIO' to turn the backlight COMPLETELY off .
        // Two bits in "MODER" per pin : 00bin for GPI,  01bin for GPO, 10bin for 'alternate function mode'.
        GPIOC->MODER = (GPIOC->MODER & ~( 3 << (6/*pin*/ * 2))) |  ( 1 << (6/*pin*/ * 2) ) ;
        // hope we didn't interrupt another read-modify-write on this register.
        // (if it happens, no big problem, the NEXT SysTick-interrupt will put things right again)
        GPIOC->BSRRH = (1<<6);  // turn BL off (completely, as GPIO, no PWM)
      }  
     else // backlight not one of the 'very dark' states, so configure PC6 as UART_TXD and send PWM pattern:
      { GPIOC->MODER = (GPIOC->MODER & ~( 3 << (6/*pin*/ * 2))) |  ( 2 << (6/*pin*/ * 2) ); // PC6 now configured as UART6_TX again
        switch( intensity ) // which UART-data to send as PWM ?
         { case 1 : USART6->DR = 0x00; // low intensity: backlight only driven by stopbit.
              // Theoretic duty cycle (DC): 100% * 0.5 / (1+9+0.5) = 4.76 % 
              break;
           case 2 : USART6->DR = 0x100; break; // DC = 100% * 1.5 / 10.5 = 14.3 % 
              // A UART sends the LSBit first. For the lowest possible RFI,
              // we only want ONE PULSE in each cycle. Thus 0x100, not 0x001 .  
              break;
           case 3 : USART6->DR = 0x180; break; // DC = 100% * 2.5 / 10.5 = 23.8 % 
           case 4 : USART6->DR = 0x1C0; break; // DC = 100% * 3.5 / 10.5 = 33.3 % 
           case 5 : USART6->DR = 0x1E0; break; // DC = 100% * 4.5 / 10.5 = 42.3 % 
           case 6 : USART6->DR = 0x1F0; break; // DC = 100% * 5.5 / 10.5 = 52.4 % 
           case 7 : USART6->DR = 0x1F8; break; // DC = 100% * 6.5 / 10.5 = 61.9 % 
           case 8 : USART6->DR = 0x1FC; break; // DC = 100% * 7.5 / 10.5 = 71.4 % 
           default: break; // max possible brightness, 100% duty cycle,
              // hopefully no hum on certain radios with this .. 2017-02-12 .      
              break;
         } // end switch( curr_intensity )
      }   // end else < backlight not completely dark >
   }     // may_turn_on_backlight ? 
#endif  // CONFIG_DIMMED_LIGHT ?

  // Morse output will go here ... after finding out how to drive the speaker
  //                               independently of the analog volume pot .

  // Restore original interrupt (enable-) status. Don't blindly re-enable it !
  if( ! irq_was_disabled ) // don't enable IRQs if they were DISABLED on entry
   { __enable_irq(); // -> cpsie I (enable IRQ by clearing PRIMASK on Cortex-M)    
   }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Invoke Tytera's original SysTick_Handler.
  // Since our SysTick_Handler also calls other subroutines,
  //  and uses local variables (which may live on the stack),
  //  we can't simply jump into the original SysTick_Handler anymore.
  // Instead, call it like a normal subroutine (the magic of Cortex-M),
  //  then let the compiler-generated epilogue clean up the stack
  //  and restore whatever the compiler wanted to save.
  (*(void_func_ptr)(IRQ_ORIGINAL_SYSTICK_HDLR|0x01/*THUMB*/))();
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  
} // end SysTick_Handler()


/* EOF < irq_handlers.c > .  Leave an empty line after this. */
