#include "stm32f4_discovery.h"
#include "stm32f4xx_conf.h" // again, added because ST didn't put it here ?

#include <stdio.h>


GPIO_InitTypeDef  GPIO_InitStructure;

void Delay(__IO uint32_t nCount);


//Firmware calls to 2.032.
const int (*spiflash_read)(char *dst, long adr, long len) = 0x0802fd83;
const void (*gfx_drawtext)(char *str,          //16-bit, little endian.
		     short sx, short sy, //Source coords, maybe?
		     short x, short y,   //X and Y position
		     int maxlen) = 0x0800D88B;
const void (*gfx_drawbmp)(char *bmp,
			  int idx,
			  uint64_t pos) = 0x08022887;

const char *welcomebmp=0x080f9ca8;
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


static void led_setup(void) {
  /* GPIOD Periph clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

  /* PE 0 and 1 to in/out mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  
}

static void green_led(int on) {
  if (on) {
    GPIO_SetBits(GPIOE, GPIO_Pin_0);
  } else {
    GPIO_ResetBits(GPIOE, GPIO_Pin_0);
  }
}


static void red_led(int on) {
  /* The RED LED is supposed to be on pin A0 by the schematic, but in
     point of fact it's on E1.  Expect more hassles like this.
  */
  
  if (on) {
    GPIO_SetBits(GPIOE, GPIO_Pin_1);
  } else {
    GPIO_ResetBits(GPIOE, GPIO_Pin_1);
  }
}

//Must be const, because globals will be wacked.
//const wchar_t mfgstr[]=L"Travis KK4VCZ";
const char mfgstr[]="Travis Goodspeed KK4VCZ"; //"\x1c\x03T\0r\0a\0v\0i\0s\0 \0K\0K\x004\0V\0C\0Z\0";

/* This copies a character string into a USB Descriptor string, which
   is a UTF16 little-endian string preceeded by a one-byte length and
   a byte of 0x03. */
const char *loadusbstr(char *usbstring,
		       char *newstring,
		       long *len){
  int i=0;
  *len=2;
  
  usbstring[1]=3;//Type

  while(newstring[i]){
    usbstring[2+2*i]=newstring[i];//The character.
    usbstring[3+2*i]=0;           //The null byte.
    i++;
  }
  *len=2*i+2;
  usbstring[0]=*len;
  return usbstring;
}

/* This copies a character string into a USB Descriptor string, which
   is a UTF16 little-endian string preceeded by a one-byte length and
   a byte of 0x03. */
const char *str2wide(char *widestring,
		     char *src){
  int i=0;
  
  while(src[i]){
    widestring[2*i]=src[i];//The character.
    widestring[2*i+1]=0;           //The null byte.
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

const char *getmfgstr(int speed, long *len){
  //This will be turned off by another thread,
  //but when we call it often the light becomes visible.
  green_led(1);
  
  static long adr=0; //This address is known to be free.
  long val;
  char *usbstring=(char*) 0x2001c080;
  char buffer[]="@________ : ________";
  
  //Read four bytes from SPI Flash.
  spiflash_read(&val,adr,4);
  
  //Print them into the manufacturer string.
  strhex(buffer+1, adr);
  strhex(buffer+12, val);
  
  //Look forward a bit.
  adr+=4;
  
  //Return the string as our value.
  return loadusbstr(usbstring,buffer,len);
}

void wipe_mem(){
  long *start=(long*) 0x10000000;
  long *end=(long*)   0x10010000;
  while(start<end)
    *start++=0xdeadbeef;
}


void gfx_drawascii(char *text,
		   int x, int y){
  static char buf[512];
  str2wide(buf,text);
  gfx_drawtext(buf,
	       0,0,
	       x,y,
	       strlen(text));
	       
}

/* Displays a startup demo on the device's screen, including some of
   the setting information and a picture or two. */
void demo(){
  char *botlinetext=(char*) 0x2001cee0;
  
  gfx_drawascii("MD380Tools",
		160,20);
  gfx_drawascii("by KK4VCZ",
		160,60);
  sleep(1000);
  
  for(int i=0;i<0x60;i+=3){
    gfx_drawbmp(welcomebmp,0,i);
    sleep(30);
  }
  
  //Restore the bottom line of text before we return.
  spiflash_read(botlinetext, 0x2054, 20);
}


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f4xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
        system_stm32f4xx.c file
     */
  
  //Initialize RAM to zero, to figure out what's free later.
  //wipe_mem();
  
  led_setup();
  
  
  for(int i=0; i<1; i++) {

    //red_led(1);
    green_led(1);

    sleep(500);

    red_led(0);
    green_led(0);

    sleep(500);

    red_led(1);

    sleep(500);
    
    red_led(0);
    
    sleep(500);
  }
  
  //Done with the blinking, so start the radio application.
  abort_to_mfgr_app();

  //These never get run, but we call them anyways to keep them in the
  //binary.
  getmfgstr(0,&main);
  demo();
}



#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
