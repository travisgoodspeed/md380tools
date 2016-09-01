/*! \file menu.c
  \brief Menu hooks and extensions.
*/


//#define DEBUG

#include <stdio.h>
#include <string.h>

#ifdef DEBUG
#include "printf.h"
#endif

#include "dmesg.h"
#include "md380.h"
#include "version.h"
#include "config.h"
#include "os.h"
#include "spiflash.h"
#include "addl_config.h"

const static wchar_t wt_addl_func[]         = L"MD380Tools";
const static wchar_t wt_datef[]             = L"Date format";
const static wchar_t wt_debug[]             = L"Debug";
const static wchar_t wt_disable[]           = L"Disable";
const static wchar_t wt_enable[]            = L"Enable";
const static wchar_t wt_rbeep[]             = L"M. RogerBeep";
const static wchar_t wt_userscsv[]          = L"UsersCSV";
const static wchar_t wt_datef_original[]    = L"YYYY/MM/DD";
const static wchar_t wt_datef_germany[]     = L"DD.MM.YYYY";
const static wchar_t wt_datef_italy[]       = L"DD/MM/YYYY";
const static wchar_t wt_datef_american[]    = L"MM/DD/YYYY";
const static wchar_t wt_promtg[]            = L"Promiscuous";
const static wchar_t wt_edit[]              = L"Edit";
const static wchar_t wt_edit_dmr_id[]       = L"Edit DMR-ID";
const static wchar_t wt_no_w25q128[]        = L"No W25Q128";
const static wchar_t wt_experimental[]      = L"Experimental";
const static wchar_t wt_micbargraph[]       = L"Mic bargraph";


struct MENU {
  const wchar_t  *menu_title;
  void    *unknownp;
  uint8_t numberof_menu_entries;
  uint8_t unknown_00;
  uint8_t unknown_01;
};


/* This hooks a function that is called a lot during menu processing.
   Its exact purpose is unknown, but I'm working on that.
 */
void *main_menu_hook(void *menu){
#ifdef DEBUG
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
    menustruct=(void*) *((int*)menu + 2);

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
#endif
  return main_menu(menu);
}



//void create_menu_entry_addl_functions_screen(void) ;

void create_menu_entry_hook(int menuid, const wchar_t * label , void * green_key, void  * red_key, int e, int f ,int enabled) {
#ifdef DEBUG
  printf("0x%x Text: 0x%x GreenKey 0x%x RedKey 0x%x 0x%x 0x%x 0x%x\n", menuid,label,green_key,red_key,e,f,enabled);
  printf("b: ");
  printhex2((char *) label,14);
  printf("\n");
  printf(" md380_menu_depth: %d\n", md380_menu_depth);
#endif
  md380_create_menu_entry(menuid,label,green_key,red_key,e,f,enabled);
}

void spiflash_write_uint8( int offset, uint8_t val )
{
    char buf[2];
    buf[0] = '0' + val ;
    buf[1] = 0 ;
    spiflash_write_with_type_check(buf, spi_flash_addl_config_start + offset, 1);
}

void create_menu_entry_promtg_enable_screen(void) {
  struct MENU *menu_mem;

  menu_mem = ( void *) ((md380_menu_memory + ((md380_menu_depth) * sizeof(struct MENU))) + sizeof(struct MENU));
  menu_mem->menu_title = wt_promtg;

  menu_mem->unknownp = 0x14 * md380_menu_id + md380_menu_mem_base;

  menu_mem->numberof_menu_entries=1;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;

#ifdef CONFIG_MENU
  create_menu_entry_hook( md380_menu_id, wt_enable, md380_menu_entry_back+1, md380_menu_entry_back+1, 6, 2 , 1);
#endif
  spiflash_write_with_type_check("1", spi_flash_addl_config_start + offset_promtg, 1);
  global_addl_config.promtg=1;
}

void create_menu_entry_promtg_disable_screen(void) {
  struct MENU *menu_mem;

  menu_mem = (void *)((md380_menu_memory + ((md380_menu_depth) * sizeof(struct MENU))) + sizeof(struct MENU));
  menu_mem->menu_title = wt_promtg;

  menu_mem->unknownp = 0x14 * md380_menu_id + md380_menu_mem_base;

  menu_mem->numberof_menu_entries=1;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;

#ifdef CONFIG_MENU
  create_menu_entry_hook( md380_menu_id, wt_disable, md380_menu_entry_back+1, md380_menu_entry_back+1, 6, 2 , 1);
#endif
  spiflash_write_with_type_check("0", spi_flash_addl_config_start + offset_promtg, 1);
  global_addl_config.promtg=0;
}

