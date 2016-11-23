/*
 *  lastheard.c
 * 
 */

#include "lastheard.h"

#include <stdarg.h>
#include "printf.h"
#include "dmesg.h"
#include "console.h"

#define LASTHEARD_SIZE 1500 

#define LH_MAXLINES 20 
int lh_lines[LH_MAXLINES];
int lh_line_poi = 0 ;
int lh_first_char = 1 ;

char lastheard_buf[LASTHEARD_SIZE];

static int end = 0 ;
static int begin = 0 ;

static int lastheard_redraw_flag = 0 ;

int inline lh_wrap( int idx )
{
    if( idx >= LASTHEARD_SIZE ) {
        idx = 0 ;
    }
    return idx ;
}

void lastheard_line_begin( int idx, char c )
{
    if( lh_first_char ) {
        lh_lines[lh_line_poi] = idx ;
        lh_line_poi++ ;
        lh_line_poi %= LH_MAXLINES ;
        lh_first_char = 0 ;
    }
    if( c == '\n' ) {
        lh_first_char = 1 ;
    }
}

void lastheard_putch( char c )
{
    if( c == '\n' ) {
    } else if ( c < ' ' || c >= 127 ) {
        c = '.' ;
    }
    lastheard_buf[end] = c ;
    lastheard_line_begin(end,c);
    end = lh_wrap(end+1);
    if( end == begin ) {
        // full.
        begin = lh_wrap(begin+1);
    }
    lastheard_redraw_flag = 1 ;
}

static void lastheard_prch(void* p, char c)
{
    lastheard_putch(c);
}

void lastheard_printf(const char *fmt, ...)
{
    if( !is_lastheard_enabled() ) {
        return ;
    }
    
    va_list lh_va;
    va_start(lh_va, fmt);
    tfp_format(0, lastheard_prch, fmt, lh_va);
    va_end(lh_va);
}

void lastheard_dump_dmesg()
{
    int i = begin ;
    while(1) {
        i = lh_wrap(i);
        if( i == end ) {
            break ;
        }
        md380_putc(0,lastheard_buf[i]);
        i++ ;
    }
}

void lastheard_redraw()
{
    lastheard_redraw_flag = 1 ;
}

void lastheard_draw_poll()
{
    if( !lastheard_redraw_flag ) {
        return ;
    }
    
    int con_height = 10 ;
    
    lastheard_redraw_flag = 0 ;
    
    con_clrscr();
    
    int idx = lh_line_poi ;
    idx += LH_MAXLINES ;
    idx -= con_height ;
    idx %= LH_MAXLINES ;
    
    int pos = lh_lines[idx];
    
    int i = pos ;
    int linecnt = 0 ;
//    int i = begin ;

    while(1) {
        i = lh_wrap(i);
        if( i == end ) {
            break ;
        }
        char c = lastheard_buf[i];
        if( c == '\n' ) {
            linecnt++ ;
            con_nl();
        } else {
            con_printc(c);            
        }
        i++ ;
    }
    for(int i=linecnt;i<con_height;i++) {
        con_nl();        
    }
}

void lastheard_clear()
{
    end = 0 ;
    begin = 0 ;
    lh_line_poi = 0 ;
    for(int i=0;i<LH_MAXLINES;i++) {
        lh_lines[i] = 0 ;
    }
}
