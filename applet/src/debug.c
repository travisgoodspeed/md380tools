/*
 *  debug.c
 * 
 */

#include "debug.h"

#include "printf.h"
#include "netmon.h"

void md380_putc(void* p, char c);

static void debug_printf2(char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    tfp_format(0, md380_putc, fmt, va);
    va_end(va);    
}

void debug_printf(char *fmt, ...)
{
    if( !global_addl_config.debug ) {
        return;
    }
    va_list va;
    va_start(va, fmt);
    tfp_format(0, md380_putc, fmt, va);
    va_end(va);
}

/* Convenience function to print in hex. */
void debug_printhex(void *buf, int len)
{
    if( !global_addl_config.debug ) {
        return ;
    }
    
    for (int i = 0; i < len; i++) {
        debug_printf2(" %02x", ((uint8_t*)buf)[i]);
    }
}

#define MAX_CHAR 200
char nm_logbuf[MAX_CHAR+1] = { "" } ; // +1 for 0 termination.
int logbuf_pos = 0 ;

void scroll_logbuf()
{
    int inc = 20 ;
    
    if( logbuf_pos > inc ) {
        char *src = nm_logbuf + inc ;
        char *dst = nm_logbuf ;
        while( *src ) {
            *dst++ = *src++ ;
        }
        *dst = 0 ;
        logbuf_pos -= inc ;
    }
}

static void nm_clr()
{
    logbuf_pos = 0 ;
    nm_logbuf[logbuf_pos] = 0 ;    
}

static void netmon_putch(void* p, char c)
{
    if( c == '\f' ) {
        nm_clr();
        return ;
    }
    if( c == '\n' ) {
        c = '|' ;
    }
    if( logbuf_pos >= MAX_CHAR ) {
        scroll_logbuf();
    }
    if( logbuf_pos < MAX_CHAR ) {
        nm_logbuf[logbuf_pos] = c ;
        logbuf_pos++ ;
        nm_logbuf[logbuf_pos] = 0 ;
    }
}

void netmon_printf(const char *fmt, ...)
{
    if( !is_netmon_enabled() ) {
        return ;
    }
    
    va_list va;
    va_start(va, fmt);
    tfp_format(0, netmon_putch, fmt, va);
    va_end(va);
}
