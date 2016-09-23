/*
 *  netmon.h
 * 
 */

#ifndef NETMON_H
#define NETMON_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void netmon_update();

extern uint8_t last_radio_event ;
extern uint8_t last_event2 ;
extern uint8_t last_event3 ;
extern uint8_t last_event4 ;
extern uint8_t last_event5 ;


#ifdef __cplusplus
}
#endif

#endif /* NETMON_H */