void create_menu_entry_micbargraph_enable_screen(void) {
  struct MENU *menu_mem;

  menu_mem = (void *)((md380_menu_memory + ((md380_menu_depth) * sizeof(struct MENU))) + sizeof(struct MENU));
  menu_mem->menu_title = wt_micbargraph;

  menu_mem->unknownp = 0x14 * md380_menu_id + md380_menu_mem_base;

  menu_mem->numberof_menu_entries=1;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;

#ifdef CONFIG_MENU
  create_menu_entry_hook( md380_menu_id, wt_enable, md380_menu_entry_back+1, md380_menu_entry_back+1, 6, 2 , 1);
#endif
  spiflash_write_with_type_check("1", spi_flash_addl_config_start + offset_micbargraph, 1);
  global_addl_config.micbargraph=1;
}

void create_menu_entry_micbargraph_disable_screen(void) {
  struct MENU *menu_mem;

  menu_mem = (void *)((md380_menu_memory + ((md380_menu_depth) * sizeof(struct MENU))) + sizeof(struct MENU));
  menu_mem->menu_title = wt_micbargraph;

  menu_mem->unknownp = 0x14 * md380_menu_id + md380_menu_mem_base;

  menu_mem->numberof_menu_entries=1;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;

#ifdef CONFIG_MENU
  create_menu_entry_hook( md380_menu_id, wt_disable, md380_menu_entry_back+1, md380_menu_entry_back+1, 6, 2 , 1);
#endif
  spiflash_write_with_type_check("0", spi_flash_addl_config_start + offset_micbargraph, 1);
  global_addl_config.micbargraph=0;
}

void create_menu_entry_rbeep_enable_screen(void) {
  struct MENU *menu_mem;

  menu_mem = (void *)((md380_menu_memory + ((md380_menu_depth) * sizeof(struct MENU))) + sizeof(struct MENU));
  menu_mem->menu_title = wt_rbeep;

  menu_mem->unknownp = 0x14 * md380_menu_id + md380_menu_mem_base;

  menu_mem->numberof_menu_entries=1;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;

#ifdef CONFIG_MENU
  create_menu_entry_hook( md380_menu_id, wt_enable, md380_menu_entry_back+1, md380_menu_entry_back+1, 6, 2 , 1);
#endif
  spiflash_write_with_type_check("1", spi_flash_addl_config_start + offset_rbeep, 1);
  global_addl_config.rbeep = 1;
}

void create_menu_entry_rbeep_disable_screen(void) {
  struct MENU *menu_mem;

  menu_mem = (void *)((md380_menu_memory + ((md380_menu_depth) * sizeof(struct MENU))) + sizeof(struct MENU));
  menu_mem->menu_title = wt_rbeep;

  menu_mem->unknownp = 0x14 * md380_menu_id + md380_menu_mem_base;

  menu_mem->numberof_menu_entries=1;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;

#ifdef CONFIG_MENU
  create_menu_entry_hook( md380_menu_id, wt_disable, md380_menu_entry_back+1, md380_menu_entry_back+1, 6, 2 , 1);
#endif
  spiflash_write_with_type_check("0", spi_flash_addl_config_start + offset_rbeep, 1);
  global_addl_config.rbeep = 0;
}

void create_menu_entry_datef_original_screen(void) {
  struct MENU *menu_mem;

  menu_mem = (void *)((md380_menu_memory + ((md380_menu_depth) * sizeof(struct MENU))) + sizeof(struct MENU));
  menu_mem->menu_title = wt_datef_original;

  menu_mem->unknownp = 0x14 * md380_menu_id + md380_menu_mem_base;

  menu_mem->numberof_menu_entries=1;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;

#ifdef CONFIG_MENU
  create_menu_entry_hook( md380_menu_id, wt_datef_original, md380_menu_entry_back+1, md380_menu_entry_back+1, 6, 2 , 1);
#endif
  global_addl_config.datef = 0;
  spiflash_write_uint8( offset_datef, global_addl_config.datef );
}

void create_menu_entry_datef_germany_screen(void) {
  struct MENU *menu_mem;

  menu_mem = (void *)((md380_menu_memory + ((md380_menu_depth) * sizeof(struct MENU))) + sizeof(struct MENU));
  menu_mem->menu_title = wt_datef_germany;

  menu_mem->unknownp = 0x14 * md380_menu_id + md380_menu_mem_base;

  menu_mem->numberof_menu_entries=1;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;

#ifdef CONFIG_MENU
  create_menu_entry_hook( md380_menu_id, wt_datef_germany, md380_menu_entry_back+1, md380_menu_entry_back+1, 6, 2 , 1);
#endif
  global_addl_config.datef = 1;
  spiflash_write_uint8( offset_datef, global_addl_config.datef );
}

