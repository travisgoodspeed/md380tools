// File:    md380tools/applet/src/app_menu.h
// Authors: Wolf (DL4YHF) [initial version], .. (?) 
// Date:    2017-04-23 
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
#define AM_RESULT_OCCUPY_SCREEN 16 // for APPMENU_EVT_ENTER and APPMENU_EVT_PAINT:
        // "the callback now owns the screen, no-one else should paint anything".
        // Instead of the default paint procedure, Menu_DrawIfVisible()
        //  will repeatedly invoke the callback with event=APPMENU_EVT_PAINT,
        //  as long as the callback function returns AM_RESULT_OCCUPY_SCREEN .
        // Simple example in am_cbk_ColorTest() .
#define AM_RESULT_EXIT_AND_RELEASE_SCREEN 17 // returned by menu callback
        // to release the 'occupied' screen, and return control to the menu.
        // Simple example in am_cbk_ColorPicker() .


  // flags to mark focused lines or individual characters ("edit cursor") in the menu:
#define SEL_FLAG_NONE     0 // normal output attributes, neither focused nor editing
#define SEL_FLAG_FOCUSED  1 // momentarily focused item, steerable "navigation bar", etc
#define SEL_FLAG_CURSOR   2 // edit cursor or (in "inc/dec"-editing mode) edit field
#define SEL_FLAG_CURRENT  2 // same combination of colours for the currently
                            // active whatever-it-is, e.g. currently used ZONE.

// Data types and structs ...
// For shortest code, put everything inside SMALL structs, and reference it
// via a pointers stored in LOCAL variables (faster to access than globals).

#if( CONFIG_APP_MENU )

typedef struct tScrollListControl
{
  int num_items;    // total number of entries in the list (e.g. number of zones).
                    // If num_items < n_visible_items, don't need to scroll
  int n_visible_items; // number of visible items (depends on the font, etc;
      // usually detected when painting a scrollable list for the first time)
  int current_item; // zero-based index of the currently active item (e.g. zone)
  int focused_item; // array index of the focused item ("navigation bar")
  int scroll_pos;   // vertical scrolling position
  // To save precious space in RAM, the names (strings) are NOT stored here,
  // but (e.g. for the zone list) read from SPI flash when required for drawing.
  // Tytera's original firmware does the same, so speed is not an issue.
} scroll_list_control_t;


typedef struct tRMStringTable
{ int   value;   // numeric value to translate into (or from) a display string
  char *pszText; // text to be shown instead of a number. NULL=end of list.
                 // Length of the strings shouldn't exceed 20 chars (display).
} am_stringtable_t; 


