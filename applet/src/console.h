/*
 *  console.h
 * 
 */

#ifndef CONSOLE_H
#define CONSOLE_H

#ifdef __cplusplus
extern "C" {
#endif

void con_puts( const char *s );
void con_putc( char c );
void con_goto(int x, int y);
void con_print(int x, int y, const char *s);

void con_draw();

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

