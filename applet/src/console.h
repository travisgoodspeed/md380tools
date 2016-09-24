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
void con_print(int x, int y, const char *s);

void con_redraw();

#include "addl_config.h"

inline int is_menu_visible()
{
    return (md380_f_4225_operatingmode & 0x7F) == SCR_MODE_MENU ;
}

inline int is_console_visible()
{
    if( global_addl_config.console == 0 ) {
        return 0 ;
    }
    return !is_menu_visible();
}


#ifdef __cplusplus
}
#endif

#endif /* CONSOLE_H */

