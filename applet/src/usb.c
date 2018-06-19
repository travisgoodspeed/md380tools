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

#define DEBUG

#include <string.h>

#include "printf.h"
#include "dmesg.h"
#include "md380.h"
#include "version.h"
#include "tooldfu.h"
#include "config.h"
#include "gfx.h"
#include "spiflash.h"
#include "string.h"
#include "syslog.h"
#include "lcd_driver.h"
#include "irq_handlers.h"
#include "keyb.h"
#include "stm32f4xx_flash.h"

void reboot_into_bootloader();

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
  
#    ifdef DEBUG // debugging USB-handlers ?
      if( keypress_ascii_at_power_on == 'D' ) // debugging temporarily enabled ...
       { // .. via 'Down'-key at power on ? 
         LOGB("t=%d:USBup: l=%d,b=%d\n", // visible in what used to be Netmon3 (2017-05-21)
           (int)IRQ_dwSysTickCounter,
           (int)length, (int)blockadr );
       }
#    endif // DEBUG ?

  
  /* The DFU protocol specifies reads from block 0 as something
     special, but it doesn't say way to do about an address of 1.
     Shall I take it over?  Don't mind if I do!
   */
  if(blockadr==1){

    //Some special addresses need help before the transfer.
    if(md380_dfutargetadr == dmesg_start){
      //int state=OS_ENTER_CRITICAL();

      /* We can't send the DMESG buffer itself, because it's in the
	 tightly coupled memory, so we'll memcpy() it to a buffer in
	 SRAM and then transmit it.
      */
      memcpy(dmesg_tx_buf,dmesg_start,DMESG_SIZE);
      
      dmesg_flush();
      //OS_EXIT_CRITICAL(state);
      
      //Send the doubled buffer and return.
      usb_send_packet(iface,   //USB interface structure.
		      dmesg_tx_buf, //Send the copy, not the original.
		      length); //Length must match.

#    ifdef DEBUG
      if( keypress_ascii_at_power_on == 'D' ) 
       { LOGB("t=%d: usb_up: cp+sent\n", (int)IRQ_dwSysTickCounter ); // "copied and sent"
       }
#    endif

      return 0;
    }
    
    
    //Send the data from internal memory and return.
    usb_send_packet(iface,   //USB interface structure.
		    md380_dfutargetadr,
		    length); //Length must match.
#  ifdef DEBUG
    if( keypress_ascii_at_power_on == 'D' ) 
     { LOGB("t=%d:USBup: sent\n", (int)IRQ_dwSysTickCounter ); // "sent without memcopy"
       // (when sending the LCD framebuffer, ~~ 1 ms elapsed between two tiles)
     }
#  endif

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
  
  int state;

#    ifdef DEBUG
      if( keypress_ascii_at_power_on == 'D' ) // added 2017-05-21, trying to debug TDFU_READ_FRAMEBUFFER.
       { LOGB("t=%d:USBdn: cmd=%02X,b=%d\n",  // When NOT seeing any of these 'USBdn'-messages, 
           (int)IRQ_dwSysTickCounter,         // it's the same problem as with the SPI-flash-ID (!)
           (int)md380_packet[0], (int)(*md380_blockadr) ); // - see advice from N6YN, github issue #186
       } // (on Windows, LibUsb sometimes needed to be 'tickled' by running "TestLibUsb".
         //  No idea why this problem affected the framebuffer-transfer but NOT the normal RAM-readout...
#    endif // DEBUG ?


  
  /* DFU transfers begin at block 2, and special commands hook block
     0.  We'll use block 1, because it handily fits in the gap without
     breaking backward compatibility with the older code.
   */
  if(*md380_blockadr==1){
    switch(md380_packet[0]){

//Memory commands
    case TDFU_DMESG:
      //The DMESG buffer might move, so this command
      //sets the target address to the DMESG buffer.
      *md380_dfu_target_adr=dmesg_start;
      break;

//SPI-FLASH commands
#ifdef CONFIG_SPIFLASH
    case TDFU_SPIFLASHGETID:
      //Re-uses the dmesg transmit buffer.
      *md380_dfu_target_adr=dmesg_tx_buf;
      get_spi_flash_type((void *) dmesg_tx_buf); // 0x00aabbcc  aa=MANUFACTURER ID, bb,cc Device Identification
      break;
    case TDFU_SPIFLASHREAD:
      //Re-uses the dmesg transmit buffer.
      *md380_dfu_target_adr=dmesg_tx_buf;
      uint32_t adr= *((uint32_t*)(md380_packet+1));
      printf("Dumping %d bytes from 0x%08x in SPI Flash\n",
            DMESG_SIZE, adr);
      md380_spiflash_read(dmesg_tx_buf,
		          adr,
		          DMESG_SIZE);
      break;
    case TDFU_SPIFLASHWRITE:
      //Re-uses the dmesg transmit buffer.
      *md380_dfu_target_adr=dmesg_tx_buf;
      adr = *((uint32_t*)(md380_packet+1));
      uint32_t size = *((uint32_t*)(md380_packet+5));
      memset(dmesg_tx_buf,0,DMESG_SIZE);
      if (check_spi_flash_size()>adr) {
        printf ("TDFU_SPIFLASHWRITE %x %d %x\n", adr, size, md380_packet+9);
        md380_spiflash_write(md380_packet+9,  adr, size);
      }
      break;
    case TDFU_SPIFLASHERASE64K:   // experimental
      //Re-uses the dmesg transmit buffer.
      *md380_dfu_target_adr=dmesg_tx_buf;
      adr= *((uint32_t*)(md380_packet+1));
      memset(dmesg_tx_buf,0,DMESG_SIZE);
      if (check_spi_flash_size()>adr) {
        printf ("TDFU_SPIFLASHERASE64K %x \n", adr);
//      spiflash_wait();     
//      spiflash_block_erase64k(adr);


        md380_spiflash_enable();
        md380_spi_sendrecv(0x6);
        md380_spiflash_disable();

        md380_spiflash_enable();
        md380_spi_sendrecv(0xd8);
        md380_spi_sendrecv((adr>> 16) & 0xff);
        md380_spi_sendrecv((adr>>  8) & 0xff);
        md380_spi_sendrecv(adr & 0xff);
        md380_spiflash_disable();
      }  
//      md380_spiflash_wait();   // this is the problem :( 
                           // must be polled via dfu commenad?
      break;
    case TDFU_SPIFLASHWRITE_NEW: // not working, this is not the problem
      //Re-uses the dmesg transmit buffer.
      *md380_dfu_target_adr=dmesg_tx_buf;
      adr = *((uint32_t*)(md380_packet+1));
      size = *((uint32_t*)(md380_packet+5));
      memset(dmesg_tx_buf,0,DMESG_SIZE);
      if (check_spi_flash_size()>adr) {
        printf ("DFU_CONFIG_SPIFLASHWRITE_new %x %d %x\n", adr, size, md380_packet+9);
        // enable write

        for (int i=0;i<size;i=i+256) {
          int page_adr;
          page_adr=adr+i;
          printf("%d %x\n",i,page_adr);
          md380_spiflash_wait();

          md380_spiflash_enable();
          md380_spi_sendrecv(0x6);
          md380_spiflash_disable();

          md380_spiflash_enable();
          md380_spi_sendrecv(0x2);
          printf("%x ", ((page_adr>> 16) & 0xff));
          md380_spi_sendrecv((page_adr>> 16) & 0xff);
          printf("%x ", ((page_adr>>  8) & 0xff));
          md380_spi_sendrecv((page_adr>>  8) & 0xff);
          printf("%x ", (page_adr & 0xff));
          md380_spi_sendrecv(page_adr & 0xff);
          for (int ii=0; ii < 256; ii++) {
            md380_spi_sendrecv(md380_packet[9+ii+i]);
          }
          md380_spiflash_disable();
          md380_spiflash_wait();
          printf("\n");
        }
      }
      break;
    case TDFU_SPIFLASHSECURITYREGREAD:
      //Re-uses the dmesg transmit buffer.
      *md380_dfu_target_adr=dmesg_tx_buf;
      printf("Dumping %d bytes from adr 0 SPI Flash security_registers\n",
	     DMESG_SIZE);
      md380_spiflash_security_registers_read(dmesg_tx_buf,
                                      0,
                                      3*256);
      break;
#endif //CONFIG_SPIFLASH
      
#ifdef CONFIG_SPIC5000
//Radio Commands
    case TDFU_C5000_READREG:
      //Re-uses the dmesg transmit buffer.
      *md380_dfu_target_adr=dmesg_tx_buf;
      memset(dmesg_tx_buf,0,DMESG_SIZE);
      state=OS_ENTER_CRITICAL();
      c5000_spi0_readreg(md380_packet[1],dmesg_tx_buf);
      OS_EXIT_CRITICAL(state);
      break;
    case TDFU_C5000_WRITEREG:
      //Re-uses the dmesg transmit buffer.
      *md380_dfu_target_adr=dmesg_tx_buf;
      memset(dmesg_tx_buf,0,DMESG_SIZE);
      state=OS_ENTER_CRITICAL();
      c5000_spi0_writereg(md380_packet[1],md380_packet[2]);
      OS_EXIT_CRITICAL(state);
      break;
#endif //CONFIG_SPIC5000

#ifdef CONFIG_GRAPHICS
//Graphics commands.
    case TDFU_PRINT: // 0x80, u8 x, u8 y, u8 str[].
      drawtext((wchar_t *) (md380_packet+3),
	       md380_packet[1],md380_packet[2]);
      break;
      
    case TDFU_BOX:
      break;
#endif //CONFIG_GRAPHICS

    case TDFU_SYSLOG:
      syslog_dump_dmesg();
      break;

#if(CONFIG_APP_MENU)
    case TDFU_READ_FRAMEBUFFER_24BPP: // read a tile with 24-bit RGB from the framebuffer.
       // Re-uses the dmesg tx buffer. If the app-menu's LCD driver is available,
       // this command reads out the LCD framebuffer in sufficiently small chunks ("tiles").
       // 160 pixels (x) / 16 pixels per tile = 10 tiles per screen horizontally,
       // 128 pixels (y) / 16 pixels per tile =  8 tile per screen vertically,
       // 10 * 8 = 80 tiles to read the entire framebuffer via USB .
       *md380_dfu_target_adr=dmesg_tx_buf; // dmesg_tx_buf = (char*)DMESG_START = 0x2001F700
       dmesg_tx_buf[0] = md380_packet[0];  // echo command byte
       dmesg_tx_buf[1] = dmesg_tx_buf[2] = 0xFF; // 'function currently not available'
       dmesg_tx_buf[3] = dmesg_tx_buf[4] = 0xFF; // invalid coords to indicate problem 
       if( LCD_busy ) 
        { // Don't "interrupt" the drawing process, instead let the remote "viewer"
          // (or screenshot utility) try again a few milliseconds later !
        }
       else // LCD controller/driver not busy, try to read pixels from framebuffer:
        {
          if( 0 < LCD_CopyRectFromFramebuffer_RGB(
                *((uint8_t*)(md380_packet+1)), // [in] x1 (tile start coord)
                *((uint8_t*)(md380_packet+2)), // [in] y1
                *((uint8_t*)(md380_packet+3)), // [in] x2 (tile end coord)
                *((uint8_t*)(md380_packet+4)), // [in] y2
                (uint8_t *)dmesg_tx_buf+5, // destination buffer, with rect-coords followed by 3 bytes per pixel
                    DMESG_SIZE ) ) // [in] sizeof_dest, for sanity check
           { // Positive value returned LCD_CopyRectFromFramebuffer() : "ok" !
             dmesg_tx_buf[1] = md380_packet[1]; // echo parameters ..
             dmesg_tx_buf[2] = md380_packet[2]; // .. so the client can request
             dmesg_tx_buf[3] = md380_packet[3]; //    retransmission if a packet got lost
             dmesg_tx_buf[4] = md380_packet[4]; // last parameter: y2
           }
        }
       break;
#endif // app-menu's LCD driver available (to read from framebuffer) ?
#if( CAN_POLL_KEYS )
    case TDFU_REMOTE_KEY_EVENT: // process 'remote keyboard' event
       // [in] md380_packet[1] = ASCII key ('M','U','D','B','0'..'9','*','#')
       //      md380_packet[2] = key_down_flag (1=pressed, 0=released)
       kb_OnRemoteKeyEvent( md380_packet[1]/*key_ascii*/, md380_packet[2]/*key_down_flag*/ ); 
       break;
#endif // can poll keys ?

    case TDFU_REBOOT_TO_BOOTLOADER:
       reboot_into_bootloader();
       break; 
    default:
      printf("Unhandled DFU packet type 0x%02x.\n",md380_packet[0]);
    }
    
    md380_thingy2[0]=0;
    md380_thingy2[1]=0;
    md380_thingy2[2]=0;
    md380_thingy2[3]=3;
    *md380_dfu_state=3;
    
    *md380_blockadr=0;
    *md380_packetlen=0;
    return 0;
  }else{
    /* For all other blocks, we default to the internal handler.
     */
    return usb_dnld_handle();
  }
}


