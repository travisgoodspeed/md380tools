/*! \file irq_handlers.c
    \brief Own interrupt handler(s), used for optional dimmed backlight
     and the low-level Morse code generator with audio output.

  Drives the MD380's backlight LEDs with a PWMed backlight,
  intensity depending on 'idle' / 'active', configurable in menu.c . 

  Also contains a simple Morse code generator (with 'audio modulator'),
  used to output the text assembled by the 'narrator' (narrator.c) .  
  
  Details may still be at www.qsl.net/dl4yhf/RT3/md380_fw.html#dimmed_light .
    
 To include the 'dimmed backlight' feature in the patched firmware:
    
  1. Add the following line in applet/Makefile (after SRCS += beep.o) :
      SRCS += irq_handlers.o 
  
  2. #define CONFIG_DIMMED_LIGHT 1  in  md380tools/applet/config.h  .
    
  3. Remove src/stm32f4xx_it.c from the project/make (applet/Makefile) .
        stm32f4xx_it.c is useless, because all 'weak' default handlers
        alreay exist in startup_stm32f4xx_asm.S (ex startup_stm32f4xx.s) .
  
  4. Add a patch for SysTick_Handler in merge_d13.020.py (etc) .
  
  5. Issue "make clean image_D13", and carefully watch the output.
       There should be a message indicating SysTick_Handler being patched.
 
 To include also the Morse generator (for visually impaired hams):

  6. #define CONFIG_MORSE_OUTPUT 1  in  md380tools/applet/config.h .

  7. Add the 'Narrator' to applet/makefile (besides irq_handlers.o):
      SRCS += narrator.o 

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
#include "display.h"  // contains "gui_opmode3", which indicates RX-squelch state, too
#include "keyb.h"     // contains "backlight_timer", which is Tytera's own software-timer
#include "narrator.h" // optional: tells channel, zone, menu in Morse code
#include "app_menu.h" // optional 'application' menu, activated by red BACK-button
#include "irq_handlers.h" // header for THIS module


#ifndef IRQ_ORIGINAL_SYSTICK_HDLR  // Do we know the address of SysTick_Handler in the *original* firmware ?
#  error "Missing address of the original SysTick_Handler in 'irq_handlers.h' !"
#endif

 


typedef void (*void_func_ptr)(void);


  // How to control audio power amplifier and speaker ?
  // Details: www.qsl.net/dl4yhf/RT3/md380_fw.html#audio_message_beeps .
  // Note: BSRRH sets a portbit LOW in an atomic sequence,
  //       BSRRL sets a portbit HIGH (counter-intuitive names) !
#define PINPOS_C_AUDIO_BEEP 8 /* pin pos of the 'beep' output, GPIO_C */
#define PINPOS_B_AUDIO_PA   9 /* pin pos of AF Control (PA),   GPIO_B */
#define PINPOS_B_SPK_C      8 /* pin pos of SPK_C (Anti-Pop?), GPIO_B */
#define AUDIO_BEEP_LO  GPIOC->BSRRH=(1<<PINPOS_C_AUDIO_BEEP) /* squarewave L */
#define AUDIO_BEEP_HI  GPIOC->BSRRL=(1<<PINPOS_C_AUDIO_BEEP) /* squarewave H */
#define AUDIO_AMP_ON   GPIOB->BSRRL=(1<<PINPOS_B_AUDIO_PA) /* audio PA on ("AFCO") */
#define AUDIO_AMP_OFF  GPIOB->BSRRH=(1<<PINPOS_B_AUDIO_PA) /* audio PA off */
#define IS_AUDIO_AMP_ON ((GPIOB->ODR&(1<<PINPOS_B_AUDIO_PA))!=0) /* check audio PA */
#define SPKR_SWITCH_ON GPIOB->BSRRH=(1<<PINPOS_B_SPK_C)  /* speaker on */
#define SPKR_SWITCH_OFF GPIOB->BSRRL=(1<<PINPOS_B_SPK_C) /* speaker off*/
#define IS_SPKR_SWITCH_ON ((GPIOB->ODR&(1<<PINPOS_B_SPK_C))==0) /*check spkr switch*/


uint8_t boot_flags = 0; // 0 : none of the 'essential' functions has been called yet

volatile uint32_t IRQ_dwSysTickCounter = 0; // Incremented each 1.5 ms. Rolls over from FFFFFFFF to 0 after 74 days

uint16_t keypress_timer_ms = 0; // measures key-down time in MILLISECONDS 
uint8_t  keypress_ascii = 0;    // code of the currently pressed key, 0 = none .
                // intended to detect certain 'very long pressed keys' anywhere,
                // e.g.in app_menu.c : CheckLongKeypressToActivateMorse() .

#if( CONFIG_MORSE_OUTPUT )
typedef struct tMorseGenerator
 { uint8_t u8State; // state machine to generate Morse output:
#    define MORSE_GEN_PASSIVE 0 // not active, waiting for start
#    define MORSE_GEN_START   1 // request to start output
   // All other states (below) must be considered 'volatile',
   // because the timer interrupt may switch u8State anytime:
#  define MORSE_GEN_START_AP_OPEN  2 // waiting for 'Anti-Pop' switch to open (!)
#  define MORSE_GEN_START_AUDIO_PA 3 // waiting for audio PA to start
#  define MORSE_GEN_START_AP_CLOSE 4 // waiting for 'Anti-Pop' switch to close
#  define MORSE_GEN_START_CHAR_TX  5 // waiting for begin of the next char (maybe long gap)
#  define MORSE_GEN_SENDING_TONE   6 // sending a tone (dash or dot)
#  define MORSE_GEN_SENDING_GAP    7 // sending gap (between dashes and dots)
#  define MORSE_GEN_END_OF_MESSAGE 8 // all characters sent; reached end of message
#  define MORSE_GEN_STOP_ANTI_POP  9 // opening 'Anti-Pop' switch (to stop output)
#  define MORSE_GEN_STOP_AUDIO_PA 10 // shutting down audio PA (with 'Anti-Pop')
#  define MORSE_GEN_PASSIVE_NOT_MUTED 11 // "should be passive but couldn't turn off the PA yet"
                          // (even in this state, channel scanning should be paused
                          //  because it causes a terrible noise in the speaker
                          //  whenever the audio PA is enabled. See 
   uint8_t  u8ShiftReg;   // shift register. MSbit first, 0=dot, 1=dash.
   uint8_t  u8NrElements; // number of elements (dots and dashes) remaining
   uint16_t u16Timer;     // countdown timer, decrements in 1.5 ms - steps.
                          // state transition when counted down to zero.
   int8_t i8PitchShift;   // CW pitch offset (+/- N whole tones, 0=no offset)
   uint16_t u16Freq_Hz;   // current audio frequency in Hertz (for Mr Beep)
#  define MORSE_TX_FIFO_LENGTH 40 // must be long enough for 'verbose' output !
   uint8_t u8Fifo[MORSE_TX_FIFO_LENGTH]; // lock-free buffer for transmission
   uint8_t u8FifoHead;  // produce index, modified ONLY by 'writer'
   uint8_t u8FifoTail;  // consume index, modified ONLY by 'reader'
      // If  head == tail, the above buffer is empty.
      // If (head+1) % MORSE_TX_FIFO_LENGTH == tail, it's completely full.
 } T_MorseGen;  
static T_MorseGen morse_generator;

