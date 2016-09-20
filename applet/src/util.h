/*
 *  util.h
 * 
 *  Created on 20-Sep-2016 23:53:14 by Simon IJskes
 * 
 */

#ifndef UTIL_H
#define UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>    
    
void mkascii( char *dst, int dstlen, wchar_t *src );


#ifdef __cplusplus
}
#endif

#endif /* UTIL_H */

