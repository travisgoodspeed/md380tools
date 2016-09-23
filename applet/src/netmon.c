/*
 *  netmon.c
 * 
 */

#include "netmon.h"

#include "console.h"
#include "md380.h"
#include "printf.h"
#include "dmr.h"

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

// radio events
// 0x01 = idle
// 0x02 = sync error? (tx only?)
// 0x04 = sync
// 0x07 = tx sound 
// 0x08 = rx (but for different TG)
// 0x09 = rx sound
// 0x0a = rx idle (tail of rx)
// 0x0e = sync attempt? (tx only?)
uint8_t last_radio_event ;
//

// beep events 
// 0x0e negative on ptt
// 0x0f not programmed channel
// 0x11 postive on ptt
// 0x24 beep end-of-rx
uint8_t last_event2 ;

// 0x01 = tx
// 0x02 = rx
uint8_t last_event3 ;

// ?
// 0x17 = ?
uint8_t last_event4 ;

void netmon_update()
{
    if( !is_console_visible() ) {
        return ;
    }
    
    progress++ ;
    progress %= sizeof( progress_info ) - 1 ;
    
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
        uint8_t *chan = (uint8_t *)0x2001e8c1 ;
        sprintf(status_buf, "ch: %d ", *chan ); 
        con_puts(status_buf);
        //con_nl();    
    }
    {
        // current channel name.
        wchar_t *p = (void*)0x2001cddc ;
        con_puts("cn:");
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
        con_puts("radio: ");
        char *str = "?" ;
        switch( last_radio_event ) {
            case 0x1 :
                str = "nosig" ;
                break ;
            case 0x4 :
                str = "sync" ;
                break ;
            case 0x9 :
                str = "rx voice" ;
                break ;
            case 0xa :
                str = "rx silence" ;
                break ;
            case 0x7 :
                str = "rx idle" ;
                break ;
        }
        con_puts(str);
        con_nl();    
    }
    {
        sprintf(status_buf,"re:%02x e2:%02x e3:%02x e4:%02x\n", last_radio_event, last_event2, last_event3, last_event4 );
        con_puts(status_buf);
    }
#ifdef FW_D13_020
    {
        uint8_t *smeter = (uint8_t *)0x2001e534 ;
        sprintf(status_buf,"sm:%d\n", *smeter );
        con_puts(status_buf);
    }
#endif    
    {
        sprintf(status_buf, "%d -> %d\n", g_src, g_dst); 
        con_puts(status_buf);        
    }
    
}
