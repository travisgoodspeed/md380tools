/*
 *  syslog.c
 * 
 */

#include "syslog.h"

#include <stdarg.h>
#include <stdio.h>
#include "printf.h"
#include "dmesg.h"
#include "console.h"

#define SYSLOG_SIZE 200 

#define MAXLINES 10 
int lines[MAXLINES];
int line_poi = 0 ;
int line_end = 0 ;
int first_char = 1 ;

char syslog_buf[SYSLOG_SIZE];

static int end = 0 ;
static int begin = 0 ;

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

void syslog_dump_console()
{
    con_clrscr();
    
    int idx = line_poi ;
    int pos = lines[idx];
    
    int i = pos ;
//    int i = begin ;
    while(1) {
        i = wrap(i);
        if( i == end ) {
            break ;
        }
        con_putc(syslog_buf[i]);
        i++ ;
    }
}

void syslog_clear()
{
    end = 0 ;
    begin = 0 ;
    line_poi = 0 ;
}
