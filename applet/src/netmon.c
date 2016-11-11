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
#include "syslog.h"
#include "console.h"
#include "radio_config.h"
#include "codeplug.h"
#include "unclear.h"

uint8_t nm_screen = 0 ;

char progress_info[] = { "|/-\\" } ;
int progress = 0 ;

#ifndef FW_D13_020
#warning should be symbols, not sure if it is worth the effort
#endif
uint16_t *cntr2 = (void*)0x2001e844 ;
    
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
uint8_t last_event5;

void print_hdr()
{
    con_printf("hdr: %d:%d:%d\n", rst_hdr_src, rst_hdr_dst, rst_hdr_sap);
}

void print_vce()
{
    con_printf("vce: %d:%d %d\n", rst_src, rst_dst, rst_grp);
}

void print_smeter()
{
#if defined(FW_D13_020) || defined(FW_S13_020)		
    extern uint8_t smeter_rssi ;
    con_printf("rssi:%d\n", smeter_rssi );
#endif
}

#if defined(FW_D02_032)
uint8_t gui_opmode3 = 0xFF ;
#endif

void netmon1_update()
{
    progress++ ;
    progress %= sizeof( progress_info ) - 1 ;
    
    int progress2 = progress ; // sample (thread safe) 

    progress2 %=  sizeof( progress_info ) - 1 ;
    char c = progress_info[progress2];
    
    con_clrscr();
    
    con_printf("%c|%02d|%2d|%2d|%4d\n", c, gui_opmode1 & 0x7F, gui_opmode2, gui_opmode3, *cntr2 ); 
    
#if defined(FW_D13_020) || defined(FW_S13_020)
    extern uint8_t channel_num ;
    con_printf("ch:%d ", channel_num ); 
    
    con_printf("zn:%S\n",zone_name);
    con_printf("con:%S\n",contact.name);
    
    extern wchar_t channel_name[] ;
    con_printf("cn:%S\n",channel_name); 
#endif   
    {
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
            case 0x5 :
                str = "num5" ; 
                break ;
            case 0x7 :
                str = "data_idle/csbk_rx" ;
                break ;
            case 0x8 :
                str = "Other_Call" ; // TS 102 361-2 clause p 5.2.1.3.2
                break ;
            case 0x9 :
                str = "My_Call" ; // TS 102 361-2 clause p 5.2.1.3.2
                break ;
            case 0xa :
                str = "rx silence" ;
                break ;
            case 0xd :
                str = "num13 0xd" ;
                break ;
            case 0xe :
                str = "Wait_TX_Resp" ;
                break ;
        }
        con_printf("radio: %s\n", str);
    }
    {
        con_printf("re:%02x be:%02x e3:%02x e4:%02x\ne5:%02x ", last_radio_event, last_event2, last_event3, last_event4, last_event5 );
    }
    print_smeter();
#if defined(FW_D13_020) || defined(FW_S13_020)
    {
//        uint8_t *p = (void*)0x2001e5f0 ; // @D13
        uint8_t *p = radio_status_1 ;
        con_printf("st: %2x %2x %2x %2x\n", p[0], p[1], p[2], p[3]); 
    }
#endif    
#ifdef FW_D13_020
    {
        // only valid when transmitting or receiving.
        uint32_t *recv = (void*)0x2001e5e4 ;
        con_printf("%d\n", *recv); 
    }
#endif    
#ifdef FW_S13_020
    {
        // only valid when transmitting or receiving.
        uint32_t *recv = (void*)0x2001e6b4 ;			// needs to be confirmed!
        con_printf("%d\n", *recv); 
    }
#endif     
}

void print_bcd( uint8_t bcd )
{    
    con_printf("%d%d", (bcd>>4)&0xf, bcd&0xf );
}

void printfreq( void *p2 )
{
    uint8_t *p = p2 ;
    print_bcd( p[3] );
    print_bcd( p[2] );
    print_bcd( p[1] );
    print_bcd( p[0] );
}

void netmon2_update()
{
    con_clrscr();
#if defined(FW_D13_020) || defined(FW_S13_020)
    channel_info_t *ci = &current_channel_info ;
    
    {
        con_printf("mde:%02x prv:%02x pow:%02x\n", ci->mode, ci->priv, ci->power );
        
        con_puts("rx:");
        printfreq(&ci->rxf);
        con_nl();
        
        con_puts("tx:");
        printfreq(&ci->txf);
        con_nl();

        int cc = ( ci->cc_slot_flags >> 4 ) & 0xf ;
        int ts1 = ( ci->cc_slot_flags >> 2 ) & 0x1 ;
        int ts2 = ( ci->cc_slot_flags >> 3 ) & 0x1 ;
        con_printf("cc:%d ts1:%d ts2:%d\n", cc, ts1, ts2 );

        con_printf("cn:%S\n", ci->name ); // assume zero terminated.
    }
#else
    con_puts("D13 has more\n");
#endif    
    print_hdr();
    print_vce();
    
//    {
//        extern uint32_t kb_handler_count ;
//        extern uint32_t f4225_count ; 
//
//        con_printf("%d %d\n", kb_handler_count, f4225_count);
//    }
}

void netmon3_update()
{
    syslog_draw_poll();
    con_nl();

}

void netmon_update()
{
    if( !is_netmon_visible() ) {
        return ;
    }
    
    switch( nm_screen ) {
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
