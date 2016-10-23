/*
 *  util.c
 * 
 */

#include <stdint.h>
#include "util.h"

void mkascii( char *dst, int dstlen, wchar_t *src )
{
    char *end = dst + dstlen - 1 ;
    while( *src ) {
        *dst++ = *src++ ;
        if( dst >= end ) {
            break ;
        }
    }
    *dst = 0 ;
}

uint32_t uli2w(uint32_t num, wchar_t *bf)
{
    int n = 0;
    unsigned int d = 1;
    while (num / d >= 10)
        d *= 10;
    while (d != 0) {
        int dgt = num / d;
        num %= d;
        d /= 10;
        if( n || dgt > 0 || d == 0 ) {
            *bf++ = dgt + '0';
            ++n;
        }
    }
    *bf = 0;
    return (n); // number of char
}

