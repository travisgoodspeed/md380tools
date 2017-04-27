/*
 *  netmon.c
 * 
 */

#include "netmon.h"

#include "md380.h"
#include "version.h"
#include "tooldfu.h"
#include "printf.h"
#include "string.h"
#include "dmr.h"
#include "gfx.h"
#include "radiostate.h"
#include "syslog.h"
#include "lastheard.h"
#include "console.h"
#include "radio_config.h"
#include "console.h"
#include "codeplug.h"
#include "unclear.h"
#include "usersdb.h"
#include "etsi.h"
#include <wchar.h>

uint8_t nm_screen = 0 ;
uint8_t nm_started = 0 ;
uint8_t nm_started5 = 0 ;
uint8_t nm_started6 = 0 ;
uint8_t rx_voice = 0 ;
uint8_t rx_new = 0 ;
uint8_t ch_new = 0 ;
uint8_t call_start_state = 0;
uint8_t previous_call_state = 0;

wchar_t curr_channel[20];                       // current channel name
wchar_t sh_last_channel[20];                    // last channel name sh log
wchar_t ch_last_channel[20];                    // last channel name ch log

char progress_info[] = { "|/-\\" } ;
int progress = 0 ;

#ifndef FW_D13_020
#warning should be symbols, not sure if it is worth the effort
#endif
/* uint16_t *cntr2 = (void*)0x2001e844 ;*/

#if defined(FW_D13_020) || defined(FW_S13_020)  
    extern uint16_t m_cntr2 ;
#endif

#if defined(FW_D02_032)
uint8_t gui_opmode3 = 0xFF ;
uint16_t m_cntr2 = 0x00;
#endif
    
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

void netmon1_update()
{
    progress++ ;
    progress %= sizeof( progress_info ) - 1 ;
    
    int progress2 = progress ; // sample (thread safe) 

    progress2 %=  sizeof( progress_info ) - 1 ;
    char c = progress_info[progress2];
    
    con_clrscr();
    
    con_printf("%c|%02d|%2d|%2d|%4d\n", c, gui_opmode1 & 0x7F, gui_opmode2, gui_opmode3, m_cntr2 ); 
    
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
                str = "TX denied" ;
                break ;
            case 0x3 :
                str = "FM" ;
                break ;
            case 0x4 :
                str = "Out_of_sync" ; // TS 102 361-2 clause p 5.2.1.3.2
                break ;
            case 0x5 :
                str = "num5 0x5" ; 
                break ;
            case 0x7 :
                str = "RX csbk/idle" ;
                break ;
            case 0x8 :
                str = "RX other" ; // TS 102 361-2 clause p 5.2.1.3.2
                break ;
            case 0x9 :
                str = "RX myreq" ; // TS 102 361-2 clause p 5.2.1.3.2
                break ;
            case 0xa :
                str = "RX silence" ;
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
        con_printf("st: %2x %2x %2x %2x\n", 
                radio_status_1.m0, radio_status_1.m1, 
                radio_status_1.m2, radio_status_1.m3 );
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
        uint32_t *recv = (void*)0x2001e6b4 ;                    // needs to be confirmed!
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
}


void netmon4_update()
{
#if defined(FW_D13_020) || defined(FW_S13_020)
    lastheard_draw_poll();

    int src;
    char log = 'l';

    if ( nm_started == 0 ) {
        lastheard_printf("Netmon 4 - Lastheard ====\n");
        nm_started = 1;                         // flag for restart of LH list
    }   

    char mode = ' ' ;
    if( rst_voice_active != previous_call_state && rst_voice_active)
    {
        call_start_state = 1;
        talkerAlias.displayed = 0;
    }
    previous_call_state = rst_voice_active;

    if( rst_voice_active ) {
        if( rst_mycall ) {
            mode = '*' ; // on my tg            
        } else {
            mode = '!' ; // on other tg
        }
        src = rst_src;
        user_t usr;
   
        if( ( src != 0 ) && ( rst_flco < 4 ) && call_start_state == 1 ) {
            call_start_state = 0;
            rx_new = 1;                         // set status to new for netmon5
            ch_new = 1;                         // set status to new for netmon6
            print_time_hook(log);
            if( usr_find_by_dmrid(&usr, src) == 0 ) {
                lastheard_printf("=%d->%d %c\n", src, rst_dst, mode);
            } else {
                lastheard_printf("=%s->%d %c\n", usr.callsign, rst_dst, mode);
            }
        }
        if ( global_addl_config.userscsv > 1 && (talkerAlias.displayed != 1 && talkerAlias.length > 0) )
        {
            talkerAlias.displayed = 1;
            lastheard_printf("TA: %s\n",talkerAlias.text);
        }
    }
#else
    lastheard_printf("No lastheard available\n");    
#endif 

}

