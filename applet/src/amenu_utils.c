// File:    md380tools/applet/src/amenu_utils.c
// Author:  Wolf (DL4YHF) [initial version]
//
// Date:    2017-04-23
//  Utility functions for the 'application menu' (app_menu.c) .
//  Any function formerly in app_menu.c that does NOT depend
//  on app_menu_t is a good candidate for being moved into amenu_utils.c .

#include "config.h"

#if (CONFIG_APP_MENU) // only available in combination with app_menu.c

#include <stm32f4xx.h>    // <- fancy stuff, 'uint8_t', etc
#include <string.h>       // memset(), ...
#include <limits.h>       // INT_MIN, INT_MAX, ...
#include "lcd_driver.h"   // alternative LCD driver (DL4YHF), doesn't depend on 'gfx'
#include "app_menu.h"     // 'simple' alternative menu activated by red BACK-button
#include "printf.h"       // Kustaa Nyholm's tinyprintf (printf.c, snprintfw)
#include "addl_config.h"  // customizeable colours in global_addl_config




//---------------------------------------------------------------------------
// String parsing functions for the menu (mostly menu_item_t.pszText)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
BOOL Menu_IsFormatStringDelimiter( char c )
  // Part of a very simple parser for a optional format specifications
  // in menu_item_t.pszText .
  // Notes:
  //  - the trailing zero is also considered a 'delimiter' here !
  //  - space is NOT a delimiter because it may be used in separator labels. 
{ return (c<' ' || c==',' || c==';' || c=='}' || c==']' );
}

//---------------------------------------------------------------------------
int Menu_ParseDecimal( char **ppszSource )
  // Returns ZERO if there's nothing to parse. KISS.
  // If the caller needs to know IF something was parsed,
  // he may compare the source-pointer before and after the call.
{ char *cp = *ppszSource;
  int iValue = 0;
  int iSign  = 1;
  if( *cp=='-' )
   { ++cp;
     iSign = -1;
   }
  while( *cp>='0' && *cp<='9' )
   { iValue = 10*iValue + (*(cp++)-'0');
   }
  *ppszSource = cp; // pass back the INCREMENTED 'source pointer'
  return iValue * iSign;
}

//---------------------------------------------------------------------------
int Menu_HexDigitToInt( char c )
{ if( c>='0' && c<='9' )
   { return c-'0';
   }
  if( c>='a' && c<='f' )
   { return c-'a'+10;
   }
  if( c>='A' && c<='F' )
   { return c-'A'+10;
   }
  return -1; // not a valid hex digit
} 

//---------------------------------------------------------------------------
int Menu_ParseHex( char **ppszSource )
  // Similar as Menu_ParseDecimal(), but for HEX strings. 
  // No sign, but an optional hex prefix (0x) is supported.
{ char *cp = *ppszSource;
  int iDigit, iValue = 0;
  if( cp[0]=='0' && cp[1]=='x'  )
   { cp += 2;
   }
  while( (iDigit=Menu_HexDigitToInt(*cp)) >= 0 )
   { iValue = (iValue << 4) + iDigit;
     ++cp;
   }
  *ppszSource = cp; // pass back the INCREMENTED 'source pointer'
  return iValue;
}

//---------------------------------------------------------------------------
int Menu_ParseBinary( char **ppszSource )
  // Only for geeks and development .. allows editing fields with BINARY display.
{ char *cp = *ppszSource;
  int iValue = 0;
  while( *cp>='0' && *cp<='1' )
   { iValue = (iValue << 1) + ((*cp++)-'0');
   }
  *ppszSource = cp; // pass back the INCREMENTED 'source pointer'
  return iValue;
}

