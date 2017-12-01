// File:    md380tools/applet/src/irq_handlers.h
// Authors: Wolf (DL4YHF), 
// Date:    2017-01-09 
//  Contains prototypes for a few (!) hijacked interrrupt handlers,
//           for example for the software PWMed 'dimmed backlight'.
//   Module prefix : "IRQ_" .

#ifndef  CONFIG_DIMMED_LIGHT   // want 'dimmable backlight' ?
# define CONFIG_DIMMED_LIGHT 0 // guess not (else set CONFIG_DIMMED_LIGHT>0 in config.h)
#endif

#ifndef  CONFIG_MORSE_OUTPUT   // want output in Morse code ?
# define CONFIG_MORSE_OUTPUT 0 // guess not (else set CONFIG_MORSE_OUTPUT>0 in config.h)
#endif

#ifndef  CONFIG_APP_MENU   // Alternative menu activated by red 'BACK'-button ?
# define CONFIG_APP_MENU 0 // only if defined > 0 in config.h
#endif


// Offsets into the STM32's vector table. These may be defined somewhere else, but WB didn't find anything like this.
// See also: description of 'tested vectors' at www.qsl.net/dl4yhf/RT3/md380_fw.html#vector_table  .
#define IRQ_VT_OFFSET_INITIAL_SP      0x000 /* initial stack pointer. NOT an IRQ, only here for completeness    */
#define IRQ_VT_OFFSET_SYSTICK_HANDLER 0x03C /* SysTick_Handler : occupied, but used for DL4YHF's PWM experiment */
#define IRQ_VT_OFFSET_TIM8_UP_TIM3    0x0F0 /* unfortunately both (TIM8 + TIM3) occupied by Tytera firmware     */
#define IRQ_VT_OFFSET_TIM5            0x108 /* Timer5 used for what ? Got to find out one day .                 */
#define IRQ_VT_OFFSET_FPU             0x188 /* Floating Point Something. Not used, only here for completeness   */
 
// Address of the VT in Tytera's original firmware. Used to "jump into their code" somewhere :
#ifdef FW_D13_020 // <- for MD380/RT3, passed to the compiler via command line from md380tools/applet/Makefile 
#  define IRQ_VT_ADDRESS_ORIGINAL_FW  0x0800C000
#  define IRQ_VT_ADDRESS_OUR_FIRMWARE 0x0809D000
#  define IRQ_ORIGINAL_SYSTICK_HDLR   0x08093f1c // original SysTick_Handler address (without 'Thumb' indicator in bit 0)
#  define MD380_ADDR_DMR_POWER_SAVE_COUNTDOWN 0x2001e5d0 /* alias 'msg_timer_500' */
#endif
#ifdef FW_S13_020 // <- for MD390/RT8 (with GPS)
#  define IRQ_VT_ADDRESS_ORIGINAL_FW  0x0800C000
#  define IRQ_VT_ADDRESS_OUR_FIRMWARE 0x0809D000
#  define IRQ_ORIGINAL_SYSTICK_HDLR   0x08094D5A // original SysTick_Handler address (without 'Thumb' indicator in bit 0)
#  define MD380_ADDR_DMR_POWER_SAVE_COUNTDOWN 0x2001e6a0
#endif
#ifdef FW_D02_032 // <- for stoneage firmware (only here to avoid errors in the main makefile's output)
#  define IRQ_VT_ADDRESS_ORIGINAL_FW  0x0800C000
#  define IRQ_VT_ADDRESS_OUR_FIRMWARE 0x0809D000
#  define IRQ_ORIGINAL_SYSTICK_HDLR   0x0809381C // original SysTick_Handler address (without 'Thumb' indicator in bit 0)
#  undef  MD380_ADDR_DMR_POWER_SAVE_COUNTDOWN /* unknown -> not supported ! */
#endif
#ifndef IRQ_ORIGINAL_SYSTICK_HDLR
# error "Please add new 'ifdef' with the interrupt vector table for the new firmware above !"
#endif

// Low-level hardware access, bypassing the firmware (use carefully!).
// How to drive the LEDs (here, mostly for testing) ? From gfx.c :
// > The RED LED is supposed to be on pin A0 by the schematic, but in
// > point of fact it's on E1.  Expect more hassles like this.
#define PINPOS_C_BL 6 /* pin position of backlight output on GPIO_C */
#define PINPOS_E_TX 1 /* pin position of the red   TX LED on GPIO_E */
#define PINPOS_E_RX 0 /* pin position of the green RX LED on GPIO_E */
#define PINPOS_E_PTT 11 /* pin position of the PTT input  on GPIO_E */
#define LED_GREEN_ON  GPIOE->BSRRL=(1<<PINPOS_E_RX) /* turn green LED on  */
#define LED_GREEN_OFF GPIOE->BSRRH=(1<<PINPOS_E_RX) /* turn green LED off */
#define IS_GREEN_LED_ON ((GPIOE->ODR&(1<<PINPOS_E_RX))!=0) /* check green LED*/
#define LED_RED_ON    GPIOE->BSRRL=(1<<PINPOS_E_TX) /* turn red LED on  */
#define LED_RED_OFF   GPIOE->BSRRH=(1<<PINPOS_E_TX) /* turn red LED off */
#define IS_RED_LED_ON ((GPIOE->ODR&(1<<PINPOS_E_TX))!=0) /* check red LED*/
#define IS_PTT_PRESSED ((GPIOE->IDR&(1<<PINPOS_E_PTT))==0) /* check PTT */ 