void create_menu_entry_datef_italy_screen(void) {
  struct MENU *menu_mem;

  menu_mem = (void *)((md380_menu_memory + ((md380_menu_depth) * sizeof(struct MENU))) + sizeof(struct MENU));
  menu_mem->menu_title = wt_datef_italy;

  menu_mem->unknownp = 0x14 * md380_menu_id + md380_menu_mem_base;

  menu_mem->numberof_menu_entries=1;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;

#ifdef CONFIG_MENU
  create_menu_entry_hook( md380_menu_id, wt_datef_italy, md380_menu_entry_back+1, md380_menu_entry_back+1, 6, 2 , 1);
#endif
  global_addl_config.datef = 2;
  spiflash_write_uint8( offset_datef, global_addl_config.datef );
}

void create_menu_entry_datef_american_screen(void) {
  struct MENU *menu_mem;

  menu_mem = (void *)((md380_menu_memory + ((md380_menu_depth) * sizeof(struct MENU))) + sizeof(struct MENU));
  menu_mem->menu_title = wt_datef_american;

  menu_mem->unknownp = 0x14 * md380_menu_id + md380_menu_mem_base;

  menu_mem->numberof_menu_entries=1;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;

#ifdef CONFIG_MENU
  create_menu_entry_hook( md380_menu_id, wt_datef_american, md380_menu_entry_back+1, md380_menu_entry_back+1, 6, 2 , 1);
#endif
  global_addl_config.datef = 3;
  spiflash_write_uint8( offset_datef, global_addl_config.datef );
}

void create_menu_entry_userscsv_enable_screen(void) {
  struct MENU *menu_mem;

  menu_mem = (void *)((md380_menu_memory + ((md380_menu_depth) * sizeof(struct MENU))) + sizeof(struct MENU));
  menu_mem->menu_title = wt_userscsv;

  menu_mem->unknownp = 0x14 * md380_menu_id + md380_menu_mem_base;

  menu_mem->numberof_menu_entries=1;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;

#ifdef CONFIG_MENU
  create_menu_entry_hook( md380_menu_id, wt_enable, md380_menu_entry_back+1, md380_menu_entry_back+1, 6, 2 , 1);
#endif
  spiflash_write_with_type_check("1", spi_flash_addl_config_start + offset_userscsv, 1);
  global_addl_config.userscsv = 1;
}

void create_menu_entry_userscsv_disable_screen(void) {
  struct MENU *menu_mem;

  menu_mem = (void *)((md380_menu_memory + ((md380_menu_depth) * sizeof(struct MENU))) + sizeof(struct MENU));
  menu_mem->menu_title = wt_userscsv;

  menu_mem->unknownp = 0x14 * md380_menu_id + md380_menu_mem_base;

  menu_mem->numberof_menu_entries=1;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;

#ifdef CONFIG_MENU
  create_menu_entry_hook( md380_menu_id, wt_disable, md380_menu_entry_back+1, md380_menu_entry_back+1, 6, 2 , 1);
#endif
  spiflash_write_with_type_check("0", spi_flash_addl_config_start + offset_userscsv, 1);
  global_addl_config.userscsv = 0;
}


void create_menu_entry_experimental_enable_screen(void) {
  struct MENU *menu_mem;

  menu_mem = (void *)((md380_menu_memory + ((md380_menu_depth) * sizeof(struct MENU))) + sizeof(struct MENU));
  menu_mem->menu_title = wt_experimental;

  menu_mem->unknownp = 0x14 * md380_menu_id + md380_menu_mem_base;

  menu_mem->numberof_menu_entries=1;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;

#ifdef CONFIG_MENU
  create_menu_entry_hook( md380_menu_id, wt_enable, md380_menu_entry_back+1, md380_menu_entry_back+1, 6, 2 , 1);
#endif
  global_addl_config.experimental = 1;
}

void create_menu_entry_experimental_disable_screen(void) {
  struct MENU *menu_mem;

  menu_mem = (void *)((md380_menu_memory + ((md380_menu_depth) * sizeof(struct MENU))) + sizeof(struct MENU));
  menu_mem->menu_title = wt_experimental;

  menu_mem->unknownp = 0x14 * md380_menu_id + md380_menu_mem_base;

  menu_mem->numberof_menu_entries=1;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;

#ifdef CONFIG_MENU
  create_menu_entry_hook( md380_menu_id, wt_disable, md380_menu_entry_back+1, md380_menu_entry_back+1, 6, 2 , 1);
#endif
  global_addl_config.experimental = 0;
}



