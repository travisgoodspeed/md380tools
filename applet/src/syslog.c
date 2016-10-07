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

void syslog_putch( char c )
{
    if( c == '\n' ) {
    } else if ( c < ' ' || c >= 127 ) {
        c = '.' ;
    }
    syslog_buf[end] = c ;
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
    int e = end + SYSLOG_SIZE ;
    
    for(int i=begin;i<=e;i++) {
        int i2 = wrap(i);
        md380_putc(0,syslog_buf[i2]);
    }
}

void syslog_dump_console()
{
    con_clrscr();
    int e = end + SYSLOG_SIZE ;
    for(int i=begin;i<=e;i++) {
        int i2 = wrap(i);
        con_putc(syslog_buf[i2]);
    }
}
