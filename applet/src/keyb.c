/*
 *  keyb.c
 * 
 */

#include "keyb.h"

//#include "netmon.h"
#include "debug.h"

#include <stdint.h>

void trace_keyb()
{
    uint8_t *keypress_flag = 0x2001e5f0 ;
    
    netmon_printf("%02x ", *keypress_flag);    
}

void f_4101_hook()
{
    trace_keyb();
#if defined(FW_D13_020)
    f_4101();
#else
#warning please consider hooking.    
#endif    
    
}