typedef struct tAppMenu // instance data (in RAM, not Flash)
{ uint8_t visible;      // 0=no, 1=menu visible, 2="screen occupied by painting callback":
#         define APPMENU_OFF     0
#         define APPMENU_VISIBLE 1
#         define APPMENU_USERSCREEN_VISIBLE 2
#         define APPMENU_VISIBLE_UNTIL_KEY_RELEASED 3
  uint8_t redraw;       // flag is a (full-screen) redraw is necessary: 0=no, 1=yes (immediately)
  uint8_t depth;        // current 'depth' into the menu, 0 = top level,
                        // also acts like a 'stack pointer' into 
  uint8_t vert_scroll_pos; // index into pItems[] of the topmost visible entry
  uint8_t item_index;   // zero-based index of the currently focused item
  uint8_t num_items;    // number of items that pItems (below) points to
  uint8_t n_items_visible; // height of the menu-display, number of TEXT lines
  uint8_t edit_mode;    // one of the following (switched by repeatedly pressing ENTER):
#         define APPMENU_EDIT_OFF     0 // not EDITING but SHOWING the parameter
#         define APPMENU_EDIT_INC_DEC 1 // whole field selected, increment/decrement value via cursor keys
#         define APPMENU_EDIT_OVERWRT 2 // only one digit selected ('cursor'), digit from keyboard OVERWRITES
#         define APPMENU_EDIT_INSERT  3 // similar, but digit from keyboard is INSERTED into the string
  int8_t new_edit_mode; // when non-negative, "request to switch to a different edit mode",
           // may be set in any thread, polled and reset (to -1) in Menu_DrawIfVisible() when done.
  uint8_t edit_length;  // current length of the VALUE when *editing*
  uint8_t cursor_pos;   // edit digit position, 0..edit_length-1
  uint8_t save_on_exit; // flag set if any member of global_addl_config was edited
           // (only save ONCE on exit, don't unnecessarily stress the Flash.
           //  Also, if the user thinks he made a mistake, he can turn off
           //  the radio without leaving the menu, to avoid SAVING the config)
  int  iEditValue; // "offline" value, used while editing, written back when done.
           // If an modifyable parameter uses a STRING TABLE for display,
           // iEditValue is an index into that table, but not directly
           // visible on the screen.
  int  iMinValue; // min/max-range copied from menu_item_t, but may be modified...
  int  iMaxValue; // ... via callback shortly before beginning to EDIT the value.
           //
  int  iValueBeforeEditing; // original value before editing; required for 'undo'
           //
  char sz40EditBuf[44]; // also used "offline" while editing, for direct input
           // via keyboard (or even Morse code ?), because this is easier than
           // adding or subtracting powers of 2, 10, or 16 to iEditValue ...
  int  value_chksum; // checksum (CRC16) over all currently visible 
           // values (strings and numbers) to check if a screen update is necessary.
           // (If the cable between CPU and display didn't emit QRM, we'd simply
           //  redraw the screen periodically. But try this on an FM channel
           //  with a rubber-duck antenna: "buzzz, buzzz, pfft, pfft, pfft" ! )
  int  dialog_field_index;  // general storage for dialogs and similar gadgets,
           //  first used in color_picker.c to select RED, GREEN or BLUE component
  uint32_t stopwatch;       // stopwatch (timer) to limit the screen-update-rate / QRM reduction
  uint32_t stopwatch_late_redraw;    // stopwatch for a 'delayed' full-screen update (see amenu_codeplug.c)
  scroll_list_control_t scroll_list; // control data for a scrollable list, e.g. Zones
  char sz40MorseTextFromFocusedLine[44]; // plain text for Morse output from the currently focused line
         
  // Anything that doesn't need to be in RAM should be in Flash (ROM).
  // Here's a pointer to the currently active items:
  // The following should be: menu_item_t         *pItems; 
  //             or at least: struct tAppMenuItem *pItems;
  // but we need  app_menu_t  *before* menu_item_t because the callback function
  // shall contain pointers to BOTH struct-types in the argument list. So:
  void *pItems; // use this ugly "anoymous pointer" even though it's always a menu_item_t* !

#if( CONFIG_MORSE_OUTPUT )
  uint32_t morse_stopwatch; // stopwatch for delayed Morse output while editing, etc
  uint8_t  morse_request;   // bitwise combination of the following:
#         define AMENU_MORSE_REQUEST_NONE       0x00
#         define AMENU_MORSE_REQUEST_ITEM_TEXT  0x01
#         define AMENU_MORSE_REQUEST_ITEM_VALUE 0x02
#endif 

} app_menu_t;


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
# define APPMENU_OPT_IMM_UPDATE 2 // "WHEN editing, immediately update *pvValue"
# define APPMENU_OPT_FACTOR     4 // multiply the value (from *pvValue) by 'opt_value' for the display
# define APPMENU_OPT_BITMASK    8 // extract a group of bits from *pvValue, specified in 'opt_value'
# define APPMENU_OPT_BITMASK_R 16 // similar as APPMENU_OPT_BITMASK, with right-aligned bits for the display
# define APPMENU_OPT_STEPWIDTH 32 // use opt_value as stepwidth when incrementing/decrementing in edit mode
# define APPMENU_OPT_RESERVE1  64
# define APPMENU_OPT_RESERVE2 128
# define APPMENU_OPT_BACK     255 // "back to the parent menu or the main screen"
      // Note: Rare display formats don't occupy bits in menu_item_t.options .
      // They are specified in pszText, see app_menu.c : Menu_DrawLineWithItem().

  int opt_value; // factor for display, bitmask, or stepwidth when inc/decrementing

  void *pvValue; // optional pointer to a 'value' (variable). NULL if there's none.
  int iMinValue; // min value accepted for inc/dec or numeric input
  int iMaxValue; // max value accepted for inc/dec or numeric input .

  const am_stringtable_t *pStringTable; // optional address of a value/string table (NULL:none)

  // Address of a menu-item-specific callback. Because some items may
  // share the same callback (especially array-like items), the address
  // of the menu_item_t is passed to the callback. Thus the callback-
  // function can access anything in the menu *and* the item, to simplify things. 
  // For simple menu items (even with a value, referenced via pointer),
  // it's not necessary to implement the following callback function at all.
  int (*callback)(app_menu_t *pMenu, struct tAppMenuItem *pMenuItem, int event, int param );
  // Possible values for 'event' in the above callback:
# define APPMENU_EVT_PAINT      0 // this menu item is just about to be 'painted'.
         // Can be used to update 'dynamic content', etc, or for custom drawing.
         // If the callback function paints the screen itself ("custom screen"),
         // it must return AM_RESULT_OCCUPY_SCREEN on each APPMENU_EVT_PAINT,
         // as long as it wants to 'remain visible' . Example: color_picker.c .
# define APPMENU_EVT_GET_VALUE  1 // let the callback function provide the 'display' value.
         // ONLY CALLED IF menu_item_t.pvValue is NULL, and only for numeric displays !
         // The callback returns the to-be-displayed value instead of a result code.
         //