uint8_t Morse_table[] = 
{ // Morse code table, without mutated vowels, diacritics & Co
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // As compact as possible to avoid wasting precious code memory. 
  // Subtract 32 from the character code (ASCII) 
  //          to get an array index into this table. 
  // Convert lower to upper case, because this table ends at 'Z' !
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

   /* Entries [0..15] are ASCII #32..48 :                       */
   0x00,  /* SPACE */
   0x40 + 0x2B,  /*  !  -.-.-- : startbit(6) + 101011bin ("KW") */
   0x00,  /*  "  doesn't exist in Morse code so send as space ? */
   0x00,  /*  #  .. etc, etc ..  */
   0x00,  /*  $                  */ 
   0x00,  /*  %                  */
   0x40 + 0x00,  /*  &  .-...  : startbit(5) +  01000bin ("AS") */
   0x40 + 0x1E,  /*  '  .----. : startbit(6) + 011110bin ("JN") */
   0x20 + 0x16,  /*  (  -.--.  : startbit(5) +  10110bin ("KN") */
   0x40 + 0x2D,  /*  )  -.--.- : startbit(6) + 101101bin ("KK") */ 
   0x00 + 0x00,  /*  *  not seen in any Morse code table yet    */
   0x20 + 0x0A,  /*  +  .-.-.  : startbit(5) +  01010bin ("AR") */
   0x40 + 0x00,  /*  ,  --..-- : startbit(6) + 110011bin        */
   0x40 + 0x21,  /*  -  -....- : startbit(6) + 100001bin        */
   0x40 + 0x15,  /*  .  .-.-.- : startbit(6) + 010101bin        */
   0x20 + 0x12,  /*  /  -..-.  : startbit(5) +  10010bin ("DN") */

   /* Entries [16..25] are the digits '0'..'9' (ASCII # 48..57):*/
   /*0*/ 0x3F, /*1*/ 0x2F, /*2*/ 0x27, /*3*/ 0x23, /*4*/ 0x21,
   /*5*/ 0x20, /*6*/ 0x30, /*7*/ 0x38, /*8*/ 0x3C, /*9*/ 0x3E,

   /* Entries [26..47] are ASCII 58..64, characters ':'..'@' :  */
   0x40 + 0x38,  /*  :  ---... : startbit(6) + 111000bin ("OS") */
   0x00 + 0x00,  /*  ;  -.-.-. : startbit(6) + 101010bin ("KR") */
   0x00 + 0x00,  /*  <  not seen in any Morse code table yet    */
   0x20 + 0x11,  /*  =  -...-  : startbit(5) +  10001bin        */
   0x00 + 0x00,  /*  >  not seen in any Morse code table yet    */
   0x40 + 0x0C,  /*  ?  ..--.. : startbit(6) + 001100bin        */
   0x40 + 0x1A,  /*  @  .--.-. : startbit(6) + 011010bin ("AC") */

   /* Entries #48..#58 for ASCII 65..90, characters 'A'..'Z' :  */
   /* A .-   */  0x05,   /* B -... */ 0x18,   /* C -.-. */ 0x1A,
   /* D -..  */  0x0C,   /* E .    */ 0x02,   /* F ..-. */ 0x12,
   /* G --.  */  0x0E,   /* H .... */ 0x10,   /* I ..   */ 0x04,
   /* J .--- */  0x17,   /* K -.-  */ 0x0D,   /* L .-.. */ 0x14,
   /* M --   */  0x07,   /* N -.   */ 0x06,   /* O ---  */ 0x0F,
   /* P .--. */  0x16,   /* Q --.- */ 0x1D,   /* R .-.  */ 0x0A,
   /* S ...  */  0x08,   /* T -    */ 0x03,   /* U ..-  */ 0x09,
   /* V ...- */  0x11,   /* W .--  */ 0x0B,   /* X -..- */ 0x19,
   /* Y -.-- */  0x1B,   /* Z --.. */ 0x1C,

   /* Entries #59..#63 for ASCII 91..95, characters '['..'_' :  */
   /* ASCII 91: [ (omitted)             */ 0x00,
   /* ASCII 92: \ (like '/' )  : -..-.  */ 0x32,
   /* ASCII 93: ] (omitted)             */ 0x00,
   /* ASCII 94: ^ (like ''' )) : .----. */ 0x5E,
   /* ASCII 95: _ (like '-' )) : -....- */ 0x61
 
 }; /* end Morse_table[] */


#endif // CONFIG_MORSE_OUTPUT ?

uint8_t red_led_timer = 0; // 'countdown' timer, in 1.5 ms steps,
                           // to TEMPORARILY turn the red LED on.
                           // ZERO means 'Tytera controls the LED'.
uint8_t green_led_timer = 0; // similar for the GREEN (RX) LED .

uint8_t  volume_pot_percent; // position of the audio volume pot [percent]
uint16_t battery_voltage_mV; // battery voltage [millivolts]
uint32_t battery_voltage_lp; // internal, for digital lowpass (not 'static' to ease debugging)
uint32_t volume_pot_lp;      // internal, for digital lowpass


// Internal 'forward' references (avoid compiler warnings)
#if( CONFIG_MORSE_OUTPUT )
 static void MorseGen_OnTimerTick( T_MorseGen *pMorseGen);
 static void MorseGen_BeginToSendChar( T_MorseGen *pMorseGen, uint8_t u8ASCII );
#endif // CONFIG_MORSE_OUTPUT ?


//---------------------------------------------------------------------------
// Implementation of functions and interrupt handlers
//---------------------------------------------------------------------------

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
  
  USART_TypeDef *pUSART = USART6; // load the UART's base address (save code space)

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
  
  // Except when the backlight is completely off (dark), the MD380's "Lamp"-
  // output on PC6 is reconfigured as UART6_TX .
  // The pulse width modulation is realized by different UART tx patterns,
  // and by varying the number of STOP BITS for the lower intensity range.
  //
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
  pUSART->BRR = 0xFFF0; // bitrate register 
  // PWM frequency = UART Frame rate. With BRR=0xFFF0, 'OVER8'=0, and 1+9+0.5 bits per frame:
  //   f_pwm = 72 MHz [f_APB2] / ( 10.5 [bits/frame] * 4095.0 [BRR] * 16 [samples/bit] )
  //         = 104.65 Hz .
 
} // InitDimming()
#endif // CONFIG_DIMMED_LIGHT ?

//---------------------------------------------------------------------------
void StartStopwatch( uint32_t *pu32Stopwatch )
  // Starts measuring a time interval .
  // Pass in the address of an 32-bit integer.
  // To determine the elapsed time in milliseconds,
  // call ReadStopwatch_ms() with the same 'stopwatch address'
  // as often as you like.
{ uint32_t t = IRQ_dwSysTickCounter;
  if( t == 0 ) // avoid zero, because zero means 'stopped' 
   {  t = 1;   // (this causes an additional, neglectable delay of 1.5 ms)
   }
  *pu32Stopwatch = t;
}

//---------------------------------------------------------------------------
int ReadStopwatch_ms( uint32_t *pu32Stopwatch )
  // Returns the number of milliseconds elapsed
  // since the last call of StartStopwatch() [for the same watch].
  // You can have as many stopwatches running at the same time
  // as permitted by the amount of RAM. A "running stopwatch" doesn't
  // consume any CPU time. ReadStopwatch_ms() does NOT stop anything.
{ uint32_t diff, t;

  if( *pu32Stopwatch == 0 )  // the time measured by a 'stopped' stopwatch is ZERO
   { return 0;
   }
  t = IRQ_dwSysTickCounter;  // tick counter, increments every 1.5 millseconds
  if( t == 0 ) // avoid zero, because zero means 'stopped' (used in app_menu.c)
   {  t = 1;   // (this causes an additional, neglectable delay of 1.5 ms)
   }
  diff = (int32_t)( t - *pu32Stopwatch);
  // The magic of two's complement makes sure the difference
  // is even correct if the tick counter (t) runs over from 0xFFFFFFFF 
  // to 0x00000000 between starting and reading the 'stopwatch' .
  // If the difference is NEGATIVE here, the time-difference must
  // have been *very* large (2^31 * 1.5 ms = ca. 37 days) :
  if( diff < 0 )
   {  return 0x7FFFFFFF;    // over 37 days have passed .. impressive :)
   }
  else // convert from 1.5ms-timer-ticks to milliseconds:
   {  return diff + diff/2; // return (int)((float)diff * 1.5) [without floats]
   }
} 

