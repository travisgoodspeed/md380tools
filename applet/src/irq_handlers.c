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
  
  4. Add a patch for SysTick_Handler in merge_d13.020.py (etc) .
  
  5. Issue "make clean image_D13", and carefully watch the output.
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
  // For trivial GPIO accesses as used here, avoid ST's bulky library !
#define PINPOS_C_BL 6 /* pin position of backlight output within GPIO_C */
#define PINPOS_E_TX 1 /* pin position of the red   TX LED within GPIO_E */
#define PINPOS_E_RX 0 /* pin position of the green RX LED within GPIO_E */
#define LED_GREEN_ON  GPIOE->BSRRL=(1<<PINPOS_E_RX) /* green LED on  */
#define LED_GREEN_OFF GPIOE->BSRRH=(1<<PINPOS_E_RX) /* green LED off */
#define IS_RX_LED_ON (GPIOE->ODR&(1<<PINPOS_E_RX)!=0) /* poll green RX LED*/
#define LED_RED_ON    GPIOE->BSRRL=(1<<PINPOS_E_TX) /* red LED on  */
#define LED_RED_OFF   GPIOE->BSRRH=(1<<PINPOS_E_TX) /* red LED off */

  // How to poll a few keys 'directly' after power-on ? 
  // The schematic shows "K3" from the PTT pad to the STM32, 
  // but it doesn't look like an INPUT (may be a multiplexer
  // to poll the two "side buttons" shared with LCD_D6 & D7 .. eeek)
  // -> leave that for later... something like
  // "keep 'M' pressed on power-on for Morse output" will be difficult.

  // How to control audio power amplifier and speaker ?
  // (to generate message beeps without wading through the mud 
  //  of the original firmware, which seems to use DMA, DAC,
  //  and a bunch of hardware timers for this purpose...)
  // Again, the schematic diagram doesn't tell the full story.
  //  Details: www.qsl.net/dl4yhf/RT3/md380_fw.html#audio_message_beeps .
  // And again, beware of the dreadful register names:
  //   BSRRH sets a portbit LOW in an atomic sequence,
  //   BSRRL sets a portbit HIGH ! ! !
#define PINPOS_C_AUDIO_BEEP 8 /* pin pos of the 'beep' output, GPIO_C */
#define PINPOS_B_AUDIO_PA   9 /* pin pos of AF Control (PA),   GPIO_B */
#define PINPOS_B_SPK_C      8 /* pin pos of SPK_C (Anti-Pop?), GPIO_B */
#define AUDIO_BEEP_LO  GPIOC->BSRRH=(1<<PINPOS_C_AUDIO_BEEP) /* squarewave L */
#define AUDIO_BEEP_HI  GPIOC->BSRRL=(1<<PINPOS_C_AUDIO_BEEP) /* squarewave H */
#define AUDIO_AMP_ON   GPIOB->BSRRL=(1<<PINPOS_B_AUDIO_PA) /* audio PA on ("AFCO") */
#define AUDIO_AMP_OFF  GPIOB->BSRRH=(1<<PINPOS_B_AUDIO_PA) /* audio PA off */
#define SPKR_SWITCH_ON GPIOB->BSRRH=(1<<PINPOS_B_SPK_C)  /* speaker on */
#define SPKR_SWITCH_OFF GPIOB->BSRRL=(1<<PINPOS_B_SPK_C) /* speaker off*/


volatile uint32_t IRQ_dwSysTickCounter = 0; // Incremented each 1.5 ms. Rolls over from FFFFFFFF to 0 after 74 days
#if( CONFIG_DIMMED_LIGHT )
 static uint8_t may_turn_on_backlight = 0; // ok to turn on the backlight ? 0=no, 1=yes
#endif // CONFIG_DIMMED_LIGHT ?

