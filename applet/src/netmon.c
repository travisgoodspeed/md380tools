/*
 *  netmon.c
 * 
 */

#include "netmon.h"

#include "console.h"
#include "md380.h"
#include "printf.h"

#define MAX_STATUS_CHARS 40
char status_buf[MAX_STATUS_CHARS] = { "uninitialized statusline" };

char progress_info[] = { "|/-\\" } ;
int progress = 0 ;

#ifndef FW_D13_020
#warning should be symbols, not sure if it is worth the effort
#endif
uint8_t *mode2 = (void*)0x2001e94b ;
uint16_t *cntr2 = (void*)0x2001e844 ;
uint8_t *mode3 = (void*)0x2001e892 ;
    
// mode2
// 1 idle
// 2 rx/tx
// 4 post-rx/tx
// 10 menu

// mode3 
// 0 = idle?
// 3 = unprog channel

uint8_t last_radio_event ;

// 0x24 roger beep?
// 0x0f not programmed channel
uint8_t last_event2 ;
uint8_t last_event3 ;

void netmon_update()
{
    if( !has_console() ) {
        return ;
    }
    
    progress++ ;
    progress %= sizeof( progress_info );
    
    int progress2 = progress ; // sample (thread safe) 

    progress2 %=  sizeof( progress_info ) - 1 ;
    char c = progress_info[progress2];
    
    //int dst = g_dst ;
    
    sprintf(status_buf,"%c|%02d|%2d|%2d|%4d", c, md380_f_4225_operatingmode & 0x7F, *mode2, *mode3, *cntr2 ); // potential buffer overrun!!!
        
    con_clrscr();
    con_puts(status_buf);
    con_nl();    
#ifdef FW_D13_020
    {
        // current channel name.
        wchar_t *p = (void*)0x2001cddc ;
        con_puts("ch:");
        con_putsw(p);
        con_nl();    
    }
    {        
        // current tg name.
        wchar_t *p = (void*)0x2001e1f4 ;
        con_puts("tg:");
        con_putsw(p);
        con_nl();    
    }
#endif    
    {
        sprintf(status_buf,"re:%02x e2:%02x e3:%02x \n", last_radio_event, last_event2, last_event3 );
        con_puts(status_buf);
    }
}