void create_menu_entry_debug_enable_screen(void) {
  struct MENU *menu_mem;

  menu_mem = (void *)((md380_menu_memory + ((md380_menu_depth) * sizeof(struct MENU))) + sizeof(struct MENU));
  menu_mem->menu_title = wt_debug;

  menu_mem->unknownp = 0x14 * md380_menu_id + md380_menu_mem_base;

  menu_mem->numberof_menu_entries=1;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;

#ifdef CONFIG_MENU
  create_menu_entry_hook( md380_menu_id, wt_enable, md380_menu_entry_back+1, md380_menu_entry_back+1, 6, 2 , 1);
#endif
  spiflash_write_with_type_check("0", spi_flash_addl_config_start + offset_debug, 1);
  global_addl_config.debug=1;
}

void create_menu_entry_debug_disable_screen(void) {
  struct MENU *menu_mem;

  menu_mem = (void *)((md380_menu_memory + ((md380_menu_depth) * sizeof(struct MENU))) + sizeof(struct MENU));
  menu_mem->menu_title = wt_debug;

  menu_mem->unknownp = 0x14 * md380_menu_id + md380_menu_mem_base;

  menu_mem->numberof_menu_entries=1;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;

#ifdef CONFIG_MENU
  create_menu_entry_hook( md380_menu_id, wt_disable, md380_menu_entry_back+1, md380_menu_entry_back+1, 6, 2 , 1);
#endif
  spiflash_write_with_type_check("0", spi_flash_addl_config_start + offset_debug, 1);
  global_addl_config.debug=0;
}


void create_menu_entry_promtg_screen(void) {
  int i;
  struct MENU *menu_mem;
  int8_t buf[1];

  md380_spiflash_read(buf, spi_flash_addl_config_start + offset_promtg, 1);

  menu_mem = (void *)((md380_menu_memory + ((md380_menu_depth) * sizeof(struct MENU))) + sizeof(struct MENU));
  menu_mem->menu_title = wt_promtg;

  menu_mem->unknownp = 0x14 * md380_menu_id + md380_menu_mem_base;
  menu_mem->numberof_menu_entries=2;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;

  if (buf[0] == '1') {
    md380_menu_entry_selected = 0;
  } else {
    md380_menu_entry_selected = 1;
  }

#ifdef CONFIG_MENU
  create_menu_entry_hook( md380_menu_id,     wt_enable,  create_menu_entry_promtg_enable_screen+1, md380_menu_entry_back+1,  0x8b, 0 , 1);
  create_menu_entry_hook( md380_menu_id + 1, wt_disable, create_menu_entry_promtg_disable_screen+1, md380_menu_entry_back+1, 0x8b, 0 , 1);
#endif

  for(i=0;i<2;i++) { // not yet known ;)
    uint8_t *p;
    p = (uint8_t *)md380_menu_mem_base + ( md380_menu_id + i ) * 0x14;
    p[0x10] = 0;
  }
}

void create_menu_entry_micbargraph_screen(void) {
  int i;
  struct MENU *menu_mem;
  int8_t buf[1];

  md380_spiflash_read(buf, spi_flash_addl_config_start + offset_micbargraph, 1);

  menu_mem = (void *)((md380_menu_memory + ((md380_menu_depth) * sizeof(struct MENU))) + sizeof(struct MENU));
  menu_mem->menu_title = wt_micbargraph;

  menu_mem->unknownp = 0x14 * md380_menu_id + md380_menu_mem_base;
  menu_mem->numberof_menu_entries=2;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;

  if (buf[0] == '1') {
    md380_menu_entry_selected = 0;
  } else {
    md380_menu_entry_selected = 1;
  }

#ifdef CONFIG_MENU
  create_menu_entry_hook( md380_menu_id,     wt_enable,  create_menu_entry_micbargraph_enable_screen+1, md380_menu_entry_back+1,  0x8b, 0 , 1);
  create_menu_entry_hook( md380_menu_id + 1, wt_disable, create_menu_entry_micbargraph_disable_screen+1, md380_menu_entry_back+1, 0x8b, 0 , 1);
#endif

  for(i=0;i<2;i++) { // not yet known ;)
    uint8_t *p;
    p = (uint8_t *)md380_menu_mem_base + ( md380_menu_id + i ) * 0x14;
    p[0x10] = 0;
  }
}