#if( CONFIG_MORSE_OUTPUT )
//---------------------------------------------------------------------------
void BeepStart( int freq_Hz, int volume )
  // Programs Mr. Beep's output on "PC8" for a given tone frequency,
  //   with an attempt to keep the volume at a tolerable level.
  // ( In contrast to the schematics, Mr Beep's output is not
  //   volume-controllable by the analog volume pot.
  //   Instead, there's only an RC lowpass between PC8 and the
  //   input to the audio power amplifier. 
  //   Details may still be available at 
  //      www.qsl.net/dl4yhf/RT3/md380_fw.html#morse_output .
  // )
{
  TIM_TypeDef *pTIM8 = TIM8; // only load the base address into a register ONCE
  uint16_t CR1val;

  // With volume = BEEP_VOLUME_AUTO, the beep volume tries to
  // 'follow' the volume control potentiometer, by selecting a 
  // PWM duty cycle depending on volume_pot_percent :
  if( volume == BEEP_VOLUME_AUTO ) 
   {  volume = volume_pot_percent;
   }

 
  // To find out how *Tytera* generate their beep tones,
  // the GPIO_C registers were inspected and analysed.
  // Results at www.qsl.net/dl4yhf/RT3/md380_hw.html#CPU_ports  .
  // 
  RCC->APB2ENR |= RCC_APB2ENR_TIM8EN; // provide a peripheral clock

  // Configure Mr Beep's pin as output for TIMER8, PWM-channel :
  // Two bits in "MODER" per pin : 01bin for GPO, 10bin for 'alternate function mode'.
  GPIOC->MODER = (GPIOC->MODER & ~( 3 << (PINPOS_C_AUDIO_BEEP * 2))) |  ( 2 << (PINPOS_C_AUDIO_BEEP * 2) ) ;
  GPIOC->AFR[1] = (GPIOC->AFR[1] & 0xFFFFFFF0 ) | 0x03; // PC8 AF#3 = "TIM8_CH3"

  // Disable TIMER8 before setting it, but already set the 'clock division':  
  CR1val =   // temporary value for Timer Control Register 1 ...
       0 * TIM_CR1_CEN  // (bit 0) Counter enable ? not yet.
     | 0 * TIM_CR1_UDIS // Update disable ? no thanks.
     | 0 * TIM_CR1_URS  // Update request source : 0=any
     | 0 * TIM_CR1_OPM  // One pulse mode ? heavens, no.
     | 0 * TIM_CR1_DIR  // Direction : 0 = count UP
     | 0 * TIM_CR1_CMS  // Center-aligned mode ? No.
     | 1 * TIM_CR1_ARPE // (bit 7) Auto-reload preload enable (required for PWM; RM0090 page 610)
     | 0 * TIM_CR1_CKD; // Clock division (2 bits) : 00bin = don't divide

  pTIM8->CR1 = CR1val; // set Control Reg1, but don't enable Timer8 yet

  pTIM8->CR2 = 0; // Control Register 2, see RM0090 Rev13 page 563...
             // (OC1..OC4, OC1N..OC3N, output idle states, master mode, CCPC, etc)

  pTIM8->BDTR = TIM_BDTR_MOE; // "break and dead-time": only "Main Output Enable"

  pTIM8->PSC = 71; // PreSCaler : divides 72 MHz by <PSC+1> to f_CK_CNT = 1 MHz
             // > The counter clock frequency (CK_CNT) is f_CK_PSC / (PSC+1).
  pTIM8->RCR = 0;  // Repetition Counter (post-scaler for events) : unused 

  if( freq_Hz < 100/*Hz*/ || freq_Hz > 3000 ) 
   {  freq_Hz = 650; // avoid overflow or div-by-zero below,
      // and 'take meaningful default' if global_addl_config not valid yet
   }
  if( volume < 1/*percent*/ || volume > 100 ) 
   {  volume = 10; 
   }

  // The 16-bit Auto-Reload-Register defines the PWM audio frequency.
  // f_PWM = f_CK_CNT / (ARR+1) ), e.g. 1 MHz / 1537 = 650 Hz :
  pTIM8->ARR = (1000000L / freq_Hz) - 1;
             
  pTIM8->CCR3= // Capture/Compare value (defines the PWM duty cycle)
             (pTIM8->ARR * volume + 999) / 1000;
       // > The active capture/compare register contains the value to be
       // > compared to the counter TIMx_CNT and signalled on OC3 output.
       // The max, ear-deafening volume would be at 50 % PWM duty cycle.
       // But with volume = 100 (%), 10 percent DC was 'loud enough' .

  // The original firmware had 'Update interrupt' enabled (DIER bit 0),
  //     and it implements TIM8_UP_TIM13_IRQHandler to reprogram
  //     the PWM duty cycle after each Timer8-"Update".
  // See www.qsl.net/dl4yhf/RT3/listing.htm#TIM8_UP_TIM13_IRQHandler
  // Here, we don't want to reprogram the timer ("bp_freq[2]"),
  //       we don't need multiple simultaneous tones ("Boooo"...)).
  // So: Disable TIMER8 interrupts while using it for Morse output.
  //   (despite that, the original firmware occasionally disturbs
  //    the Morse output, for example when Mr Beep informs us 
  //    about an unprogrammed memory channel. Patch that out one day?)
  pTIM8->DIER = 0;  // TIM DMA/interrupt enable register : none !
  // Because the original firmware had "UIE" (bit0) enabled in DIER, clear pending IRQ:
  pTIM8->SR = 0x1E7F; // clear bits 12,11,10,9,7,6..0 in 'SR' by *writing ONES*

  // 17.3.10 PWM mode :
  // > Pulse Width Modulation mode allows generating a signal with a frequency 
  // > determined by the value of the TIMx_ARR register and a duty cycle 
  // > determined by the value of the TIMx_CCRx register.
  //  (DL4YHF: here use to reduce the volume to a tolerable level)
  // > The PWM mode can be selected independently on each channel (one PWM 
  // > per OCx output) by writing '110' (PWM mode 1) or '111' (PWM mode 2) 
  // > in the OCxM bits in the TIMx_CCMRx register. The corresponding preload 
  // > register must be enabled by setting the OCxPE bit in TIMx_CCMRx , 
  // > and eventually the auto-reload preload register (in upcounting or 
  // > center-aligned modes) by setting the ARPE bit in the TIMx_CR1 register.
  // > As the preload registers are transferred to the shadow registers only 
  // > when an update event occurs, before starting the counter, the user must 
  // > initialize all the registers by setting the UG bit in TIMx_EGR . (...)
  //  (in other words, the SEQUENCE is important)
  // TIM8_CCER: capture/compare enable register. RM0090 Rev13 page 577..
  pTIM8->CCER &= ~TIM_CCER_CC3E; // first step in TIM_OC3Init() !
  pTIM8->CCMR2 = // Output Compare Mode, for channels 4 and 3, RM0090 page 576 
     0 * TIM_CCMR2_CC3S_0 // Capture/Compare 3 Selection : 00bin = CC3 channel is configured as output
   | 0 * TIM_CCMR2_OC3FE  // Output Compare 3 Fast enable: unnecessary
   | 1 * TIM_CCMR2_OC3PE  // Output Compare 3 Preload enable (*must* be used for PWM, RM0090 page 610)
   | 6 * TIM_CCMR2_OC3M_0 // Output Compare 3 Mode : 110bin = "PWM Mode 1"
   | 0 * TIM_CCMR2_OC3CE; // Output Compare 3 Clear Enable
  pTIM8->CCER |= TIM_CCER_CC3E; // Capture/Compare 3 output enable

  pTIM8->EGR  = TIM_EGR_UG; // "..must initialize all the registers by setting the UG bit.."

  pTIM8->CR1 = CR1val | (1<<0); // enable timer 8 (the counter should be running now)
 
} // BeepStart()

//---------------------------------------------------------------------------
void BeepMute(void) // mutes the beep without turning the audio PA off
{
  TIM8->CCR3 = 0;  // duty cycle = 0, but keep timer running
  TIM8->DIER = 0;  // disable Timer8 "update"-interrupt (reason in BeepStart)
}


//---------------------------------------------------------------------------
void BeepReset(void) 
  // Resets the beeper, and returns to Tytera's default setting .
  // Called from the Morse generator's state machine when 'done'.
{
  TIM_TypeDef *pTIM8 = TIM8; // only load the base address into a register ONCE

  // The original firmware configures Timer8 as PWM output (TIM8_CH3),
  //  counter- and duty cycle range 0..255. Operates like an 8-bit DAC
  //  with a very fast running interrupt. 
  // Below the original Timer8 register values, while generating 
  //  the "Booooo..."-sound to indicate an unprogrammed memory channel.
  //
  //  $ python tool2.py hexdump32 TIM8 80
  //  40010400: 00000081 00000000 00000000 00000001 (CR1, CR2, SMCR, DIER)
  //  40010410: 0000001E 00000000 00006868 00000068 (SR,  EGR, CCMR1,CCMR2)
  //  40010420: 00001111 000000F9 00000013 000000FF (CCER,CNT, PSC,  ARR)
  //  40010430: 00000000 00000000 00000000 00000061 (RCR, CCR1,CCR2, CCR3)
  //  40010440: 00000000 00008000 00000000 00000081 (CCR4,BDTR,DCR,  DMAR)
  // 
  pTIM8->CR1 = 0x80; // set Control Reg1, but don't enable Timer8 yet
  pTIM8->CR2 = 0;    // Control Register 2, as in the original FW
  pTIM8->PSC = 0x13; // PreSCaler : divides 72 MHz by <PSC+1> to f_CK_CNT = 3.6 MHz
  pTIM8->RCR = 0;    // Repetition Counter (post-scaler for events)
  pTIM8->ARR = 0xFF; // PWM base freq = 3.6 MHz / (255+1) = ~~ 14 kHz
  // Don't re-enable the Timer8-"Update" yet,
  //     because ~14062 interrupts per second are a 'heavy CPU load',
  //     which drew about 10 mA from the battery because the STM32F4
  //     can spend less time saving power in WaitForInterruptInIdle()
  //     - see www.qsl.net/dl4yhf/RT3/listing.htm#WaitForInterruptInIdle .
  // The original firmware will enable the interrupt when it needs to,
  //     for example to generate it's own 'Tones/Alerts' ("Boooo...").
  pTIM8->DIER = 0;  // TIM DMA/interrupt enable register : none !
  pTIM8->CCER &= ~TIM_CCER_CC3E; // first step in TIM_OC3Init() !
  pTIM8->CCMR2 = 0x68; // CCMR2 register value as in the original FW
  pTIM8->CCER |= TIM_CCER_CC3E;  // Capture/Compare 3 output enable
  pTIM8->EGR  = TIM_EGR_UG; // "..must initialize all the registers by setting the UG bit.."
  pTIM8->CR1  = 0x81; // original configuration of TIM8_CR1 as in the original FW
  pTIM8->CCR3 = 0x61; // quasi-analog output (after lowpass filtering) to mid level
  pTIM8->BDTR = TIM_BDTR_MOE; // "Main Output Enable" for the PWM (0x8000)

} // end BeepReset()


