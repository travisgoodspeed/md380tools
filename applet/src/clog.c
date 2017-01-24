/*
 *  clog.c
 * 
 */

#include "clog.h"

#include <stdarg.h>
#include "printf.h"
#include "dmesg.h"
#include "console.h"

#define CLOG_SIZE 1500 

#define CMAXLINES 10 
int clog_lines[CMAXLINES];
int clog_line_poi = 0 ;
int clog_first_char = 1 ;

char clog_buf[CLOG_SIZE];

static int end = 0 ;
static int begin = 0 ;

static int clog_redraw_flag = 0 ;

int inline wrap( int idx )
{
    if( idx >= CLOG_SIZE ) {
        idx = 0 ;
    }
    return idx ;
}

void clog_line_begin( int idx, char c )
{
    if( clog_first_char ) {
        clog_lines[clog_line_poi] = idx ;
        clog_line_poi++ ;
        clog_line_poi %= CMAXLINES ;
        clog_first_char = 0 ;
    }
    if( c == '\n' ) {
        clog_first_char = 1 ;
    }
}

void clog_putch( char c )
{
    if( c == '\n' ) {
    } else if ( c < ' ' || c >= 127 ) {
        c = '.' ;
    }
    clog_buf[end] = c ;
    clog_line_begin(end,c);
    end = wrap(end+1);
    if( end == begin ) {
        // full.
        begin = wrap(begin+1);
    }
    clog_redraw_flag = 1 ;
}

static void clog_prch(void* p, char c)
{
    clog_putch(c);
}

void clog_printf(const char *fmt, ...)
{
    if( !is_clog_enabled() ) {
        return ;
    }
    
    va_list va;
    va_start(va, fmt);
    tfp_format(0, clog_prch, fmt, va);
    va_end(va);
}

void clog_dump_dmesg()
{
    int i = begin ;
    while(1) {
        i = wrap(i);
        if( i == end ) {
            break ;
        }
        md380_putc(0,clog_buf[i]);
        i++ ;
    }
}

void clog_redraw()
{
    clog_redraw_flag = 1 ;
}

void clog_draw_poll()
{
    if( !clog_redraw_flag ) {
        return ;
    }
    
    int con_height = 10 ;
    
    clog_redraw_flag = 0 ;
    
    con_clrscr();
    
    int idx = clog_line_poi ;
    idx += CMAXLINES ;
    idx -= con_height ;
    idx %= CMAXLINES ;
    
    int pos = clog_lines[idx];
    
    int i = pos ;
    int linecnt = 0 ;
//    int i = begin ;
    while(1) {
        i = wrap(i);
        if( i == end ) {
            break ;
        }
        char c = clog_buf[i];
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

void clog_clear()
{
    end = 0 ;
    begin = 0 ;
    clog_line_poi = 0 ;
    for(int i=0;i<CMAXLINES;i++) {
        clog_lines[i] = 0 ;
    }
}