void create_menu_entry_rbeep_screen(void) {
  int i;
  struct MENU *menu_mem;
  int8_t buf[1];

  md380_spiflash_read(buf, spi_flash_addl_config_start + offset_rbeep, 1);

  menu_mem = (void *)((md380_menu_memory + ((md380_menu_depth) * sizeof(struct MENU))) + sizeof(struct MENU));
  menu_mem->menu_title = wt_rbeep;

  menu_mem->unknownp = 0x14 * md380_menu_id + md380_menu_mem_base;
  menu_mem->numberof_menu_entries=2;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;
  if (buf[0] == '1') {
    md380_menu_entry_selected = 0;
  } else {
    md380_menu_entry_selected = 1;
  }

#ifdef CONFIG_MENU
  create_menu_entry_hook( md380_menu_id,     wt_enable,  create_menu_entry_rbeep_enable_screen + 1,  md380_menu_entry_back+1, 0x8b, 0 , 1);
  create_menu_entry_hook( md380_menu_id + 1, wt_disable, create_menu_entry_rbeep_disable_screen + 1, md380_menu_entry_back+1, 0x8b, 0 , 1);
#endif

  for(i=0;i<2;i++) { // not yet known ;)
    uint8_t *p;
    p = (uint8_t *)md380_menu_mem_base + ( md380_menu_id + i ) * 0x14;
    p[0x10] = 0;
  }
}

void create_menu_entry_datef_screen(void) {
  int i;
  struct MENU *menu_mem;
  int8_t buf[1];

  md380_spiflash_read(buf, spi_flash_addl_config_start + offset_datef, 1);

  menu_mem = (void *)((md380_menu_memory + ((md380_menu_depth) * sizeof(struct MENU))) + sizeof(struct MENU));
  menu_mem->menu_title = wt_datef;

  menu_mem->unknownp = 0x14 * md380_menu_id + md380_menu_mem_base;
  menu_mem->numberof_menu_entries=4;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;

  if (buf[0] == '0') {
    md380_menu_entry_selected = 0;
  }
  if (buf[0] == '1') {
    md380_menu_entry_selected = 1;
  }
  if (buf[0] == '2') {
    md380_menu_entry_selected = 2;
  }
  if (buf[0] == '3') {
    md380_menu_entry_selected = 3;
  }

#ifdef CONFIG_MENU
  create_menu_entry_hook( md380_menu_id,     wt_datef_original,  create_menu_entry_datef_original_screen + 1, md380_menu_entry_back+1,  0x8b, 0 , 1);
  create_menu_entry_hook( md380_menu_id + 1, wt_datef_germany,  create_menu_entry_datef_germany_screen + 1, md380_menu_entry_back+1, 0x8b, 0 , 1);
  create_menu_entry_hook( md380_menu_id + 2, wt_datef_italy,  create_menu_entry_datef_italy_screen + 1, md380_menu_entry_back+1, 0x8b, 0 , 1);
  create_menu_entry_hook( md380_menu_id + 3, wt_datef_american,  create_menu_entry_datef_american_screen + 1, md380_menu_entry_back+1, 0x8b, 0 , 1);
#endif

  for(i=0;i<4;i++) { // not yet known ;)
    uint8_t *p;
    p = (uint8_t *)md380_menu_mem_base + ( md380_menu_id + i ) * 0x14;
    p[0x10] = 0;
  }
}

void create_menu_entry_userscsv_screen(void) {
  int i;
  struct MENU *menu_mem;
  int8_t buf[1];

  /*
    Previously, this would check the flash type, but as all known
    flash chips are at least 1MB, we can skip the check and always use
    the memory.

  if(check_spi_flash_type()) {
  */
    md380_spiflash_read(buf, spi_flash_addl_config_start + offset_userscsv, 1);
    menu_mem = (void *)((md380_menu_memory + ((md380_menu_depth) * sizeof(struct MENU))) + sizeof(struct MENU));
    menu_mem->menu_title = wt_userscsv;
    menu_mem->unknownp = 0x14 * md380_menu_id + md380_menu_mem_base;
    menu_mem->numberof_menu_entries=2;
    menu_mem->unknown_00 = 0;
    menu_mem->unknown_01 = 0;
    if (buf[0] == '1') {
      md380_menu_entry_selected = 0;
    } else {
      md380_menu_entry_selected = 1;
    }

#ifdef CONFIG_MENU
    create_menu_entry_hook( md380_menu_id,     wt_enable,  create_menu_entry_userscsv_enable_screen + 1, md380_menu_entry_back+1,  0x8b, 0 , 1);
    create_menu_entry_hook( md380_menu_id + 1, wt_disable, create_menu_entry_userscsv_disable_screen + 1, md380_menu_entry_back+1, 0x8b, 0 , 1);
#endif

    for(i=0;i<2;i++) { // not yet known ;)
      uint8_t *p;
      p = (uint8_t *)md380_menu_mem_base + ( md380_menu_id + i ) * 0x14;
      p[0x10] = 0;
    }
    /*
  } else {
    menu_mem = (md380_menu_memory + ((md380_menu_depth) * sizeof(struct MENU))) + sizeof(struct MENU);
    menu_mem->menu_title = wt_userscsv;
    menu_mem->unknownp = 0x14 * md380_menu_id + md380_menu_mem_base;
    menu_mem->numberof_menu_entries=1;
    menu_mem->unknown_00 = 0;
    menu_mem->unknown_01 = 0;
    create_menu_entry_hook( md380_menu_id, wt_no_w25q128, md380_menu_entry_back+1, md380_menu_entry_back+1, 6, 2 , 1);
  }
    */
}

