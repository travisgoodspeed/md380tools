/*
 *  console.c
 * 
 */

#include "console.h"

#include "md380.h"


#define MAX_XPOS 20 
#define MAX_YPOS 20 

#define MAX_BUF (MAX_XPOS * MAX_YPOS)
char con_buf[MAX_YPOS][MAX_XPOS];

int con_xpos = 0 ;
int con_ypos = 0 ;

void con_addchar( char c )
{
    if( con_xpos >= MAX_XPOS ) {
        return ;
    }
    if( con_ypos >= MAX_YPOS ) {
        return ;        
    }
    
    con_buf[con_ypos][con_xpos] = c ;
    con_xpos++ ;
    if( con_xpos >= MAX_XPOS ) {
        return ;
    }
    con_buf[con_ypos][con_xpos] = 0 ;
}

void con_puts( const char *s )
{
    while( *s ) {
        con_putc( *s++ );
        if( con_xpos >= MAX_XPOS ) {
            con_putc( '\n');
        }
    }    
}

void con_clrscr()
{
    con_xpos = 0 ;
    con_ypos = 0 ;
    for(int y=0;y<MAX_YPOS;y++) {
        con_buf[y][0] = 0 ;
    }
}

void con_putc( char c )
{
    switch( c ) {
        case '\f' :
            con_clrscr();
            return ;
        case '\n' :
            con_xpos = 0 ;
            con_ypos++ ;
            return ;
    }
    con_addchar(c);    
}

int within_update = 0 ;

int con_enabled = 0 ;

wchar_t wide[MAX_XPOS];
    
void con_draw1()
{
    gfx_set_fg_color(0xff000000);
    gfx_set_bg_color(0x00ff8032); 
    void *old = gfx_select_font(gfx_font_small);

    for(int y=0;y<=con_ypos;y++) {
        char *p = con_buf[y];
        wchar_t *w = wide ;
        wchar_t *we = wide + MAX_BUF -1 ;
        for(int x=0;x<MAX_XPOS;x++) {
            if( *p == 0 ) {
                break ;
            }
            *w++ = *p++ ;
            if( w >= we ) {
                break ;
            }
        }
        *w = 0 ;
        gfx_chars_to_display(wide, 0, y * 18, 0);
    }

    gfx_select_font(old);    
}

void con_draw()
{
    if( con_enabled == 0 ) {
        return ;
    }
    
    // be prepared, excelent opportunity to be bitten by stack overflow.
    
    // TODO: make real atomic, and skip if recursive.
//    if( within_update ) {
//        return ;
//    }
    
    within_update = 1 ;

    con_draw1();
        
    within_update = 0 ;
}
