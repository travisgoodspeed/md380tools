/*! \file dmr.c
  \brief DMR Hook functions.

  This module hooks some of the DMR packet handler functions,
  in order to extend the functionality of the radio.  Ideally,
  we'd like to use just the hooks, but for the time-being some
  direct patches and callbacks are still necessary.
 
 * glue layer 
*/

#define CONFIG_DMR

//#define NETMON
#define DEBUG

#include "dmr.h"

#include <string.h>

#include "md380.h"
#include "printf.h"
#include "dmesg.h"
#include "version.h"
#include "tooldfu.h"
#include "config.h"
#include "gfx.h"
#include "addl_config.h"
#include "os.h"
#include "debug.h"
#include "radiostate.h"


/* global Bufferspace to transfer data*/
//char DebugLine1[30];
//char DebugLine2[160];  // only for debug normal is 80

//int g_dst;  // transferbuffer users.csv
//int g_dst_is_group;
//int g_src;

// Table 6.1: Data Type information element definitions

// unused?
enum data_type {
    PI_HDR = 0,
    VOICE_LC_HDR = 1,
    TERM_WITH_LC = 2,
    CSBK = 3,
    MBC_HDR = 4,
    MBC_CONT = 5,
    DATA_HDR = 6,
    RATE_1_2_DATA = 7,
    RATE_3_4_DATA = 8,
    IDLE = 9,            
    RATE_1_DATA = 10            
};

typedef struct pkt {
    uint16_t hdr ;
    uint8_t b0 ;
    uint8_t b1 ;
    uint8_t unk1 ;
    adr_t dst ;
    adr_t src ;    
} pkt_t;

// 9.3.18 SAP identifier (SAP)
enum sap_t {
    UDT = 0,
    TCP = 1,
    UDP = 2,
    IP = 3,
    ARP = 4,
    PPD = 5,
    SD = 0xa, // Short Data 
};

typedef struct raw_sh_hdr {
    uint8_t b0 ;
    // carefull bitfields are dangerous.
    uint8_t sap : 4 ;  // bit 7..4 (reverse from normal)
    uint8_t ab2 : 4 ;  // bit 3..0 (reverse from normal)
    adr_t dst ;
    adr_t src ;    
    uint8_t sp : 3 ; 
    uint8_t dp : 3 ;
    uint8_t sf : 2 ; // S & F
} raw_sh_hdr_t;

// unvalidated.
void dump_raw_short_header( const char *tag, raw_sh_hdr_t *pkt )
{
    NMPRINT("%s(sap=%d,src=%d,dst=%d,sp=%d,dp=%d) ", tag, pkt->sap, get_adr(pkt->src), get_adr(pkt->dst), pkt->sp, pkt->dp );
    PRINT("%s(sap=%d,src=%d,dst=%d,sp=%d,dp=%d)\n", tag, pkt->sap, get_adr(pkt->src), get_adr(pkt->dst), pkt->sp, pkt->dp );
}

typedef struct lc_hdr {
    uint8_t pf_flco ;    
    uint8_t fid ;
} lc_hdr_t ;

// Control Signalling Block (CSBK) PDU
// TODO: finish / validate
typedef struct mbc {
    uint8_t lb_pf_csbko ;    
    uint8_t fid ;    
    union {
        struct {
            //uint8_t sap ; // ??
            adr_t dst ;
            adr_t src ;                
        } sms ;
    } ;	
} mbc_t ;

inline uint8_t get_csbko( mbc_t *mbc )
{
    return mbc->lb_pf_csbko & 0x3f ;
}


// unvalidated
void dump_mbc( mbc_t *mbc )
{
    uint8_t csbko = get_csbko(mbc);
    uint8_t fid = mbc->fid ;
    
    PRINT("csbko=%02x fid=%02x ", csbko, fid);
    PRINT("src=%d dst=%d\n",get_adr(mbc->sms.src),get_adr(mbc->sms.dst));
}

//void dump_data( data_hdr_t *data )
//{
//    //TODO: print DPF (6.1.1))
//    // 9.3.17 from part 1
//    int sap = get_sap(data);
//    int blocks = get_blocks(data);
//    int dpf = get_dpf(data);
//    PRINT("sap=%d %s dpf=%d %s src=%d dst=%d %d\n", sap, sap_to_str(sap), dpf, dpf_to_str(dpf), get_adr(data->src),get_adr(data->dst), blocks);
//}

