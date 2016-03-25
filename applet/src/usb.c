/*! \file usb.c
  \brief USB Hook functions.
  
  This module includes hooks for the USB DFU protocol
  of the Tytera MD380, in order to preserve compatibility
  with Tytera's own tools while also hooking those commands
  to include new functions for our own use.
  
  All hooked commands and transfers occur when block=1, as DFU
  normally uses block=0 for commands and block>=2 for transfers.
  
  You can use these command hooks to process things while the
  radio is running, but the AMBE+ codec chip emulator will
  take priority and block USB so, so you can't do it during
  an audio transmission or reception.
*/


#include <stdio.h>
#include <string.h>

#include "printf.h"
#include "dmesg.h"
#include "md380.h"
#include "version.h"
#include "tooldfu.h"
#include "config.h"
#include "gfx.h"

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
  
  //This is the target address of the Application's DFU engine.
  char *dfutargetadr=(char*) *((int*)0x2000112c); //2.032 DFU target adr.
  
  /* The DFU protocol specifies reads from block 0 as something
     special, but it doesn't say way to do about an address of 1.
     Shall I take it over?  Don't mind if I do!
   */
  if(blockadr==1){
    //Some special addresses need help before the transfer.
    if(dfutargetadr== dmesg_start){
      /* We can't send the DMESG buffer itself, because it's in the
	 tightly coupled memory, so we'll memcpy() it to a buffer in
	 SRAM and then transmit it.
      */
      memcpy(dmesg_tx_buf,dmesg_start,DMESG_SIZE);
      
      //Wipe out the old buffer and set the pointer to the beginning.
      for(int i=0;i<DMESG_SIZE;i++)
	dmesg_start[i]=0;
      dmesg_wcurs=0;
      
      //Send the doubled buffer and return.
      usb_send_packet(iface,   //USB interface structure.
		      dmesg_tx_buf, //Send the copy, not the original.
		      length); //Length must match.
      return 0;
    }
    
    
    //Send the data from internal memory and return.
    usb_send_packet(iface,   //USB interface structure.
		    dfutargetadr,
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
  static char *dfu_state=(char*) 0x2001d405;//2.032
  static char **dfu_target_adr=(char**) 0x2000112c; //2.032
  
  //Don't know what these do.
  //char *thingy=(char*) 0x2001d276;
  char *thingy2=(char*) 0x2001d041;
  
  /* DFU transfers begin at block 2, and special commands hook block
     0.  We'll use block 1, because it handily fits in the gap without
     breaking backward compatibility with the older code.
   */
  if(*blockadr==1){
    switch(packet[0]){
    case TDFU_DMESG:
      //The DMESG buffer might move, so this command
      //sets the target address to the DMESG buffer.
      *dfu_target_adr=dmesg_start;
      break;
    case TDFU_SPIFLASHREAD:
      //Re-uses the dmesg transmit buffer.
      *dfu_target_adr=dmesg_tx_buf;
      uint32_t adr= *((uint32_t*)(packet+1));
      printf("Dumping %d bytes from 0x%08x in SPI Flash\n",
	     DMESG_SIZE, adr);
      spiflash_read(dmesg_tx_buf,
		    adr,
		    DMESG_SIZE);
      break;
    case TDFU_PRINT: // 0x80, u8 x, u8 y, u8 str[].
      drawtext((wchar_t *) (packet+3),
	       packet[1],packet[2]);
      break;
      
    case TDFU_BOX:
    default:
      printf("Unhandled DFU packet type 0x%02x.\n",packet[0]);
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


const char *getmfgstr(int speed, long *len){
  //This will be turned off by another thread,
  //but when we call it often the light becomes visible.
  green_led(1);
  
  //Hook the USB DNLOAD handler.
  hookusb();
  
  static long adr=0;
  long val;
  char *usbstring=(char*) 0x2001c080; //2.032
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
  memcpy(buf,VERSIONDATE,22);
  return;
}

//Must be const, because globals will be wacked.
//const wchar_t mfgstr[]=L"Travis KK4VCZ";
const char mfgstr[]="Travis Goodspeed KK4VCZ"; //"\x1c\x03T\0r\0a\0v\0i\0s\0 \0K\0K\x004\0V\0C\0Z\0";

