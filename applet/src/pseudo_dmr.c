/*
 *  pseudo_dmr.c
 * 
 *  Created on Sep 25, 2016 10:05:50 AM by Simon IJskes
 * 
 */

void F_414()
{
    if( mode3 == 5 ) {
        return ;
    }
 
// SP + 4 = buffer[0]
//    
//|           0x080407b2    9df80500     ldrb.w r0, [sp, 5]
//|           0x080407b6    c0b2         uxtb r0, r0
//|           0x080407b8    0409         lsrs r4, r0, 4
//|           0x080407ba    9df80500     ldrb.w r0, [sp, 5]
//|           0x080407be    10f00305     ands.w r5, r0, 3
//
//
//      
//
//
//    
//
//
//    
    
    // embedded signalling (or idle?)
    // from voice (6.1) & d&c (6.2) bursts
    
    // 6.2 slot type PDU 
    // 9.3.6 data type.
    
    // 0x0804087e
    int datatype = sp[5] & 0xF ;
    int colorcode = ( sp[5] & 0xF0 ) >> 4 ;
    
    if( unknown ) {
        // ...
        dmr_audio_start();
        return ;
    }
    
    switch( datatype ) {
        case 0 :
            // ?
            goto 0x080408a4 ;
        case 1 :
            // dmr audio start
            f();
            dmr_audio_start();
            return ;
        case 2 :
            // dmr call end
            goto 0x080408d0 ;
        case 3 :
            // CBSK
            0x08040028(130,0,buf,10);
            // post 7 to radio_event_mbox
            goto 0x080408e6 ;
        case 4 :
            // @ 0x08040912 ;
            // MBC header
            return ;
        case 5 :
            // @ 0x08040914 ;
            // MBC next
            return ;
        case 6 :
            // @ 0x08040916 ;
            // data_hdr
            // (sms arrive)
            0x08040028(130,0,buf,12);
            sms_arrive(buf);
            return ;
        case 7 :
            // data_rate_1_2
            // (dmr data)
            0x08040028(130,0,buf,12);
            goto 0x0804092c ;
        case 8 :
            // data_rate_3_4
            // {dmr data)
            0x08040028(130,0,buf,18);
            goto 0x08040944 ;
        case 9 :
            // idle
            // post 0x7 to radio_event_mbox
            // post 0x17 to other_event_mbox
            goto 0x0804095c ;
        case 10 :
            // data_rate_1
            // (dmr data)
            0x08040028(130,0,buf,24);
            goto 0x0804098a ;
        default:
            return ;
    }
    
}