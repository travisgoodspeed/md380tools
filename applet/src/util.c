/*
 *  util.c
 * 
 */

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