#endif // "beeper" for CONFIG_MORSE_OUTPUT ?


//---------------------------------------------------------------------------
int IsRxAudioMuted(void) // returns 1 when RX-audio is muted ("no RX signal")
{
  // Don't remove the following table - it's referenced from other modules !
  // From netmon.c (2017-202) : 
  // > mode3   (what's this, "gui_opmode3" or radio_status_1.m3 ? (*)
  // > 0 = idle?
  // > 3 = unprog channel
  // > 5 = block dmr processing?
  // WB: Watched both,  (*)   "gui_opmode3" and "radio_status_1", 
  //     in D13.020-based FW:  @0x2001e892   |    @0x2001e5f0
  //   --------------------------------------+-----------------
  // RX, FM, with signal, busy: 0x10010600   |     0x02002200
  // RX, FM, no sig but active: 0x10010400   |     0x02000200
  // RX, FM, no signal, idle  : 0x10010100.. |     0x00000200..
  //     (bit 9 toggling --->)  0x10010300   |     0x02000200
  // RX, DMR, with signal,busy: 0x10080600   |     0x03604200..
  //                                         |     0x03600200
  // RX, DMR,no sig but active: 0x10010400   |     0x02000200
  // RX, DMR, no signal, idle : 0x10010100.. |     0x02000200 
  //                            0x10010300   |
  // On unprogrammed channel  : 0x00010003   |     0x00000200
  // TX, FM                   : 0x00070A00   |     0x00000D00
  // TX, DMR, talkaround      : 0x10070B00   |     0x00000900..
  //                                   |||   |     0x00000D00 <- bit 10 toggles rapidly 
  //        'mode_bits' below _________/||   |            |
  //                                    ||   |  bit  8: transmitting
  //  mode3 aka gui_opmode3 indicates __//   |  bit  9: receiving ?
  //       an unprogrammed channel           |  bit 10: TX "in current timeslot" ?
  //                                         |  
  // Note: The orignal firmware MOSTLY accesses 'gui_opmode3' as a byte.
  //       But it's in fact a 32-bit word, accessed 8-, 16-, and 32-bit wide.
  //
  uint8_t mode_bits = 0x0F & ((uint8_t*)&gui_opmode3)[1];
  if( mode_bits==4 || mode_bits==1 ) // see lenghty table above ...
   { return 1; // receiver seems to be 'squelched', i.e. audio muted
   }  
  else         // receiver NOT squelched, i.e. RX signal audible:
   { return 0; // the Morse generator must not turn off the audio-PA  now !
   }
} // IsRxAudioMuted()

//---------------------------------------------------------------------------
int MayScanChannels(void) // returns 1 when channels MAY be scanned
  // (because the Morse generator, or whatever in OUR part of the firmware, 
  //  doesn't "occupy" the audio PA. 
  //  Scanning WHILE the PA is active creates a horrible noise in the speaker)
{

#if( CONFIG_MORSE_OUTPUT )
  if( morse_generator.u8State != MORSE_GEN_PASSIVE )
   { // "Morse generator is active so please DON'T scan channels now"
     // (that's the easy part. The difficult part is how to convince the
     //  Tytera firmware *NOT* to scan when this function returns FALSE)
     return FALSE; 
   }
#endif // CONFIG_MORSE_OUTPUT ?

  // Arrived here ? "No objections" (against channel scanning)
  return TRUE;   // "may scan channels" 

} // MayScanChannels()


#if( CONFIG_MORSE_OUTPUT )
//---------------------------------------------------------------------------
uint16_t MorseGen_GetDotLengthInTimerTicks(void)
{
  uint16_t u16Temp = global_addl_config.cw_speed_WPM; 
  if( u16Temp<5 || u16Temp>100)  // avoid div-by-zero use meaningful default:
   { 
     return 40; // default for 20 WPM : 60 ms for a dot
   }
  else // 1.5 ms per timer tick, thus not 1200/WPM, but:
   { return 800 / u16Temp; 
   }
}

//---------------------------------------------------------------------------
static void MorseGen_BeginToSendChar( T_MorseGen *pMorseGen, uint8_t u8ASCII )
  // Prepares the transmission of a single character in Morse code.
  //  [out] pMorseGen->u8ShiftReg, 
  //        pMorseGen->u8NrElements,
  //        pMorseGen->u8State,
  //        pMorseGen->u16Timer .
{
  uint8_t morse_code, nr_elements;
  int i;

  if( global_addl_config.cw_pitch_10Hz < 20 ) // CW pitch not configured ?
   {  global_addl_config.cw_pitch_10Hz = 65;  // use default: 65 * 10 Hz
   }
  if( global_addl_config.cw_volume == 0 )     // Output volume not configured ?
   {  global_addl_config.cw_volume = BEEP_VOLUME_AUTO; // use 'automatic' volume
   }
  if( global_addl_config.cw_speed_WPM == 0 )  // CW speed not configured ?
   {  global_addl_config.cw_speed_WPM = 18;   // use 18 WPM per default 
   }
  // Is the to-be-transmitted character in the Morse code table ?
  if( u8ASCII<=0x12 ) // anything below ASCII 32 is a CONTROL CHARACTER:
   { // \x08 = decrease pitch (audio frequency) by two steps, etc,
     // \x09 = decrease pitch by one step,
     // \x10 = restore normal pitch, as configured in menu,
     // \x11 = increase pitch by one step,
     // \x12 = increase pitch by two steps.
     // Added 2017-03-26 to tell a menu title from a menu item,
     //       when 'announcing' an entire menu on request:
     //   ___ Contacts ___  (title, gray background, sent with slightly lower pitch)
     //       Contacts      (menu item, cyan background, sent with normal pitch)
     //   ##  New Contact## (currently focused item, dark blue background, higher pitch)
     //       Manual Dial   (another unselected item, also cyan background)
     pMorseGen->i8PitchShift/*-2..0..+2*/ = (int8_t)u8ASCII - 0x10;
     // Besides shifting the pitch, control characters are sent like spaces !
   } // end if( u8ASCII< 32 )
  i = global_addl_config.cw_pitch_10Hz * 10;
  i +=  (i * (int)pMorseGen->i8PitchShift) / 64; // f * (1+1/64) = approx 1/4 tone interval
  pMorseGen->u16Freq_Hz = (uint16_t)i; 
  if( u8ASCII< 32 )
   { u8ASCII = 32;  // send other control characters like a SPACE
   }
  if( u8ASCII>='a' && u8ASCII<='z' ) // convert lower to UPPER case
   {  u8ASCII -= ('a'-'A');
   }
  if( u8ASCII>127 )  // no 'special' characters in the small table !   
   { u8ASCII = '?';  // send question mark instead
   }
  morse_code = Morse_table[ u8ASCII - 32 ];

  // Find out the number of elements (dots or dashes) by bitwise
  // shifting the pattern left until the 'start bit' is in bit 7.
  // Only one of eight bits is sacrificed to encode the length.
  // Example 'E' = 0000 0010 bin
  //                      ||__ a single dot
  //                      |___ "startbit" (will be shifted out below)
  nr_elements = 7;
  while(nr_elements>0 && (morse_code & 0x80)==0 )
   { --nr_elements; 
     morse_code <<= 1; 
   }
  morse_code <<= 1; // bit 7 now contains the next dot(0) or dash(1) to be sent

  // HERE, in this entirely state-machine-driven Morse generator,
  //  only start sending the first dot or dash.
  //  The remaining elements will be sent in MorseGen_OnTimerTick().
  pMorseGen->u16Timer = MorseGen_GetDotLengthInTimerTicks();

  if( nr_elements > 0 ) // character is NOT a space..
   { pMorseGen->u8NrElements = nr_elements-1; // # elements remaining
     pMorseGen->u8ShiftReg = morse_code << 1;
     pMorseGen->u8State = MORSE_GEN_SENDING_TONE;
     if( morse_code & 0x80 )
      { pMorseGen->u16Timer *= 3; // dash = 3 dot times
      } 
     BeepStart( pMorseGen->u16Freq_Hz, global_addl_config.cw_volume ); 
   }
  else // pMorseGen->u8NrElements == 0 : SPACE character...
   { pMorseGen->u8NrElements = 0; // neither dots nor dashes to send
     pMorseGen->u16Timer *= 3;    // additional gap, 3 dot times
     pMorseGen->u8State = MORSE_GEN_SENDING_GAP;
     BeepMute();  // silence in the speaker, but audio PA remains ON !
   }

} // MorseGen_BeginToSendChar()
#endif // CONFIG_MORSE_OUTPUT ?


