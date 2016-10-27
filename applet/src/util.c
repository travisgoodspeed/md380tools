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

//const 
char hexes[] = "0123456789abcdef"; //Needs to be const for problems.

/* This writes a 32-bit hexadecimal value into a human-readable
   string.  We do it manually to avoid heap operations. */
void strhex(char *string, long value)
{
    char b;
    for (int i = 0; i < 4; i++) {
        b = value >> (24 - i * 8);
        string[2 * i] = hexes[(b >> 4)&0xF];
        string[2 * i + 1] = hexes[b & 0xF];
    }
}

/* This writes a 32-bit hexadecimal value into a human-readable
   string.  We do it manually to avoid heap operations. */
void wstrhex(wchar_t *string, long value)
{
    char b;
    for (int i = 0; i < 4; i++) {
        b = value >> (24 - i * 8);
        string[2 * i] = hexes[(b >> 4)&0xF];
        string[2 * i + 1] = hexes[b & 0xF];
    }
}