void * get_md380_dnld_tohook_addr(){
  uint32_t * ram_start = (void  *) 0x20000000;
  int ram_size = 0x1ffff / 4;
  int i;
  int n=0;
  void * ret = NULL;
 
  for (i = 0;  i < ram_size ; i++){
    if (usb_dnld_handle == (void *) ram_start[i]){
      ret=&ram_start[i];
      n++;
    } 
  }

  if (n == 1)
    return(ret);
  return (0);
}



//! Hooks the USB DFU DNLD event handler in RAM.
void hookusb(){
  //Be damned sure to call this *after* the table has been
  //initialized.
  int * dnld_tohook;
  
  dnld_tohook = get_md380_dnld_tohook_addr();
  if (dnld_tohook){
    * dnld_tohook = (int) usb_dnld_hook;
  } else {
    printf("can't find dnld_tohook_addr\n");
  }  
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
  //green_led(1);
  
  //Hook the USB DNLOAD handler.
  hookusb();
  
  static long adr=0;
  long val;
  char buffer[]="@________ : ________";
  
#ifdef CONFIG_SPIFLASH
  //Read four bytes from SPI Flash.
  md380_spiflash_read(&val,adr,4);
#endif //CONFIG_SPIFLASH
 
  //Print them into the manufacturer string.
  strhex(buffer+1, adr);
  strhex(buffer+12, val);
  
  //Look forward a bit.
  adr+=4;
  
  //Return the string as our value.
  return loadusbstr(md380_usbstring,buffer,len);
}

