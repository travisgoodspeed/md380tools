// File:    md380tools/applet/src/app_menu.h
// Authors: Wolf (DL4YHF) [initial version], .. (?) 
// Date:    2017-03-31 
//  Implements a simple menu opened with the red BACK button,
//             along with all required low-level graphic functions.
//  Module prefix 'am_' for "application menu" or "alternative menu" .
//  Added 2017-03-31 for the Morse output for visually impaired hams,
//  because strings in 'our own' menu can be sent out much easier
//  than with the original menu by Tytera (opened via green 'MENU' button).
// 

#ifndef  CONFIG_APP_MENU   // Alternative menu activated by red 'BACK'-button ?
# define CONFIG_APP_MENU 0 // only if defined > 0 in config.h
#endif
#ifndef  BOOL
# define BOOL int // embedded C compilers don't have "stdbool.h", thus no 'bool'
#endif
#ifndef  TRUE     // while we're at it.. the old-fashioned BOOLEAN stuff
# define TRUE 1
#endif
#ifndef  FALSE
# define FALSE 0
#endif

//---------------------------------------------------------------------------
// Defines for the alternative 'App Menu' :

// Result codes for app menu callback functions :
#define AM_RESULT_NONE  0 // "proceed as if there was NO callback function"
#define AM_RESULT_OK    1 // general 'ok' (callback function 'did something').
#define AM_RESULT_ERROR 2 // general error
#define AM_RESULT_INVISIBLE 3 // special return value for APPMENU_EVT_CHECK_VISIBILITY
#define AM_RESULT_OCCUPY_SCREEN 4 // for APPMENU_EVT_ENTER and APPMENU_EVT_PAINT:
        // "the whatever-it-is (callback) now owns the screen, and doesn't want
        //  anyone else to paint into the framebuffer" .
        // Instead of the default paint procedure, Menu_DrawIfVisible()
        //  will repeatedly invoke the callback with event=APPMENU_EVT_PAINT,
        //  as long as the callback function returns AM_RESULT_OCCUPY_SCREEN .
        // Simple example in RedMenuCbk_ColorTest() .


  // flags to mark focused lines or individual characters ("edit cursor") in the menu:
#define SEL_FLAG_NONE     0 // normal output attributes, neither focused nor editing
#define SEL_FLAG_FOCUSED  1
#define SEL_FLAG_CURSOR   2


// Data types and structs ...
// For shortest code, put everything inside SMALL structs, and reference it
// via a pointers stored in LOCAL variables (faster to access than globals).

#if( CONFIG_APP_MENU )

typedef struct tRMStringTable
{ int   value;   // numeric value to translate into (or from) a display string
  char *pszText; // text to be shown instead of a number. NULL=end of list.
                 // Length of the strings shouldn't exceed 20 chars (display).
} am_stringtable_t; 

typedef struct tAppMenuItem
{ // Describes a single menu item ("line"). Located in Flash (ROM),
  // thus cannot contain VARIABLES but only POINTERS to variables.
  // Unlike Tytera's own menu, we can't occupy much space in RAM !

  const char *pszText; // fixed text (shall also be located in Flash, not RAM).
                 // Ideally 160/12 = 13 characters maximum length,
                 // plus some extra characters for hotkeys and headlines.

  uint8_t data_type; // type of pvValue, one of the following ("inspired by CANopen") :
# define DTYPE_NONE    0  
# define DTYPE_BOOL    1  // not used yet, may be used for "on"/"off" one day (with bits in a BYTE)
# define DTYPE_INT8    2  
# define DTYPE_INT16   3  
# define DTYPE_INTEGER 4  // "a normal integer" is 32 bit here  
# define DTYPE_UNS8    5  // pvValue points to a 'uint8_t', formerly known as 'BYTE'
# define DTYPE_UNS16   6  
# define DTYPE_UNS32   7
# define DTYPE_FLOAT   8  // not sure if we ever need this, but 'float' is code 8
# define DTYPE_STRING  9  // good old "C"-string with 8 bits per character
# define DTYPE_WSTRING 10 // wasteful "wide" string with 16 bits per character
# define DTYPE_SUBMENU 11 // pvValue points to an array of SUB-MENUs

  uint8_t options;   // bitwise combination of the options below:
# define APPMENU_OPT_NONE       0 // default display format, parameter not editable
# define APPMENU_OPT_EDITABLE   1 // "the value shown here is EDITABLE" 
# define APPMENU_OPT_FACTOR     2 // multiply the value (from *pvValue) by 'opt_value' for the display
# define APPMENU_OPT_BITMASK    4 // extract a group of bits from *pvValue, specified in 'opt_value'
# define APPMENU_OPT_BITMASK_R  8 // similar as APPMENU_OPT_BITMASK, with right-aligned bits for the display
# define APPMENU_OPT_STEPWIDTH 16 // use opt_value as stepwidth when incrementing/decrementing in edit mode
# define APPMENU_OPT_RESERVE1  64
# define APPMENU_OPT_RESERVE2 128
# define APPMENU_OPT_BACK     255 // "back to the parent menu or the main screen"
      // Note: Rare display formats don't occupy bits in menu_item_t.options .
      // They are specified in pszText, see app_menu.c : Menu_DrawLineWithItem().

  int opt_value; // factor for display, bitmask, or stepwidth when inc/decrementing

  void *pvValue; // optional pointer to a 'value' (variable). NULL for fixed strings.
  int  minValue; // min value accepted for inc/dec or numeric input
  int  maxValue; // max value accepted for inc/dec or numeric input .

  const am_stringtable_t *pStringTable; // optional address of a value/string table (NULL:none)

  // Address of a menu-item-specific callback. Because some items may
  // share the same callback (especially array-like items), the address
  // of the menu_item_t is passed to the callback. Thus the callback-
  // function can also access the item's "menu id", to simplify things. 
  int (*callback)(struct tAppMenuItem *pMenuItem, int event, int param );
  // Possible values for 'event' in the above callback:
# define APPMENU_EVT_CHECK_VISIBILITY 0 // result: AM_RESULT_OK or AM_RESULT_INVISIBLE
# define APPMENU_EVT_PAINT  0 // this menu item is just about to be 'painted'.
                            // Can be used to update 'dynamic content', etc.
# define APPMENU_EVT_ENTER  1 // operator has pressed ENTER while this item was focused.
         // Often used to prepare items shown in a SUBMENU.
         // Callback may return AM_RESULT_ERROR to prevent 'entering'
         // or being edited (even if menu_item_t.options contains APPMENU_OPT_EDITABLE) 
# define APPMENU_EVT_EXIT   2 // sent to focused item when pressing EXIT ("Back")
# define APPMENU_EVT_EDIT   3 // sent when beginning to edit, and while editing
# define APPMENU_EVT_UNDO   4 // sent when operator wants to 'abort' editing
# define APPMENU_EVT_KEY    5 // keyboard event (only sent to callbacks that occupied the screen)
} menu_item_t;

