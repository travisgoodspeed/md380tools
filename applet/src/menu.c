/*! \file menu.c
  \brief Menu hooks and extensions.
*/

#include <stdio.h>
#include <string.h>

#include "printf.h"
#include "dmesg.h"
#include "md380.h"
#include "version.h"
#include "config.h"
#include "os.h"
#include "spiflash.h"
#include "addl_config.h"

static wchar_t wt_addl_func[]         = L"Addl. Funct";
static wchar_t wt_datef[]             = L"Date format";
static wchar_t wt_debug[]             = L"Debug";
static wchar_t wt_disable[]           = L"Disable";
static wchar_t wt_enable[]            = L"Enable";
static wchar_t wt_rbeep[]             = L"M. RogerBeep";
static wchar_t wt_userscsv[]          = L"UsersCSV";
static wchar_t wt_datef_original[]    = L"Original";
static wchar_t wt_datef_germany[]     = L"German";

/* This hooks a function that is called a lot during menu processing.
   Its exact purpose is unknown, but I'm working on that.
 */
void *main_menu_hook(void *menu){
  void *menustruct;

//  printf("main_menu() ");
//  printhex(menu,32);
//  printf("\n");
  
  
  switch(* ((int*)menu)){
  case 0x0b:
    //printf("Exiting menu.\n");
    break;
  case 0x24:
    //Third word of the parameter is a structure with
    //more entries.
    menustruct=*((int*)menu + 2);
    printf("Menu struct: @0x%08x\n",
	   menustruct);
    printf("Item %5d/%5d selected. %s\n",
	   (int) *((unsigned short*) (menustruct+0x42)),
	   (int) *((unsigned short*)menustruct),
	   "test");
    
    
    
    //printhex(*((int*) menu+2),128);
    //printf("\n");
    
    /*

Main menu:
Menu struct: @0x20001398
06000000 Total Entries
02000000 Selected Page Index
04000000 4a000000 00000000 91000000 8c0f0d08 00000000 3280ff00
1414ff00 c0c0c000 c0c0c000 00000000 ffffff00 ffffff00 80808000 00000500
                                                                   \--/
                                                                Selected item
00000000 4a001600 00436f6e 74616374 73006361 27001600 00536361 6e001600
                    \--Contacts begins here.
0a000c00 27001600 005a6f6e 65006c20 0a000c00 0b006573 49001600

Contacts Menu, last Entry:
Menu struct: @0x20001390
e4020000 Total Entries
e0020000 Selected Page Index
         04000000 9c000000 00000000 91000000 8c0f0d08 00000000 3280ff00
1414ff00 c0c0c000 c0c0c000 00000000 ffffff00 ffffff00 80808000 0000e302
                                                                   \--/
                                                                Selected item
00000000 7d001600 00547269 2d537461 74652028 4c322900 41001600 004c6f63
                    \--First contact entry starts here.
616c2039 00436f6e 09000b00 3b001600 00444d52 204e4100 09000b00
     */
    break;
  default:
    //do nothing
    break;
  }
  return main_menu(menu);
}




void create_menu_entry_hook(int a, void * b , void * c, void  * d, int e, int f ,int g) {
  printf("0x%x Text: 0x%x GreenKey 0x%x RedKey 0x%x 0x%x 0x%x 0x%x\n", a,b,c,d,e,f,g);
  printf("b: ");
  printhex2(b,14);
  printf("\n");
  create_menu_entry(a,b,c,d,e,f,g);
}

struct MENU {
  void    *menu_titel;
  void    *unknownp;
  uint8_t numberofentrys;
  uint8_t unknown_00;
  uint8_t unknown_01;
};




