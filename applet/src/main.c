/*! \file main.c
  \brief MD380Tools Main Application
  
*/

#define COMPILING_MAIN_C 1  // flag to show warnings in headers only ONCE
                   // (e.g. "please consider finding symbols.." in gfx.h)

#include "stm32f4_discovery.h"
#include "stm32f4xx_conf.h" // again, added because ST didn't put it here ?

#include <string.h>

#include "config.h"  // need to know CONFIG_DIMMED_LIGHT (defined in config.h since 2017-01-03)

#include "md380.h"
#include "printf.h"
#include "dmesg.h"
#include "version.h"
#include "tooldfu.h"
#include "config.h"
#include "addl_config.h"
#include "radio_config.h"
#include "gfx.h"
#include "display.h"
#include "usersdb.h"
#include "util.h"
#include "spiflash.h"

#include "irq_handlers.h" // Initially written by DL4YHF as a 'playground' with various interrupt handlers .
                          // Details in applet/src/irq_handlers.c . 

						  
GPIO_InitTypeDef  GPIO_InitStructure;

void Delay(__IO uint32_t nCount);



/**
  * @brief  Delay Function.
  * @param  nCount:specifies the Delay time length.
  * @retval None
  */
void Delay(__IO uint32_t nCount)
{
  while(nCount--)
  {
  }
}

void sleep(__IO uint32_t ms){
  //Delay(0x3FFFFF);
  Delay(0x3fff*ms);
}




static void do_jump(uint32_t stacktop, uint32_t entrypoint);

#define MFGR_APP_LOAD_ADDRESS     0x0800C000
#define SIDELOAD_RESET_VECTOR     8
#define SIDELOAD_APP_LOAD_ADDRESS 0x0809D000

static void abort_to_mfgr_app(void) {
	const uint32_t *app_base = (const uint32_t *)MFGR_APP_LOAD_ADDRESS;
	SCB->VTOR = MFGR_APP_LOAD_ADDRESS;
	do_jump(app_base[0], app_base[SIDELOAD_RESET_VECTOR]);
}

static void do_jump(uint32_t stacktop, uint32_t entrypoint)
{
	asm volatile(
		"msr msp, %0	\n"
		"bx	%1	\n"
		: : "r" (stacktop), "r" (entrypoint) : );
	// just to keep noreturn happy
	for (;;) ;
}



/* This copies a character string into a USB Descriptor string, which
   is a UTF16 little-endian string preceeded by a one-byte length and
   a byte of 0x03. */
const char *str2wide(char *widestring,
		     char *src){
  int i=0;
  
  while(src[i]){
    widestring[2*i]=src[i];//The character.
    widestring[2*i+1]=0;   //The null byte.
    i++;
  }
  widestring[2*i]=0;
  widestring[2*i+1]=0;
  
  return widestring;
}



//Make the welcome image scroll across the screen.

void demo_show_animation(void)
{
    for (int i = 0; i < 0x60; i += 3) {
        gfx_drawbmp(welcomebmp, 0, i);
        sleep(30);
    }
}

void demo_clear(void)
{
    // Clear screen
    uint32_t oldfg = gfx_get_fg_color();
    gfx_set_fg_color(0xffffff);
    gfx_blockfill(0, 0, 160, 128);
    gfx_set_fg_color(oldfg);
}

void demo(void)
{
    display_credits();
    sleep(1000);
    demo_show_animation();
    demo_clear();
}

void boot_splash_set_topline(void)
{
    if( (global_addl_config.cp_override & CPO_BL1) == CPO_BL1 ) {
        snprintfw(toplinetext, 10, "%s", global_addl_config.bootline1);
    } 
}

void boot_splash_set_bottomline(void)
{
    if( (global_addl_config.cp_override & CPO_BL2) == CPO_BL2 ) {
        snprintfw(botlinetext, 10, "%s", global_addl_config.bootline2);
    } 
}

void splash_hook_handler(void)
{
  
    if( global_addl_config.boot_demo == 0 ) {
        demo();
    }
    
//    test_mbox();

    md380_spiflash_read(botlinetext, FLASH_OFFSET_BOOT_BOTTONLINE, 20);
    // possibly botlinetext does not need to be zero-terminated, but we dont know yet
    botlinetext[9] = 0 ; // safety
    
    boot_splash_set_topline();
    boot_splash_set_bottomline();
}


/* Our RESET handler is called instead of the official one, so this
   main() method comes before the official one.  Our global variables
   have already been initialized, but the MD380 firmware has not yet
   been initialized because we haven't called it.
   
   So the general idea is to initialize our stuff here, then trigger
   an early callback later in the MD380's main() function to perform
   any late hooks that must be applied to function pointers
   initialized in the stock firmware.
*/
int main(void) {

  dmesg_init();
  
  /*
  RTC_TimeTypeDef RTC_TimeTypeTime;
  md380_RTC_GetTime(RTC_Format_BIN, &RTC_TimeTypeTime);
  printf("%d:%d:%d\n", RTC_TimeTypeTime.RTC_Hours,
                       RTC_TimeTypeTime.RTC_Minutes,
                       RTC_TimeTypeTime.RTC_Seconds);
  */
     
  //Done with the blinking, so start the radio application.
  printf("Starting main()\n");
  abort_to_mfgr_app();
}