//---------------------------------------------------------------------------
char *Menu_GetParamsFromItemText( char *pszText, int *piNumBase, int *piFixedDigits, char **cppHotkey )
  // Returns a pointer to the first "plain text" character in the item text
  // after the list of printing options in squared brackets.
  // All outputs are optional (piNumBase, piFixedDigits, cppHotkey may be NULL).
  // Thus usable to skip the 'non-printable' characters at the begin.
{
  int num_base = 10;    // default: decimal numeric output
  int fixed_digits = 0; // default: no FIXED number of digits

  if( cppHotkey != NULL )
   { *cppHotkey = NULL;
   }
  if( (pszText!=NULL) && (*pszText == '[') ) // begin of a headline / hotkey indicator ?
   { ++pszText; 
     if( *pszText>='0' && *pszText<='9' ) // Digit -> Hotkey !
      { if( cppHotkey != NULL )
         { *cppHotkey = pszText;
         }
        // skip hotkey (with optional text for the 'separator line') :
        while( !Menu_IsFormatStringDelimiter( *pszText ) ) 
         { ++pszText;
         }
        if( *pszText!='\0' && *pszText!=']' ) // Skip delimiter after hotkey name ?
         { ++pszText;  // e.g. comma or semicolon
         }
      } 
     // After the hotkey, parse optional parameters .
     // Examples for formatting- and output options :
     //  [b]  = binary with as many digits as necessary
     //  [b8] = binary with 8 digits (fixed)
     //  [d3] = decimal with 3 digits
     //  [h8] = hexadecimal with 8 digits, but no hex prefix ("0x")
     //  [5h4] = item invokable via hotkey '5', with 4-digit hex display
     //  [5 Chapter Five;h4] = first item in "Chapter Five", 
     //          invokable via hotkey '5', with 4-digit hex display
     while( !Menu_IsFormatStringDelimiter( *pszText ) )
      { switch( *pszText++ )
         { case 'b': // [b] = binary display (for the value) ..
              num_base = 2; 
              fixed_digits = Menu_ParseDecimal( &pszText ); // FIXED number of digits ?
              break;
           case 'd': // [d] = decimal display with a fixed number of digits
              num_base = 10; 
              fixed_digits = Menu_ParseDecimal( &pszText );
              break;
           case 'h': // [d] = hex display with a fixed number of digits, no prefix
              num_base = 16; 
              fixed_digits = Menu_ParseDecimal( &pszText );
              break;
           default:  // "normal" character (not a delimiter) : just skip and ignore
              break;
         }
      } 
     if( *pszText == ']' ) // what began with a '[' *should* end with a ']' ..
      { ++pszText; 
      }
   } // end if < fixed menu text beginning with '[' >

  if( piNumBase != NULL )  // pass back optional outputs
   { *piNumBase = num_base;
   }
  if( piFixedDigits != NULL )
   { *piFixedDigits = fixed_digits;
   }
  return pszText; // returns a pointer to the first "plain text" character 
                  // after the list of printing options in squared brackets.

} // end Menu_GetParamsFromItemText()



//---------------------------------------------------------------------------
// String-building functions for the menu (mostly to 'show values')
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void IntToBinaryString(
        int iValue,      // [in] value to be converted
        int nDigits,     // [in] number of fixed digits, 
                         //      0=variable length without leading zeroes
        char *psz40Dest) // [out] string, expect up to 33(!) bytes
  // Converts an integer into a binary representation (string).
  // Beware: nDigits=0 means 'as many digits as necessary'.
  //         For a 32-bit integer, the destination string may
  //         be up to 32 characters long, plus one byte for the trailing zero !
{
  int iBitNr;
  if( nDigits <= 0 )
   { nDigits = 32;  // begin testing for nonzero bits at the MSB (31)
     while( nDigits>1 && (iValue & (1<<(nDigits-1)))==0 )
      { --nDigits;  // eliminate trailing zeroes 
      }
   }
  for( iBitNr=nDigits-1; iBitNr>=0; --iBitNr )
   { if( iValue & (1<<iBitNr) )
      { *psz40Dest++ = '1';
      }
     else
      { *psz40Dest++ = '0';
      }
   }
  *psz40Dest = '\0'; // never forget the trailing zero in a "C"-string !
} // end IntToBinaryString()

