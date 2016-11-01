/*
 *  beep.h
 * 
 */

#ifndef BEEP_H
#define BEEP_H

#ifdef __cplusplus
extern "C" {
#endif


#define BEEP_ROGER 0x24
#define BEEP_DENY_TX 0xe

    
#define BEEP_TEST_1 0x40
#define BEEP_TEST_2 0x41
#define BEEP_TEST_3 0x42
    
#if 0    
    
04: 3 tone
09: 2 tone long-short high-low
0a:
0b: single short-short high-med
0c: rpt 0xb
0d:
0e: low long
0f: 0x3 permanent
10: tonescale down
14: single
15: single
16: single tingeling
17: keeps ringing.
18: increased urgency beeps high
19: increased urgency beeps low
1a: ringing
1b: ringing
1c: ringing
1d: ringing
1e: ringing
1f: ringing

24: roger beep

20...29 

#endif

void bp_send_beep( uint8_t beep );



#ifdef __cplusplus
}
#endif

#endif /* BEEP_H */