void create_menu_entry_debug_screen(void) {
  int i;
  struct MENU *menu_mem;
  int8_t buf[1];

  md380_spiflash_read(buf, spi_flash_addl_config_start + offset_debug, 1);

  menu_mem = (void *)((md380_menu_memory + ((md380_menu_depth) * sizeof(struct MENU))) + sizeof(struct MENU));
  menu_mem->menu_title = wt_debug;

  menu_mem->unknownp = 0x14 * md380_menu_id + md380_menu_mem_base;
  menu_mem->numberof_menu_entries=2;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;

  if (buf[0] == '1') {
    md380_menu_entry_selected = 0;
    global_addl_config.debug = 1;
  } else {
    md380_menu_entry_selected = 1;
    global_addl_config.debug = 0;
  }

#ifdef CONFIG_MENU
  create_menu_entry_hook( md380_menu_id,     wt_enable,  create_menu_entry_debug_enable_screen + 1, md380_menu_entry_back+1,  0x8b, 0 , 1);
  create_menu_entry_hook( md380_menu_id + 1, wt_disable, create_menu_entry_debug_disable_screen + 1, md380_menu_entry_back+1, 0x8b, 0 , 1);
#endif

  for(i=0;i<2;i++) { // not yet known ;)
    uint8_t *p;
    p = (uint8_t *)md380_menu_mem_base + ( md380_menu_id + i ) * 0x14;
    p[0x10] = 0;
  }
}

void create_menu_entry_experimental_screen(void) {
  int i;
  struct MENU *menu_mem;

  menu_mem = (void *)((md380_menu_memory + ((md380_menu_depth) * sizeof(struct MENU))) + sizeof(struct MENU));
  menu_mem->menu_title = wt_experimental;

  menu_mem->unknownp = 0x14 * md380_menu_id + md380_menu_mem_base;
  menu_mem->numberof_menu_entries=2;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;

  if (global_addl_config.experimental == 1) {
    md380_menu_entry_selected = 0;
  } else {
    md380_menu_entry_selected = 1;
  }

#ifdef CONFIG_MENU
  create_menu_entry_hook( md380_menu_id,     wt_enable,  create_menu_entry_experimental_enable_screen + 1, md380_menu_entry_back+1,  0x8b, 0 , 1);
  create_menu_entry_hook( md380_menu_id + 1, wt_disable, create_menu_entry_experimental_disable_screen + 1, md380_menu_entry_back+1, 0x8b, 0 , 1);
#endif //CONFIG_MENU

  for(i=0;i<2;i++) { // not yet known ;)
    uint8_t *p;
    p = (uint8_t *)md380_menu_mem_base + ( md380_menu_id + i ) * 0x14;
    p[0x10] = 0;
  }
}



void create_menu_entry_edit_screen_store(void) {
#ifdef DEBUG
  printf("your enter: ");
  printhex2((char *) md380_menu_edit_buf,14);
  printf("\n");
#endif
  md380_menu_id    = md380_menu_id - 1;
  md380_menu_depth = md380_menu_depth - 1;

#ifdef CONFIG_MENU
  create_menu_entry_hook( md380_menu_id, md380_menu_edit_buf,    md380_menu_entry_back+1,  md380_menu_entry_back+1  ,6, 1 , 1);
#endif
}


