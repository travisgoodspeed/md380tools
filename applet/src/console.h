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

#ifdef __cplusplus
}
#endif

#endif /* CONSOLE_H */

