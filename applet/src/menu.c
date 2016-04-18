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




void F_249_Create_MenuEntry_hook(int a, void * b , void * c, void  * d, int e, int f ,int g) {
  printf("0x%x Text: 0x%x GreenKey 0x%x RedKey 0x%x 0x%x 0x%x 0x%x\n", a,b,c,d,e,f,g);
  printf("b: ");
  printhex2(b,14);
  printf("\n");
//  printf("%x \n", * (unsigned int*) 0x20019df0);
  F_249_Create_MenuEntry(a,b,c,d,e,f,g);
}

/*
static void do_jump(uint32_t entrypoint) {
  asm volatile(
               "bx     %0      \n"
                : : "r" (entrypoint) : );
  // just to keep noreturn happy
  for (;;) ;
}
*/
 
struct MENU {
  void    *menu_titel;
  void    *unknownp;
  uint8_t numberofentrys;
  uint8_t unknown_00;
  uint8_t unknown_01;
};
#define MENU_DEPTH  0x200011e4
#define UNKNOWN_01  0x2001d3c2
#define UNKNOWN_02  0x20019df0
#define MENU_MEM    0x2001c148
#define SELECTED 0x2001d3b2
                 

void Create_My_Menu_Entry_DebugEnableScreen(void) {
  uint8_t menu_depth;
  struct MENU *menu_mem; 
  static wchar_t wt_enable[40];
  static wchar_t wt_menu[40];
  const char t_enable[]="Enable";
  const char t_menu[]="Debug";
  int i;
  
                               
  for(i=0;i<strlen(t_menu);i++)
    wt_menu[i]=t_menu[i];
  wt_menu[i]='\0';   

  for(i=0;i<strlen(t_enable);i++)
    wt_enable[i]=t_enable[i];
  wt_enable[i]='\0';   

  menu_depth =  *(uint8_t *)MENU_DEPTH;  
  menu_mem = ((void *) MENU_MEM + ( menu_depth * 0xc) ) + 0xc;

  menu_mem->menu_titel = wt_menu;

 
  menu_mem->unknownp = 0x14 * (*(uint8_t *)UNKNOWN_01) + (void *)UNKNOWN_02;
  

  menu_mem->numberofentrys=1;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;

  F_249_Create_MenuEntry_hook( (*(char *) 0x2001d3c2), wt_enable, (void *) 0x800f453, (void *) 0x800f453, 6, 2 , 1);

}

