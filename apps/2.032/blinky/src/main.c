/*! \file main.c
  \brief MD380Tools Main Application
  
*/


#include "stm32f4_discovery.h"
#include "stm32f4xx_conf.h" // again, added because ST didn't put it here ?

#include <stdio.h>
#include <string.h>

#include "md380.h"
#include "version.h"
#include "tooldfu.h"
#include "config.h"

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


void drawtext(wchar_t *text,
	      int x, int y){
  gfx_drawtext(text,
	       0,0,
	       x,y,
	       15); //strlen(text));
	       
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


//TODO Move this to the right place.


int usb_upld_hook(void* iface, char *packet, int bRequest, int something){
  /* This hooks the USB Device Firmware Update upload function,
     so that we can transfer data out of the radio without asking
     the host's permission.
  */
  
  //Really ought to do this with a struct instead of casts.
  
  //This is 1 if we control it, 0 or >=2 if the old code should take it.
  uint16_t blockadr = *(short*)(packet+2);

  //Seems to be forced to zero.
  //uint16_t index = *(short*)(packet+4);

  //We have to send this much.
  uint16_t length= *(short*)(packet+6);
  
  /* The DFU protocol specifies reads from block 0 as something
     special, but it doesn't say way to do about an address of 1.
     Shall I take it over?  Don't mind if I do!
   */
  if(blockadr==1){
    usb_send_packet(iface,   //USB interface structure.
		    //(char*) 0x2001d098,
		    (char*) *((int*)0x2000112c), //TODO move to header. (usb_dfu_baseadr)
		    length); //Length must match.
    return 0;
  }
  
  //Return control the original function.
  return usb_upld_handle(iface, packet, bRequest, something);
}



int usb_dnld_hook(){
  /* These are global buffers to the packet data, its length, and the
     block address that it runs to.  The stock firmware has a bug
     in that it assumes the packet size is always 2048 bytes.
  */
  static char *packet=(char*) 0x200199f0;//2.032
  static int *packetlen=(int*) 0x2001d20c;//2.032
  static int *blockadr=(int*) 0x2001d208;//2.032
  static char *dfu_state=(char*) 0x2001d405;
  
  //Don't know what these do.
  //char *thingy=(char*) 0x2001d276;
  char *thingy2=(char*) 0x2001d041;
  
  /* DFU transfers begin at block 2, and special commands hook block
     0.  We'll use block 1, because it handily fits in the gap without
     breaking backward compatibility with the older code.
   */
  if(*blockadr==1){
    switch(packet[0]){
    case TDFU_PRINT: // 0x80, u8 x, u8 y, u8 str[].
      drawtext((wchar_t *) (packet+3),
	       packet[1],packet[2]);
      break;
    }
    
    thingy2[0]=0;
    thingy2[1]=0;
    thingy2[2]=0;
    thingy2[3]=3;
    *dfu_state=3;
    
    *blockadr=0;
    *packetlen=0;
    return 0;
  }else{
    /* For all other blocks, we default to the internal handler.
     */
    return usb_dnld_handle();
  }
}

//! Hooks the USB DFU DNLD event handler in RAM.
void hookusb(){
  //Be damned sure to call this *after* the table has been
  //initialized.
  *dnld_tohook= (int) usb_dnld_hook;
  return;
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

void loadfirmwareversion(){
  wchar_t *buf=(wchar_t*) 0x2001cc0c;
  hookusb();
  memcpy(buf,VERSIONDATE,22);
  return;
}



/* Displays a startup demo on the device's screen, including some of
   the setting information and a picture or two. */
void demo(){

  
  hookusb();
  
  drawtext(L"MD380Tools ",
	   160,20);
  drawtext(L"by KK4VCZ  ",
	   160,60);
  drawtext(L"and Friends",
	   160,100);
  
  sleep(1000);
  
  //Make the welcome image scroll across the screen.
  for(int i=0;i<0x60;i+=3){
    gfx_drawbmp(welcomebmp,0,i);
    sleep(30);
  }
  
  //Restore the bottom line of text before we return.
  spiflash_read(botlinetext, 0x2054, 20);
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
  led_setup();
  
  //Blink the LEDs a few times to show that our code is starting.
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
  getmfgstr(0,(void*) NULL);
  demo();
}

