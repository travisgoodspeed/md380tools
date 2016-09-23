/*
 *  console.c
 * 
 */

#include "console.h"

#include "md380.h"
#include "gfx.h"


#define MAX_XPOS 25 
#define MAX_YPOS 20 

#define MAX_BUF (MAX_XPOS + 1)
char con_buf[MAX_YPOS][MAX_XPOS+1]; // +1 for terminating 0 every line.

int con_xpos = 0 ;
int con_ypos = 0 ;

void con_goto(int x, int y)
{
    con_xpos = x ;
    con_ypos = y ;
}

void con_print(int x, int y, const char *s)
{
    con_goto(x,y);
    con_puts(s);
}

void con_puts( const char *s )
{
    while( *s ) {
        con_putc( *s++ );
        if( con_xpos >= MAX_XPOS ) {
            con_nl();
        }
    }    
}

void con_putsw( const wchar_t *s )
{
    while( *s ) {
        con_putc( *s++ );
        if( con_xpos >= MAX_XPOS ) {
            con_nl();
        }
    }        
}

void con_nl()
{
    con_xpos = 0 ;
    con_ypos++ ;    
}

void con_clrscr()
{
    con_xpos = 0 ;
    con_ypos = 0 ;
    for(int y=0;y<MAX_YPOS;y++) {
        con_buf[y][0] = 0 ;
    }
}

static void con_addchar( char c )
{
    if( con_xpos >= MAX_XPOS ) {
        return ;
    }
    if( con_ypos >= MAX_YPOS ) {
        return ;        
    }
    
    con_buf[con_ypos][con_xpos] = c ;
    con_xpos++ ;
    con_buf[con_ypos][con_xpos] = 0 ;
}

void con_putc( char c )
{
    switch( c ) {
        case '\f' :
            con_clrscr();
            return ;
        case '\n' :
            con_nl();
            return ;
    }
    con_addchar(c);    
}

int within_update = 0 ;

wchar_t wide[MAX_BUF];
    
#define LINE_HEIGHT 12 

static void con_draw1()
{
    // TODO: save old values first.
    void *old = gfx_select_font(gfx_font_small);
    
    // slow?
    {
        static int cnt = 0 ;
        cnt++ ;
        if( cnt % 16 == 0 ) {
            gfx_set_fg_color(0x00ff8032); 
            gfx_blockfill(0,0,159,109);
        }
    }
    
    gfx_set_fg_color(0xff000000);
    gfx_set_bg_color(0x00ff8032); 
    
    for(int y=0;y<=con_ypos;y++) {
        char *p = con_buf[y];
        wchar_t *w = wide ;
        wchar_t *we = wide + MAX_BUF -1 ;
        for(int x=0;x<MAX_XPOS;x++) {
            if( *p == 0 ) {
                *w++ = ' ';                
            } else {
                *w++ = *p++ ;                
            }
            if( w >= we ) {
                break ;
            }
        }
        *w = 0 ;
#if defined(FW_D13_020)
        gfx_drawtext4(wide, 0, y * LINE_HEIGHT, 0, 20);
#else
#warning should find symbol gfx_drawtext4        
        gfx_chars_to_display(wide, 0, y * LINE_HEIGHT, 0);
#endif
    }

    gfx_select_font(old);    
}

void con_redraw()
{
    if( !is_console_visible() ) {
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
