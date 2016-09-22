/*
 *  debug.h
 * 
 *  Created on 21-Sep-2016 10:10:11 by Simon IJskes
 * 
 */

#ifndef DEBUG_H
#define DEBUG_H

#include "addl_config.h"


#ifdef __cplusplus
extern "C" {
#endif

//inline debug_printf(char *fmt, ...) 

#ifdef DEBUG
#define PRINT(fmt, args...)  { if( global_addl_config.debug ) {  printf(fmt, ## args); } } 
#else
#define PRINT(fmt, args...)    /* Don't do anything in release builds */
#endif

#define UNTHUMB_POI( adr ) (((uint32_t)adr) & ~1)


#ifdef __cplusplus
}
#endif

#endif /* DEBUG_H */

