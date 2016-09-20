/*
 *  console.c
 * 
 */

#include "console.h"

#include "md380.h"


#define MAX_XPOS 20 
#define MAX_BUF 100
char con_buf[MAX_BUF] = { "empty screen" } ;

int xpos = 0 ;

void addchar( char c )
{
    con_buf[xpos] = c ;
    xpos++ ;
    con_buf[xpos] = 0 ;
    if( xpos > MAX_XPOS ) {
        xpos = 0 ;
    }
}

void con_puts( const char *s )
{
    while( *s ) {
        con_putc( *s++ );
    }
}

void con_putc( char c )
{
    switch( c ) {
        
    }
    
}

int within_update = 0 ;

wchar_t wide[MAX_BUF];
    
void con_draw1()
{
    gfx_set_fg_color(0xff000000);
    gfx_set_bg_color(0x00ff8032); 
    void *old = gfx_select_font(gfx_font_small);
    
    char *p = con_buf ;
    wchar_t *w = wide ;
    wchar_t *e = wide + MAX_BUF -1 ;
    while( *p ) {
        *w++ = *p++ ;
        if( w >= e ) {
            break ;
        }
    }
    *e = 0 ;

    gfx_chars_to_display(wide, 1, 10, 0);

    gfx_select_font(old);    
}

void con_draw()
{
//    if( within_update ) {
//        return ;
//    }
    
    within_update = 1 ;

    con_draw1();
        
    within_update = 0 ;
}