void create_menu_entry_rbeep_enable_screen(void) {
  struct MENU *menu_mem;
  
  menu_mem = (menu_memory + ((*menu_depth) * 0xc)) + 0xc;
  menu_mem->menu_titel = wt_rbeep;

  menu_mem->unknownp = 0x14 * (*menu_unkonwn_01) + menu_unknown_02;

  menu_mem->numberofentrys=1;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;

  create_menu_entry_hook( (*menu_id), wt_enable, menu_entry_back, menu_entry_back, 6, 2 , 1);
  spiflash_write("1", spi_flash_addl_config_start + offset_rbeep, 1);
  global_addl_config.rbeep = 1;
}

void create_menu_entry_rbeep_disable_screen(void) {
  struct MENU *menu_mem;

  menu_mem = (menu_memory + ((*menu_depth) * 0xc)) + 0xc;
  menu_mem->menu_titel = wt_rbeep;

  menu_mem->unknownp = 0x14 * (*menu_unkonwn_01) + menu_unknown_02;

  menu_mem->numberofentrys=1;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;

  create_menu_entry_hook( (*menu_id), wt_disable, menu_entry_back, menu_entry_back, 6, 2 , 1);
  spiflash_write("0", spi_flash_addl_config_start + offset_rbeep, 1);
  global_addl_config.rbeep = 0;
}

void create_menu_entry_datef_original_screen(void) {
  struct MENU *menu_mem;

  menu_mem = (menu_memory + ((*menu_depth) * 0xc)) + 0xc;
  menu_mem->menu_titel = wt_datef_original;

  menu_mem->unknownp = 0x14 * (*menu_unkonwn_01) + menu_unknown_02;

  menu_mem->numberofentrys=1;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;

  create_menu_entry_hook( (*menu_id), wt_datef_original, menu_entry_back, menu_entry_back, 6, 2 , 1);
  spiflash_write("0", spi_flash_addl_config_start + offset_datef, 1);
  global_addl_config.datef = 0;
}

void create_menu_entry_datef_germany_screen(void) {
  struct MENU *menu_mem;

  menu_mem = (menu_memory + ((*menu_depth) * 0xc)) + 0xc;
  menu_mem->menu_titel = wt_datef_germany;

  menu_mem->unknownp = 0x14 * (*menu_unkonwn_01) + menu_unknown_02;

  menu_mem->numberofentrys=1;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;

  create_menu_entry_hook( (*menu_id), wt_datef_germany, menu_entry_back, menu_entry_back, 6, 2 , 1);
  spiflash_write("1", spi_flash_addl_config_start + offset_datef, 1);
  global_addl_config.datef = 1;
   
}

void create_menu_entry_userscsv_enable_screen(void) {
  struct MENU *menu_mem;

  menu_mem = (menu_memory + ((*menu_depth) * 0xc)) + 0xc;
  menu_mem->menu_titel = wt_userscsv;

  menu_mem->unknownp = 0x14 * (*menu_unkonwn_01) + menu_unknown_02;

  menu_mem->numberofentrys=1;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;

  create_menu_entry_hook( (*menu_id), wt_enable, menu_entry_back, menu_entry_back, 6, 2 , 1);
  spiflash_write("1", spi_flash_addl_config_start + offset_userscsv, 1);
  global_addl_config.userscsv = 1;
}

void create_menu_entry_userscsv_disable_screen(void) {
  struct MENU *menu_mem;

  menu_mem = (menu_memory + ((*menu_depth) * 0xc)) + 0xc;
  menu_mem->menu_titel = wt_userscsv;

  menu_mem->unknownp = 0x14 * (*menu_unkonwn_01) + menu_unknown_02;

  menu_mem->numberofentrys=1;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;

  create_menu_entry_hook( (*menu_id), wt_disable, menu_entry_back, menu_entry_back, 6, 2 , 1);
  spiflash_write("0", spi_flash_addl_config_start + offset_userscsv, 1);
  global_addl_config.userscsv = 0;
}


