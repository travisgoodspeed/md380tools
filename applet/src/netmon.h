/*
 *  netmon.h
 * 
 */

#ifndef NETMON_H
#define NETMON_H

#include <stdint.h>

#include "console.h"
#include "addl_config.h"

#ifdef __cplusplus
extern "C" {
#endif

void netmon_update();

extern uint8_t last_radio_event ;
extern uint8_t last_event2 ;
extern uint8_t last_event3 ;
extern uint8_t last_event4 ;
extern uint8_t last_event5 ;

inline int is_netmon_enabled()
{
    return global_addl_config.console ;
}

inline int is_netmon_visible()
{
    if( !is_netmon_enabled() ) {
        return 0 ;
    }
    return !is_menu_visible();
}

#ifdef __cplusplus
}
#endif

#endif /* NETMON_H */