void dumpraw_lc(uint8_t *pkt)
{
    uint8_t tp = (pkt[1] >> 4) ;
    PRINT("type=%d ", tp );
    
    lc_t *lc = (lc_t*)(pkt + 2);
    dump_full_lc(lc);

    uint8_t flco = get_flco(lc);
    
    if( flco != 0 && flco != 3 ) {
        PRINTHEX(pkt,14);        
        PRINT("\n");
    }
}

// unvalidated
void dumpraw_mbc(uint8_t *pkt)
{
    uint8_t tp = (pkt[1] >> 4) ;
    PRINT("type=%d ", tp );

    mbc_t *mbc = (mbc_t*)(pkt + 2);
    dump_mbc(mbc);
}

//void dumpraw_data(uint8_t *pkt)
//{
//    uint8_t tp = (pkt[1] >> 4) ;
//    PRINT("type=%d ", tp );
//
//    data_hdr_t *data = (data_hdr_t*)(pkt + 2);
//    dump_data(data);
//}

#ifdef FW_D13_020
void dmr_CSBK_handler_hook(uint8_t *pkt)
{
//    PRINTRET();
//    PRINT("CSBK: ");
//    PRINTHEX(pkt,14);
//    PRINT("\n");

    dmr_CSBK_handler(pkt);
}
#else
#warning please consider hooking this handler.
#endif

void *dmr_call_end_hook(uint8_t *pkt)
{
    /* This hook handles the dmr_contact_check() function, calling
       back to the original function where appropriate.

       pkt points to something like this:

                      /--dst-\ /--src-\
       08 2a 00 00 00 00 00 63 30 05 54 7c 2c 36

       In a clean, simplex call this only occurs once, but on a
       real-world link, you'll find it called multiple times at the end
       of the packet.
     */

    {
        lc_t *data = (void*)(pkt + 2);
        rst_term_with_lc( data );
    }

    //Forward to the original function.
    return dmr_call_end(pkt);
}

void *dmr_call_start_hook(uint8_t *pkt)
{
//    PRINTRET();
//    PRINTHEX(pkt,11);
//    PRINT("\n");

    /* This hook handles the dmr_contact_check() function, calling
       back to the original function where appropriate.

       It is called several times per call, presumably when the
       addresses are resent for late entry.  If you need to trigger
       something to happen just once per call, it's better to put that
       in dmr_call_end_hook().

       pkt looks like this:

       overhead
       /    /         /--dst-\ /--src-\
       08 1a 00 00 00 00 00 63 30 05 54 73 e3 ae
       10 00 00 00 00 00 00 63 30 05 54 73 2c 36
     */

//    //Destination adr as Big Endian.
//    int dst = (pkt[7] |
//            (pkt[6] << 8) |
//            (pkt[5] << 16));
//
//    int src = (pkt[10] |
//            (pkt[9] << 8) |
//            (pkt[8] << 16));
//            
//    int groupcall = (pkt[2] & 0x3F) == 0;

    {
        lc_t *data = (void*)(pkt + 2);
        rst_voice_lc_header( data );
    }

    //  OSSemPend(debug_line_sem, 0, &err);
    //
    //printf("Call start %d -> %d\n", src,dst);
    //  sprintf(DebugLine1, "%d -> %d", src, dst );

    //  if( find_dmr_user(DebugLine2, src, (void *) 0x100000, 80) == 0){
    //    sprintf(DebugLine2, ",ID not found,in users.csv,see README.md,on Github");   // , is line seperator ;)
    //  }

    //  OSSemPost(debug_line_sem);

//    int primask = OS_ENTER_CRITICAL();
//    g_dst = dst;
//    g_dst_is_group = groupcall;
//    g_src = src;
//    OS_EXIT_CRITICAL(primask);

    //Forward to the original function.
    return dmr_call_start(pkt);
}