//---------------------------------------------------------------------------
void IntToDecHexBinString(
        int iValue,      // [in] value to be converted
        int num_base,    // [in] 2=binary, 10=decimal, 16=hex
        int nDigits,     // [in] number of fixed digits, 
                         //      0=variable length without leading zeroes
        char *psz40Dest) // [out] string, expect up to 33(!) bytes
  // Converts an integer into a binary representation (string).
  // Beware: nDigits=0 means 'as many digits as necessary'.
  //         For a 32-bit integer, the binary representation (string) may
  //         be up to 32 characters long, plus one byte for the trailing zero !
  //   Thus, for safety, psz40Dest should be a buffer with up to 40 characters.
{
  char sz7Format[8] = "%d";
  switch( num_base )
   { case 2: // binary: not supported by printf.c : sprintf() -> tfp_format() 
        IntToBinaryString( iValue, nDigits, psz40Dest );
        return;
     case 10:
        if( nDigits>0 )
         { sprintf( sz7Format+1, "%dd", nDigits );
         }
        else
         { // default ("%d") already set
         }
        break;
     case 16:
        if( nDigits>0 )
         { sprintf( sz7Format+1, "0%dX", nDigits ); // note the leading ZERO
         }
        else
         { strcpy(  sz7Format+1, "%X" ); // note the absence of a leading zero
         }
        break;
     default:  // whatever the intention was, it's not supported here yet:
        sprintf( psz40Dest, "?base%d?", num_base );
        return;
   }
  // Arrived here ? Should be something supported by the
  //      small-footprint implementation of sprintf in printf.c  !
  sprintf( psz40Dest, sz7Format, iValue );
} // end IntToDecHexBinString()


//---------------------------------------------------------------------------
// Miscellaneous .. :)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
int MenuItem_HasValue(menu_item_t *pItem ) // TRUE=yes (item has a "displayable value"), FALSE=no.
{ if( pItem != NULL )
   { if( (pItem->data_type==DTYPE_NONE) || (pItem->data_type==DTYPE_SUBMENU) )
      { return FALSE;
      }
     else if( pItem->pvValue )
      { return TRUE;
      }
     else // even without a "value pointer", menu items can display a value..
     if( pItem->callback ) // .. delivered via callback on APPMENU_EVT_GET_VALUE
      { return TRUE;
      }
   }
  // Arrived here ? The menu item doesn't seem to have a displayable "value" 
  return FALSE; 
}

//---------------------------------------------------------------------------
int Menu_GetNumItems( menu_item_t *pItems )
{ int nItems = 0;
  if( pItems )
   { while( pItems->pszText != NULL )
      { ++nItems;
        ++pItems;
      } 
   }
  return nItems;
} 

//---------------------------------------------------------------------------
void Menu_GetColours( int sel_flags, uint16_t *pFgColor, uint16_t *pBgColor )
{ 
  uint16_t fg_color, bg_color;

  if( sel_flags & SEL_FLAG_CURSOR ) // "edit cursor" or the field subject to "inc/dec-editing"
   { fg_color = global_addl_config.edit_fg_color;
     bg_color = global_addl_config.edit_bg_color;
   }
  else if( sel_flags & SEL_FLAG_FOCUSED ) 
   { fg_color = global_addl_config.sel_fg_color;
     bg_color = global_addl_config.sel_bg_color;
   }
  else // neither the edit cursor nor not marked : 
   { fg_color = global_addl_config.fg_color;
     bg_color = global_addl_config.bg_color;
   }

  if( LCD_GetColorDifference( fg_color, bg_color ) < 10 )
   { // difference too small -> discard customized colours to make the menu "readable" again
     // Imitate Tytera's menu colours instead :
     if( sel_flags & SEL_FLAG_CURSOR ) // "edit cursor" or the field subject to "inc/dec-editing"
      { fg_color = LCD_COLOR_WHITE;
        bg_color = LCD_COLOR_BLUE;
      }
     else if( sel_flags & SEL_FLAG_FOCUSED ) 
      { fg_color = LCD_COLOR_WHITE;
        bg_color = LCD_COLOR_RED;
      }
     else // neither the edit cursor nor not marked : 
      { fg_color = LCD_COLOR_BLACK;
        bg_color = LCD_COLOR_WHITE;
      }
   }
  *pFgColor = fg_color;
  *pBgColor = bg_color;
} // end Menu_GetColours()