void create_menu_entry_edit_screen(void) {
  struct MENU *menu_mem;
  uint8_t i;
  uint8_t *p;

  md380_menu_0x2001d3c1 = md380_menu_0x200011e4;
  md380_menu_0x20001114 =  (uint32_t) md380_menu_edit_buf;


/*
  0x08012a8e      dff8200b       ldr.w r0, [pc, 0xb20]       ; [0x80135b0:4]=0x20001114
  0x08012a92      dff8201b       ldr.w r1, [pc, 0xb20]       ; [0x80135b4:4]=0x2001b716
  0x08012a96      0160           str r1, [r0]
  0x08012a98      0020           movs r0, 0
  0x08012a9a      07e0           b 0x8012aac
  0x08012a9c      c0b2           uxtb r0, r0
  0x08012a9e      dff8101b       ldr.w r1, [pc, 0xb10]       ; [0x80135b0:4]=0x20001114
  0x08012aa2      0968           ldr r1, [r1]
  0x08012aa4      0022           movs r2, 0
  0x08012aa6      21f81020       strh.w r2, [r1, r0, lsl 1]
  0x08012aaa      401c           adds r0, r0, 1
  0x08012aac      c0b2           uxtb r0, r0
  0x08012aae      1128           cmp r0, 0x11
  0x08012ab0      f4d3           blo 0x8012a9c
  */

// clear retrun buffer //  see 0x08012a98
 for (i=0; i < 0x11; i++) {
   p=(uint8_t *) md380_menu_0x20001114;
   p = p + i;
   *p = 0;
   }


  md380_menu_0x2001d3ed = 8;
  md380_menu_0x2001d3ee = 0;
  md380_menu_0x2001d3ef = 0;
  md380_menu_0x2001d3f0 = 3;
  md380_menu_0x2001d3f1 = 0;
  md380_menu_0x2001d3f4 = 0;

  menu_mem =(void *) ((md380_menu_memory + ((md380_menu_depth) * sizeof(struct MENU))) +  sizeof(struct MENU));
  menu_mem->menu_title = wt_edit;
  menu_mem->unknownp = 0x14 * md380_menu_id + md380_menu_mem_base;
  menu_mem->numberof_menu_entries=1;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;

#ifdef CONFIG_MENU
  create_menu_entry_hook( md380_menu_id,  wt_edit ,  create_menu_entry_edit_screen_store + 1 , md380_menu_numerical_input  + 1,  0x81, 0 , 1);
#endif
}


void create_menu_entry_edit_dmr_id_screen_store(void) {
  uint32_t new_dmr_id=0;
  uint32_t *dmr_id;
  wchar_t *bf;

#ifdef DEBUG
  printf("your enter: ");
  printhex2((char *) md380_menu_edit_buf,14);
  printf("\n");
#endif

  bf=md380_menu_edit_buf;
  while( *bf != 0) {
    new_dmr_id *= 10;
    new_dmr_id += (*bf++)-'0';
  }
#ifdef DEBUG
  printf("\n%d\n",new_dmr_id);
#endif
// store new dmr_id to ram and spi flash (codeplug)
  dmr_id = (uint32_t*) &md380_radio_config[4];   // form D002.32 @ 0x0803ef02
  *dmr_id=new_dmr_id;

  md380_spiflash_write(&new_dmr_id, 0x2084, 4);

  md380_menu_id    = md380_menu_id - 1;
  md380_menu_depth = md380_menu_depth - 1;
#ifdef CONFIG_MENU
  create_menu_entry_hook( md380_menu_id, md380_menu_edit_buf,    md380_menu_entry_back+1,  md380_menu_entry_back+1  ,6, 1 , 1);
#endif
}


uint32_t uli2w( uint32_t num, wchar_t *bf) {
  int n=0;
  unsigned int d=1;
  while (num/d >= 10)
    d*=10;
  while (d!=0) {
    int dgt = num / d;
    num%=d;
    d/=10;
    if (n || dgt>0|| d==0) {
      *bf++ = dgt+ '0';
      ++n;
    }
  }
  *bf=0;
  return (n); // number of char
}

void create_menu_entry_edit_dmr_id_screen(void) {
  struct MENU *menu_mem;
  uint8_t i;
  uint8_t *p;
  uint32_t nchars;
  uint32_t * dmr_id;

  md380_menu_0x2001d3c1 = md380_menu_0x200011e4;
  md380_menu_0x20001114 =  (uint32_t) md380_menu_edit_buf;



// clear retrun buffer //  see 0x08012a98
 for (i=0; i < 0x11; i++) {
   p=(uint8_t *) md380_menu_0x20001114;
   p = p + i;
   *p = 0;
   }

  dmr_id = (uint32_t *) &md380_radio_config[4];
  nchars=uli2w(*dmr_id, md380_menu_edit_buf);

#ifdef DEBUG
  printf("\ncreate_menu_entry_edit_dmr_id_screen %x %d \n", md380_menu_edit_buf, nchars);
  printhex2((char *) md380_menu_edit_buf ,14);
  printf("\n");
#endif

  md380_menu_0x2001d3ed = 8;      // max char
  md380_menu_0x2001d3ee = nchars; //  startpos cursor
  md380_menu_0x2001d3ef = nchars; //  startpos cursor
  md380_menu_0x2001d3f0 = 3;      // 3 = numerical input
  md380_menu_0x2001d3f1 = 0;
  md380_menu_0x2001d3f4 = 0;
  menu_mem = (void *)(md380_menu_memory + ((md380_menu_depth) * sizeof(struct MENU))) +  sizeof(struct MENU);
  menu_mem->menu_title = wt_edit_dmr_id;
  menu_mem->unknownp = 0x14 * md380_menu_id + md380_menu_mem_base;
  menu_mem->numberof_menu_entries=1;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;

#ifdef CONFIG_MENU
  create_menu_entry_hook( md380_menu_id, wt_edit_dmr_id,  create_menu_entry_edit_dmr_id_screen_store + 1 , md380_menu_numerical_input  + 1,  0x81, 0 , 1);
#endif
}