void dmr_apply_squelch_hook(OS_EVENT *event, char * mode)
{
#ifdef CONFIG_DMR
    /* The *mode byte is 0x09 for an unmuted call and 0x08 for a muted
       call.
     */

    //printf("dmr_apply_squelch_hook for *mode=0x%02x.\n",*mode);

    if( *mode == 0x8 ) {
        rst_signal_other_call();
    }
    if( *mode == 0x9 ) {
        rst_signal_my_call();
    }

    //Promiscuous mode!
    if( *mode == 0x08 && global_addl_config.promtg == 1 ) {
	if(global_addl_config.devmode_level >= 3) {				// verbose USB mode must be enabled to show debug messages
        	printf("Applying monitor mode to a public call.\n");
	}
        *mode = 0x09;

        /* I'm not quite sure what this function does, but it must be
           called before dmr_apply_squelch() if the squelch mode is being
           changed. --Travis
         */
        dmr_before_squelch();
    }

    /* This is really OSMboxPost().  We should probably change up these
       names now that we're figuring out what the functions really
       do. --Travis
     */
    md380_OSMboxPost(event, mode);
#endif
}

void dmr_apply_privsquelch_hook(OS_EVENT *event, char *mode)
{
#ifdef CONFIG_DMR
    /* The *mode byte is 0x09 for an unmuted call and 0x08 for a muted
       call.
     */

    //printf("dmr_apply_squelch_hook for *mode=0x%02x.\n",*mode);

    if( *mode == 0x8 ) {
        rst_signal_other_call();
    }
    if( *mode == 0x9 ) {
        rst_signal_my_call();
    }

    //Promiscuous mode!
    if( *mode == 0x08 && global_addl_config.promtg == 1 ) {
	if(global_addl_config.devmode_level >= 3) {				// verbose USB mode must be enabled to show debug messages
	        printf("Applying monitor mode to a private call.\n");
	}
        *mode = 0x09;
        dmr_before_squelch();
    }
    md380_OSMboxPost(event, mode);
#endif
}

void *dmr_handle_data_hook(char *pkt, int len)
{
    //    PRINTRET();
    //    PRINTHEX(pkt,len);
    //    PRINT("\n");

#ifdef CONFIG_DMR
    /* This hook handles the dmr_contact_check() function, calling
       back to the original function where appropriate.

       Packes are up to twelve bytes, but they are always preceeded by
       two bytes of C5000 overhead.
     */

	if(global_addl_config.devmode_level >= 3) {				// verbose USB mode must be enabled to show debug messages
		red_led(1);							// turn on the red LED to know that we're here.
		printf("Data:       ");
		printhex(pkt, len + 2);
		printf("\n");
	}

    {
        data_blk_t *data = (void*)(pkt + 2);
        rst_data_block(data,len);
    }

    //Forward to the original function.
    return dmr_handle_data(pkt, len);
#else
    return 0xdeadbeef;
#endif
}

void *dmr_sms_arrive_hook(void *pkt)
{
#ifdef CONFIG_DMR

    /* This hooks the SMS arrival routine, 
       dmr_sms_arrive() only handles the header and not the actual
       data payload, which is managed by dmr_handle_data() in each
       fragment chunk.

     *pkt points to a twelve byte header with two bytes of C5000
       overhead.  The body packets will arrive at dmr_handle_data_hook()
       in chunks of up to twelve bytes, varying by data rate.

       A full transaction from 3147092 to 99 looks like this:

               header
               |   / /flg\ /--dst-\ /--src-\ /flg\ /crc\
  SMS header:  08 6a 02 40 00 00 63 30 05 54 88 00 83 0c
         Data: 08 7a 45 00 00 5c 00 03 00 00 40 11 5c a8
         Data: 08 7a 0c 30 05 54 0c 00 00 63 0f a7 0f a7
         Data: 08 72 00 48 d1 dc 00 3e e0 00 92 04 0d 00
         Data: 08 72 0a 00 54 00 68 00 69 00 73 00 20 00
         Data: 08 72 69 00 73 00 20 00 61 00 20 00 74 00
         Data: 08 7a 65 00 73 00 74 00 20 00 66 00 72 00
         Data: 08 7a 6f 00 6d 00 20 00 6b 00 6b 00 34 00
         Data: 08 7a 76 00 63 00 7a 00 21 00 9e 21 5a 5c
     */

	if(global_addl_config.devmode_level >= 3) {				// verbose USB mode must be enabled to show debug messages
		red_led(1);							// turn on the red LED to know that we're here.
		printf("SMS header: ");
		printhex(pkt, 12 + 2);
		printf("\n");
	}

    {
        data_hdr_t *data = (pkt + 2);
        rst_data_header(data);
    }
    
    //Forward to the original function.
    return dmr_sms_arrive(pkt);
#else
    return 0xdeadbeef;
#endif
}