//---------------------------------------------------------------------------
char *Menu_FindInStringTable( const am_stringtable_t *pTable, int value)
  // Returns the address of a string if 'value' was found,
  // otherwise returns NULL.
{
  if( pTable )
   { while( pTable->pszText != NULL )
      { if( pTable->value == value )
         { return pTable->pszText;
         }
        ++pTable;
      }
   }
  return NULL;
} // end Menu_FindInStringTable()

//---------------------------------------------------------------------------
int Menu_ReadIntFromPtr( void *pvValue, int data_type )
{
  if(pvValue==NULL)  
   { return 0;
   }
  // When called from the simple hex monitor (amenu_hexmon.c) with an address
  // just above the end of the internal RAM ( > 0x2001FFF ), the radio RE-BOOTED,
  // most likely due to an access violation / unhandled exception. 
  // To prevent crashing, refuse to read from the following address ranges:
  if( (pvValue>=(void*)0x20020000 && pvValue<=(void*)0x40000000) // gap between RAM and SFRs
    ||(pvValue>=(void*)0x10010000 && pvValue<=(void*)0x1FFEFFFF) // "reserved" between CCM & OTP/System
    ||(pvValue>=(void*)0x08100000 && pvValue<=(void*)0x0FFFFFFF) // "reserved" between CCM & Flash
    ||(pvValue>=(void*)0x00100000 && pvValue<=(void*)0x07FFFFFF) // "reserved" between aliased Flash & Flash
    )
   { return -1;  // indicates an illegal address when reading a BYTE ("uint8_t")
   }

  switch( data_type )
   { case DTYPE_NONE   /*0*/ :
        break;  
     case DTYPE_BOOL   /*1*/ :
        break;  
     case DTYPE_INT8   /*2*/ :  
        return *(int8_t*)pvValue;  
     case DTYPE_INT16  /*3*/ :  
        return *(int16_t*)pvValue;  
     case DTYPE_INTEGER/*4*/ :
        return *(int*)pvValue;  
     case DTYPE_UNS8   /*5*/ :
        return *(uint8_t*)pvValue;  
     case DTYPE_UNS16  /*6*/ :  
        return *(uint16_t*)pvValue;  
     case DTYPE_UNS32  /*7*/ :
        return *(uint32_t*)pvValue;  
     case DTYPE_FLOAT  /*8*/ :
        return (int)(*(float*)pvValue);  
     case DTYPE_STRING /*9*/ : // dare to PARSE A STRING here ?
        break;
     case DTYPE_WSTRING/*10*/: // "wide" string: let this nonsense die, use UTF-8 instead !
        break;
   }
  return 0;
} // Menu_ReadIntFromPtr()

//---------------------------------------------------------------------------
void Menu_WriteIntToPtr( int iValue, void *pvValue, int data_type )
{
  if(pvValue==NULL)  // safety first !
   { return;
   }
  switch( data_type )
   { case DTYPE_NONE   /*0*/ :
        break;  
     case DTYPE_BOOL   /*1*/ :
        break;  
     case DTYPE_INT8   /*2*/ :  
        *(int8_t*)pvValue = (int8_t)iValue;
        break;  
     case DTYPE_INT16  /*3*/ :  
        *(int16_t*)pvValue = (int16_t)iValue;
        break;  
     case DTYPE_INTEGER/*4*/ :
        *(int*)pvValue = (int)iValue;
        break;  
     case DTYPE_UNS8   /*5*/ :
        *(uint8_t*)pvValue = (uint8_t)iValue;
        break;  
     case DTYPE_UNS16  /*6*/ :  
        *(uint16_t*)pvValue = (uint16_t)iValue;
        break;  
     case DTYPE_UNS32  /*7*/ :
        *(uint32_t*)pvValue = (uint32_t)iValue;
        break;  
     case DTYPE_FLOAT  /*8*/ :
        *(float*)pvValue = (float)iValue;
        break;  
     case DTYPE_STRING /*9*/ :
     case DTYPE_WSTRING/*10*/: // string types not supported HERE
        break;
   }
} // end Menu_WriteIntToPtr()