void create_menu_entry_debug_enable_screen(void) {
//  uint8_t menu_depth;
  struct MENU *menu_mem;

  menu_mem = (menu_memory + ((*menu_depth) * 0xc)) + 0xc;
  menu_mem->menu_titel = wt_debug;

  menu_mem->unknownp = 0x14 * (*menu_unkonwn_01) + menu_unknown_02;

  menu_mem->numberofentrys=1;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;

  create_menu_entry_hook( (*menu_id), wt_enable, menu_entry_back, menu_entry_back, 6, 2 , 1);
  spiflash_write("1", spi_flash_addl_config_start + offset_debug, 1);
  global_addl_config.debug=1;  
}

void create_menu_entry_debug_disable_screen(void) {
  struct MENU *menu_mem;

  menu_mem = (menu_memory + ((*menu_depth) * 0xc)) + 0xc;
  menu_mem->menu_titel = wt_debug;

  menu_mem->unknownp = 0x14 * (*menu_unkonwn_01) + menu_unknown_02;

  menu_mem->numberofentrys=1;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;

  create_menu_entry_hook( (*menu_id), wt_disable, menu_entry_back, menu_entry_back, 6, 2 , 1);
  spiflash_write("0", spi_flash_addl_config_start + offset_debug, 1);
  global_addl_config.debug=0;
}


void create_menu_entry_rbeep_screen(void) {
  int i;
  struct MENU *menu_mem;
  int8_t buf[1];
  
  spiflash_read(buf, spi_flash_addl_config_start + offset_rbeep, 1); 

  menu_mem = (menu_memory + ((*menu_depth) * 0xc)) + 0xc;
  menu_mem->menu_titel = wt_rbeep;

  menu_mem->unknownp = 0x14 * (*menu_unkonwn_01) + menu_unknown_02;
  menu_mem->numberofentrys=2;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;
  if (buf[0] == '1') {
    *menu_entry_selected = 0;
  } else {
    *menu_entry_selected = 1;
  }    
  create_menu_entry_hook( (*menu_id),     wt_enable,  create_menu_entry_rbeep_enable_screen + 1,  menu_entry_back, 0x8b, 0 , 1);
  create_menu_entry_hook( (*menu_id) + 1, wt_disable, create_menu_entry_rbeep_disable_screen + 1, menu_entry_back, 0x8b, 0 , 1);

  for(i=0;i<2;i++) { // not yet known ;)
    uint8_t *p;
    p = menu_unknown_02 + ( (*menu_unkonwn_01) + i ) * 0x14;
    p[0x10] = 0;
  }
}

void create_menu_entry_datef_screen(void) {
  int i;
  struct MENU *menu_mem;
  int8_t buf[1];
  
  spiflash_read(buf, spi_flash_addl_config_start + offset_datef, 1);
  
  menu_mem = (menu_memory + ((*menu_depth) * 0xc)) + 0xc;
  menu_mem->menu_titel = wt_datef;

  menu_mem->unknownp = 0x14 * (*menu_unkonwn_01) + menu_unknown_02;
  menu_mem->numberofentrys=2;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;

  if (buf[0] == '1') {
    *menu_entry_selected = 1;
  } else {
    *menu_entry_selected = 0;
  }    

  create_menu_entry_hook( (*menu_id),     wt_datef_original,  create_menu_entry_datef_original_screen + 1, menu_entry_back,  0x8b, 0 , 1);
  create_menu_entry_hook( (*menu_id) + 1, wt_datef_germany,  create_menu_entry_datef_germany_screen + 1, menu_entry_back, 0x8b, 0 , 1);

  for(i=0;i<2;i++) { // not yet known ;)
    uint8_t *p;
    p = menu_unknown_02 + ( (*menu_unkonwn_01) + i ) * 0x14;
    p[0x10] = 0;
  }
}