void Create_My_Menu_Entry_DebugDisableScreen(void) {
  uint8_t menu_depth;
  struct MENU *menu_mem; 
  static wchar_t wt_disable[40];
  static wchar_t wt_menu[40];
  const char t_disable[]="Disable";
  const char t_menu[]="Debug";
     
  int i;
  
                               
  for(i=0;i<strlen(t_menu);i++)
    wt_menu[i]=t_menu[i];
  wt_menu[i]='\0';   
       
  for(i=0;i<strlen(t_disable);i++)
    wt_disable[i]=t_disable[i];
  wt_disable[i]='\0';   
       

  menu_depth =  *(uint8_t *)MENU_DEPTH;  
  menu_mem = ((void *) MENU_MEM + ( menu_depth * 0xc) ) + 0xc;

  menu_mem->menu_titel = wt_menu;

 
  menu_mem->unknownp = 0x14 * (*(uint8_t *)UNKNOWN_01) + (void *)UNKNOWN_02;
  

  menu_mem->numberofentrys=1;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;

  F_249_Create_MenuEntry_hook( (*(char *) 0x2001d3c2), wt_disable, (void *) 0x800f453, (void *) 0x800f453, 6, 2 , 1);

}

                 
void Create_My_Menu_Entry_Debug_Screen(void) {
  uint8_t menu_depth;
  struct MENU *menu_mem; 
  static wchar_t wt_enable[40];
  static wchar_t wt_disable[40];
  static wchar_t wt_menu[40];
  const char t_enable[]="Enable";
  const char t_disable[]="Disable";
  const char t_menu[]="Debug";
     
  int i;
  
                               
  for(i=0;i<strlen(t_menu);i++)
    wt_menu[i]=t_menu[i];
  wt_menu[i]='\0';   

  for(i=0;i<strlen(t_enable);i++)
    wt_enable[i]=t_enable[i];
  wt_enable[i]='\0';   
       
  for(i=0;i<strlen(t_disable);i++)
    wt_disable[i]=t_disable[i];
  wt_disable[i]='\0';   
       
       
                                       
  
/*
 / (fcn) Create_Menu_Entry_Intro_Screen 250
 |           0x08019734      e0b5           push {r5, r6, r7, lr}
*/

/* pull MENU_DEPTH
 |           0x08019736      dff80c08       ldr.w r0, [pc, 0x80c]       ; [0x8019f44:4]=0x200011e4
 |           0x0801973a      0078           ldrb r0, [r0]
*/
//  #define MENU_DEPTH  0x200011e4
  menu_depth =  *(uint8_t *)MENU_DEPTH;  

/* calculate pos of new menu memory   
 |           0x0801973c      0c21           movs r1, 0xc
 |           0x0801973e      dff80828       ldr.w r2, [pc, 0x808]       ; [0x8019f48:4]=0x2001c148
 |           0x08019742      01fb0020       mla r0, r1, r0, r2
 |           0x08019746      0c30           adds r0, 0xc
*/
//  #define MENU_MEM 0x2001c148
  menu_mem = ((void *) MENU_MEM + ( menu_depth * 0xc) ) + 0xc;

/*  Set Menu Title complicate ... no  idear why 
 |           0x08019748      3949           ldr r1, [pc, 0xe4]          ; [0x8019830:4]=0x2001d3e2
 |           0x0801974a      0978           ldrb r1, [r1]
 |           0x0801974c      0129           cmp r1, 1
 |       ,=< 0x0801974e      0ad1           bne 0x8019766
 |       |   0x08019750      dff87418       ldr.w r1, [pc, 0x874]       ; [0x8019fc8:4]=0x2001d1a0
 |       |   0x08019754      0968           ldr r1, [r1]
 |       |   0x08019756      dff87428       ldr.w r2, [pc, 0x874]       ; [0x8019fcc:4]=0x20000000
 |       |   0x0801975a      12eb8101       adds.w r1, r2, r1, lsl 2
 |       |   0x0801975e      d1f89c11       ldr.w r1, [r1, 0x19c]
 |       |   0x08019762      0160           str r1, [r0]
 |      ,==< 0x08019764      04e0           b 0x8019770
*/
  

/*  Set MenuTitle
 |      |`-> 0x08019766      dff86418       ldr.w r1, [pc, 0x864]       ; [0x8019fcc:4]=0x20000000
 |      |    0x0801976a      d1f89c11       ldr.w r1, [r1, 0x19c]
 |      |    0x0801976e      0160           str r1, [r0]
*/
//  menu_mem->menu_titel =  (void *) 0x080d1580;
  menu_mem->menu_titel = wt_menu;

/* Set MenuExecFunction 
 |      `--> 0x08019770      dff8d013       ldr.w r1, [pc, 0x3d0]       ; [0x8019b44:4]=0x2001d3c2
 |           0x08019774      0978           ldrb r1, [r1]
 |           0x08019776      1422           movs r2, 0x14
 |           0x08019778      dff8743d       ldr.w r3, [pc, 0xd74]       ; [0x801a4f0:4]=0x20019df0
 |           0x0801977c      02fb0131       mla r1, r2, r1, r3
 |           0x08019780      4160           str r1, [r0, 4]
*/
// #define UNKNOWN_01 0x2001d3c2
// #define UNKNOWN_02 0x20019df0
 
 menu_mem->unknownp = 0x14 * (*(uint8_t *)UNKNOWN_01) + (void *)UNKNOWN_02;
  

/* Number of Menu Entrys
 |           0x08019782      0221           movs r1, 2
 |           0x08019784      0172           strb r1, [r0, 8]
*/
  menu_mem->numberofentrys=2;
/*
 |           0x08019786      0021           movs r1, 0
 |           0x08019788      4172           strb r1, [r0, 9]
*/
  menu_mem->unknown_00 = 0;
/*
 |           0x0801978a      0021           movs r1, 0
 |           0x0801978c      4181           strh r1, [r0, 0xa]
*/
  menu_mem->unknown_01 = 0;

/*  Preset radiobutton with akt. setting
 |           0x0801978e      dff8b803       ldr.w r0, [pc, 0x3b8]       ; [0x8019b48:4]=0x2001c658 ConfigData ; config_byte_LED_enable_and_more
 |           0x08019792      8078           ldrb r0, [r0, 2]            ; Get ConfigData + 0x2 F_5111
 |           0x08019794      c0f30010       ubfx r0, r0, 4, 1
 |           0x08019798      c007           lsls r0, r0, 0x1f
 |       ,=< 0x0801979a      04d4           bmi 0x80197a6
 |       |   0x0801979c      dff83009       ldr.w r0, [pc, 0x930]       ; [0x801a0d0:4]=0x2001d3b2
 |       |   0x080197a0      0121           movs r1, 1
 |       |   0x080197a2      0170           strb r1, [r0]
 |      ,==< 0x080197a4      03e0           b 0x80197ae
 |      |`-> 0x080197a6      dff82809       ldr.w r0, [pc, 0x928]       ; [0x801a0d0:4]=0x2001d3b2
 |      |    0x080197aa      0021           movs r1, 0
 |      |    0x080197ac      0170           strb r1, [r0]
*/
  *(uint8_t *) SELECTED = 0;
  

/* Add first line to Menu
 |      `--> 0x080197ae      0120           movs r0, 1
 |           0x080197b0      0290           str r0, [sp, 8]             ; 1 to SP,8
 |           0x080197b2      0020           movs r0, 0
 |           0x080197b4      0190           str r0, [sp, 4]             ; 0 to SP,4
 |           0x080197b6      8b20           movs r0, 0x8b
 |           0x080197b8      0090           str r0, [sp]                ; 0x8b to SP
 |           0x080197ba      dff81839       ldr.w r3, [pc, 0x918]       ; [0x801a0d4:4]=0x800f453 ; 0x800f453 .. 0x800f452 F_5143()  to r3 - Back
 |           0x080197be      dff81829       ldr.w r2, [pc, 0x918]       ; [0x801a0d8:4]=0x8019835 ; 0x8019834 .. SetConfig0x02Bit4andmore(),r2
 |           0x080197c2      dff80408       ldr.w r0, [pc, 0x804]       ; [0x8019fc8:4]=0x2001d1a0
 |           0x080197c6      0068           ldr r0, [r0]
 |           0x080197c8      dff80018       ldr.w r1, [pc, 0x800]       ; [0x8019fcc:4]=0x20000000
 |           0x080197cc      11eb8000       adds.w r0, r1, r0, lsl 2
 |           0x080197d0      d0f8b013       ldr.w r1, [r0, 0x3b0]       ; [((0x2001d1a0 lsl 2) +[0x20000000])+0x3b0] to r1 .. [0x200003b0] = 0x080fa348 .. [0x080fa348] = "P.i.c.t.u.r.e"
                                            0x080fa348
 |           0x080197d4      db48           ldr r0, [pc, 0x36c]         ; [0x8019b44:4]=0x2001d3c2
 |           0x080197d6      0078           ldrb r0, [r0]               ; (byte)[0x2001d3c2] to r0 .. [0x2001d3c2] = 6 ???
 |           0x080197d8      f2f7aaff       bl F_249_Create_MenuEntry
*/
//  F_249_Create_MenuEntry_hook( (*(char *) 0x2001d3c2), (void *) 0x080fa348, (void *) 0x8019835, (void *) 0x800f453, 0x8b, 0 , 1);
  F_249_Create_MenuEntry_hook( (*(char *) 0x2001d3c2), wt_enable, Create_My_Menu_Entry_DebugEnableScreen + 1 /*(void *) 0x8019835*/, (void *) 0x800f453, 0x8b, 0 , 1);
  
/* Add second line to Menu
 |           0x080197dc      0120           movs r0, 1                  ; 1 to SP,8
 |           0x080197de      0290           str r0, [sp, 8]
 |           0x080197e0      0020           movs r0, 0
 |           0x080197e2      0190           str r0, [sp, 4]             ; 0 to SP,4
 |           0x080197e4      8b20           movs r0, 0x8b
 |           0x080197e6      0090           str r0, [sp]                ; 0x8b to SP
 |           0x080197e8      dff8e838       ldr.w r3, [pc, 0x8e8]       ; [0x801a0d4:4]=0x800f453 ; 0x800f453 .. 0x800f452 F_5143() to r3 - Back
 |           0x080197ec      dff8ec28       ldr.w r2, [pc, 0x8ec]       ; [0x801a0dc:4]=0x80198c1 ; 0x80198c0 .. UnsetConfig0x02Bit4andmore(),r2
 |           0x080197f0      dff8d407       ldr.w r0, [pc, 0x7d4]       ; [0x8019fc8:4]=0x2001d1a0
 |           0x080197f4      0068           ldr r0, [r0]
 |           0x080197f6      dff8d417       ldr.w r1, [pc, 0x7d4]       ; [0x8019fcc:4]=0x20000000
 |           0x080197fa      11eb8000       adds.w r0, r1, r0, lsl 2
 |           0x080197fe      d0f8b413       ldr.w r1, [r0, 0x3b4]       ; [((0x2001d1a0 lsl 2) +[0x20000000])+0x3b4] to r1 .. [0x200003b4] = 0x080d1f48 .. [0x080d1f48] ="C.h.a.r...S.t.r.i.n.g"
                                            0x080d1f48.
 |           0x08019802      d048           ldr r0, [pc, 0x340]         ; [0x8019b44:4]=0x2001d3c2
 |           0x08019804      0078           ldrb r0, [r0]
 |           0x08019806      401c           adds r0, r0, 1
 |           0x08019808      c0b2           uxtb r0, r0                 ; (byte)[0x2001d3c2]+1 to r0 .. [0x2001d3c2]+1 = 7 ???
 |           0x0801980a      f2f791ff       bl F_249_Create_MenuEntry
*/
//  F_249_Create_MenuEntry_hook( (*(char *) 0x2001d3c2) + 1, (void * ) 0x080d1f48 , (void *) 0x80198c1 , (void *) 0x800f453 , 0x8b, 0 , 1);
//  F_249_Create_MenuEntry_hook( (*(char *) 0x2001d3c2) + 1, wt_disable, (void *) 0x80198c1 , (void *) 0x800f453 , 0x8b, 0 , 1);
  F_249_Create_MenuEntry_hook( (*(char *) 0x2001d3c2) + 1, wt_disable, Create_My_Menu_Entry_DebugDisableScreen + 1 , (void *) 0x800f453 , 0x8b, 0 , 1);

/*
 |           0x0801980e      0020           movs r0, 0
 |       ,=< 0x08019810      0ae0           b 0x8019828
 |      .--> 0x08019812      cc49           ldr r1, [pc, 0x330]         ; [0x8019b44:4]=0x2001d3c2 ... sw menu_depth
 |      ||   0x08019814      0978           ldrb r1, [r1]
 |      ||   0x08019816      4118           adds r1, r0, r1
 |      ||   0x08019818      1422           movs r2, 0x14
 |      ||   0x0801981a      dff8d43c       ldr.w r3, [pc, 0xcd4]       ; [0x801a4f0:4]=0x20019df0  ... global menu
 |      ||   0x0801981e      02fb0131       mla r1, r2, r1, r3
 |      ||   0x08019822      0022           movs r2, 0
 |      ||   0x08019824      0a74           strb r2, [r1, 0x10]
 |      ||   0x08019826      401c           adds r0, r0, 1
 |      |`-> 0x08019828      0228           cmp r0, 2                    ; number of menu entrys
 |      `==< 0x0801982a      f2db           blt 0x8019812
 \           0x0801982c      07bd           pop {r0, r1, r2, pc}
*/  
 // #define UNKNOWN_01 0x2001d3c2
// #define UNKNOWN_02 0x20019df0
 
 for(i=0;i<2;i++) {
   uint8_t n;
   uint8_t *p;
   
   n = *(uint8_t*)UNKNOWN_01;
   n = n + i;
   p = (void *)UNKNOWN_02;
   p = p + n * 0x14;
   p[0x10]=0;
 } 
}