//---------------------------------------------------------------------------
void Menu_GetMinMaxForDataType( int data_type, int *piMinValue, int *piMaxValue )
{
  int min=0, max=0;
  switch( data_type )
   { case DTYPE_NONE   /*0*/ :
        break;  
     case DTYPE_BOOL   /*1*/ :
        max = 1;
        break;  
     case DTYPE_INT8   /*2*/ :  
        min = -128;
        max = 127;
        break;  
     case DTYPE_INT16  /*3*/ :  
        min = -32768;
        max = 32767;
        break;  
     case DTYPE_INTEGER/*4*/ :
        min = INT_MIN;
        max = INT_MAX;
        break;  
     case DTYPE_UNS8   /*5*/ :
        max = 255;
        break;  
     case DTYPE_UNS16  /*6*/ :  
        max = 65535;
        break;  
     case DTYPE_UNS32  /*7*/ :
     // max = 0xFFFFFFFF; // dilemma.. this wouldn't work with SIGNED 32 BIT INTEGERS !
        max = INT_MAX; // better than a MAX LIMIT of "-1" (=0xFFFFFFFF)..
        break;  
     case DTYPE_FLOAT  /*8*/ : // permitted value range cannot be expressed with an int..
        min = INT_MIN;
        max = INT_MAX;
        break;  
     case DTYPE_STRING /*9*/ :
     case DTYPE_WSTRING/*10*/: // string types not supported HERE
        break;
   }
  if( piMinValue != NULL )
   { *piMinValue = min;
   }
  if( piMaxValue != NULL )
   { *piMaxValue = max;
   }
} // end Menu_GetMinMaxForDataType()

//---------------------------------------------------------------------------
int safe_stringcopy( char *pszSource, char *pszDest, int iSizeOfDest )
  // Unlike the often-misunderstood strncpy(), only copies as many characters
  // as present in the SOURCE, and only as many as the destination can accept.
  // The output (c_string) is ALWAYS zero-terminated, unless iSizeOfDest is zero.
  // The destination string may be TRUNCATED to iSizeOfDest-1(!) characters .
  // Note the sequence of arguments : Copy FROM, TO;  not TO, FROM !
  // Returns the number of characters copied, NOT including the trailing zero.
{ int n_chars_copied = 0;
  if( iSizeOfDest>0 )
   { while( *pszSource && (iSizeOfDest>1/*!!*/) )
      { *(pszDest++) = *(pszSource++);
        --iSizeOfDest;
        ++n_chars_copied;
      }
     *pszDest = '\0'; // ALWAYS terminate C-strings with a trailing zero !
   }
  return n_chars_copied;
} // end safe_stringcopy()


//---------------------------------------------------------------------------
int wide_strnlen( wchar_t *wide_string, int maxlen )
{ int n = 0;
  while( (maxlen>0) && (*wide_string!=0) )
   { ++n;
     --maxlen;
     ++wide_string;
   }
  return n;
}

//---------------------------------------------------------------------------
int wide_to_C_string( wchar_t *wide_string, char *c_string, int iSizeOfDest )
  // Converts a wasteful 'wide' string into a good old C string .
  // Returns the NUMBER OF CHARACTERS copied, not including the trailing zero.
  // The output (c_string) is ALWAYS zero-terminated, unless iSizeOfDest is zero. 
{ int n_chars_copied = 0;
  if( iSizeOfDest>0 )
   { while( *wide_string && (iSizeOfDest>1/*!!*/) )
      { *(c_string++) = (char)(*(wide_string++));
        --iSizeOfDest;
        ++n_chars_copied;
      }
     *c_string = '\0'; // ALWAYS terminate C-strings with a trailing zero !
   }
  return n_chars_copied;
} // end wide_to_C_string()