void create_menu_entry_addl_functions_screen(void) {
  struct MENU *menu_mem;
  int i;

#ifdef DEBUG
  printf("create_menu_entry_addl_functions_screen %d\n",md380_menu_depth);
#endif
  menu_mem = (void *)(md380_menu_memory + md380_menu_depth * sizeof(struct MENU) + sizeof(struct MENU));

  menu_mem->menu_title = wt_addl_func;
  menu_mem->unknownp = 0x14 * md380_menu_id + md380_menu_mem_base;
  menu_mem->numberof_menu_entries=9;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;

#ifdef CONFIG_MENU
  create_menu_entry_hook( md380_menu_id,     wt_rbeep,       create_menu_entry_rbeep_screen+1,
                          md380_menu_entry_back+1, 0x98, 0 , 1);
  create_menu_entry_hook( md380_menu_id + 1, wt_datef,       create_menu_entry_datef_screen+1,
                          md380_menu_entry_back+1, 0x98, 0 , 1);
  create_menu_entry_hook( md380_menu_id + 2, wt_userscsv,    create_menu_entry_userscsv_screen+1,
                          md380_menu_entry_back+1, 0x98, 0 , 1);
  create_menu_entry_hook( md380_menu_id + 3, wt_debug,       create_menu_entry_debug_screen+1,
                          md380_menu_entry_back+1, 0x98, 0 , 1);
  create_menu_entry_hook( md380_menu_id + 4, wt_promtg,      create_menu_entry_promtg_screen+1,
                          md380_menu_entry_back+1, 0x98, 0 , 1);
  create_menu_entry_hook( md380_menu_id + 5, wt_edit,        create_menu_entry_edit_screen+1,
                          md380_menu_entry_back+1, 0x8a, 0 , 0);  // disable this menu entry - no function jet
  create_menu_entry_hook( md380_menu_id + 6, wt_edit_dmr_id, create_menu_entry_edit_dmr_id_screen+1,
                          md380_menu_entry_back+1, 0x8a, 0 , 1);
  create_menu_entry_hook( md380_menu_id + 7, wt_micbargraph, create_menu_entry_micbargraph_screen+1,
                          md380_menu_entry_back+1, 0x98, 0 , 1);
  create_menu_entry_hook( md380_menu_id + 8, wt_experimental, create_menu_entry_experimental_screen+1,
                          md380_menu_entry_back+1, 0x8a, 0 , 1);
#endif

 for(i=0;i<9;i++) {  // not yet known ;)
   uint8_t *p;
   p =(uint8_t *) md380_menu_mem_base + ( md380_menu_id + i ) * 0x14;
   p[0x10] = 2;
 }
}


void create_menu_utilies_hook(void) {
  struct MENU *menu_mem;
  int enabled;

  if ( (md380_program_radio_unprohibited[4] & 0x4) == 0x4 ) {
    enabled=0;
  } else {
    enabled=1;
  }

#ifdef DEBUG
   printf("create_menu_utilies_hook %d\n",md380_menu_depth);
#endif

  menu_mem = (void *)(md380_menu_memory + md380_menu_depth * sizeof(struct MENU) + sizeof(struct MENU));
  menu_mem->unknownp = 0x14 * md380_menu_id + md380_menu_mem_base;
  menu_mem->numberof_menu_entries++;

#ifdef CONFIG_MENU
  create_menu_entry_hook(8, md380_wt_programradio, md380_menu_entry_programradio+1 ,           md380_menu_entry_back+1, 0x8a, 0 , enabled);

  if (menu_mem->numberof_menu_entries == 6 ) { // d13.020 has hidden gps entrys on this menu
    create_menu_entry_hook(11, wt_addl_func,          create_menu_entry_addl_functions_screen+1 , md380_menu_entry_back+1, 0x8a,0 , 1);
  } else {
    create_menu_entry_hook(9, wt_addl_func,          create_menu_entry_addl_functions_screen+1 , md380_menu_entry_back+1, 0x8a,0 , 1);
  }
#endif
}
