/*! \file main.c
  \brief MD380Tools Main Application
  
*/


#include "stm32f4_discovery.h"
#include "stm32f4xx_conf.h" // again, added because ST didn't put it here ?

#include <stdio.h>
#include <string.h>

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


//const 
char hexes[]="0123456789abcdef"; //Needs to be const for problems.

/* This writes a 32-bit hexadecimal value into a human-readable
   string.  We do it manually to avoid heap operations. */
void strhex(char *string, long value){
  char b;
  for(int i=0;i<4;i++){
    b=value>>(24-i*8);
    string[2*i]=hexes[(b>>4)&0xF];
    string[2*i+1]=hexes[b&0xF];
  }
}

/* This writes a 32-bit hexadecimal value into a human-readable
   string.  We do it manually to avoid heap operations. */
void wstrhex(wchar_t *string, long value){
  char b;
  for(int i=0;i<4;i++){
    b=value>>(24-i*8);
    string[2*i]=hexes[(b>>4)&0xF];
    string[2*i+1]=hexes[b&0xF];
  }
}

//Make the welcome image scroll across the screen.
void demo_show_animation(void) {
  for(int i=0;i<0x60;i+=3){
    gfx_drawbmp(welcomebmp,0,i);
    sleep(30);
  }
}

void demo_clear(void) {
  // Clear screen
  uint32_t oldfg = gfx_get_fg_color();
  gfx_set_fg_color(0xffffff);
  gfx_blockfill(0, 0, 160, 128);
  gfx_set_fg_color(oldfg);
}

void dyn_bootscreen(void)
{
    if( global_addl_config.boot_custom_str == 0 ) {
        //Restore the bottom line of text before we return.
        md380_spiflash_read(botlinetext, 0x2054, 20);
        return ;
    }
    
    // Set the bottom line to the config's dmr id
    char dmridstr[10];
    sprintf(dmridstr, "%u\0", (uint32_t) global_addl_config.dmrid);
    
    // We need to pad for wchar, someone will probably rip out their eyeballs reading this
    //mbstowcs(&botlinetext, dmridstr, 10);
    for (uint8_t ii = 0; ii < 20; ii++) {
        botlinetext[ii] = 0x00;
        if( ii % 2 == 0 ) {
            botlinetext[ii] = dmridstr[ii / 2];
        }
    }

    for (uint8_t ii = 0; ii < 20; ii++) {
        toplinetext[ii] = global_addl_config.rname[ii];
    }
}

void demo(void) {
#ifdef CONFIG_GRAPHICS
  switch (global_addl_config.bootscr) {
    case 2:
      break;
    case 1:
      display_credits();
      sleep(500);
      demo_clear();
      break;
    default:
      display_credits();
      sleep(1000);
      demo_show_animation();
  }
#endif //CONFIG_GRAPHICS

  // Setup dynamic bootscreen
  dyn_bootscreen();
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

