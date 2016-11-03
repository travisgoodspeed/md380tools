/*
 *  syslog.c
 * 
 */

#include "syslog.h"

#include <stdarg.h>
#include "printf.h"
#include "dmesg.h"
#include "console.h"

#define SYSLOG_SIZE 1500 

#define MAXLINES 20 
int lines[MAXLINES];
int line_poi = 0 ;
int first_char = 1 ;

char syslog_buf[SYSLOG_SIZE];

static int end = 0 ;
static int begin = 0 ;

static int syslog_redraw_flag = 0 ;

int inline wrap( int idx )
{
    if( idx >= SYSLOG_SIZE ) {
        idx = 0 ;
    }
    return idx ;
}

void register_line_begin( int idx, char c )
{
    if( first_char ) {
        lines[line_poi] = idx ;
        line_poi++ ;
        line_poi %= MAXLINES ;
        first_char = 0 ;
    }
    if( c == '\n' ) {
        first_char = 1 ;
    }
}

void syslog_putch( char c )
{
    if( c == '\n' ) {
    } else if ( c < ' ' || c >= 127 ) {
        c = '.' ;
    }
    syslog_buf[end] = c ;
    register_line_begin(end,c);
    end = wrap(end+1);
    if( end == begin ) {
        // full.
        begin = wrap(begin+1);
    }
    syslog_redraw_flag = 1 ;
}

static void syslog_prch(void* p, char c)
{
    syslog_putch(c);
}

void syslog_printf(const char *fmt, ...)
{
    if( !is_syslog_enabled() ) {
        return ;
    }
    
    va_list va;
    va_start(va, fmt);
    tfp_format(0, syslog_prch, fmt, va);
    va_end(va);
}

void syslog_dump_dmesg()
{
    int i = begin ;
    while(1) {
        i = wrap(i);
        if( i == end ) {
            break ;
        }
        md380_putc(0,syslog_buf[i]);
        i++ ;
    }
}

void syslog_redraw()
{
    syslog_redraw_flag = 1 ;
}

void syslog_draw_poll()
{
    if( !syslog_redraw_flag ) {
        return ;
    }
    
    int con_height = 10 ;
    
    syslog_redraw_flag = 0 ;
    
    con_clrscr();
    
    int idx = line_poi ;
    idx += MAXLINES ;
    idx -= con_height ;
    idx %= MAXLINES ;
    
    int pos = lines[idx];
    
    int i = pos ;
    int linecnt = 0 ;
//    int i = begin ;
    while(1) {
        i = wrap(i);
        if( i == end ) {
            break ;
        }
        char c = syslog_buf[i];
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

void syslog_clear()
{
    end = 0 ;
    begin = 0 ;
    line_poi = 0 ;
    for(int i=0;i<MAXLINES;i++) {
        lines[i] = 0 ;
    }
}