# define APPMENU_EVT_ENTER      2 // operator has pressed ENTER while this item was focused.
         // Often used to prepare items shown in a SUBMENU or extra dialog.
         // Callback may return AM_RESULT_ERROR to prevent 'entering'
         // or being edited (even if menu_item_t.options contains APPMENU_OPT_EDITABLE) 
# define APPMENU_EVT_EXIT       3 // sent to focused item when pressing EXIT ("Back")
# define APPMENU_EVT_BEGIN_EDIT 4 // beginning to edit (allows to modify min/max range for editing, etc)
# define APPMENU_EVT_EDIT       5 // sent WHILE editing (after each edit value modification)
# define APPMENU_EVT_END_EDIT   6 // stopped editing (just an "info", there's no way to intercept "end of editing")
         // "END_EDIT" comes in two flavours : 
         // with param = 1, it means "finished, write back the result", e.g. pMenu->iEditValue
         // with param = 0, it means "aborted, discard whatever was entered".
# define APPMENU_EVT_KEY        7 // keyboard event (only sent to callbacks that occupied the screen)
} menu_item_t;


typedef int (*am_callback_t)(app_menu_t *pMenu, menu_item_t *pItem, int event, int param );

// 'Utility' functions for the application menu, implemented in amenu_utils.c :
BOOL Menu_IsFormatStringDelimiter( char c );
int  Menu_ParseDecimal( char **ppszSource );
int  Menu_HexDigitToInt( char c );
int  Menu_ParseHex( char **ppszSource );
int  Menu_ParseBinary( char **ppszSource );
char *Menu_GetParamsFromItemText( char *pszText, int *piNumBase, int *piFixedDigits, char **cppHotkey );
void IntToDecHexBinString( int iValue, int num_base, int nDigits, char *psz40Dest);
int  MenuItem_HasValue(menu_item_t *pItem );
int  Menu_GetNumItems( menu_item_t *pItems );
void Menu_GetColours( int sel_flags, uint16_t *pFgColor, uint16_t *pBgColor );
char *Menu_FindInStringTable( const am_stringtable_t *pTable, int value);
int  Menu_ReadIntFromPtr( void *pvValue, int data_type );
void Menu_WriteIntToPtr( int iValue, void *pvValue, int data_type );
void Menu_GetMinMaxForDataType( int data_type, int *piMinValue, int *piMaxValue );
uint16_t CRC16( uint16_t u16CRC, uint16_t *pwData, int nWords );
int  safe_stringcopy( char *pszSource, char *pszDest, int iSizeOfDest );
int  wide_to_C_string( wchar_t *wide_string, char *c_string, int maxlen );
int  wide_strnlen( wchar_t *wide_string, int maxlen );

void ScrollList_Init( scroll_list_control_t *pSL );
BOOL ScrollList_AutoScroll( scroll_list_control_t *pSL );


// Application-menu "API" (and interface to the keyboard handler, etc), in app_menu.c :
void Menu_Open( app_menu_t *pMenu, menu_item_t *pItems, char *cpJumpToItem, int edit_mode); 
void Menu_OnKey( uint8_t key); // called on keypress from some interrupt handler .
int  Menu_IsVisible(void);     // 1=currently visible (open), 0=not open; don't intercept keys
int  Menu_GetItemIndex(void);  // used by the Morse narrator (narrator.c) to detect "changes"
void Menu_GetColours( int sel_flags, uint16_t *pFgColor, uint16_t *pBgColor );
int  Menu_DrawIfVisible(int caller); // Paints the 'application menu' 
   // into the framebuffer. Must only be called from a DISPLAY task !
   // Returns 0 when invisible, 1 when visible (used in various hooks).
   // For debugging, the "caller" is passed in as an argument:
#  define AM_CALLER_STATUSLINE_HOOK     1
#  define AM_CALLER_DATETIME_HOOK       2
#  define AM_CALLER_F_4225_HOOK         3
#  define AM_CALLER_F_4315_HOOK         4
#  define AM_CALLER_RX_SCREEN_BLUE_HOOK 5
#  define AM_CALLER_RX_SCREEN_GRAY_HOOK 6
#  define AM_CALLER_RTC_TIMER           7

void Menu_FinishEditing( app_menu_t *pMenu, menu_item_t *pItem ); // [in] pMenu->iEditValue [out] *pItem->pvValue


// menu callback functions implemented in external modules:
extern int am_cbk_ColorPicker(app_menu_t *pMenu, menu_item_t *pItem, int event, int param ); // color_picker.c
extern int am_cbk_ColorSchemes(app_menu_t *pMenu, menu_item_t *pItem, int event, int param ); // "   "   "

#if( CONFIG_MORSE_OUTPUT )
void Menu_ReportItemInMorseCode(int morse_request); // used internally and by the Morse narrator
#endif // CONFIG_MORSE_OUTPUT ?


#endif // CONFIG_APP_MENU ?
