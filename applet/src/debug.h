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

#define UNTHUMB_POI( adr ) (((uint32_t)adr) & ~1)

void debug_printf(char *fmt, ...);
void debug_printhex(void *buf, int len);


#ifdef DEBUG
#define PRINT(fmt, args...) do { debug_printf(fmt, ## args); } while (0)
#else
#define PRINT(fmt, args...) /* Don't do anything in release builds */
#endif

#ifdef DEBUG
#define PRINTRET() do { debug_printf("@ 0x%x ", UNTHUMB_POI(__builtin_return_address(0)) ); } while (0)
#else
#define PRINTRET() /* Don't do anything in release builds */
#endif    

#ifdef DEBUG
#define PRINTHEX(buf,len) do { debug_printhex(buf,len); } while (0)
#else
#define PRINTHEX(buf,len) /* Don't do anything in release builds */
#endif


#ifdef __cplusplus
}
#endif

#endif /* DEBUG_H */

