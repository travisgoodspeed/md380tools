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

void debug_printasc(void *buf, int len)
{
    if( !global_addl_config.debug ) {
        return ;
    }
    
    char *p = buf ;
    
    for (int i = 0; i < len; i++,p++) {
        char c = *p ;
        if( c < ' ' ) {
            c = '.' ;
        } else if( c > 126 ) {
            c = '.' ;            
        }
        md380_putc(0,c);
    }
}