//---------------------------------------------------------------------------
void ScrollList_Init( scroll_list_control_t *pSL )
{
  memset( pSL, 0, sizeof( scroll_list_control_t ) );
  // "Initial asssumption" about the number of items visible w/o scrolling:
  pSL->n_visible_items = 4; // "worst case" with large fonts + fixed title line
  // Most screens adjust this parameter during the first screen update,
  // after all currently visible items are painted. After that, call 
  // ScrollList_AutoScroll() to make sure the currently focused item is visible.
}

//---------------------------------------------------------------------------
BOOL ScrollList_AutoScroll( scroll_list_control_t *pSL )
  // Adjusts pSL->scroll_pos to make the currently focused
  // item (with index pSL->focused_item) visible.
  // Returns TRUE when that was necessary,  which for the caller means 
  //   'redraw as soon as possible' .
{ 
  int i_max, i = pSL->focused_item;
  int nVisibleItems = pSL->n_visible_items;
  BOOL fModified = FALSE;
  if( (i>=0) && (pSL->num_items>0) )
   { LimitInteger( &i, 0, pSL->num_items-1 );
   
     // Scroll the currently selected item into view :
     // Visible range of items is scroll_pos to vert_scroll_pos+n-1 .
     // If item_index is outside that range, adjust vert_scroll_pos:
     if( i < pSL->scroll_pos ) // "invisible above the top"
      { pSL->scroll_pos = i;
        fModified = TRUE; 
      }
     i_max = pSL->scroll_pos + nVisibleItems - 1;
     if( i > i_max ) // selected item "invisible below the bottom"
      { pSL->scroll_pos = i-(nVisibleItems-1);
        fModified = TRUE; 
      }
   }
  return fModified;
} // end ScrollList_AutoScroll()



//---------------------------------------------------------------------------
// Table-less but sufficiently fast CRC-16 calculation .
//      Used in the simple menu to check if values have been modified,
//      and in the 'hex monitor' for a similar purpose.
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
uint16_t CRC16( uint16_t u16CRC,  // [in] "seed" or CRC from previous block
                uint16_t *pwData, // [in] start address of a block of 16-bit words
                int nWords )      // [in] number of words to 'add' to the CRC
{
  // Based on IEEE TRANSACTIONS ON COMPUTERS, VOL. 58, NO. 10, OCTOBER 2009,
  //       'Fast CRCs' by Gam D. Nguyen,
  //        Fig. 16, 'C program for the fast h-bit CRC (s = h)' .
  //   ("Q" became u16CRC here, Qi and Qn were removed with some unused vars) .
  // > Abstract:  CRCs have desirable properties for effective error detection. 
  // > But their software implementation, which relies on many steps of the 
  // > polynomial division, is typically slower than other codes such as weaker 
  // > checksums. A relevant question is whether there are some particular CRCs 
  // > that have fast implementation. In this paper, we introduce such fast CRCs 
  // > as well as an effective technique to implement them. For these fast CRCs, 
  // > even without using table lookup, it is possible either to eliminate or to 
  // > greatly reduce many steps of the polynomial division during their computation.
  //
  // Used in the menu (display) and the hex monitor to check for modified
  // variables (numeric and strings), or the currently displayed address range.
  // 
# define F 0x7    /* F = X^16 +X^2 + X + 1 */
# define K 0x8000 /* K = 2^(h-1), h=s=16   */
# define s 16
  uint16_t A, C;
  if( (uint32_t)pwData & 1 ) // source pointer must be aligned to 16-bit..
   { // If it's not, avoid crashing with an access violation, and cheat a "bit":
     pwData = (uint16_t*)( ((uint32_t)pwData+1) & ~1); // "round up" to next even address..
     --nWords; // .. and punish the caller by omitting ONE 16-bit word
   }
  while( nWords-- )
   {
     A = u16CRC ^ *pwData++;
     if (A&K) C = (A<<1) ^ F;
     else     C = A<<1;
     if (C&K) u16CRC = (C<<1) ^ F;
     else     u16CRC = C<<1;
     u16CRC ^= C ^ A;
   }
  return u16CRC;
}


#endif // CONFIG_APP_MENU ?