#if( CONFIG_MORSE_OUTPUT )
typedef struct tMorseGenerator
 { uint8_t u8State; // state machine to generate Morse output:
#    define MORSE_GEN_PASSIVE 0 // not active, waiting for start
#    define MORSE_GEN_START   1 // request to start output
   // All other states (below) must be considered 'volatile',
   // because the timer interrupt may switch u8State anytime:
#  define MORSE_GEN_START_AUDIO_PA 2 // waiting for audio PA to start
#  define MORSE_GEN_START_ANTI_POP 3 // waiting for 'Anti-Pop' switch
#  define MORSE_GEN_START_CHAR_TX  4 // waiting for begin of the next char
#  define MORSE_GEN_SENDING_TONE   5 // sending a tone (dash or dot)
#  define MORSE_GEN_SENDING_GAP    6 // sending gap (between dashes and dots)
#  define MORSE_GEN_END_OF_MESSAGE 7 // all characters sent; reached end of message
#  define MORSE_GEN_STOP_ANTI_POP  8 // opening 'Anti-Pop' switch (to stop output)
#  define MORSE_GEN_STOP_AUDIO_PA  9 // shutting down audio PA (with 'Anti-Pop')
   uint8_t  u8ShiftReg;   // shift register. MSbit first, 0=dot, 1=dash.
   uint8_t  u8NrElements; // number of elements (dots and dashes) remaining
   uint16_t u16DotLength; // length of a Morse code dot in 1.5 ms - units
   uint16_t u16Freq;      // configurable tone frequency in Hertz
   uint8_t  u8Volume;     // volume (-> PWM duty cycle) in percent 
   uint16_t u16Timer;     // countdown timer, decrements in 1.5 ms - steps,
                          // state transition when counted down to zero
#  define MORSE_TX_FIFO_LENGTH 20 // long enough for channel- or zone names ?
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

   /* Entries [48..58] for ASCII 65..90, characters 'A'..'Z' :  */
   /* A .-   */  0x05,   /* B -... */ 0x18,   /* C -.-. */ 0x1A,
   /* D -..  */  0x0C,   /* E .    */ 0x02,   /* F ..-. */ 0x12,
   /* G --.  */  0x0E,   /* H .... */ 0x10,   /* I ..   */ 0x04,
   /* J .--- */  0x17,   /* K -.-  */ 0x0D,   /* L .-.. */ 0x14,
   /* M --   */  0x07,   /* N -.   */ 0x06,   /* O ---  */ 0x0F,
   /* P .--. */  0x16,   /* Q --.- */ 0x1D,   /* R .-.  */ 0x0A,
   /* S ...  */  0x08,   /* T -    */ 0x03,   /* U ..-  */ 0x09,
   /* V ...- */  0x11,   /* W .--  */ 0x0B,   /* X -..- */ 0x19,
   /* Y -.-- */  0x1B,   /* Z --.. */ 0x1C
 
 }; /* end Morse_table[] */


#endif // CONFIG_MORSE_OUTPUT ?


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