#if( CONFIG_MORSE_OUTPUT )
//---------------------------------------------------------------------------
void MorseGen_ClearTxBuffer(void) // aborts the current Morse transmission (if any)
{
  // Don't lock interrupts here .. only modify the FIFO *HEAD* index:
  morse_generator.u8FifoHead = morse_generator.u8FifoTail; // head==tails means "empty"
  morse_generator.i8PitchShift = 0; // begin with nominal pitch
} 

//---------------------------------------------------------------------------
int MorseGen_GetTxBufferUsage(void) // number of characters waiting for TX
{
  int buffer_usage = morse_generator.u8FifoHead - morse_generator.u8FifoTail;
  if( buffer_usage < 0 )
   {  buffer_usage += MORSE_TX_FIFO_LENGTH; // circular FIFO index wrapped
   }
  return buffer_usage;
}

//---------------------------------------------------------------------------
int MorseGen_AppendChar( char c )
{
  T_MorseGen *pMorseGen = &morse_generator;
  uint8_t u8NewFifoHead;

  pMorseGen->u8FifoHead %= MORSE_TX_FIFO_LENGTH; // safety first
  u8NewFifoHead = (pMorseGen->u8FifoHead + 1) % MORSE_TX_FIFO_LENGTH; 
  if( u8NewFifoHead == pMorseGen->u8FifoTail )
   { return 0; // oops.. running out of buffer space
   }
  pMorseGen->u8Fifo[ pMorseGen->u8FifoHead ] = (uint8_t)c;
  pMorseGen->u8FifoHead = u8NewFifoHead;

  // Start output (a few milliseconds later, in a background process) ?
  if( pMorseGen->u8FifoHead != pMorseGen->u8FifoTail )
   { if( pMorseGen->u8State == MORSE_GEN_PASSIVE 
      || pMorseGen->u8State == MORSE_GEN_PASSIVE_NOT_MUTED )
      {  pMorseGen->u8State =  MORSE_GEN_START;
         // MorseGen_OnTimerTick() will do the rest..
      }
   } 
  return 1;    // successfully appended a character
} // end MorseGen_AppendChar()

//---------------------------------------------------------------------------
int MorseGen_AppendString( // API to send good old 8-bit C-strings
     char *pszText ) // [in] plain old C-string (zero-terminated)
{
  int nCharsAppended = 0;
  int iMaxLen = MORSE_TX_FIFO_LENGTH; // ultimate limit for unterminated strings

  while( iMaxLen > 0 )
   { if( *pszText=='\0' )
      { break; // reached the end of the source string
      }
     if( ! MorseGen_AppendChar( *pszText++ ) )
      { break; // Morse output buffer exhausted
      }
     ++nCharsAppended;
     --iMaxLen;
   }
  return nCharsAppended;
} // MorseGen_AppendString()

int MorseGen_AppendWideString( wchar_t *pwsText ) // API to send 'wide' (16-bit) strings .
{
  int nCharsAppended = 0;
  int iMaxLen = MORSE_TX_FIFO_LENGTH; // ultimate limit for unterminated strings

  while( iMaxLen > 0 )
   { if( *pwsText==0 )
      { break; // reached the end of the source string
      }
     if( ! MorseGen_AppendChar( (char)*pwsText++ ) )
      { break; // Morse output buffer exhausted
      }
     ++nCharsAppended;
     --iMaxLen;
   }
  return nCharsAppended;
}

int MorseGen_AppendDecimal( int i )
{ char sz15[16];
  sprintf(sz15, "%d", i );
  return MorseGen_AppendString( sz15 );
}

#endif // CONFIG_MORSE_OUTPUT ?


