/*
 *  console.h
 * 
 */

#ifndef CONSOLE_H
#define CONSOLE_H

#include <stddef.h>   

#include "md380.h"
#include "display.h"

#define CONSOLE_Y_SIZE   10 // in console.c: #define Y_SIZE   10 (but not exposed in .h)
#define CONSOLE_MAX_XPOS 27 // in console.c: #define MAX_XPOS 27 (but not exposed in .h)  
extern char con_buf[CONSOLE_Y_SIZE][CONSOLE_MAX_XPOS+1]; // +1 for terminating 0 every line.

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

void con_printf(const char* fmt, ...);

void con_redraw();

#include "addl_config.h"

inline int is_menu_visible()
{
    return gui_opmode2 == OPM2_MENU ;
}

#ifdef __cplusplus
}
#endif

#endif /* CONSOLE_H */

