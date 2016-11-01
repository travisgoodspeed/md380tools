/*
 *  mbox.h
 * 
 * safe usage across fw versions.
 * 
 */

#ifndef MBOX_H
#define MBOX_H

#include <stdint.h>
#include "os.h"

#ifdef __cplusplus
extern "C" {
#endif

void * OSMboxPend_hook(OS_EVENT *pevent, uint32_t timeout, int8_t *perr);

typedef OS_EVENT ** osev_t ;

extern osev_t event1_mbox_poi_radio ;
extern osev_t event2_mbox_poi_beep ;
extern osev_t event3_mbox_poi ;
extern osev_t event4_mbox_poi ;
extern osev_t event5_mbox_poi ;

#ifdef __cplusplus
}
#endif

#endif /* MBOX_H */