typedef struct tRedMenu // instance data (in RAM, not Flash)
{ uint8_t visible;      // 0=no, 1=menu visible, 2="screen occupied by painting callback":
#         define APPMENU_OFF     0
#         define APPMENU_VISIBLE 1
#         define APPMENU_USERSCREEN_VISIBLE 2
#         define APPMENU_VISIBLE_UNTIL_KEY_RELEASED 3
  uint8_t redraw;
  uint8_t depth;        // current 'depth' into the menu, 0 = top level,
                        // also acts like a 'stack pointer' into 
  uint8_t vert_scroll_pos; // index into pItems[] of the topmost visible entry
  uint8_t item_index;   // zero-based index of the currently focused item
  uint8_t num_items;    // number of items that pItems (below) points to
  uint8_t n_items_visible; // height of the menu-display, number of TEXT lines
  uint8_t edit_mode;    // one of the following (switched by repeatedly pressing ENTER):
#         define APPMENU_EDIT_OFF     0
#         define APPMENU_EDIT_INC_DEC 1 // whole field selected, increment/decrement value via cursor keys
#         define APPMENU_EDIT_OVERWRT 2 // only one digit selected ('cursor'), digit from keyboard OVERWRITES
#         define APPMENU_EDIT_INSERT  3 // similar, but digit from keyboard is INSERTED into the string
  uint8_t edit_length;  // current length of the VALUE when *editing*
  uint8_t cursor_pos;   // edit digit position, 0..edit_length-1
  int  iEditValue; // "offline" value, used while editing, written back when done.
           // If an modifyable parameter uses a STRING TABLE for display,
           // iEditValue is an index into that table, but not directlyy
           // visible on the screen.
  char sz40EditBuf[41]; // also used "offline" while editing, for direct input
           // via keyboard (or even Morse code ?), because this is easier than
           // adding or subtracting powers of 2, 10, or 16 to iEditValue ...
  int     value_chksum; // crude 'Fletcher' checksum over all currently visible 
           // values (strings and numbers) to check if a screen update is necessary.
           // (If the cable between CPU and display didn't emit QRM, we'd simply
           //  redraw the screen periodically. But try this on an FM channel
           //  with a rubber-duck antenna: "buzzz, buzzz, pfft, pfft, pfft" ! )
           
  // Anything that doesn't necessarily need to be in RAM should be in Flash (ROM).
  // Here's a pointer to the currently active items:
  menu_item_t *pItems;
  
  // Before entering a SUB-menu, num_items, item_index, and pItems are stacked here:
# define APPMENU_STACKSIZE 4 // ~~maximum nesting level
  struct
   { menu_item_t *pItems;
     uint8_t item_index;
     uint8_t vert_scroll_pos;
   } submenu_stack[APPMENU_STACKSIZE];

} app_menu_t;

typedef int (*am_callback_t)(menu_item_t *pItem, int event, int param );

// Helper functions to deal with those bloody 'wide' strings, etc..
int wide_to_C_string( wchar_t *wide_string, char *c_string, int maxlen );
int my_wcslen( wchar_t *wide_string ); // kludge because there was no wcslen() 


// application-menu "API" (and interface to the keyboard handler, etc)
 
void Menu_OnKey( uint8_t key); // called on keypress from some interrupt handler
int  Menu_IsVisible(void);     // 1=currently visible (open), 0=not open; don't intercept keys
int  Menu_DrawIfVisible(int caller); // Paints the 'red button menu' 
   // into the framebuffer. Must only be called from a DISPLAY task !
   // Returns 0 when invisible, 1 when visible (used in various hooks).
   // For debugging, the "caller" is passed in as an argument:
#  define AM_CALLER_STATUSLINE_HOOK     1
#  define AM_CALLER_DATETIME_HOOK       2
#  define AM_CALLER_F_4225_HOOK         3
#  define AM_CALLER_F_4315_HOOK         4
#  define AM_CALLER_RX_SCREEN_BLUE_HOOK 5
#  define AM_CALLER_RTC_TIMER           6

char *Menu_FindInStringTable( const am_stringtable_t *pTable, int value);



#endif // CONFIG_APP_MENU ?