#if( CONFIG_MORSE_OUTPUT )
//---------------------------------------------------------------------------
static void BeepStart( int freq_Hz, int volume )
  // Programs Mr. Beep's output on "PC8") for a given tone frequency,
  //   with an attempt to keep the volume at a tolerable level.
  // ( In contrast to the schematics, Mr Beep's output is not
  //   volume-controllable by the analog volume pot.
  //   Instead, there's only an RC lowpass between PC8 and the
  //   input to the audio power amplifier. 
  //   Details may still be available at 
  //      www.qsl.net/dl4yhf/RT3/md380_fw.html#morse_output  
  // )
{
  TIM_TypeDef *pTIM8 = TIM8; // only load the base address into a register ONCE
  uint16_t CR1val;
 
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

  if( freq_Hz < 100/*Hz*/ ) 
   {  freq_Hz = 100; // avoid div-by-zero below !
   }
  // The 16-bit Auto-Reload-Register defines the PWM audio frequency.
  // f_PWM = f_CK_CNT / (ARR+1) ), e.g. 1 MHz / 1537 = 650 Hz :
  pTIM8->ARR = (1000000L / freq_Hz) - 1;
             
  pTIM8->CCR3= // Capture/Compare value (defines the PWM duty cycle)
             (pTIM8->ARR * volume + 999) / 1000;
       // > The active capture/compare register contains the value to be
       // > compared to the counter TIMx_CNT and signalled on OC3 output.
       // The max, ear-deafening volume would be at 50 % PWM duty cycle.

  // The original firmware had 'Update interrupt' enabled (bit 0),
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
static void BeepMute(void) // mutes the beep without turning the audio PA off
{
  TIM8->CCR3 = 0;  // duty cycle = 0, but keep timer running
  TIM8->DIER = 0;  // disable Timer8 "update"-interrupt (reason in BeepStart)
}


//---------------------------------------------------------------------------
static void BeepReset(void) 
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
  pTIM8->EGR = TIM_EGR_UG; // "..must initialize all the registers by setting the UG bit.."
  pTIM8->CR1 = 0x81; // original configuration of TIM8_CR1 as in the original FW
  pTIM8->CCR3= 0x61; // quasi-analog output (after lowpass filtering) to mid level
  pTIM8->BDTR = TIM_BDTR_MOE; // "Main Output Enable" for the PWM (0x8000)

} // end BeepReset()


#endif // "beeper" for CONFIG_MORSE_OUTPUT ?


