/*
 *  netmon.c
 * 
 */

#include "netmon.h"

#include "console.h"
#include "md380.h"
#include "printf.h"
#include "dmr.h"
#include "radiostate.h"

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
// 5 = block dmr processing?

// radio events (todo fix)
// 0x01 = nosig
// 0x02 = sync error? (tx only?) 
// 0x03 = FM 
// 0x04 = sync
// 0x05 = ?
// 0x07 = idle
// 0x08 = rx (but for different TG)
// 0x09 = rx sound
// 0x0a = rx idle (tail of rx)
// 0x0c = ?
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

// ?
uint8_t last_event5 ;

void netmon1_update()
{
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
            case 0x2 :
                str = "tx denied" ;
                break ;
            case 0x3 :
                str = "FM" ;
                break ;
            case 0x4 :
                str = "Out_Of_SYNC" ; // TS 102 361-2 clause p 5.2.1.3.2
                break ;
            case 0x7 :
                str = "rx idle" ;
                break ;
            case 0x8 :
                str = "Other_Call" ; // TS 102 361-2 clause p 5.2.1.3.2
                break ;
            case 0x9 :
                str = "My_Call" ; // TS 102 361-2 clause p 5.2.1.3.2
                break ;
            case 0xe :
                str = "Wait_TX_Resp" ;
                break ;
            case 0xa :
                str = "rx silence" ;
                break ;
        }
        con_puts(str);
        con_nl();    
    }
    {
        sprintf(status_buf,"re:%02x be:%02x e3:%02x e4:%02x\ne5:%02x ", last_radio_event, last_event2, last_event3, last_event4, last_event5 );
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
        sprintf(status_buf,"hdr: %d:%d:%d\n", rst_hdr_src, rst_hdr_dst, rst_hdr_sap );
        con_puts(status_buf);
    }
    {
        sprintf(status_buf, "vce: %d:%d\n", g_src, g_dst); 
        con_puts(status_buf);        
    }
#ifdef FW_D13_020
//    {
//        // only valid when transmitting or receiving.
//        uint32_t *recv = 0x2001e5e4 ;
//        sprintf(status_buf, "%d\n", *recv); 
//        con_puts(status_buf);        
//    }
#endif    
    
}

void print_bcd( uint8_t bcd )
{    
    sprintf(status_buf,"%d%d", (bcd>>4)&0xf, bcd&0xf );
    con_puts(status_buf);        
}

void printfreq( uint8_t p[] )
{
    print_bcd( p[3] );
    print_bcd( p[2] );
    print_bcd( p[1] );
    print_bcd( p[0] );
}

void netmon2_update()
{
    con_clrscr();
    {
        uint8_t *p = 0x2001deb8 + 0x10 ;
        
        con_puts("rx:");
        printfreq(p);
        con_nl();
        
        p += 4 ;
        
        con_puts("tx:");
        printfreq(p);
        con_nl();
    }
    
}

void netmon3_update()
{
    extern char nm_logbuf[];
    
    con_clrscr();
    con_puts(nm_logbuf);
}

void netmon_update()
{
    if( !is_netmon_visible() ) {
        return ;
    }
    
    switch( global_addl_config.console ) {
        case 0 :
            return ;
        case 1 :
            netmon1_update();
            return ;
        case 2 :
            netmon2_update();
            return ;
        case 3 :
            netmon3_update();
            return ;
    }
}
