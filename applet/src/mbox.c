/*
 *  mbox.c
 * 
 */

#define DEBUG

#include "mbox.h"

#include "md380.h"
#include "debug.h"

#if defined(FW_D13_020)


void test_mbox()
{
//    PRINT("1: %x\n", mbox_beep);
    PRINT("2: %x\n", event2_mbox_poi_beep);
    PRINT("3: %x\n", *event2_mbox_poi_beep);
}


//uint8_t beep_msg ; // it cannot live on the stack.
//
//void mb_send_beep( int beep )
//{
//    beep_msg = beep ;
//    md380_OSMboxPost(*event2_mbox_poi_beep, &beep_msg);    
//}
#else
void test_mbox()
{
    
}
//void mb_send_beep( int beep )
//{
//    // dummy. no implementation.
//}
#endif
