/*
 *  display.c
 * 
 * for high-level drawing functions.
 * 
 */

#include "display.h"

#include "md380.h"
#include "version.h"
#include "tooldfu.h"
#include "config.h"
#include "gfx.h"
#include "printf.h"
#include "string.h"
#include "addl_config.h"
#include "ambe.h"
#include "usersdb.h"
#include "dmr.h"
#include "console.h"
#include "netmon.h"
#include "radiostate.h"
#include "unclear.h"
#include "etsi.h"
#include "app_menu.h"
#include "syslog.h"        // LOGB()
#include "irq_handlers.h"  // boot_flags, BOOT_FLAG_DREW_STATUSLINE
#include "lcd_driver.h"

#if defined(FW_D13_020) || defined(FW_S13_020)
	#include "amenu_set_tg.h"
	#include "codeplug.h"
#else
#warning old firmware
#endif 

//#include "amenu_channels.h"
#include <stdlib.h>

char eye_paltab[] = {
    0xd7, 0xd8, 0xd6, 0x00, 0x88, 0x8a, 0x85, 0x00, 0xe1, 0xe2, 0xe0, 0x00, 0xff, 0xff, 0xff, 0x00,
    0xae, 0xae, 0xaf, 0x00, 0x24, 0x4e, 0x8a, 0x00, 0x5d, 0x88, 0xbb, 0x00, 0xd1, 0xd2, 0xd4, 0x00,
    0xf4, 0xf4, 0xf4, 0x00, 0x3c, 0x66, 0x9f, 0x00, 0xdb, 0xe6, 0xf3, 0x00, 0x48, 0x73, 0xaa, 0x00,
    0xb6, 0xb8, 0xb4, 0x00, 0x5e, 0x6a, 0x77, 0x00
};
char eye_pix[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x11, 0x10, 0x00, 0x00, 0x23,
    0x41, 0x11, 0x31, 0x00, 0x01, 0x14, 0x55, 0x55, 0x61, 0x00, 0x21, 0x75, 0x88, 0x59, 0x94, 0x31, 0x3a, 0x85, 0x88, 0x56,
    0x57, 0x73, 0x21, 0x86, 0x55, 0x5b, 0x67, 0x41, 0x13, 0x48, 0x66, 0x69, 0x71, 0xc1, 0x0c, 0x13, 0x47, 0x33, 0x11, 0x10,
    0x00, 0x03, 0xdc, 0xd1, 0xd0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
const gfx_pal eye_pal = {14, 0, eye_paltab};
const gfx_bitmap bmp_eye = {12, 12, 6, 4, eye_pix, &eye_pal, 0};
int ch_ts = 0;				// 20180104 current channel TS 

#ifdef FW_D13_020
#define D_ICON_EYE_X 65
#define D_ICON_EYE_Y 1
#endif
#ifdef FW_S13_020
// on MD390 draw promiscous mode eye closed to S-Meter due to GPS-symbol an standard position
#define D_ICON_EYE_X 20
#define D_ICON_EYE_Y 1
#endif

void draw_eye_opt()
{
#if defined(FW_D13_020) || defined(FW_S13_020)
    // draw promiscous mode eye symbol 
    if( global_addl_config.promtg == 1 ) {
        gfx_drawbmp((char *) &bmp_eye, D_ICON_EYE_X, D_ICON_EYE_Y);
    }
#endif
}

// Takes a positive(!) integer amplitude and computes 200*log10(amp),
// centi Bel, approximtely. If the given parameter is 0 or less, this
// function returns -1.  tnx to sellibitze

int intCentibel(long ampli)
{
    if( ampli <= 0 )
        return -1; // invalid
    int log_2 = 0;
    while (ampli >= 32 * 8) {
        ampli >>= 1 + 3;
        log_2 += 1 + 3;
    }
    while (ampli >= 32) {
        ampli >>= 1;
        log_2 += 1;
    }
    // 1 <= ampli < 32
    static const short fine[] = {
        -1, 0, 60, 95, 120, 140, 156, 169,
        181, 191, 200, 208, 216, 223, 229, 235,
        243, 249, 253, 258, 262, 266, 270, 274,
        278, 281, 285, 288, 291, 294, 297, 300
    };
    return (log_2 * 301 + 2) / 5 + fine[ampli];
}

#define RX_POPUP_Y_START 24
#define RX_POPUP_X_START 10

void draw_txt(char* testStr, int x, int y, char font){
	char c=0;
	int maxLen=16;
	uint16_t fg_color = 0, bg_color = 0;
#if defined(FW_D13_020) || defined(FW_S13_020)
	Menu_GetColours(SEL_FLAG_NONE, &fg_color, &bg_color);
#endif
	while( ((c=*testStr)!=0)  && maxLen>0)
	{ x = LCD_DrawCharAt( c, x, y, fg_color, bg_color, font);
		//++i; // character index and limiting counter
	    ++testStr; 
		// (in rare cases, some of the leading text may be OVERWRITTEN below)
		maxLen--;
	}		 
}

int fDoOnce = 0;

void draw_micbargraph()
{
    if( gui_opmode2 == OPM2_MENU
# if (CONFIG_APP_MENU)
            || Menu_IsVisible()
#endif
    ) {
        // case for pressing the PTT during 'Manual Dial' in 'Contacts', or if 'app menu' is visible
        return ;
    }
    
    static int rx_active; // flag to syncronice this hook ( operatingmode == 0x11 is also on rx seeded)
    static int fullscale_offset = 0;
    static uint32_t lastframe = 0;
    static int red = 0;
    static int green = 0;

    int relative_peak_cb;
    int centibel_val;

    if( fullscale_offset == 0 ) { // init int_centibel()
        fullscale_offset = intCentibel(3000); // maybe wav max max_level
    }
    
    int is_tx = 0 ;
    int is_rx = 0 ;

    is_tx = gui_opmode1 == SCR_MODE_RX_VOICE && max_level > 10 ;
    is_rx = gui_opmode1 == SCR_MODE_RX_TERMINATOR ;

#if defined(FW_D13_020) || defined(FW_S13_020)
    {
//        uint8_t *rs = (void*)0x2001e5f0 ;
        uint8_t s = radio_status_1.m1 ;
        
        is_tx = s & 1 ;
        is_rx = s & 2 ;
    }
#endif    

    if( is_tx && max_level < 4500 ) { 
        if( lastframe < ambe_encode_frame_cnt ) { // check for new frame
            lastframe = ambe_encode_frame_cnt;
            rx_active = 1;

            relative_peak_cb = intCentibel(max_level) - fullscale_offset;
            centibel_val = relative_peak_cb;


            if( lastframe % 5 == 1 ) { // reduce drawing
                if( centibel_val < -280 ) { // limit 160 pixel bargraph 10 150 -> 140 pixel for bargraph
                    centibel_val = -280;
                } else if( centibel_val > 0 ) {
                    centibel_val = 0;
                }
                centibel_val += 280; // shift to positive
                centibel_val /= 2; // scale

                gfx_set_fg_color(0x999999);
                gfx_set_bg_color(0xff000000);
                gfx_blockfill(9, 54, 151, 66);

                // paint legend
                gfx_set_fg_color(0x0000ff);
                gfx_blockfill((-30 + 280) / 2 + 10, 67, 150, 70);
                gfx_set_fg_color(0x00ff00);
                gfx_blockfill((-130 + 280) / 2 + 10, 67, (-30 + 280) / 2 - 1 + 10, 70);
                gfx_set_fg_color(0x555555);
                gfx_blockfill(10, 67, (-130 + 280) / 2 - 1 + 10, 70);

                // set color
                if( relative_peak_cb > -3 || red > 0 ) {
                    if( red > 0 ) red--;
                    if( relative_peak_cb > -3 ) red = 30;
                    gfx_set_fg_color(0x0000ff);
                } else if( relative_peak_cb > -130 || green > 0 ) {
                    if( green > 0 ) green--;
                    if( relative_peak_cb > -130 ) green = 30;
                    gfx_set_fg_color(0x00ff00);
                } else {
                    gfx_set_fg_color(0x555555);
                }
                gfx_set_bg_color(0xff000000);
                gfx_blockfill(10, 55, centibel_val, 65);
                gfx_set_fg_color(0xff8032);
                gfx_set_bg_color(0xff000000);
            }
        }
    }

    if( is_rx && rx_active == 1 ) { // clear screen area
        gfx_set_fg_color(0xff8032);
        gfx_set_bg_color(0xff000000);
        gfx_blockfill(9, 54, 151, 70);
        rx_active = 0;
        red = 0;
        green = 0;
    }
}

#define RX_POPUP_Y_START 24
#define RX_POPUP_X_START 10

void draw_rx_screen(unsigned int bg_color)
{
    static int dst;
    int src;
    int grp ;
    
    int primask = OS_ENTER_CRITICAL(); // for form sake
//    dst = g_dst;
//    src = g_src;
    
    dst = rst_dst ;
    src = rst_src ;
    grp = rst_grp ;
    
    OS_EXIT_CRITICAL(primask);

    // clear screen
    gfx_set_fg_color(bg_color);
    gfx_blockfill(0, 16, MAX_X, MAX_Y); 

    gfx_set_bg_color(bg_color);
    gfx_set_fg_color(0x000000);
    gfx_select_font(gfx_font_small);

    user_t usr ;
    
    if( usr_find_by_dmrid(&usr,src) == 0 ) {
        usr.callsign = "ID unknown" ;
        usr.firstname = "" ;
        usr.name = "No entry in" ;
        usr.place = "your users.csv" ;
        usr.state = "see README.md" ;
        usr.country = "on Github" ;
    }

    int y_index = RX_POPUP_Y_START;
    int scr_row = RX_POPUP_Y_START;

    if ( global_addl_config.userscsv > 1 && talkerAlias.length > 0 )		// 2017-02-19 show Talker Alias depending on setup 0=CPS 1=DB 2=TA 3=TA & DB
    {
    	if( grp ) {
	    if( global_addl_config.lh_tsstat == 0 ) {
			gfx_printf_pos( RX_POPUP_X_START, scr_row, "%d->TG%d", src, dst );
		} else {
			gfx_printf_pos( RX_POPUP_X_START, scr_row, "%d->TS%d TG%d", src, lh_ts, dst );
		}
	} else {
	    if( global_addl_config.lh_tsstat == 0 ) {
			gfx_printf_pos( RX_POPUP_X_START, scr_row, "%d-> ID:%d", src, dst );
		} else {
	                gfx_printf_pos( RX_POPUP_X_START, scr_row, "%d->TS%d ID:%d", src, lh_ts, dst );
		}
	}
	scr_row += GFX_FONT_SMALL_HEIGHT ;

	gfx_select_font(gfx_font_norm);

 	if ( global_addl_config.userscsv > 1 && talkerAlias.length > 0 )		// 2017-02-19 show Talker Alias depending on setup 0=CPS 1=DB 2=TA 3=TA & DB
 	{    
		gfx_printf_pos2(RX_POPUP_X_START, scr_row, 10, "%s", talkerAlias.text );
 	        scr_row += GFX_FONT_NORML_HEIGHT; // previous line was in big font
 	} else {
		gfx_printf_pos2(RX_POPUP_X_START, scr_row, 10, "DMRID: %d", src );
		scr_row += GFX_FONT_NORML_HEIGHT; // previous line was in big font
 	}

	gfx_select_font(gfx_font_small);

	switch( global_addl_config.userscsv ) {
	        case 0 :
		gfx_puts_pos(RX_POPUP_X_START, scr_row, "Userinfo: CPS mode");
            	break ;
	   
		case 1 :
		gfx_puts_pos(RX_POPUP_X_START, scr_row, "Userinfo: UserDB mode");
            	break ;

	        case 2 :
		if ( talkerAlias.length > 0 )  {
			gfx_puts_pos(RX_POPUP_X_START, scr_row, "Userinfo: TalkerAlias");
		} else {
			gfx_puts_pos(RX_POPUP_X_START, scr_row, "Userinfo: TA not rcvd!");
		}
            	break ;

	        case 3 :
		gfx_puts_pos(RX_POPUP_X_START, scr_row, "Userinfo: TA/DB mode");
            	break ;
	   }
	   scr_row += GFX_FONT_SMALL_HEIGHT ; // previous line was in small font


           gfx_select_font(gfx_font_small);
           gfx_puts_pos(RX_POPUP_X_START, scr_row, "------------------------");
           scr_row += GFX_FONT_SMALL_HEIGHT ;

	   if( global_addl_config.userscsv == 3 )	// 3 = TA & DB
	   {
   	        gfx_printf_pos(RX_POPUP_X_START, scr_row, "%s %s", usr.callsign, usr.firstname );
		scr_row += GFX_FONT_SMALL_HEIGHT ; // previous line was in small font

		gfx_puts_pos(RX_POPUP_X_START, scr_row, usr.country );
		scr_row += GFX_FONT_SMALL_HEIGHT ;
 	   }

    } else {	
    
    gfx_select_font(gfx_font_small);
    if( grp ) {
	if( global_addl_config.lh_tsstat == 0 ) {
		gfx_printf_pos( RX_POPUP_X_START, y_index, "%d->TG%d", src, dst );
	} else {
        	gfx_printf_pos( RX_POPUP_X_START, y_index, "%d->TS%d TG%d", src, lh_ts, dst );
	}
    } else {
	if( global_addl_config.lh_tsstat == 0 ) {
        	gfx_printf_pos( RX_POPUP_X_START, y_index, "%d-> ID:%d", src, dst );
	} else {
        	gfx_printf_pos( RX_POPUP_X_START, y_index, "%d->TS%d ID:%d", src, lh_ts, dst );
	}
    }
    y_index += GFX_FONT_SMALL_HEIGHT ;

    gfx_select_font(gfx_font_norm);
    gfx_printf_pos2(RX_POPUP_X_START, y_index, 10, "%s %s", usr.callsign, usr.firstname );
    y_index += GFX_FONT_NORML_HEIGHT; // previous line was in big font
    
    gfx_select_font(gfx_font_small);

    if ( global_addl_config.userscsv > 1 && talkerAlias.length > 0 )		// 2017-02-19 show Talker Alias depending on setup 0=CPS 1=DB 2=TA 3=TA & DB
    {	
	gfx_puts_pos(RX_POPUP_X_START, y_index, talkerAlias.text );
    } else {
	gfx_puts_pos(RX_POPUP_X_START, y_index, usr.name );
    }
    y_index += GFX_FONT_SMALL_HEIGHT ; // previous line was in small font

    gfx_puts_pos(RX_POPUP_X_START, y_index, usr.place );
    y_index += GFX_FONT_SMALL_HEIGHT ;
    
    gfx_puts_pos(RX_POPUP_X_START, y_index, usr.state );
    y_index += GFX_FONT_SMALL_HEIGHT ;
    
    gfx_puts_pos(RX_POPUP_X_START, y_index, usr.country );
    y_index += GFX_FONT_SMALL_HEIGHT ;
    }
    
    gfx_select_font(gfx_font_norm);
    gfx_set_fg_color(0xff8032);
    gfx_set_bg_color(0xff0000);
}

void draw_ta_screen(unsigned int bg_color)
{
    int dst;
    int src;
    int grp ;
    
    int primask = OS_ENTER_CRITICAL(); // for form sake
    int scr_row = RX_POPUP_Y_START;
    
    dst = rst_dst ;
    src = rst_src ;
    grp = rst_grp ;
    
    OS_EXIT_CRITICAL(primask);
 
    // clear screen
    gfx_set_fg_color(bg_color);
    gfx_blockfill(0, 16, MAX_X, MAX_Y); 

    gfx_set_bg_color(bg_color);
    gfx_set_fg_color(0x000000);
    gfx_select_font(gfx_font_small);

    user_t usr ;
    
    gfx_select_font(gfx_font_small);
    if( grp ) {
	if( global_addl_config.lh_tsstat == 0 ) {
        	gfx_printf_pos( RX_POPUP_X_START, scr_row, "%d->TG%d", src, dst );
	} else {
        	gfx_printf_pos( RX_POPUP_X_START, scr_row, "%d->TS%d TG%d", src, lh_ts, dst );
	}
    } else {
	if( global_addl_config.lh_tsstat == 0 ) {
		gfx_printf_pos( RX_POPUP_X_START, scr_row, "%d->ID:%d", src, dst );
	} else {
		gfx_printf_pos( RX_POPUP_X_START, scr_row, "%d->TS%d ID:%d", src, lh_ts, dst );
	}
    }
    scr_row += GFX_FONT_SMALL_HEIGHT ;

    gfx_select_font(gfx_font_norm);

    if ( global_addl_config.userscsv > 1 && talkerAlias.length > 0 )		// 2017-02-19 show Talker Alias depending on setup 0=CPS 1=DB 2=TA 3=TA & DB
    {
	gfx_printf_pos2(RX_POPUP_X_START, scr_row, 10, "%s", talkerAlias.text );
        scr_row += GFX_FONT_NORML_HEIGHT;
    } else {
	gfx_printf_pos2(RX_POPUP_X_START, scr_row, 10, "DMRID: %d", src );
	scr_row += GFX_FONT_NORML_HEIGHT;
    }

    gfx_select_font(gfx_font_small);
    if ( talkerAlias.length > 0 )  {
	gfx_puts_pos(RX_POPUP_X_START, scr_row, "Userinfo: TalkerAlias");
    } else {
	gfx_puts_pos(RX_POPUP_X_START, scr_row, "Userinfo: TA not rcvd!");
    }

    scr_row += GFX_FONT_SMALL_HEIGHT ;
    gfx_select_font(gfx_font_small);
    gfx_puts_pos(RX_POPUP_X_START, scr_row, "------------------------");
    scr_row += GFX_FONT_SMALL_HEIGHT ;

    if( usr_find_by_dmrid(&usr,src) == 1 || usr_find_by_dmrid(&usr,src) == 3 )
    {
        gfx_printf_pos(RX_POPUP_X_START, scr_row, "%s %s", usr.callsign, usr.firstname );
	scr_row += GFX_FONT_SMALL_HEIGHT ;

	gfx_puts_pos(RX_POPUP_X_START, scr_row, usr.country );
	scr_row += GFX_FONT_SMALL_HEIGHT ;
    }
    
    gfx_select_font(gfx_font_norm);
    gfx_set_fg_color(0xff8032);
    gfx_set_bg_color(0xff0000);
}


/*
#include <stdlib.h>

int main(void)
{
    long peak = 6000;
    if (peak > 0) {
	int fullscale_offset = int_centibel(32767);
	int relative_peak_cb = int_centibel(peak) - fullscale_offset;
	printf("%i.%i dBFS\en", relative_peak_cb / 10,
	       abs(relative_peak_cb % 10));
    } else {
	printf("-Inf dBFS\n");
    }
    return 0;
}
*/


void draw_statusline_hook( uint32_t r0 )
{
   if( ! (boot_flags & BOOT_FLAG_DREW_STATUSLINE) )
    { LOGB("t=%d: draw_stat\n", (int)IRQ_dwSysTickCounter ); // 4383(!) SysTicks after power-on
    }
   boot_flags |= BOOT_FLAG_DREW_STATUSLINE; // important for SysTick_Handler to know when we're "open for business" !

# if (CONFIG_APP_MENU)
    // If the screen is occupied by the optional 'red button menu', 
    // update or even redraw it completely:
    if( Menu_DrawIfVisible(AM_CALLER_STATUSLINE_HOOK) )  
     { return; // the menu covers the entire screen, so don't draw anything else
     }
    // NOTE: draw_statusline_hook() isn't called when the squelch
    //       is 'open' in FM, i.e. when the channel is BUSY .
    //       -> call Menu_DrawIfVisible() from other places, too.
# endif // CONFIG_APP_MENU ?

    if( is_netmon_visible() ) {
        con_redraw();
        return ;
    }
    draw_statusline( r0 );
}
	
void draw_alt_statusline()
{
    int src;
    user_t usr;
    user_t dst;
    src = rst_src;

    gfx_set_fg_color(0);
    gfx_set_bg_color(0xff8032);
    gfx_select_font(gfx_font_small);

    char mode = ' ' ;
    if( rst_voice_active ) {
        if( rst_mycall ) {
            mode = '*' ; // on my tg            
        } else {
            mode = '!' ; // on other tg
        }
    }
    
    if( src == 0 ) {
	if ( global_addl_config.datef == 5 )
	{
	        gfx_printf_pos2(RX_POPUP_X_START, 96, 157, "lh:");
	} else {
	        gfx_printf_pos2(RX_POPUP_X_START, 96, 157, "TA:");
	}
    } else {
	if ( global_addl_config.datef == 6 && talkerAlias.length > 0 )				// 2017-02-18 show talker alias in status if rcvd valid
	{
		if( global_addl_config.lh_tsstat == 0 ) {
			gfx_printf_pos2(RX_POPUP_X_START, 96, 157, "TA:%s", talkerAlias.text);
		} else {
			gfx_printf_pos2(RX_POPUP_X_START - 7, 96, 157, "[%d] TA:%s", lh_ts, talkerAlias.text);
		}
	} else {										// 2017-02-18 otherwise show lastheard in status line
										
		switch ( usr_find_by_dmrid(&usr, src) ) {  					// lookup source DMRID from UserDB
			case 0 :
				if( usr_find_by_dmrid(&dst, rst_dst) != 0 ) {			// lookup destination DMRID from UserDB and show if found
					if( global_addl_config.lh_tsstat == 0 ) {
						gfx_printf_pos2(RX_POPUP_X_START, 96, 157, "lh:%d->%s %c", src, dst.callsign, mode);
					} else {
						gfx_printf_pos2(RX_POPUP_X_START - 7, 96, 157, "[%d] lh:%d->%s %c", lh_ts, rst_src, dst.callsign, mode);
					}
				} else  {
					if( global_addl_config.lh_tsstat == 0 ) {
	        				gfx_printf_pos2(RX_POPUP_X_START, 96, 157, "lh:%d->%d %c", src, rst_dst, mode);
					} else {
	        				gfx_printf_pos2(RX_POPUP_X_START - 7, 96, 157, "[%d] lh:%d->%d %c", lh_ts, rst_src, rst_dst, mode);
					}
				}
				break;

	    		case 1 : 
		                if( usr_find_by_dmrid(&dst, rst_dst) != 0 ) {			// lookup destination DMRID from UserDB and show if found
					if( global_addl_config.lh_tsstat == 0 ) {
						gfx_printf_pos2(RX_POPUP_X_START, 96, 157, "lh:%s->%s %c", usr.callsign, dst.callsign, mode);
					} else {
						gfx_printf_pos2(RX_POPUP_X_START - 7, 96, 157, "[%d] lh:%s->%d %c", lh_ts, usr.callsign, rst_dst, mode);
					}
		                } else  {
					if( global_addl_config.lh_tsstat == 0 ) {
						gfx_printf_pos2(RX_POPUP_X_START, 96, 157, "lh:%s->%d %c", usr.callsign, rst_dst, mode);
					} else {
						gfx_printf_pos2(RX_POPUP_X_START - 7, 96, 157, "[%d] lh:%s->%d %c", lh_ts, usr.callsign, rst_dst, mode);
					}
				}
				break;
		}
	}
    }
    
    gfx_set_fg_color(0);
    gfx_set_bg_color(0xff000000);
    gfx_select_font(gfx_font_norm);
}

#if defined(FW_D13_020) || defined(FW_S13_020)	
void draw_adhoc_statusline()
{
//	int x = RX_POPUP_X_START + 36;							// 36=standard position aligned with channel info
	int x = RX_POPUP_X_START + 35;
//	int y = 55;									// 55=standard position from top
	int y = 53;
	int top_y = 17;									// upper status below fw statusline

	gfx_set_fg_color(0x000000);
	gfx_set_bg_color(0xff8032);
	gfx_select_font(gfx_font_small);

	char top_status[25];								// top status line
	char bot_status[25];								// bottom status line

	char ch_rx[12];
	char ch_tx[12];
	char freq_rx[12];
	char freq_tx[12];

//	char ch_mode[3];								// DMR / FM / FM-N / FM-W
//	char ch_wide[2];								// DMR / FM / FM-N / FM-W
//	char ch_rpt[4];									// [-R] / [+R] repeater shift
//	char dmr_cc[2];									// [CC1] color code
//	char dmr_compact[5];								// [1|2| ... CC/TS prefix
	char ch_offset[5];								// repeater offset
//	char ch_tmp[10];								// temp
//	char ch_cc[1];									// temp CC

	char fm_bw_stat[3];								// |N or |W
	char mic_gain_stat[5];								// off, 3dB, 6dB
	char fm_sql[4];									// CTS oder DCS
	char tg_fill[7];								// talkgroup space filler

	char ch_tone_type[2];								// N=none D=DCS 0-9=CTS
//	long ch_rxfreq = 0;
//	long ch_txfreq = 0;
//	float ch_freqoff = 0;

	strncpy(ch_rx, current_channel_info_E.rxFreq.text, 12);				// read RX frequency from codeplug
	strncpy(ch_tx, current_channel_info_E.txFreq.text, 12);				// read TX frequency from codeplug

	strncpy(freq_rx, current_channel_info_E.rxFreq.text, 12);				// read RX frequency from codeplug
	strncpy(freq_tx, current_channel_info_E.txFreq.text, 12);				// read TX frequency from codeplug

	//strcat(ch_rx, '\0');
	//strcat(ch_tx, '\0');

	strncpy(ch_tone_type, current_channel_info_E.EncTone.text, 1);
	ch_tone_type[1] = '\0';

//	ch_rxfreq = atol(ch_rx);
//	ch_txfreq = atol(ch_tx);
	//sprintf(ch_rxfreq, ch_rx);
	//sprintf(ch_txfreq, ch_tx);

//	ch_freqoff = ((ch_rxfreq - ch_txfreq) / 100000);

	user_t usr;									// reference user DB
	
     channel_info_t *ci = &current_channel_info ;
     BOOL fIsWideBandwidth = ( ci->mode >> 3 ) & 0x1 ;
    
	//========================================================================================================================//
	// First build general mode independent status info 				// RPT shift and Mic gain
	//========================================================================================================================//

	if (strcmp(ch_rx, ch_tx) == 0) {
		if (global_addl_config.mode_stat != 3) {				// if MODE/CC compact display set in config
			strcpy(ch_offset, "|   ");
		} else {
			strcpy(ch_offset, "|  ");
		}
	} else if (strcmp(ch_rx, ch_tx) > 0) {
		if (global_addl_config.mode_stat != 3) {				// if MODE/CC compact display set in config
			strcpy(ch_offset, "|-R|");
		} else {
			strcpy(ch_offset, "|-R");
		}
	} else {
		if (global_addl_config.mode_stat != 3) {				// if MODE/CC compact display set in config
			strcpy(ch_offset, "|+R|");
		} else {
			strcpy(ch_offset, "|+R");
		}
	}

	if (global_addl_config.mode_stat > 1) {						// if MODE/CC/gain display set in config for both modes (DMR/FM)
		if (global_addl_config.mic_gain == 0) {
			strcpy(mic_gain_stat, "|0dB");
		} else if (global_addl_config.mic_gain == 1) {
			strcpy(mic_gain_stat, "|3dB");
		} else if (global_addl_config.mic_gain == 2) {
			strcpy(mic_gain_stat, "|6dB");
		}
	} else {
		strcpy(mic_gain_stat, "    ");						// blank if now mic gain display status selected
        }

//	BOOL fIsAnalog = current_channel_info_E.bIsAnalog;
	BOOL fIsDigital = current_channel_info_E.bIsDigital;
     BOOL fIsCTSvalid = (strlen(current_channel_info_E.EncTone.text) <= 6 );

	// the top statusline is build by the following strings:
	// -----------------------------------------------------
	//      |         DMR  |-R|
	//      |         DMR |-R[1|2|2623445]  --- [n|n|   = 5 DMR compact mode
	//      |         DMR  |-R| [CC1]
	//      |         FM |N|-R| [CTS]
	//      |         FM |N|-R| [DCS]
	//                 !  !  !    !
	//                 !  !  !    ! 
	//                 !  !  !    +------------- [CCn]    = 5
	//                 !  !  +------------------ |-R|     = 4
	//                 !  +--------------------- |N or |W = 2
	//                 +------------------------ Mode     = 3

	//========================================================================================================================//
	if (!fIsDigital) {								// DMR channel active
	//========================================================================================================================//
		int ch_cc = current_channel_info_E.CC;					// current color code
		ch_ts = current_channel_info_E.Slot;					// current timeslot
		int tgNum = (ad_hoc_tg_channel ? ad_hoc_talkgroup : current_TG());	// current talkgroup
		int callType = (ad_hoc_tg_channel ? ad_hoc_call_type : contact.type);	// current calltype
		//sprintf(dmr_cc, ch_cc);

		// build the top statusline -------------------------------------------------------------------
		if (global_addl_config.mode_stat != 3) {				// if MODE/CC compact display set in config
		strcpy(top_status, "DMR  ");						// init DMR string
		} else {
		strcpy(top_status, "DMR");						// init DMR string compact
		}

		strcat(top_status, ch_offset);						// DMR + repeaterstatus

		if (global_addl_config.mode_stat != 3) {				// if MODE/CC compact display set in config
			strcat(top_status, " [CC");					// DMR [-R] [CCn]
		} else {
			strcat(top_status, "[");					// DMR [-R] [CCn] in compact mode
		}

		// build some spaces between [CC|TS|TG] and db-Status -----------------------------------------
		if (tgNum > 999999) {
			strcpy(tg_fill, "");
		} else if (tgNum > 99999) {
			strcpy(tg_fill, "");
		} else if (tgNum > 9999) {
			strcpy(tg_fill, "");
		} else if (tgNum > 999) {
			strcpy(tg_fill, "");
		} else if (tgNum > 99) {
			strcpy(tg_fill, "");
		} else if (tgNum > 9) {
			strcpy(tg_fill, " ");
		} else {
			strcpy(tg_fill, "  ");
		}

		// ... the remaining info about DCS/CTS/CC is build dynamically during output

		// build the bottom statusline ----------------------------------------------------------------
		strcpy(bot_status, "TS:");						// init bottom string
		// ... the remaining info about TG/adhoc TG/private ID is build dynamically during output


		if (global_addl_config.mode_stat != 0) { 
			if (global_addl_config.mode_color == 1) { gfx_set_fg_color(0xffffff); gfx_set_bg_color(0xff4f32);}
				if (global_addl_config.mode_stat != 3) {					// if MODE/CC compact display set in config
					gfx_printf_pos2(x, top_y, 120, "%s%d]%s ", top_status, ch_cc, mic_gain_stat);
				} else {
					gfx_printf_pos2(x, top_y, 120, "%s%d|%d|%s%s%d]%s%s   ", top_status, ch_cc, ch_ts, (ad_hoc_tg_channel ? "A":""), (callType == CONTACT_GROUP || callType == CONTACT_GROUP2 ? "" : "P"), tgNum, tg_fill, mic_gain_stat);
				}
			gfx_set_fg_color(0x000000);
			gfx_set_bg_color(0xff8032);
		}

		if (global_addl_config.chan_stat != 0) {
  		    if (usr_find_by_dmrid(&usr, tgNum) == 0) {
			if (global_addl_config.chan_color == 1) { gfx_set_fg_color(0x261162); gfx_set_bg_color(0xff9f32);}

			if (global_addl_config.chan_stat == 1) {						// show TS / TG / CTS / DCS status
				if (global_addl_config.mode_stat != 3) {					// if MODE/CC compact display set in config 
					gfx_printf_pos2(x, y, 120, "%s%d %s%s:%d          ", bot_status, ch_ts, (ad_hoc_tg_channel ? "Ad" : ""), (callType == CONTACT_GROUP || callType == CONTACT_GROUP2 ? "TG" : "Priv"), tgNum);
				} else {
					if (global_addl_config.chan_stat != 4) {		// top=compact - bottom not rx/tx, so show rx, or if 3 = tx
						gfx_printf_pos2(x, y, 120, "%s:%s MHz   ", (global_addl_config.chan_stat == 3 ? "TX" : "RX"), (global_addl_config.chan_stat == 3 ? freq_tx : freq_rx) );
					} else {

						gfx_printf_pos2(x, y, 120, "%s:%s MHz   ", "RX", freq_rx );
						gfx_printf_pos2(x, y + 10, 120, "%s:%s MHz   ", "TX", freq_tx);
					}
				}
			} else {
				if (global_addl_config.chan_stat != 4) {
					//gfx_printf_pos2(x, y, 120, "%s:%s MHz   ", (global_addl_config.chan_stat == 3 ? "TX" : "RX"), (global_addl_config.chan_stat == 3 ? ch_tx : ch_rx) );
					gfx_printf_pos2(x, y, 120, "%s:%s MHz   ", (global_addl_config.chan_stat == 3 ? "TX" : "RX"), freq_rx );
				} else {

					gfx_printf_pos2(x, y, 120, "%s:%s MHz   ", "RX", freq_rx );
					gfx_printf_pos2(x, y + 10, 120, "%s:%s MHz   ", "TX", freq_tx);
				}
			}

		    } else {
			if (global_addl_config.chan_color == 1) { gfx_set_fg_color(0x261162); gfx_set_bg_color(0xff9f32);}
			if (global_addl_config.chan_stat == 1) { 
				//gfx_printf_pos2(x, y, 320, "%s - %s", (ad_hoc_call_type == CONTACT_GROUP ? "TG" : "Priv"), usr.callsign);
				gfx_printf_pos2(x, y, 120, "%s%d %s%s:%s          ", bot_status, ch_ts, (ad_hoc_tg_channel ? "Ad" : ""), (callType == CONTACT_GROUP || callType == CONTACT_GROUP2 ? "TG" : "Priv"), usr.callsign);
			} else {
					if (global_addl_config.chan_stat < 4) {
						gfx_printf_pos2(x, y, 120, "%s:%s MHz   ", (global_addl_config.chan_stat == 3 ? "TX" : "RX"), (global_addl_config.chan_stat == 3 ? freq_tx : freq_rx) );
					} else {
						gfx_printf_pos2(x, y, 120, "RX:%s MHz   ", freq_rx );
						gfx_printf_pos2(x, y + 10, 120, "TX:%s MHz   ", freq_tx );
					}
			}
		    }
		}
	//========================================================================================================================//
	}	 				// analog channel active
	//========================================================================================================================//
	else {
		if ( *ch_tone_type == 'N') {
			strcpy(fm_sql, "Off");
			strcpy(bot_status, "TX:");					// init bottom string
			strcat(bot_status, ch_tx);					// concat tx frequency
			strcat(bot_status, "MHz");
			strcpy(tg_fill, "   ");
		} else if ( *ch_tone_type == 'D')  {
			strcpy(fm_sql, "DCS");
			strcpy(bot_status, fm_sql);					// init bottom string
			strcat(bot_status, ":");
			strcat(bot_status, current_channel_info_E.EncTone.text);	// add DCS code
			strcpy(tg_fill, "");
		} else {
               if (! fIsCTSvalid ) {		
				strcpy(fm_sql, "");			
				strcpy(bot_status, fm_sql);
			} else {	
				strcpy(fm_sql, "CTS");
				strcpy(bot_status, fm_sql);					// init bottom string
				strcat(bot_status, ":");
				strcat(bot_status, current_channel_info_E.EncTone.text);	// add CTS tone freq
				strcat(bot_status, "Hz");					// add CTS tone freq
			}
			strcpy(tg_fill, "");
		}

		if (global_addl_config.mode_stat != 3) {				// if MODE/CC compact display set in config
			strcpy(top_status, "FM ");					// init FM string
		} else {
			strcpy(top_status, "FM");					// init FM string
		}
		if (fIsWideBandwidth) { strcpy(fm_bw_stat, "|W"); } else { strcpy(fm_bw_stat, "|N"); }

		strcat(top_status, fm_bw_stat);						// |N or |W
		strcat(top_status, ch_offset);						// |-R| or |=>| simplex

		if (global_addl_config.mode_stat != 3) {				// if MODE/CC compact display set in config
			strcat(top_status, " [");					// space
			strcat(top_status, fm_sql);					// add the tone type to status
		} else {	
			if (*ch_tone_type != 'N') {					// if MODE/CC compact display set in config
				if (fIsCTSvalid) {
					strcat(top_status, "[");					// less space in compact mode
					strcat(top_status, fm_sql);					// add the tone type to status
					strcat(top_status, ":");
					strcat(top_status, current_channel_info_E.EncTone.text);// add DCS/CTS tone to topstatus in compact mode
				}
			}
		}

		strcat(top_status, "]");						// Tone squelch status close bracket
	
		if (global_addl_config.mode_stat != 0) { 
			if (global_addl_config.mode_color == 1) { gfx_set_fg_color(0xffffff); gfx_set_bg_color(0xff4f32);}
			gfx_printf_pos2(x, top_y, 120, "%s%s%s    ", top_status, tg_fill, mic_gain_stat);
			gfx_set_fg_color(0x000000);
			gfx_set_bg_color(0xff8032);
		}

		if (global_addl_config.chan_stat != 0) { 
			if (global_addl_config.chan_color == 1) { gfx_set_fg_color(0x261162); gfx_set_bg_color(0xff9f32);}

				if (global_addl_config.chan_stat == 1) { 		// 1=show Status CC/CTS/DCS Info
					if (global_addl_config.mode_stat != 3) {	// if MODE/CC compact display set in config
						gfx_printf_pos2(x, y, 120, "%s                  ", bot_status);
					} else {
						if (global_addl_config.chan_stat != 4) {
							gfx_printf_pos2(x, y, 120, "%s:%s MHz     ", (global_addl_config.chan_stat == 3 ? "TX" : "RX"), (global_addl_config.chan_stat == 3 ? freq_tx : freq_rx) );
						} else {
							gfx_printf_pos2(x, y, 120, "RX:%s MHz     ", freq_rx );
							gfx_printf_pos2(x, y + 10, 120, "TX:%s MHz     ", freq_tx );
						}
					}
				} else {
					if (global_addl_config.chan_stat != 4) {
						gfx_printf_pos2(x, y, 120, "%s:%s MHz     ", (global_addl_config.chan_stat == 3 ? "TX" : "RX"), (global_addl_config.chan_stat == 3 ? freq_tx : freq_rx) );
					} else {
						gfx_printf_pos2(x, y, 120, "RX:%s MHz     ", freq_rx );
						gfx_printf_pos2(x, y + 10, 120, "TX:%s MHz     ", freq_tx );
					}
				}
			}
		}
	//========================================================================================================================//
	gfx_set_fg_color(0x000000);
	gfx_set_bg_color(0xff0000);
	gfx_select_font(gfx_font_norm);
}
#endif




void draw_datetime_row_hook()
{
# if (CONFIG_APP_MENU)
    if( Menu_DrawIfVisible(AM_CALLER_DATETIME_HOOK) )  
     { return; // the menu covers the entire screen, so don't draw anything else
     }
# endif


#if defined(FW_D13_020) || defined(FW_S13_020)
    if( is_netmon_visible() ) {
        return ;
    }
    //if( global_addl_config.mode_stat != 0 || global_addl_config.chan_stat != 0 ) {
	draw_adhoc_statusline(); 
    //}
    if( is_statusline_visible() || global_addl_config.datef == 6 ) {
        draw_alt_statusline();
        return ; 
    }
    draw_datetime_row();
#else
#warning please consider hooking.    
#endif    
}

/* Displays a startup demo on the device's screen, including some of
   the setting information and a picture or two. */
void display_credits()
{
    drawtext(L"MD380Tools ", 160, 20);
    drawtext(L"by KK4VCZ  ", 160, 60);
    drawtext(L"and Friends", 160, 100);
#ifdef MD380_d13_020
    drawtext(L"@ D13.020", 160, 140);
#endif
#ifdef MD380_d02_032
    drawtext(L"@ D02.032", 160, 140);
#endif
#ifdef MD380_s13_020
    drawtext(L"@ S13.020", 160, 140);
#endif

    drawascii(GIT_VERSION, 160, 180);

    drawtext(VERSIONDATE, 160, 220);
}
