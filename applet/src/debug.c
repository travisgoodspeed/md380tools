/*
 *  debug.c
 * 
 */

#include "debug.h"

#include "printf.h"

void md380_putc(void* p, char c);

inline void debug_printf2(char *fmt, ...)
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