void netmon5_update()
{
#if defined(FW_D13_020) || defined(FW_S13_020)
    slog_draw_poll();
    
    int src;

    extern wchar_t channel_name[20] ;           // read current channel name from external  
    static int sl_cnt = 0 ;                     // lastheard line counter
    static int cp_cnt = 1 ;                     // lastheard channel page counter
    char slog = 's';

    if ( nm_started5 == 0 ) {
        slog_printf("Netmon 5 LH Channel =========\n");
        nm_started5 = 1;                                // flag for restart of LH list
        sl_cnt++;                               // reset lh counter 
    }   

    char mode = ' ' ;
    if( rst_voice_active ) {
        if( rst_mycall ) {
            mode = '*' ; // on my tg            
        } else {
            mode = '<' ; // on other tg
        }
    src = rst_src;
    user_t usr;

    if( ( src != 0 ) && ( rx_new == 1 ) ) {

        for (int i = 0; i < 20; i++)
           {
              curr_channel[i] = channel_name[i];
                if (channel_name[i] == '\0')
                break;
           }

        if ( (wcscmp(sh_last_channel, curr_channel) != 0)  || (sl_cnt >= 9) ) { // compare channel_name with last_channel from latest rx

                if (sl_cnt >= 9 ) {
                        slog_printf(">>:%S #%d <<< \n", curr_channel, cp_cnt);  // show page # on new log pages
                        sl_cnt = 0;                                             // reset sl_cnt at end of screen
                        cp_cnt++;                                               // increment lh page count
                } else {
                        slog_printf("cn:%S ---------------\n", curr_channel);   // show divider line when channel changes       
                }
                wcscpy(sh_last_channel, curr_channel);
                sl_cnt++;
        }

        print_time_hook(slog);

        // loookup source ID in user database to show callsign instead of ID
        if( usr_find_by_dmrid(&usr, src) == 0 ) {
                // lookup destination ID in user database for status requests etc.
                if( usr_find_by_dmrid(&usr, rst_dst) != 0 ) {
                        slog_printf("=%d->%s %c\n", src, usr.callsign, mode);
                } else  {
                        slog_printf("=%d->%d %c\n", src, rst_dst, mode);
                }
        } else {
            slog_printf("=%s->", usr.callsign);
                if( usr_find_by_dmrid(&usr, rst_dst) != 0 ) {
                        slog_printf("%s %c\n", usr.callsign, mode);
                } else  {
                        slog_printf("%d %c\n", rst_dst, mode);
                }
        }
        rx_new = 0 ; // call handled, wait until new voice call status received
        ch_new = 1 ; // status for netmon6
        sl_cnt++;
     }
    }
#else
    slog_printf("No lastheard available\n");    
#endif 

}


void netmon6_update()
{
#if defined(FW_D13_020) || defined(FW_S13_020)
    clog_draw_poll();
    
    extern wchar_t channel_name[20] ;           // read current channel name from external  
    static int ch_cnt = 0 ;                     // lastheard line counter
    char clog = 'c';

    if ( nm_started6 == 0 ) {
        clog_printf("Netmon 6 RX channel ========\n");
        nm_started6 = 1;                        // flag for restart of LH list
        ch_cnt = 1;                             // reset lh counter 
    }   

    if( ch_new == 1 ) {

        for (int i = 0; i < 20; i++)
           {
              curr_channel[i] = channel_name[i];
                if (channel_name[i] == '\0')
                break;
           }

        if (wcscmp(ch_last_channel, curr_channel) != 0)  {         // compare channel_name with last_channel from latest rx
                print_time_hook(clog);
                clog_printf("-%02d:%S \n", ch_cnt, curr_channel);  // show divider line when channel changes       
                wcscpy(ch_last_channel, curr_channel);
                ch_cnt++;
                }
        ch_new = 0 ;                            // call handled, wait until new voice call status received
     }
#else
    clog_printf("No channellog available\n");    
#endif 
}

void netmon_update()
{
    if( !is_netmon_visible() ) {
        netmon6_update();
        netmon4_update();
        netmon5_update();
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
        case 4 :
            netmon6_update();
            netmon4_update();
            return ;
        case 5 :
            netmon4_update();
            netmon6_update();
            netmon5_update();
            return ;
        case 6 :
            netmon4_update();
            //netmon5_update();
            netmon6_update();
            return ;
    }
}
