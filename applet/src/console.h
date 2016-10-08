/*
 *  console.h
 * 
 */

#ifndef CONSOLE_H
#define CONSOLE_H

#include <stddef.h>   

#include "md380.h"
#include "display.h"

#ifdef __cplusplus
extern "C" {
#endif

// stdio compat.    
void con_puts( const char *s );
void con_putc( char c );

void con_putsw( const wchar_t *s );

//
void con_clrscr();
void con_nl();

void con_goto(int x, int y);
void con_print_pos(int x, int y, const char *s);
void con_print(const char *s);
void con_printc(char c);

void con_redraw();

#include "addl_config.h"

inline int is_menu_visible()
{
    return gui_opmode2 == OPM2_MENU ;
//#ifdef FW_D13_020
//    if( gui_opmode2 == 10 ) {
//        return 1 ;
//    }
//#endif    
//    return (md380_f_4225_operatingmode & 0x7F) == SCR_MODE_MENU ;
}

inline int is_console_visible()
{
#if defined(FW_D13_020)    
    if( global_addl_config.console == 0 ) {
        return 0 ;
    }
    return !is_menu_visible();
#else
    return 0 ;
#endif    
}


#ifdef __cplusplus
}
#endif

#endif /* CONSOLE_H */