#if( CONFIG_MORSE_OUTPUT )
//---------------------------------------------------------------------------
static void MorseGen_OnTimerTick(T_MorseGen *pMorseGen)
  // Called 666 (!) times per second from SysTick_Handler,
  //  as long as the Morse output is active (busy) .
  // Controls the audio output (power amplifier), and produces
  // the Morse code. The 'tone' (waveform) is generated
  // via hardware (simple PWM, no CPU-hogging 'wavetable').
{

  // Because the original firmware isn't aware of the Morse output,
  // it sometimes interfered with the Morse tone, for example when 
  // trying to activate its own (annoying) alert- and keypad tone.
  // When that happened, it enabled the Timer8 interrupt to use
  // Timer8_CH3 as 8-bit pseudo digital/analog converter for the tones
  // (more on that in BeeperReset). So watch out for Tytera's reprogramming
  // of Timer8's "Update Interrupt" enable flag, and if set, immediately
  // reprogram timer 8 as *WE* need it (for the simple/single-frequency tone):
  if( TIM8->DIER ) // oops.. someone has enabled the Timer8 interrupt !
   { // "Whoever" modified TIM8->DIER may also have overwritten
     // the timer's output frequency and PWM duty cycle. Defeat this:
     switch( pMorseGen->u8State )
      { case MORSE_GEN_SENDING_TONE: // immediately turn *OUR* tone on again:
           BeepStart( pMorseGen->u16Freq_Hz, global_addl_config.cw_volume );
           break; 
        case MORSE_GEN_SENDING_GAP : // immediately turn *THEIR* tone off:
           BeepMute();
           break; 
        default: // let Tytera's firmware say "beep", "boooo", "dingdong", etc
           break;
      } 
   }
 
  if(  pMorseGen->u16Timer > 0 ) // state timer not expired yet ?
   { --pMorseGen->u16Timer;   // nothing else to do, besides WAITING ?
     // The original firmware occasionally tried to turn off 
     // the audio PA (because from its point of view there's no
     // reason to keep it on). Patching out all those accesses 
     // to the GPIO registers would be a nightmare. So instead:
     // As long as the Morse output is 'busy', keep on updating
     // the GPIOs for the audio output, 666 times per second.
     if(  (pMorseGen->u8State==MORSE_GEN_SENDING_TONE )
       || (pMorseGen->u8State==MORSE_GEN_SENDING_GAP  )
       || (pMorseGen->u8State==MORSE_GEN_START_CHAR_TX) // <- avoid 'rattling' during gaps
       )
      {
#      if(1) // TEST : What caused the 'rattling noise' during CW output ?
        if( (!IS_AUDIO_AMP_ON) || (!IS_SPKR_SWITCH_ON) )
         { // Gotcha.. the original firmware interfeared :)
           // Sometimes, while a Morse message was still being sent,
           // the firmware decided to reduce the power consumption
           // by turning the audio PA (or something else) off, 
           // causing a 'rattling' noise in the speaker .
           //   (same 10-Hz-"rhythm" as the DC input current,
           //    observable on an old-fashioned ammeter)
           // Cured by forcing the audio-PA on in all 'active' generator states.
           if( global_addl_config.narrator_mode & NARRATOR_MODE_TEST )
            { // show when something 'interfered':
              if( ! IS_RED_LED_ON )
               { red_led_timer = 10; // let the RED (TX) LED flash up for a few ms
               }
            }
         }
#      endif // TEST ?
        AUDIO_AMP_ON;   // keep the supply voltage for the audio PA on
        SPKR_SWITCH_ON; // keep the speaker connected to the audio PA

        // When on an inactive DMR channel, AND the volume pot not at MINIMUM,
        // there was an annoying 'rattling noise' in the speaker.
        // This happened when the DMR chip (C5000) was periodically switched
        // into "sleep mode", causing a step at its 'LINEOUT' pin.
        // When watching the RAM contents (periodically updated),
        // the 16-bit location at 0x2001E5D0 in D13.020 had counted
        // counted down from ~~0x01FF (when active) to ~~ 3 or 4,
        // when the chattering started. Tried to prevent this as follows:
#      ifdef MD380_ADDR_DMR_POWER_SAVE_COUNTDOWN
        *((uint16_t*)MD380_ADDR_DMR_POWER_SAVE_COUNTDOWN) |= 0x80;
#      endif
        // This eliminated the 'rattling' noise, but it's ugly.
        // The least significant bits of this variable seem to have 
        // a special meaning (e.g. in D13.020 at 0x08031f1c),
        // so don't modify the lower bits here.
      }
   }
  else // timer expired, so what's up next (state transition) ?
   { pMorseGen->u16Timer = 30;  // ~~50 ms for most PA on/off- and anti-pop transitions
     switch( pMorseGen->u8State )
      { case MORSE_GEN_PASSIVE : // not active, waiting for start
           break;  // nothing to do
        case MORSE_GEN_START   : // someone request to start the Morse Machine..
           // TODO: Wait until the radio stops TRANSMITTING (RF),
           //       or let the caller (e.g. the 'narrator')
           //       decide whether 'to morse' during TX or not ? 
           // If the radio's Audio power amplifier isn't powered up yet,
           // turn it on. To reduce the 'pop' in the speaker, open the
           // two N-channel MOSFET switches between audio PA and speaker:
           SPKR_SWITCH_OFF; // 'anti-pop' switch between PA and speaker open
           pMorseGen->u8State = MORSE_GEN_START_AP_OPEN;
           break;
        case MORSE_GEN_START_AP_OPEN:  // anti-pop switch is now open
           AUDIO_AMP_ON;    // turn on the supply voltage for the audio PA
           pMorseGen->u8State = MORSE_GEN_START_AUDIO_PA;
           break;      
        case MORSE_GEN_START_AUDIO_PA: // been waiting for audio PA to start
           SPKR_SWITCH_ON; // close 'anti-pop' switch between PA and speaker
           pMorseGen->u8State = MORSE_GEN_START_AP_CLOSE;
           break;
        case MORSE_GEN_START_AP_CLOSE: // anti-pop switch is now closed
           pMorseGen->u8State = MORSE_GEN_START_CHAR_TX;
           break;
        case MORSE_GEN_START_CHAR_TX:  // start transmission of next char
           if( pMorseGen->u8FifoHead != pMorseGen->u8FifoTail )
            { // there's more to send...
              MorseGen_BeginToSendChar( pMorseGen, 
                 pMorseGen->u8Fifo[pMorseGen->u8FifoTail++] );
              pMorseGen->u8FifoTail %= MORSE_TX_FIFO_LENGTH;  
            }
           else // transmit FIFO is empty -> end of Morse message(s) !
            { pMorseGen->u8State = MORSE_GEN_END_OF_MESSAGE;
            }
           break; 
        case MORSE_GEN_SENDING_TONE: // finished sending a tone (dash or dot)
           BeepMute(); 
           pMorseGen->u8State = MORSE_GEN_SENDING_GAP; 
           break;
        case MORSE_GEN_SENDING_GAP:  // finished sending a gap 
           pMorseGen->u16Timer = MorseGen_GetDotLengthInTimerTicks();
           // Send another dot or dash, or begin the next character ?
           if( pMorseGen->u8NrElements > 0 ) // send next dot or dash
            { --pMorseGen->u8NrElements;
              if( pMorseGen->u8ShiftReg & 0x80 )
               { pMorseGen->u16Timer *= 3; // dash = 3 dot times
               } 
              pMorseGen->u8ShiftReg <<= 1;
              pMorseGen->u8State = MORSE_GEN_SENDING_TONE;
              BeepStart( pMorseGen->u16Freq_Hz, global_addl_config.cw_volume );
            }
           else // inter-character gap ...
            { // The spacing between two characters (within a WORD)
              // is 3 dots; a gap of 1 dot is already over. Thus:
              pMorseGen->u16Timer *= 2;                     // after long gap,
              pMorseGen->u8State = MORSE_GEN_START_CHAR_TX; //  send next char
            }
           break;

        case MORSE_GEN_END_OF_MESSAGE: // all characters sent, 'back to normal'
           BeepReset();    // reprogram Timer8 to Tytera's own configuration
           // If the audio PA was off *before* sending the Morse message(s),
           // we may be tempted to simply turning it off again here.
           // But in the meantime, the receiver's squelch may have opened !
           // So only turn the audio-PA off if the RX-audio is muted NOW :
           if( IsRxAudioMuted() )
            { // guess it's ok to turn the audio-PA off now:
              SPKR_SWITCH_OFF; // disconnect speaker from audio PA,
              // but wait before powering down the PA itself :
              pMorseGen->u8State = MORSE_GEN_STOP_ANTI_POP;
            }
           else // end of Morse message, but keep audio-PA on:
            { pMorseGen->u8State = MORSE_GEN_PASSIVE_NOT_MUTED;
              red_led_timer = 20; // "there was a problem with the audio PA control"
            }  
           break;

        case MORSE_GEN_STOP_ANTI_POP: // speaker has been disconnected from audio PA
           AUDIO_AMP_OFF;  // .. so turn off the audio PA without a 'pop'
           pMorseGen->u8State = MORSE_GEN_STOP_AUDIO_PA;
           break;
        case MORSE_GEN_STOP_AUDIO_PA: // audio PA has been turned off (completely)
           pMorseGen->u8State = MORSE_GEN_PASSIVE;
           break;
        case MORSE_GEN_PASSIVE_NOT_MUTED: // end of audio message but audio PA kept running..
           BeepMute();
           // check again if the rx-audio is muted by the original firmware,
           // or if the radio is about to enter power-saving mode 
           // (during which the C5000 creates the 'rattling noise' on LINEOUT):
           if( IsRxAudioMuted() 
#          ifdef MD380_ADDR_DMR_POWER_SAVE_COUNTDOWN
             || (MD380_ADDR_DMR_POWER_SAVE_COUNTDOWN<10) 
#          endif
             )
            { // guess it's ok to turn the audio-PA off now:
              SPKR_SWITCH_OFF; // disconnect speaker from audio PA,
              // but wait before powering down the PA itself :
              pMorseGen->u8State = MORSE_GEN_STOP_ANTI_POP;
              // (don't leave this state without turning the audio PA off,
              //  even if it takes minutes. WE turned the PA on, so WE must
              //  turn it off again, when the 'rx audio' is over)
            }
           break;
        default: // oops..
           pMorseGen->u8State = MORSE_GEN_PASSIVE;
           break; 
      }   // end switch( pMorseGen->u8State )
   }     // end else < timer expired >
}       // end MorseGen_OnTimerTick()
#endif // CONFIG_MORSE_OUTPUT ?

//---------------------------------------------------------------------------
static void PollAnalogInputs(void)
  // [out] battery_voltage_mV, volume_pot_pos [%] .
{ // Battery voltage and the analog volume potentiometer
  // are sampled per DMA and ADC1 on PA1. 
  // Only READ the conversion results here, but don't interfere
  // with the A/D conversion itself !
  // SIX of the ADC's inputs are 'transported' via DMA2, stream 0.
  // Details at www.qsl.net/dl4yhf/RT3/md380_hw.html#ADC1 .
  uint32_t result; 
  // Retrieve the address of the conversion result from the DMA controller.
  // Eliminates the daunting task to find out the RAM address for different
  // firmware versions and add them in the symbol file. "Hardware doesn't lie". 
  uint16_t *pwConvResults = (uint16_t *)DMA2_Stream0->M0AR;
  // We don't want to crash with an access violation if someone calls us at
  // the wrong time (before DMA2.Stream0.Mode0.AddressRegister is set), thus:
  if( (pwConvResults > (uint16_t*)0x20000000)   // DMA destination address..
   && (pwConvResults < (uint16_t*)0x20020000) ) // ..looks like RAM, ok
   { // last of the 6 converted channels from ADC1 = 'battery voltage' :
     result = pwConvResults[5];  // battery voltage, raw value from ADC
     // The 'raw' result was very noisy, even with a rock-solid 8.4 V supply.
     // Theory: "Vbatt" divided by 100k/(100k+200k) = 1/3, Vref=3.3 V;
     //     0x0FFF = 4095 at 3.3 V * 3 ->
     //     conversion factor = 9900 mV/4095 = 2.41 .
     // Practice: Don't bet on the 3.3 V "reference". It's rubbish.
     result = (290 * result) / 100;  // -> vbat in millivolts 
     // Because the conversion result was very noisy,
     // run the result through a crude 1st order lowpass .
     //  See en.wikipedia.org/wiki/Low-pass_filter,
     //  "algorithmic implementation of a time-discrete filter":
     // > alpha = t_sample / ( tau + t_sample )
     // > y[i]  = alpha * x[i] + (1-alpha) * y[i-1]
     //  where:
     //   tau      = time constant (R*C), here: ca. 2 seconds
     //   t_sample = sampling interval,   here: 16 * 1.5 ms = 0.024 seconds
     //   alpha    = 0.024 / 2.024 = 0.01185 = circa 0.01 to simplify: 
     // To avoid floating point coeffs, multiply all terms by 100:
     // > 100 * y[i] = 100 * alpha * x[i] + 100 * (1-alpha) * y[i-1]
     // With alpha = 0.01, and y_lp = 100*y :
     // > y_lp[i] = x[i] + 99 * y[i-1] = x[i] + 99*y_lp[i-1] / 100 
     //    |         |________                      |
     //    |                  |                     |
     battery_voltage_lp = result + (99 * battery_voltage_lp) / 100;
     battery_voltage_mV = battery_voltage_lp / 100; // scale to mV for convenience
     // Similar digital low-pass for the analog volume pot:
     // This ADC channel suffers a lot from noise caused
     // by the "DMR chip" (C5000) periodically entering sleep-
     // mode. At 100% volume in FM (squelched, muted), the result
     // was around 2000, but IN IDLE on a DMR channel,
     // it jumped between 1000 and 4000(!) . Thus only update
     // the 'volume pot position' here when the radio is NOT in
     // power-saving (idle) mode:
#   ifdef MD380_ADDR_DMR_POWER_SAVE_COUNTDOWN
     if( *((uint16_t*)MD380_ADDR_DMR_POWER_SAVE_COUNTDOWN) > 10 )
#   endif
      { // "DMR chip" (C5000) not toggling it's LINEOUT-pin at the moment,
        // so we MAY get a 'good reading' from this ADC channel :
        result = pwConvResults[4];  // volume pot, also read per DMA from ADC1
        volume_pot_lp = result + (99 * volume_pot_lp) / 100;
        result = (uint16_t)(volume_pot_lp / 2000); // scale to percent
        if( result>100 )
         {  result=100;
         }
        volume_pot_percent = (uint8_t)result; // -> position of the volume pot, 0..100 percent
      } // end if <not in "power-saving mode" (no "rattling" on LINEOUT)>
   }
} // end PollAnalogInputs() 