void loadfirmwareversion_hook()
{
    static int version_state = 0;
    switch (version_state) {
      default:
        version_state = 0;
      case 0:
        snprintfw(print_buffer, 18, GIT_VERSION);
        version_state++;
        break;
      case 1:
        memcpy(print_buffer, VERSIONDATE, 24);
        version_state++;
        break;
    }
    return;
}
typedef void (*void_func_ptr)(void);
void reboot_into_bootloader(){
  /*from #193: 
   * travis says:
    There's a nice generic way to do it:

    Disable interrupts.
    Erase the block at 0x0800C000, so that the interrupt table is empty and the bootloader won't start the main application.
    Branch to the address stored in 0x08000004, which is the bootloader's RESET vector.
    The bootloader will then launch and wait for comms, rather than branch into the main application.

   */
  __disable_irq(); //disable interrupts
  // from STM32F4xx_DSP_StdPeriph_Lib/Project/STM32F4xx_StdPeriph_Examples/FLASH/FLASH_Program/main.h
  //#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base address of Sector 3, 16 Kbytes   */
  //this is just copied here so I can remember why I chose sector 3 -mike
  //
  int *reset_handler_ptr = (int *) 0x08000004; 
  int * reset_handler = (int *) * reset_handler_ptr;
  FLASH_Unlock();
  //no idea on the voltage range, but 3 works for me - mike
  FLASH_EraseSector(FLASH_Sector_3, VoltageRange_3 ); //just erased that sector
  (*(void_func_ptr)(reset_handler))(); //branch to address kept at 0x08000004
  //bootloader should be ready and waiting, good to go
}

//Must be const, because globals will be wacked.
//const wchar_t mfgstr[]=L"Travis KK4VCZ";
const char mfgstr[]="Travis Goodspeed KK4VCZ"; //"\x1c\x03T\0r\0a\0v\0i\0s\0 \0K\0K\x004\0V\0C\0Z\0";