void create_menu_entry_userscsv_screen(void) {
  int i;
  struct MENU *menu_mem;
  int8_t buf[1];
  
  spiflash_read(buf, spi_flash_addl_config_start + offset_userscsv, 1);
  
  menu_mem = (menu_memory + ((*menu_depth) * 0xc)) + 0xc;
  menu_mem->menu_titel = wt_userscsv;

  menu_mem->unknownp = 0x14 * (*menu_unkonwn_01) + menu_unknown_02;
  menu_mem->numberofentrys=2;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;

  if (buf[0] == '1') {
    *menu_entry_selected = 0;
  } else {
    *menu_entry_selected = 1;
  }    
                   
  
  create_menu_entry_hook( (*menu_id),     wt_enable,  create_menu_entry_userscsv_enable_screen + 1, menu_entry_back,  0x8b, 0 , 1);
  create_menu_entry_hook( (*menu_id) + 1, wt_disable, create_menu_entry_userscsv_disable_screen + 1, menu_entry_back, 0x8b, 0 , 1);

  for(i=0;i<2;i++) { // not yet known ;)
    uint8_t *p;
    p = menu_unknown_02 + ( (*menu_unkonwn_01) + i ) * 0x14;
    p[0x10] = 0;
  }
}

void create_menu_entry_debug_screen(void) {
  int i;
  struct MENU *menu_mem;
  int8_t buf[1];
    
  spiflash_read(buf, spi_flash_addl_config_start + offset_debug, 1);

  menu_mem = (menu_memory + ((*menu_depth) * 0xc)) + 0xc;
  menu_mem->menu_titel = wt_debug;

  menu_mem->unknownp = 0x14 * (*menu_unkonwn_01) + menu_unknown_02;
  menu_mem->numberofentrys=2;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;
  
  if (buf[0] == '1') {
    *menu_entry_selected = 0;
    global_addl_config.debug = 1;
  } else {
    *menu_entry_selected = 1;
    global_addl_config.debug = 0;
  }
  
  create_menu_entry_hook( (*menu_id),     wt_enable,  create_menu_entry_debug_enable_screen + 1, menu_entry_back,  0x8b, 0 , 1);
  create_menu_entry_hook( (*menu_id) + 1, wt_disable, create_menu_entry_debug_disable_screen + 1, menu_entry_back, 0x8b, 0 , 1);

  for(i=0;i<2;i++) { // not yet known ;)
    uint8_t *p;
    p = menu_unknown_02 + ( (*menu_unkonwn_01) + i ) * 0x14;
    p[0x10] = 0;
  }
}

void create_menu_entry_addl_functions_screen(void) {
  struct MENU *menu_mem;
  int i;

  menu_mem = (menu_memory + ((*menu_depth) * 0xc)) + 0xc;
  menu_mem->menu_titel = wt_addl_func;

  menu_mem->unknownp = 0x14 * (*menu_unkonwn_01) + menu_unknown_02;

  menu_mem->numberofentrys=4;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;

  create_menu_entry_hook( (*menu_id),     wt_rbeep,    create_menu_entry_rbeep_screen + 1,    menu_entry_back, 0x98, 0 , 1);
  create_menu_entry_hook( (*menu_id) + 1, wt_datef,    create_menu_entry_datef_screen + 1,    menu_entry_back, 0x98, 0 , 1);
  create_menu_entry_hook( (*menu_id) + 2, wt_userscsv, create_menu_entry_userscsv_screen + 1, menu_entry_back, 0x98, 0 , 1);
  create_menu_entry_hook( (*menu_id) + 3, wt_debug,    create_menu_entry_debug_screen + 1,    menu_entry_back, 0x98, 0 , 1);

 for(i=0;i<4;i++) {  // not yet known ;)
   uint8_t *p;
   p = menu_unknown_02 + ( (*menu_unkonwn_01) + i ) * 0x14;
   p[0x10] = 2;
 }
}


void create_menu_utilies_hook(void) {
  if ( (* program_radio_unprohibited & 0x4) == 0x4 ) {
    printf("program_radio_unprohibited\n");
  } else {
    printf("program_radio_prohibited\n");
  }

  create_menu_entry_hook(8, wt_programradio, menu_entry_programradio , menu_entry_back, 0x8a, 0 , 1);
  create_menu_entry_hook(9, wt_addl_func,     create_menu_entry_addl_functions_screen + 1 , menu_entry_back, 0x8a,0 , 1);
}

