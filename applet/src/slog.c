/*
 *  slog.c
 * 
 */

#include "slog.h"

#include <stdarg.h>
#include "printf.h"
#include "dmesg.h"
#include "console.h"

#define SLOG_SIZE 1500 

#define SMAXLINES 10 
int slog_lines[SMAXLINES];
int slog_line_poi = 0 ;
int slog_first_char = 1 ;

char slog_buf[SLOG_SIZE];

static int end = 0 ;
static int begin = 0 ;

static int slog_redraw_flag = 0 ;

int inline wrap( int idx )
{
    if( idx >= SLOG_SIZE ) {
        idx = 0 ;
    }
    return idx ;
}

void slog_line_begin( int idx, char c )
{
    if( slog_first_char ) {
        slog_lines[slog_line_poi] = idx ;
        slog_line_poi++ ;
        slog_line_poi %= SMAXLINES ;
        slog_first_char = 0 ;
    }
    if( c == '\n' ) {
        slog_first_char = 1 ;
    }
}

void slog_putch( char c )
{
    if( c == '\n' ) {
    } else if ( c < ' ' || c >= 127 ) {
        c = '.' ;
    }
    slog_buf[end] = c ;
    slog_line_begin(end,c);
    end = wrap(end+1);
    if( end == begin ) {
        // full.
        begin = wrap(begin+1);
    }
    slog_redraw_flag = 1 ;
}

static void slog_prch(void* p, char c)
{
    slog_putch(c);
}

void slog_printf(const char *fmt, ...)
{
    if( !is_slog_enabled() ) {
        return ;
    }
    
    va_list va;
    va_start(va, fmt);
    tfp_format(0, slog_prch, fmt, va);
    va_end(va);
}

void slog_dump_dmesg()
{
    int i = begin ;
    while(1) {
        i = wrap(i);
        if( i == end ) {
            break ;
        }
        md380_putc(0,slog_buf[i]);
        i++ ;
    }
}

void slog_redraw()
{
    slog_redraw_flag = 1 ;
}

void slog_draw_poll()
{
    if( !slog_redraw_flag ) {
        return ;
    }
    
    int con_height = 10 ;
    
    slog_redraw_flag = 0 ;
    
    con_clrscr();
    
    int idx = slog_line_poi ;
    idx += SMAXLINES ;
    idx -= con_height ;
    idx %= SMAXLINES ;
    
    int pos = slog_lines[idx];
    
    int i = pos ;
    int linecnt = 0 ;
//    int i = begin ;
    while(1) {
        i = wrap(i);
        if( i == end ) {
            break ;
        }
        char c = slog_buf[i];
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

void slog_clear()
{
    end = 0 ;
    begin = 0 ;
    slog_line_poi = 0 ;
    for(int i=0;i<SMAXLINES;i++) {
        slog_lines[i] = 0 ;
    }
}