#if( CAN_POLL_KEYS ) // <- def'd as 0 or 1 in keyb.h, depends on firmware variant, subject to change
//---------------------------------------------------------------------------
char KeyRowColToASCII(uint16_t kb_row_col)
{ // Converts a hardware-specific keyboard code into a character.
  // Implemented 2017-03-31 for the alternative menu .
  //   [in]  16-bit "row/column" combination shown below
  //   [out] simple 8-bit character also shown in the table:
  //       (lower case letters are reserved for Morse input)
  //    ___________________________________    
  //   | 'M'ENU | cursor | cursor | 'B'ACK |   __
  //   |(green) |  up, U | down,D | (red)  |     \  may have to be used
  //   | 0x000A | 0x0012 | 0x0022 | 0x0402 |   __/  as 'A'..'D' for DTMF
  //   |--------+--------+--------+--------|   __
  //   |  '1'   |  '2'   |  '3'   |  '*'   |     |
  //   | 0x000C | 0x0014 | 0x0024 | 0x0404 |     |
  //   |--------+--------+--------+--------|     |
  //   |  '4'   |  '5'   |  '6'   |  '0'   |      \  "DTMF"-like,
  //   | 0x0044 | 0x0084 | 0x0104 | 0x0204 |      /  with non-standard
  //   |--------+--------+--------+--------|     |   layout
  //   |  '7'   |  '8'   |  '9'   |  '#'   |     |
  //   | 0x0042 | 0x0082 | 0x0102 | 0x0202 |     |
  //   |________|________|________|________|   --
  //  
  switch( kb_row_col ) // sorted by switch-value for shortest code..
   { case 0x000A : return 'M'; // Green 'Menu' key (which usually opens TYTERA's menu)
     case 0x000C : return '1';
     case 0x0012 : return 'U'; // cursor up
     case 0x0014 : return '2';
     case 0x0022 : return 'D'; // cursor down
     case 0x0024 : return '3';
     case 0x0042 : return '7';
     case 0x0044 : return '4';
     case 0x0082 : return '8';
     case 0x0084 : return '5';
     case 0x0102 : return '9';
     case 0x0104 : return '6';
     case 0x0202 : return '#';
     case 0x0204 : return '0';
     case 0x0402 : return 'B'; // 'back' aka 'red button'
     case 0x0404 : return '*';
     // kb_row_col_pressed also supports a few COMBINATIONS:
     //   MENU+BACK (simultaneously pressed) : 0x040A
     case 0x040A : return 'X'; // eXit all menus and sub-menus
     default     : return 0;
   }
} // end KeyRowColToASCII()
#endif // CAN_POLL_KEYS ?


#if( CAN_POLL_KEYS && CONFIG_APP_MENU ) // optional feature ...
//---------------------------------------------------------------------------
static void PollKeysForAppMenu(void)
  // Non-intrusive polling of keys for the 'app menu' (activated 
  //  by pressing the red 'BACK'-button),
  // when that button isn't used to control Tytera's own 'geen' menu.
  // Called approximately once every 24 milliseconds from SysTick_Handler(), 
  // so don't call anything else from here (especially nothing in
  // the original firmware, unless you know exactly what you're doing).
  // Only peek at a few locations in RAM, and carefully set some others.
{
  static uint8_t green_menu_countdown=0;
  static uint8_t autorepeat_countdown=0;
  static uint8_t longpress_countdown=0;
  static uint16_t prev_key;
  uint8_t key = KeyRowColToASCII( kb_row_col_pressed ); 
  // 'kb_keycode' is useless here because it doesn't return to zero 
  // when releasing a key.
  // So use 'kb_row_col_pressed' (16 bit) instead . Seems to be the
  // lowest level of polling the keyboard matrix without rolling our own.
  // 
  // Our own ("app-") menu must not interfere with Tytera's "green" menu,
  // where the red "BACK"-button switches back from any submenu to the
  // parent, and from the main menu to the main screen.
  // Only if the app-menu is already open (possibly kicked open via sidekey),
  // ignore gui_opmode2 and pass keyboard events to our own app-menu.
  if( (gui_opmode2 == OPM2_MENU ) && (!Menu_IsVisible()) )
   { // keyboard focus currently on Tytera's 'green' menu 
     // -> ignore kb_row_col_pressed until the key was released .
     // But because the screen sometimes froze when trying to 
     //   force redrawing the "idle" screen by setting
     //   gui_opmode2=OPM2_MENU; gui_opmode1=SCR_MODE_IDLE|0x80 ),
     // holding down the red 'BACK' button pressed for a second
     // will always activate the alternative 'app menu' .
     green_menu_countdown = 200/*ms*/ / 24; 
   }
  else // keyboard focus not on Tytera's ('green') menu...
   {   // so is it "our" key now ?  Not necessarily !
     // Tytera's menu already quits when PRESSING the red button,
     // so just because the red button is PRESSED doesn't mean 
     // the operator wants to open our 'red menu'.  Thus:
     if( green_menu_countdown > 0 )
      { if( key==0 )
         { --green_menu_countdown;
         }
        else // guess the RED BUTTON is still pressed after leaving the GREEN-button-menu
         { green_menu_countdown = 200/*ms*/ / 24; // ignore keypress for another 200 ms
         }
      }
   }
  // Independent keyboard polling for the alternative menu.. and maybe others
  if( prev_key==0 && key!=0 )
   { if( green_menu_countdown == 0)
      { Menu_OnKey( key ); 
      }
     // no fancy FIFO but a simple 1-level buffer.
     // Consumed in another task or thread, see app_menu.c 
     autorepeat_countdown = 500/*ms*/ / 24; // <- autorepeat DELAY
     longpress_countdown = 2000/*ms*/ / 24;
     keypress_timer_ms = 1; 
   }
  else // no CHANGE in the keyboard matrix, but maybe...
   { if( key!=0 )
      { if(  longpress_countdown > 0 )
         { --longpress_countdown;
         }
        if( keypress_timer_ms < (65535-24) )
         {  keypress_timer_ms += 24;
         }
      }
     else
      { keypress_timer_ms = 0;
      }
     if( key=='U' || key=='D' || key=='#' ) // 'auto-repeatable' key still pressed ?
      { if(  autorepeat_countdown > 0 )
         { --autorepeat_countdown;
         }
        else // send the same key again, prevents rubbing the paint off..  
         { autorepeat_countdown = 130/*ms*/ / 24; // 1 / "autorepeat RATE"
           Menu_OnKey( key );
         }   
      }
     if( key=='B' )    // red 'BACK' key..
      { if( longpress_countdown==1 ) // ..pressed for a "long" time..
         { if( ! Menu_IsVisible() )  // ... so enter the alternative menu regardless of "gui_opmode2" & Co !
            { Menu_OnKey( key ); 
            }
         }
      }
   } 
  prev_key = key;
  keypress_ascii = key;  // also store ASCII in global var, can be polled anywhere

} // end PollKeysForAppMenu()
#endif // CONFIG_APP_MENU ?


