/*
 *  console.h
 * 
 */

#ifndef CONSOLE_H
#define CONSOLE_H

#include "stddef.h"    

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

inline int has_console()
{
    return global_addl_config.console ;
}

inline int has_gui()
{
    if( !has_console() ) {
        return 1 ;
    }
    return global_addl_config.debug ;
}

#ifdef __cplusplus
}
#endif

#endif /* CONSOLE_H */