void CreateMenuEntryAddlFunctionsScreen(void) {
  uint8_t menu_depth;
  struct MENU *menu_mem; 
  static wchar_t wt_rbeep[40];
  static wchar_t wt_datef[40];
  static wchar_t wt_userscsv[40];
  static wchar_t wt_debug[40];
  static wchar_t wt_menu[40];
  const char t_rbeep[]="RogerBeep";
  const char t_datef[]="Date format";
  const char t_userscsv[]="UsersCSV";
  const char t_debug[]="Debug";
  const char t_menu[]="Addl. Funct";

  static wchar_t wt_menu1[]=L"Addl. Funct";
     
  int i;
                                 
  for(i=0;i<strlen(t_menu);i++)
    wt_menu[i]=t_menu[i];
  wt_menu[i]='\0';   

  for(i=0;i<strlen(t_rbeep);i++)
    wt_rbeep[i]=t_rbeep[i];
  wt_rbeep[i]='\0';   
       
  for(i=0;i<strlen(t_datef);i++)
    wt_datef[i]=t_datef[i];
  wt_datef[i]='\0';   
    
  for(i=0;i<strlen(t_userscsv);i++)
    wt_userscsv[i]=t_userscsv[i];
  wt_userscsv[i]='\0';   
  
  for(i=0;i<strlen(t_debug);i++)
    wt_debug[i]=t_debug[i];
  wt_debug[i]='\0';   
  
  
       
  menu_depth =  *(uint8_t *)MENU_DEPTH;  
  menu_mem = ((void *) MENU_MEM + ( menu_depth * 0xc) ) + 0xc;
  menu_mem->menu_titel = wt_menu1;

  menu_mem->unknownp = 0x14 * (*(uint8_t *)UNKNOWN_01) + (void *)UNKNOWN_02;
  
  menu_mem->numberofentrys=4;
  menu_mem->unknown_00 = 0;
  menu_mem->unknown_01 = 0;

  F_249_Create_MenuEntry_hook( (*(char *) 0x2001d3c2),     wt_rbeep,    Create_My_Menu_Entry_Debug_Screen + 1 , (void *) 0x800f453, 0x98, 0 , 1);
  F_249_Create_MenuEntry_hook( (*(char *) 0x2001d3c2) + 1, wt_datef,    Create_My_Menu_Entry_Debug_Screen + 1 , (void *) 0x800f453, 0x98, 0 , 1);
  F_249_Create_MenuEntry_hook( (*(char *) 0x2001d3c2) + 2, wt_userscsv, Create_My_Menu_Entry_Debug_Screen + 1 , (void *) 0x800f453, 0x98, 0 , 1);
  F_249_Create_MenuEntry_hook( (*(char *) 0x2001d3c2) + 3, wt_debug,    Create_My_Menu_Entry_Debug_Screen + 1 , (void *) 0x800f453, 0x98, 0 , 1);
  
 
 for(i=0;i<4;i++) {
   uint8_t n;
   uint8_t *p;
   n = *(uint8_t*)UNKNOWN_01;
   n = n + i;
   p = (void *)UNKNOWN_02;
   p = p + n * 0x14;
   p[0x10]=2;
 }
}
                                                                        