#if( CONFIG_MORSE_OUTPUT )
//---------------------------------------------------------------------------
static void MorseGen_BeginToSendChar( T_MorseGen *pMorseGen, uint8_t u8ASCII )
  // Prepares the transmission of a single character in Morse code.
  //  [out] pMorseGen->u8ShiftReg, 
  //        pMorseGen->u8NrElements,
  //        pMorseGen->u8State,
  //        pMorseGen->u16Timer .
{
  uint8_t morse_code, nr_elements;

  // Is the to-be-transmitted character in the Morse code table ?
  if( u8ASCII< 32 ) // anything below ASCII 32 is sent like a SPACE
   { u8ASCII = 32;
   }
  if( u8ASCII>='a' && u8ASCII<='z' ) // convert lower to UPPER case
   {  u8ASCII -= ('a'-'A');
   }
  if( u8ASCII>'z' )  // no 'special' characters in the small table !   
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
  pMorseGen->u16Timer = pMorseGen->u16DotLength;
  if( nr_elements > 0 ) // character is NOT a space..
   { pMorseGen->u8NrElements = nr_elements-1; // # elements remaining
     pMorseGen->u8ShiftReg = morse_code << 1;
     pMorseGen->u8State = MORSE_GEN_SENDING_TONE;
     if( morse_code & 0x80 )
      { pMorseGen->u16Timer *= 3; // dash = 3 dot times
      } 
     BeepStart( pMorseGen->u16Freq, pMorseGen->u8Volume ); 
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
int MorseGen_AppendString( // API: appends a string to the Mors tx-buffer
     char *pszText,  // [in] plain ASCII string
     int  iMaxLen )  // [in] number of chars (if pszText isn't a C string)
{
  T_MorseGen *pMorseGen = &morse_generator; // only one instance here
  int nCharsAppended = 0;
  uint8_t u8NewFifoHead;
  pMorseGen->u8FifoHead %= MORSE_TX_FIFO_LENGTH; // safety first
  if( iMaxLen==0 )  // caller promised pszText is zero-terminated
   { iMaxLen = MORSE_TX_FIFO_LENGTH; // ..so allow maximum length
   }
  while( iMaxLen > 0 )
   { if( *pszText=='\0' )
      { break; // reached the end of the source string
      }
     u8NewFifoHead = (pMorseGen->u8FifoHead + 1) % MORSE_TX_FIFO_LENGTH; 
     if( u8NewFifoHead == pMorseGen->u8FifoTail )
      { break; // oops.. running out of buffer space
      }
     pMorseGen->u8Fifo[ pMorseGen->u8FifoHead ] = *pszText++;
     pMorseGen->u8FifoHead = u8NewFifoHead;
     ++nCharsAppended;
     --iMaxLen;
   }
  // Start output (a few milliseconds later, in a background process) ?
  if( pMorseGen->u8FifoHead != pMorseGen->u8FifoTail )
   { if( pMorseGen->u8State == MORSE_GEN_PASSIVE )
      {  pMorseGen->u8State =  MORSE_GEN_START;
         // MorseGen_OnTimerTick() will do the rest..
      }
   } 
  return nCharsAppended;
} // MorseGen_AppendString()
#endif // CONFIG_MORSE_OUTPUT ?


#if( CONFIG_MORSE_OUTPUT )
//---------------------------------------------------------------------------
static void MorseGen_OnTimerTick(T_MorseGen *pMorseGen)
  // Called 666 (!) times per second from SysTick_Handler,
  //  as long as the Morse output is active (busy) .
  // Controls the audio output (power amplifier), and produces
  // the Morse code. The 'tone' (waveform) is generated
  // via hardware (only PWM is possible, there's no DAC on PC8 ).
{

  // Because the original firmware isn't aware of the Morse output,
  // it sometimes interfered with the Morse tone, for example when 
  // trying to activate its own (annoying) alert- and keypad tone.
  // When that happened, it enabled the Timer8 interrupt to use
  // Timer8_CH3 as 8-bit pseudo digital/analog converter for the tones
  // (more on that in BeeperReset). So watch out for Tytera's reprogramming
  // of Timer8's "Update Interrupt" enable flag, and if set, immediately
  // reprogram timer 8 as *WE* need it (for the simple/single-frequency tone):
  if( TIM8->DIER ) // oops.. someone has enamed the Timer8 interrupt !
   { // "Whoever" modified TIM8->DIER may also have overwritten
     // the timer's output frequency and PWM duty cycle. Defeat this:
     switch( pMorseGen->u8State )
      { case MORSE_GEN_SENDING_TONE: // immediately turn *OUR* tone on again:
           BeepStart( pMorseGen->u16Freq, pMorseGen->u8Volume );
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
     switch( pMorseGen->u8State )
      { case MORSE_GEN_START_AUDIO_PA: // been waiting for audio PA to start
           AUDIO_AMP_ON;  // keep the supply voltage for the audio PA on
           break;
        case MORSE_GEN_SENDING_TONE:  // sending a tone (dash or dot)
           // ex: LED_GREEN_ON; // TEST, used instead of beeps to check timing
           // NO BREAK HERE !
        case MORSE_GEN_START_ANTI_POP:
        case MORSE_GEN_START_CHAR_TX:
        case MORSE_GEN_SENDING_GAP:   // sending a gap (silence but active)
           AUDIO_AMP_ON;   // keep the supply voltage for the audio PA on
           SPKR_SWITCH_ON; // keep speaker connected to audio PA
           break;

        default: // really nothing to do, let 'Tytera' control the audio PA
           break; 
      }   // end switch( pMorseGen->u8State )
   }
  else // timer expired, so what's up next (state transition) ?
   { pMorseGen->u16Timer = 30;  // ~~50 ms default to turn on the amplifier, etc
     switch( pMorseGen->u8State )
      { case MORSE_GEN_PASSIVE : // not active, waiting for start
           break;  // nothing to do
        case MORSE_GEN_START   : // someone request to start the Morse Machine..
           // ToDo: Wait until the radio stops TRANSMITTING (RF),
           //       or let the caller (of MorseGen_AppendString)
           //       decide whether 'to morse' during TX or not ? 
           // If the radio's Audio power amplifier isn't powered up yet,
           // turn it on. To reduce the 'pop' in the speaker, Tytera possibly
           // invested in two N-channel MOSFETs between audio PA and speaker.
           SPKR_SWITCH_OFF; // 'anti-pop' switch between PA and speaker still disconnected
           AUDIO_AMP_ON;    // turn on the supply voltage for the audio PA
           pMorseGen->u8State = MORSE_GEN_START_AUDIO_PA;
           break;
        case MORSE_GEN_START_AUDIO_PA: // been waiting for audio PA to start
           SPKR_SWITCH_ON; // close the 'anti-pop' switch between PA and speaker
           pMorseGen->u8State = MORSE_GEN_START_ANTI_POP;
           break;
        case MORSE_GEN_START_ANTI_POP: // been waiting for Anti-Pop switch to close
           pMorseGen->u8State = MORSE_GEN_START_CHAR_TX;
           break;
        case MORSE_GEN_START_CHAR_TX:  // start transmission of next char
           if( pMorseGen->u16DotLength==0  // oops.. config not loaded yet ?
            || pMorseGen->u16Freq<100 )
            { // > '0' means take proper default .
              // 20 WPM was used by an old TR751 which sounded nice.
              // In milliseconds per dot, that's 1200 / 20 = 60 [ms] .
              // With 1.5 ms per timer tick: 60 / 1.5 = 40 ticks per dot.
              // If this Morse generator ever goes into 'production',
              // this parameter must be editable in the config menu !
              pMorseGen->u16DotLength = 40; // default for 20 WPM
              pMorseGen->u16Freq  = 650; // configurable tone frequency in Hertz
              pMorseGen->u8Volume = 25;  // volume (-> PWM duty cycle) in percent 
            }
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
           // LED_GREEN_OFF; // TEST (used before the PWM-beeps worked properly)
           BeepMute(); 
           pMorseGen->u8State = MORSE_GEN_SENDING_GAP; 
           break;
        case MORSE_GEN_SENDING_GAP:  // finished sending a gap 
           // Send another dot or dash, or begin the next character ?
           if( pMorseGen->u8NrElements > 0 ) // send next dot or dash
            { --pMorseGen->u8NrElements;
              pMorseGen->u16Timer = pMorseGen->u16DotLength;
              if( pMorseGen->u8ShiftReg & 0x80 )
               { pMorseGen->u16Timer *= 3; // dash = 3 dot times
               } 
              pMorseGen->u8ShiftReg <<= 1;
              pMorseGen->u8State = MORSE_GEN_SENDING_TONE;
              BeepStart( pMorseGen->u16Freq, pMorseGen->u8Volume ); 
            }
           else // inter-character gap ...
            { // The spacing between two characters (within a WORD)
              // is 3 dots; a gap of 1 dot is already over. Thus:
              pMorseGen->u16Timer = 2 * pMorseGen->u16DotLength;
              pMorseGen->u8State = MORSE_GEN_START_CHAR_TX; // after that, send next char
            }
           break;

        case MORSE_GEN_END_OF_MESSAGE: // all characters sent, 'back to normal'
           BeepReset();    // reprogram Timer8 to Tytera's own configuration
           // If the audio PA was off *before* sending the Morse message(s),
           // we may be tempted to simply turning it off again here.
           // But in the meantime, the receiver's squelch may have opened !
           // In that case, do NOT turn the audio PA off.
           // But how to find out if the receiver squelch is OPEN ?
           //  - there's "rx_voice" in radiostate.h ,
           //  - there's "rst_voice_active", 
           if( IS_RX_LED_ON ) 
            { // guess the RX-squelch is open now...
              //  so skip shutting down the audio PA:
              pMorseGen->u8State = MORSE_GEN_PASSIVE;
            }
           else // guess the RX-squelch is CLOSED so turn audio off:
            { SPKR_SWITCH_OFF; // disconnect speaker from audio PA,
              // but wait before powering down the PA itself :
              pMorseGen->u8State = MORSE_GEN_STOP_ANTI_POP;
            }  
           break;

        case MORSE_GEN_STOP_ANTI_POP : // speaker has been disconnected from audio PA
           AUDIO_AMP_OFF;  // .. so turn off the audio PA without a 'pop'
           pMorseGen->u8State = MORSE_GEN_STOP_AUDIO_PA;
           break;
        case MORSE_GEN_STOP_AUDIO_PA : // audio PA has been turned off (completely)
           pMorseGen->u8State = MORSE_GEN_PASSIVE;
           break;
        default: // oops..
           pMorseGen->u8State = MORSE_GEN_PASSIVE;
           break; 
      }   // end switch( pMorseGen->u8State )
   }     // end else < timer expired >
}       // end MorseGen_OnTimerTick()
#endif // CONFIG_MORSE_OUTPUT ?


//---------------------------------------------------------------------------
void SysTick_Handler(void)
  // ISR to generate the PWM'ed output for the backlight,
  //     and to produce a well-timed output in Morse code. 
  // Called every 1.5 ms instead of the original SysTick_Handler() .
  // The address of *OUR* SysTick_Handler must be patched into the ORIGINAL
  // vector table, and the address of the original handler must be known
  // because we also call the original handler from here.
{
  uint32_t dw;
  uint32_t oldSysTickCounter = IRQ_dwSysTickCounter++; 
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

#   if( CONFIG_MORSE_OUTPUT ) 
     // The C runtime should have cleared global variable, 
     // but don't assume anything about the calling sequence. Thus:
     morse_generator.u8State = MORSE_GEN_PASSIVE; // initial state
#   endif // CONFIG_MORSE_OUTPUT ? 

   } // end if < 1st call of SysTick_Handler >

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
           // Test result: Tytera turns the BL on ~~1 sec after power-on .
           // Turned the BL on very early (in the 1st SysTick)  -> CRASH !
           dw = oldSysTickCounter & 0x1F; // flickering, dimmed green LED
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
              IRQ_dwSysTickCounter  = 1; // almost restart timer (to ramp up brightness)
              may_turn_on_backlight = 1; // start dimming in the next interrupt
            } // end if < backlight turned ON (by original firmware) >
         }   // end if < GPIO_C.6 configured as OUTPUT >
      }     // end if < GPIO_C supplied with a peripheral clock >
   }       // end if < may NOT turn on the backlight yet >   
  else    // may control the backlight now ...
   { 
     if( oldSysTickCounter <= 3000/* x 1.5 ms*/ )
      { dw = oldSysTickCounter / 100; // brightness ramps up during init
        intensity = (dw<9) ? dw : 9;  // ... from 0 to 9 (=max brightness)

#      if( 1 && CONFIG_MORSE_OUTPUT )  // delayed start of the "Morse demo" ?
        if( oldSysTickCounter == 3000 )
         { // TEST: send something in Morse code immediately after power-on ?
           MorseGen_AppendString( "cq test 0123456789", 0/*no 'MaxLen'*/ );
         }
#      endif // CONFIG_MORSE_OUTPUT ?

      }
     else  // not "shortly after power-on", but during normal operation ...
      {
        if( intensity==0 )   // backlight intensities not configured ? ('0' means take proper default)
         {  intensity= 0x99; // 'hum-free' default (without overwriting global_addl_config in an interrupt!)
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
   }     // may_turn_on_backlight ? 
#endif  // CONFIG_DIMMED_LIGHT ?

#if( CONFIG_MORSE_OUTPUT ) // Morse output (optional, since 2017-02-19) ?
  if( morse_generator.u8State != MORSE_GEN_PASSIVE )
   { // Only spend time on this when active !
     MorseGen_OnTimerTick( &morse_generator );
   }
#endif  // CONFIG_MORSE_OUTPUT ?

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