#define PINPOS_D_LCD_RD 4 /* PD4 = LCD_RD (low-active READ from CPU to LCD)  */
#define PINPOS_D_LCD_WR 5 /* PD5 = LCD_WR (low-active WRITE from CPU to LCD) */
#define PINPOS_D_LCD_CS 6 /* PD6 = LCD_CS, see md380hw.html#PD6 */
#define PINPOS_D_LCD_RS 12  /* PD12 = LCD_RS (register select, aka command/data)*/
#define PINPOS_D_LCD_RST 13 /* PD13 = LCD_RST (allows the CPU to reset the LCD)*/
#define PINPOS_D_LCD_D0  14 /* PD14 = LCD_D0 */
#define PINPOS_D_LCD_D1  15 /* PD15 = LCD_D1 */
#define PINPOS_D_LCD_D2  0  /* PD0  = LCD_D2 */
#define PINPOS_D_LCD_D3  1  /* PD1  = LCD_D3 */
/* Four of the eight LCD data lines are on another port .. aaargh !! */
#define PINPOS_E_LCD_D4  7  /* PE7  = LCD_D4 */
#define PINPOS_E_LCD_D5  8  /* PE8  = LCD_D5 */
#define PINPOS_E_LCD_D6  9  /* PE9  = LCD_D6 */
#define PINPOS_E_LCD_D7  10 /* PE10 = LCD_D7 */
/* Macros for some bit-banging I/O on the above ports: */
#define LCD_RD_LOW    GPIOD->BSRRH=(1<<PINPOS_D_LCD_RD) /* LCD-read active   */
#define LCD_RD_HIGH   GPIOD->BSRRL=(1<<PINPOS_D_LCD_RD) /* LCD-read passive  */
#define LCD_WR_LOW    GPIOD->BSRRH=(1<<PINPOS_D_LCD_WR) /* LCD-write active  */
#define LCD_WR_HIGH   GPIOD->BSRRL=(1<<PINPOS_D_LCD_WR) /* LCD-write passive */
#define LCD_CS_LOW    GPIOD->BSRRH=(1<<PINPOS_D_LCD_CS) /* LCD-chip-select active*/
#define LCD_CS_HIGH   GPIOD->BSRRL=(1<<PINPOS_D_LCD_CS) /* LCD-chip-select passive*/
#define LCD_RS_LOW    GPIOD->BSRRH=(1<<PINPOS_D_LCD_RS) /* select LCD 'command' (register)*/
#define LCD_RS_HIGH   GPIOD->BSRRL=(1<<PINPOS_D_LCD_RS) /* select LCD 'data' */
//  (Himax : 'DNC' = "Data Not Command" ?)

extern uint8_t boot_flags; // bitwise combination of the following flags:
#define BOOT_FLAG_INIT_BACKLIGHT  0x01 // initialized backlight (GPIO) ?
#define BOOT_FLAG_LOADED_CONFIG   0x02 // called init_global_addl_config_hook() ?
#define BOOT_FLAG_DREW_STATUSLINE 0x04 // called draw_statusline_hook() ?
#define BOOT_FLAG_OPEN_FOR_BUSINESS 0x08 // set when it's ok to open the app-menu
#define BOOT_FLAG_FIRST_KEY_POLLED  0x10 // set when keyboard variables are valid
#define BOOT_FLAG_FIRST_KEY_PRESSED 0x20 // set when kb_row_col_pressed was nonzero for the first time
#define BOOT_FLAG_POLLED_KEYBOARD 0x08 // polled the keyboard at least once ?

extern volatile uint32_t IRQ_dwSysTickCounter; // Incremented each 1.5 ms


extern uint8_t GFX_backlight_on; // 0="off" (low intensity), 1="on" (high intensity) 
//   (note: GFX_backlight_on is useless, as long as no-one calls gfx.c : 

extern uint8_t kb_backlight; // flag to disable the backlight via sidekey (in keyb.c)

extern uint8_t red_led_timer;   // 'countdown' timer, in 1.5 ms steps, to turn
            // the red LED on TEMPORARILY. ZERO when 'Tytera controls the LED'.
extern uint8_t green_led_timer; // similar for the GREEN (RX) LED.  TEST ONLY !

extern uint8_t  volume_pot_percent; // audio volume potentiometer position [%]

extern uint16_t battery_voltage_mV; // battery voltage [millivolts]

extern uint16_t keypress_timer_ms; // measures key-down time in MILLISECONDS 
extern uint8_t  keypress_ascii;    // code of the currently pressed key, 0 = none
extern uint8_t  keypress_ascii_at_power_on; // snapshot of keypress_ascii at power-on
extern uint8_t  keypress_ascii_remote; // for control via remote keyboard (USB).

void StartStopwatch(uint32_t *pu32Stopwatch);   // details and usage in *.c 
int  ReadStopwatch_ms(uint32_t *pu32Stopwatch);

#if( CONFIG_MORSE_OUTPUT )
void MorseGen_ClearTxBuffer(void); // aborts the current Morse transmission (if any)
int  MorseGen_GetTxBufferUsage(void); // number of characters waiting for TX
int  MorseGen_AppendChar(char c);
int  MorseGen_AppendString(char *pszText); // API to send 8-bit, zero-terminated strings
int  MorseGen_AppendWideString(wchar_t *pwsText); // API to send 16-bit strings
int  MorseGen_AppendDecimal(int i);
int  MorseGen_AppendHex(uint32_t num);

void BeepStart(int freq_Hz, int volume); // starts a pulse-width modulated beep
void BeepMute(void);  // mutes the beep without turning the audio PA off
void BeepReset(void); // reprograms Timer8 for tytera's funny tones ("Boooo" & co) 

#endif // CONFIG_MORSE_OUTPUT ?