void Create_Menu_Utilies_hook(void) {

    uint8_t * program_radio_unprohibited = (uint8_t *) (0x2001d030 + 4);
    uint32_t menu_enabled;   
    uint32_t menu_unknown_0; 
    uint32_t menu_unknown_1; 
    uint32_t menu_entry_nr;  
    void *menu_green_key, *menu_red_key,*menu_text;
    static wchar_t wide[40];
    char my_entry[]="Addl. Functions";
    int i;  
                                                     
  
  // 0x08012740      dff8f80a       ldr.w r0, [pc, 0xaf8]       ; [0x801323c:4]=0x2001d030
  // 0x08012744      0079           ldrb r0, [r0, 4]   
  // 0x08012746      c0f38000       ubfx r0, r0, 2, 1 
  // 0x0801274a      c007           lsls r0, r0, 0x1f
  // 0x0801274c      02d4           bmi 0x8012754               ;[2] ; Is Menu ProgramRadio allowedly  
  if ( (* program_radio_unprohibited & 0x4) == 0x4 ) {
    printf("program_radio_unprohibited\n");  
  } else {
    printf("program_radio_prohibited\n");
  }
  // 0x0801274e      54f00104       orrs r4, r4, 1                                                                 
  // 0x08012752      00e0           b 0x8012756                 ;[3]                                               
  // 0x08012754      0024           movs r4, 0                                                                     
  // 0x08012756      e4b2           uxtb r4, r4                                                                    
  // 0x08012758      a4b2           uxth r4, r4       
  // 0x0801275a      0294           str r4, [sp, 8]     

  // added the original "programm radio" menu entry... lost by the hook
  menu_enabled=1;
  menu_unknown_0=0;
  menu_unknown_1=0x8a;
  menu_green_key=(void *)0x80127d1;
  menu_red_key=(void *)0x800f453; //back
  menu_text=(void *)0x080d175c;
  menu_entry_nr=8;
  F_249_Create_MenuEntry_hook(menu_entry_nr, menu_text , menu_green_key , menu_red_key, menu_unknown_1, menu_unknown_0 , menu_enabled);

  // added the "debug" menu entry  
  for(i=0;i<strlen(my_entry);i++)
    wide[i]=my_entry[i];

  wide[i]='\0';   
  menu_enabled=1;
  menu_unknown_0=0;
  menu_unknown_1= 0x8a;
  menu_green_key= CreateMenuEntryAddlFunctionsScreen +1;
  menu_red_key=(void *) 0x800f453; //back

  menu_entry_nr=9;
  F_249_Create_MenuEntry_hook(menu_entry_nr, wide , menu_green_key , menu_red_key, menu_unknown_1, menu_unknown_0 , menu_enabled);
}  
  
/*
void Create_Menu_Utilies_hook_end(void) {
  do_jump(0x08012786+1);
}
*/

