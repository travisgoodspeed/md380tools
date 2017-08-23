/*
 *  console.c
 * 
 */

#include "console.h"

#include "md380.h"
#include "gfx.h"
#include "printf.h"
#include "netmon.h"

#include <stdarg.h>

#define MAX_XPOS 27 
#define Y_SIZE 10

#define MAX_BUF (MAX_XPOS + 1)
char con_buf[Y_SIZE][MAX_XPOS+1]; // +1 for terminating 0 every line.

int con_xpos = 0 ;
int con_ypos = 0 ;

static int con_dirty_flag = 0 ;

void con_goto(int x, int y)
{
    con_xpos = x ;
    con_ypos = y ;
}

void con_print_pos(int x, int y, const char *s)
{
    con_goto(x,y);
    con_print(s);
}

void con_print( const char *s )
{
    while( *s ) {
        con_printc( *s++ );
    }    
}

void con_puts( const char *s )
{
    while( *s ) {
        con_putc( *s++ );
    }    
}

void con_putsw( const wchar_t *s )
{
    while( *s ) {
        con_putc( *s++ );
    }        
}

void con_nl()
{
    con_xpos = 0 ;
    con_ypos++ ;  

    if( con_ypos > Y_SIZE ) {
        con_ypos = Y_SIZE ;
        return ;
    }
        
    con_dirty_flag = 1 ;
}

void con_clrscr()
{
    con_xpos = 0 ;
    con_ypos = 0 ;
    for(int y=0;y<Y_SIZE;y++) {
        con_buf[y][0] = 0 ;
    }

    con_dirty_flag = 1 ;
}

static void con_addchar( char c )
{
    // intentional non-wrap.
    if( con_xpos >= MAX_XPOS ) {
        return ;
    }
    if( con_ypos >= Y_SIZE ) {
        return ;        
    }
    
    if( c < ' ' ) {
        c = '.' ;
    }
    if( c >= 127 ) {
        c = '.' ;
    }
    
    con_buf[con_ypos][con_xpos] = c ;
    con_xpos++ ;
    con_buf[con_ypos][con_xpos] = 0 ;

    con_dirty_flag = 1 ;
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

void con_printc( char c )
{
    con_addchar(c);
}

static void con_putch(void* p, char c)
{
    con_putc(c);
}

void con_printf(const char* fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    tfp_format(0, con_putch, fmt, va);
    va_end(va);        
}

int within_update = 0 ;

wchar_t wide[MAX_BUF];
char small[MAX_BUF];

#define LINE_HEIGHT 12 

#if 0
#define bgcolor 0x00ff8032
#define fgcolor 0xff000000
#endif
#define bgcolor 0x00000000
#define fgcolor 0x00ffffff

#define LEFT_POS 3

static void con_draw1()
{
    // save old values first.
    void *old = gfx_select_font(gfx_font_small);
    
//#if defined(FW_D13_020) || defined(FW_S13_020)
//#else 
//    // slow?
//    {
//        static int cnt = 0 ;
//        cnt++ ;
//        if( cnt % 16 == 0 ) {
//            gfx_set_fg_color(bgcolor); 
//            gfx_blockfill(0,0,159,109);
//        }
//    }
//#endif
    
    // erase bottom stripe.
    gfx_set_fg_color(bgcolor); 
    gfx_blockfill(0, MAX_Y-9, MAX_X, MAX_Y); 		// fix by MAX_Y-9 instead of -10 overwrite of lowest pixel/line in bottom area of netmon screens
    // erase left 1 pixels
    gfx_blockfill(0, 0, LEFT_POS-1, MAX_Y); 
    
    
    gfx_set_fg_color(fgcolor);
    gfx_set_bg_color(bgcolor); 

    for(int y=0;y<Y_SIZE;y++) {
        char *p = con_buf[y];
        wchar_t *w = wide ;
        wchar_t *we = wide + MAX_BUF -1 ;
        char *sp2 = small ;
        for(int x=0;x<MAX_XPOS;x++) {
            if( *p == 0 ) {
                char c = ' ' ;
                *w++ = c ;         
                *sp2++ = c ;
            } else {
                char c = *p++ ;
                *w++ = c ;         
                *sp2++ = c ;
            }
            if( w >= we ) {
                break ;
            }
        }
        *w = 0 ;
        *sp2 = 0;
        gfx_drawtext7(small, LEFT_POS, y * LINE_HEIGHT);
    }

    gfx_select_font(old);    
}

void con_redraw()
{
    if( !is_netmon_visible() ) {
        return ;
    }
    
    if( !con_dirty_flag ) {
        return ;
    }
    con_dirty_flag = 0 ;
    
    // be prepared, excelent opportunity to be bitten by stack overflow.
    
    // TODO: make real atomic, and skip if recursive.
//    if( within_update ) {
//        return ;
//    }
    
    within_update = 1 ;

    con_draw1();
        
    within_update = 0 ;
}
