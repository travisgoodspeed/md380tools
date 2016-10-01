/*
 *  keyb.c
 * 
 */

#include "keyb.h"

//#include "netmon.h"
#include "debug.h"

#include <stdint.h>

void f_4101_hook()
{
    uint8_t *keypress_flag = 0x2001e5f0 ;
    
    netmon_printf("%02x ", *keypress_flag);
    f_4101();
}