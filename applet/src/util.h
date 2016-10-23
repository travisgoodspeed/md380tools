/*
 *  util.h
 * 
 */

#ifndef UTIL_H
#define UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

void mkascii( char *dst, int dstlen, wchar_t *src );

uint32_t uli2w(uint32_t num, wchar_t *bf);

#ifdef __cplusplus
}
#endif

#endif /* UTIL_H */

