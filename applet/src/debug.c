/*
 *  debug.c
 * 
 */

#include "debug.h"

#include "printf.h"

void md380_putc(void* p, char c);

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