//---------------------------------------------------------------------------
void SysTick_Handler(void)
  // ISR to generate the PWM'ed output for the backlight,
  //     and to produce a well-timed output in Morse code. 
  // Called every 1.5 ms instead of the original SysTick_Handler() .
  // The address of *OUR* SysTick_Handler must be patched into the ORIGINAL
  // vector table, and the address of the original handler must be known
  // because we also call the original handler from here.
{
  uint32_t oldSysTickCounter = IRQ_dwSysTickCounter; 
    // this local copy is more efficient to read than the global variable,
    // because the C compiler needs to load addresses of global variables
    // from the literal pool. 
    // Save code space, we may be running short of it one day !

#if( CONFIG_DIMMED_LIGHT ) // simple GPIO "bit banging", min PWM pulse with = one 'SysTick' period. 
  // [in] global_addl_config.backlight_intensities : configurable in applet/src/menu.c
  uint8_t intensity = global_addl_config.backlight_intensities; // bits 3..0 for IDLE, bits 7..4 for ACTIVE intensity
#endif  // CONFIG_DIMMED_LIGHT ?

  // Play safe, assume anything can happen, and DISABLE interrupts in the core
  // before fiddling around with GPIO-, UART-, RCC-, and who knows what else .
  // Avoid dependencies, don't use OS_ENTER_CRITICAL / OS_EXIT_CRITICAL here ! 
  int irq_was_disabled = __get_PRIMASK();  // ugly, non-portable, but .. ?
  __disable_irq(); // -> cpsid I (disable IRQ by setting PRIMASK on Cortex-M)

  if( oldSysTickCounter== 0 ) // VERY first call ? 
   { // Init whatever we need to initialize. This is ugly, but eliminates
     // the need to hook into a bunch of functions in the original firmware.

#   if( CONFIG_MORSE_OUTPUT ) // Initial settings for the Morse generator.
     // Will be in use as long as there's nothing in global_addl_config :
     morse_generator.u8State = MORSE_GEN_PASSIVE; // initial state
#   endif // CONFIG_MORSE_OUTPUT ? 

   } // end if < 1st call of SysTick_Handler >

  // "Wait" until the original firmware turns on the backlight:
  if( ! (boot_flags & BOOT_FLAG_INIT_BACKLIGHT) ) 
   { // Did the original firmware turn on the backlight ? 
     // First of all, did it turn on the peripheral clocks for GPIO A..E ?
#    define RCC_AHB1ENR_GPIO_A_to_E (RCC_AHB1ENR_GPIOAEN|RCC_AHB1ENR_GPIOBEN|RCC_AHB1ENR_GPIOCEN|RCC_AHB1ENR_GPIODEN|RCC_AHB1ENR_GPIOEEN)
     if( (RCC->AHB1ENR & RCC_AHB1ENR_GPIO_A_to_E) == RCC_AHB1ENR_GPIO_A_to_E )
      { // Did the firmware configure backlight and RX,TX-LEDs as output ?
        if(   ( (GPIOC->MODER & (3<<(PINPOS_C_BL*2)) ) == (1<<(PINPOS_C_BL*2)) ) 
           && ( (GPIOE->MODER & (3<<(PINPOS_E_TX*2)) ) == (1<<(PINPOS_E_TX*2)) )
           && ( (GPIOE->MODER & (3<<(PINPOS_E_RX*2)) ) == (1<<(PINPOS_E_RX*2)) )
          )
         { // Did the firmware turn the backlight on (for the 1st time) ?  
           if( GPIOC->ODR & (1<<6) ) // backlight-bit in Output Data Register set ?
            { // Yes; guess it's ok to "take over" backlight control now .
#            if( CONFIG_DIMMED_LIGHT ) // Support dimmed backlight ?
              InitDimming();  // switch from GPIO- to UART-generated PWM
#            endif
              LOGB("gpio initialized\n");
              boot_flags |= BOOT_FLAG_INIT_BACKLIGHT; // may start dimming now
            }
         }
      }
   }    // end if < backlight (GPIO) not initialized yet ? >   

  if( (boot_flags & ( BOOT_FLAG_INIT_BACKLIGHT | BOOT_FLAG_LOADED_CONFIG | BOOT_FLAG_DREW_STATUSLINE ) )
                 != ( BOOT_FLAG_INIT_BACKLIGHT | BOOT_FLAG_LOADED_CONFIG | BOOT_FLAG_DREW_STATUSLINE ) )
   { // As long as the original firmware is still "booting",
     // we cannot poll the keyboard, and should not drive the display.
     // Don't increment IRQ_dwSysTickCounter yet, because it is polled
     // in several modules to do 'something shortly after power-on',
     // for example check for a long keypress to activate Morse output.
     IRQ_dwSysTickCounter = 1;
   }
  else // all necessary functions have been called - "open for business" ?
   { IRQ_dwSysTickCounter++;
     if(red_led_timer) // <- rarely used, only hijack the red LED for TESTING !
      { LED_RED_ON;    // "debug signal 1" : short, single flash with the RED LED
        if( (--red_led_timer) == 0 ) // only in the moment this timer expires,
         { LED_RED_OFF;              // turn the red (TX-) LED off again .
         } // note: almost "no" time is wasted in SysTick_Handler when timer=0 .
      }
     if( green_led_timer ) // same for the green (RX-) LED : ONLY USE FOR TESTING  
      { LED_GREEN_ON; // "debug signal 2": short, single flash with the GREEN LED
        if( (--green_led_timer) == 0 )
         { LED_GREEN_OFF;
         }
      }

     if( oldSysTickCounter > 200 ) // multiplexed keyboard-polling should be finished now,
      { boot_flags |= BOOT_FLAG_POLLED_KEYBOARD; // and kb_row_col_pressed should be valid
        // (important to check for 'keys pressed during power-on')
      }


     // 2017-05-14 : Removed the brightness 'ramp-up' test. 
     // The 9 intensity levels can now be tested in app_menu.c in inc/dec edit mode.
#   if( CONFIG_DIMMED_LIGHT ) // Support dimmed backlight ?
     if( (backlight_timer>0) || (md380_radio_config.backlight_time==0) )
      { // If the backlight time is ZERO, use the 'radio-active' setting ("backlight level HI")
        intensity >>= 4;  // RADIO-ACTIVE state : use the upper 4 nibbles of this BYTE
        // When "active", the backlight shouldn't be completely off, so..
        if(intensity < 1) // invalid setting for the RADIO-ACTIVE state ?
         { intensity = 9; // 'hum-free' default : MAX brightness (no PWM) 
         }
      } // end if < backlight should be "on" (active state) > 
     else // "passive" state (no keypress for a long time), *AND* backlight_time nonzero :
      { // backlight may be off. It will be turned on when pressing a key.
        // Nothing to do here . intensity=0 is ok .
      }
     intensity &= 0x0F;   // 4-bit value, but only steps 0..9 are really used
 
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
     if( intensity == 0 || kb_backlight )  // backlight shall be COMPLETELY DARK ->
      { // Reconfigure PC6 as 'GPIO' to turn the backlight COMPLETELY off .
        // Two bits in "MODER" per pin : 00bin for GPI,  01bin for GPO, 10bin for 'alternate function mode'.
        GPIOC->MODER = (GPIOC->MODER & ~( 3 << (6/*pin*/ * 2))) |  ( 1 << (6/*pin*/ * 2) ) ;
        // hope we didn't interrupt another read-modify-write on this register.
        // (if it happens, no big problem, the NEXT SysTick-interrupt will put things right again)
        GPIOC->BSRRH = (1<<6);  // turn BL off (completely, as GPIO, no PWM)
      }  
     else // backlight not 'completely dark', so configure PC6 as UART_TXD and send PWM pattern:
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
#   endif  // CONFIG_DIMMED_LIGHT ?

     // Poll analog inputs...
     if( (oldSysTickCounter & 0x0F) == 0 ) // .. on every 16-th SysTick
      { PollAnalogInputs(); // -> battery_voltage_mV, volume_pot_pos 
      }
#   if( CAN_POLL_KEYS && CONFIG_APP_MENU ) // optional feature, 
     // depending on the value defined as CONFIG_APP_MENU in config.h:
     if( (oldSysTickCounter & 0x0F) == 1 ) // .. on every 16-th SysTick
      { // (but not in the same interrupt as PollAnalogInputs)
        PollKeysForAppMenu(); // non-intrusive polling of keys for the 
        // 'red menu' (menu activated by pressing the red 'BACK'-button,
        // when that button isn't used to control Tytera's own menu).
      }
#   endif // CONFIG_APP_MENU ?

#   if( CONFIG_MORSE_OUTPUT ) // Morse output (optional, since 2017-02-19) ?
     if( morse_generator.u8State != MORSE_GEN_PASSIVE )
      { // Only spend time on this when active !
        MorseGen_OnTimerTick( &morse_generator );
      }
#   endif  // CONFIG_MORSE_OUTPUT ?
   } // end if < all necessary bits in boot_flags set > ?
 

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
