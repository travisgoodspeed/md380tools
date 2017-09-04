/**
  ******************************************************************************
  * @file    system_hrc5000.c
  * @author  Meinhard F. Guenther, DL2MF
  * @version V1.0.0 - initial release
  * @date    04-June-2017
  * @brief   HR_C5000 is Hong Rui independent research and development in line 
  *          with ETSI TS102 361 (DMR) standard digital intercom dedicated chip. 
  *          Chip using 4FSK modulation and demodulation technology, 
  *          12.5K channel using 2-slot TDMA communication mechanism to achieve
  *          2 Way digital voice and data communication transmission.
  *             
  * 1.  This file provides access to different register functions:
  *      - hrc5000_fm_set():   Setup single register functions (see table below) to
  *			       give user access to modified FM settings.
  *                            The corresponding bits are set by bitmask functions.
  *
  *      - hrc5000_fm_reset(): Restore factory default settings to FM setup register
  *                            and save changed setup config to radio configuration.
  *
  * 2. This file configures the HR_C5000 baseband chip as follows:
  *=============================================================================
  +------+---------+---+----------------------+-----+-----+--------------------+
  | Type | Address |R/W| Name                 | Def.|     |                    |
  |      |         |   |                      | Val.| Bit | Description        | 
  +------+---------+---+----------------------+-----+-----+--------------------+
  | Hard-|  0x0F   |R/W| ADLinVol             |0xB8 |  7  |11111:12dB no mod|  | 
  | ware |         |   |                      |     |  6  |11110:10db no mod|  | 
  | Cfg. |         |   |                      |     |  5  |11101: 9dB          | 
  |      |         |   |                      |     |  4  |...                 | 
  |      |         |   |                      |     |  3  |00000:-34.5dB       | 
  +------+---------+---+----------------------+-----+-----+--------------------+
  | Hard-|  0x0F   |R/W| MicVol               |     |  2  |00= 0dB             | 
  | ware |         |   |                      |     |  1  |01= 6db             | 
  | Cfg. |         |   |                      |     |     |10=12db             | 
  |-cont-|         |   |                      |     |     |11=20db             | 
  +------+---------+---+----------------------+-----+-----+--------------------+
  |  FM  |  0x34   |R/W| FMBpfOn              |     |  7  |0=Bpfilt off 1=on   | 
  |  set |         |   | FMCompressorOn       |0xF0 |  6  |0=Comprs off 1=on   | 
  |      |         |   | FMPreEmphasisOn      |     |  5  |0=PreEmp off 1=on   | 
  |      |         |   | FMBandWidth          |     |  4  |0=12.5kHz 1=25kHz   | 
  +------+---------+---+----------------------+-----+-----+--------------------+
  |  FM  |  0x35   |R/W| FM Deviation         |0xA0 |  7  |A0...F0 mod.level   | 
  |devia.|         |   |                      |     |  6  |        settings    | 
  |      |         |   |                      |     |  5  |                    | 
  |      |         |   |                      |     |  4  |                    | 
  |      |         |   |                      |     |0...3|reserved            | 
  +------+---------+---+----------------------+-----+-----+--------------------+
  |      |         |   |                      |     |     |                    | 
  |      |         |   |                      |     |     |                    | 
  +------+---------+---+----------------------+-----+-----+--------------------+
  |      |         |   |                      |     |     |                    | 
  |      |         |   |                      |     |     |                    | 
  +------+---------+---+----------------------+-----+-----+--------------------+
  |      |         |   |                      |     |     |                    | 
  |      |         |   |                      |     |     |                    | 
  |      |         |   |                      |     |     |                    | 
  |      |         |   |                      |     |     |                    | 
  |      |         |   |                      |     |     |                    | 
  |      |         |   |                      |     |     |                    | 
  +------+---------+---+----------------------+-----+-----+--------------------+
  *============================================================================*
  ****************************************************************************** 
  * @attention
  *
  * THE PRESENT SOFTWARE IS FOR EXPERIMENTAL ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, THE AUTHOR SHALL NOT BE HELD LIABLE FOR ANY DIRECT, 
  * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  ******************************************************************************
  */

#define DEBUG

#include "config.h"

#include "md380.h"
#include "version.h"
#include "printf.h"
#include "string.h"
#include "addl_config.h"
#include "ambe.h"
#include "dmr.h"
#include "dmesg.h"
#include "console.h"
#include "util.h"
#include "debug.h"
#include "netmon.h"
#include "syslog.h"
#include "irq_handlers.h"
#include "system_hrc5000.h"


int fm_reg_val = 0x00;		// register value of HRC_5000 FM settings 
int fm_reg_set = 0x00;		// bitmask with last settings of register

int fm_dev_set = 0x00;		// bitmask with last settings of deviation
int fm_dev_val = 0x00;		// register value of HRC_5000 FM deviation

int state;

char *hrc5000_register; //=(char*) DMESG_START;
char hrc5000_reg[HRC5000_BUFFER_SIZE];

void hrc5000_check(void)
{
#if defined(FW_D13_020) || defined(FW_S13_020)
  /*----------------------------------------------------------------

    HRC_5000 - sanity check
    -----------------------------------------------------------------	
    0x34 - FM bandpass filter, compressor, preemphasis, bandwidth
    0x35 - FM deviation
  ------------------------------------------------------------------*/

	hrc5000_fm_read();				// read fm_reg_val before register handling

	if (fm_reg_val != global_addl_config.fm_mode){
		PRINT("fm_reg was last: %02x now read: %02x\n", global_addl_config.fm_mode, fm_reg_val);

		hrc5000_fm_set();			// set fm_reg_val
//		hrc5000_fm_read();			// read fm_reg_val before register handling

		if(global_addl_config.devmode_level == 3)	// developer mode 3 verbose register log to usb console
			PRINT("FM_REG set >> %02x <-r/w-> %02x\n", *hrc5000_reg, fm_reg_set );
	} /*else {
		c5000_spi0_writereg(FM_SET_REGISTER, fm_reg_set);	// rewrite register value to HRC5000
		PRINT("FM_REG rpt >> %02x <-r/w-> %02x\n", *hrc5000_reg, fm_reg_set );
	}
*/


#else
#endif		// #if defined D13.020 || S13.020
}

void hrc5000_fm_set(void)
{
#if defined(FW_D13_020) || defined(FW_S13_020)
  /*----------------------------------------------------------------

    HRC_5000 - FM setup register 0x34 - FM Settings
    -----------------------------------------------
    Bit 7 - 0x80	fm_bpf;		// 0=off, 1=on
    Bit 6 - 0x40	fm_comp;	// 0=off, 1=on
    Bit 5 - 0x20	fm_preemp;	// 0=off, 1=on
    Bit 4 - 0x10	fm_bw;		// 0=12.5kHz, 1=25kHz

  ------------------------------------------------------------------*/

	if(global_addl_config.fm_bpf == 0){
//		PRINT("fm_reg clr-1: %02x (before)\n", fm_reg_val);
		fm_reg_val &= ~(1 << FM_BPF_MASK);
		PRINT("fm_reg clr-1: %02x (mask:%02x)\n", fm_reg_val, FM_BPF_MASK);
	} else if (global_addl_config.fm_bpf == 1){
//		PRINT("fm_reg set-1: %02x (before)\n", fm_reg_val);
		fm_reg_val |= (1 << FM_BPF_MASK);
		PRINT("fm_reg set-1: %02x (mask:%02x)\n", fm_reg_val, FM_BPF_MASK);
	}

	if(global_addl_config.fm_comp == 0){
//		PRINT("fm_reg clr-2: %02x (before)\n", fm_reg_val);
		fm_reg_val &= ~(1 << FM_COMP_MASK);
		PRINT("fm_reg clr-2: %02x (mask:%02x)\n", fm_reg_val, FM_COMP_MASK);
	} else if (global_addl_config.fm_comp == 1){
//		PRINT("fm_reg set-2: %02x (before)\n", fm_reg_val);
		fm_reg_val |= (1 << FM_COMP_MASK);
		PRINT("fm_reg set-2: %02x (mask:%02x)\n", fm_reg_val, FM_COMP_MASK);
	}

	if(global_addl_config.fm_preemp == 0){
//		PRINT("fm_reg clr-3: %02x (before)\n", fm_reg_val);
		fm_reg_val &= ~(1 << FM_PREEMP_MASK);
		fm_reg_val = 0x1c;
		PRINT("fm_reg clr-3: %02x (mask:%02x)\n", fm_reg_val, FM_PREEMP_MASK);
	} else if (global_addl_config.fm_preemp == 1){
//		PRINT("fm_reg set-3: %02x (before)\n", fm_reg_val);
		fm_reg_val |= (1 << FM_PREEMP_MASK);
		PRINT("fm_reg set-3: %02x (mask:%02x)\n", fm_reg_val, FM_PREEMP_MASK);
	}

	if(global_addl_config.fm_bw == 0){
//		PRINT("fm_reg clr-4: %02x (before)\n", fm_reg_val);
		fm_reg_val &= ~(1 << FM_BANDW_MASK);
		PRINT("fm_reg clr-4: %02x (mask:%02x)\n", fm_reg_val, FM_BANDW_MASK);
	} else if (global_addl_config.fm_bw == 1){
//		PRINT("fm_reg set-4: %02x (before)\n", fm_reg_val);
		fm_reg_val |= (1 << FM_BANDW_MASK);
		PRINT("fm_reg set-4: %02x (mask:%02x)\n", fm_reg_val, FM_BANDW_MASK);
	}
	state=OS_ENTER_CRITICAL();
	c5000_spi0_writereg(FM_SET_REGISTER, fm_reg_val);	// set new register value to HRC5000
	OS_EXIT_CRITICAL(state);
	fm_reg_set = fm_reg_val;				// store new register bitmask

	global_addl_config.fm_mode = fm_reg_set;		// write to SPI flash
	cfg_save();

/*----------------------------------------------------------------*/
  
#else
#endif		// #if defined D13.020 || S13.020
}

void hrc5000_dev_set(void)
{
#if defined(FW_D13_020) || defined(FW_S13_020)
  /*----------------------------------------------------------------

    HRC_5000 - FM deviation setup register 0x35 - FM Settings
    ---------------------------------------------------------
    Bit 7-4 - 0xA0-0xF0	fm_dev;		// default 0xA0

  ------------------------------------------------------------------*/

	hrc5000_dev_read();		// read fm_reg_val before register handling

	if (fm_dev_val != fm_dev_set){

		if(global_addl_config.devmode_level > 2)		// developer mode 3 verbose register log to usb console
			PRINT("FM_DEV was last: %02x now read: %02x\n", fm_dev_set, fm_dev_val);

		if(global_addl_config.devmode_level > 2)		// developer mode 3 verbose register log to usb console
			PRINT("FM_DEV bitshift before: %02x\n", fm_dev_val);
		fm_dev_val = fm_dev_val<<4;
		fm_dev_val = fm_dev_val>>4;

		if(global_addl_config.devmode_level > 1)		// developer mode 2 register log to usb console
			PRINT("FM_DEV bitshift after : %02x\n", fm_dev_val);

  		switch ( global_addl_config.fm_dev ) {
    		case 0 :
			fm_dev_val |= FM_DEV0;
			break;
    		case 1 :
			fm_dev_val |= FM_DEV1;
			break;
    		case 2 :
			fm_dev_val |= FM_DEV2;
			break;
    		case 3 :
			fm_dev_val = FM_DEV_DEFAULT;			// no logical operation, set default!!
			break;
    		case 4 :
			fm_dev_val |= FM_DEV4;
			break;
    		case 5 :
			fm_dev_val |= FM_DEV5;
			break;
    		case 6 :
			fm_dev_val |= FM_DEV6;
			break;
		default:
			return;
		}

		state=OS_ENTER_CRITICAL();
		c5000_spi0_writereg(FM_DEV_REGISTER, fm_dev_val);	// set new register value to HRC5000
		OS_EXIT_CRITICAL(state);
		fm_dev_set = fm_dev_val;

		state=OS_ENTER_CRITICAL();
		c5000_spi0_readreg(FM_DEV_REGISTER, hrc5000_reg);
		OS_EXIT_CRITICAL(state);
		fm_dev_val = *hrc5000_reg;

		if (IRQ_dwSysTickCounter < 3500)			// log to screen after load start cfg
		    	LOGB("t=%d: FM_DEV load=%02X\n", (int)IRQ_dwSysTickCounter, (int)fm_dev_val );
		if(global_addl_config.devmode_level > 1)		// developer mode 2 register log to usb console
			PRINT("FM_DEV set >> lvl:%d reg:%02x <-r/w-> %02x\n", global_addl_config.fm_dev, *hrc5000_reg, fm_dev_set );
	} else {
		state=OS_ENTER_CRITICAL();
		c5000_spi0_writereg(FM_DEV_REGISTER, fm_dev_set);	// rewrite register value to HRC5000
		OS_EXIT_CRITICAL(state);
		if(global_addl_config.devmode_level == 3)		// developer mode 3 verbose register log to usb console
			PRINT("FM_DEV rpt >> lvl:%d reg:%02x <-r/w-> %02x\n", global_addl_config.fm_dev, *hrc5000_reg, fm_dev_val );
	}

/*----------------------------------------------------------------*/
  
#else
#endif		// #if defined D13.020 || S13.020
}


/*----------------------------------------------------------------*/
void hrc5000_fm_reset(void)
{
#if defined(FW_D13_020) || defined(FW_S13_020)

/* -- some debug code, can be removed later
    c5000_spi0_readreg(FM_SET_REGISTER, hrc5000_reg);
    fm_reg_val = *hrc5000_reg;
    LOGB("t=%d: fm_reg before=%02X\n", (int)IRQ_dwSysTickCounter, (int)hrc5000_reg[0] );
*/

/* -- Reset FM registers to default --------------------------------*/
    state=OS_ENTER_CRITICAL();
    c5000_spi0_writereg(FM_SET_REGISTER, FM_DEFAULT);		
    c5000_spi0_readreg(FM_SET_REGISTER, hrc5000_reg);
    OS_EXIT_CRITICAL(state);

    if(global_addl_config.devmode_level > 0)		// developer mode != 0 register log to usb console    
    {
	PRINT("fm_reset -> %02x\n", *hrc5000_reg);
	LOGB("t=%04x: fm_reset=%02X\n", (int)IRQ_dwSysTickCounter, (int)hrc5000_reg[0] );
    }

/* -- Reset FM deviation to default -------------------------------*/
//    c5000_spi0_writereg(FM_DEV_REGISTER, 0x28);	// read from register @first try
    state=OS_ENTER_CRITICAL();
    c5000_spi0_writereg(FM_DEV_REGISTER, FM_DEV_DEFAULT);
    c5000_spi0_readreg(FM_DEV_REGISTER, hrc5000_reg);
    OS_EXIT_CRITICAL(state);

    if(global_addl_config.devmode_level > 0)		// developer mode != 0 register log to usb console    
    {
	PRINT("fm_devreset -> %02x\n", *hrc5000_reg);
	LOGB("t=%04x: FM_DEVRST=%02X\n", (int)IRQ_dwSysTickCounter, (int)hrc5000_reg[0] );
    }

#else
#endif		// #if defined D13.020 || S13.020
}

/*----------------------------------------------------------------*/
void hrc5000_fm_read(void)
{
#if defined(FW_D13_020) || defined(FW_S13_020)
    state=OS_ENTER_CRITICAL();
    c5000_spi0_readreg(FM_SET_REGISTER, hrc5000_reg);
    OS_EXIT_CRITICAL(state);

    fm_reg_val = *hrc5000_reg;
    if (IRQ_dwSysTickCounter < 5000)	// screenlog if load during start cfg
    	LOGB("t=%d: FM_REG load=%02X\n", (int)IRQ_dwSysTickCounter, (int)fm_reg_val );

#else
#endif		// #if defined D13.020 || S13.020
}

/*----------------------------------------------------------------*/
void hrc5000_dev_read(void)
{
#if defined(FW_D13_020) || defined(FW_S13_020)
    state=OS_ENTER_CRITICAL();
    c5000_spi0_readreg(FM_DEV_REGISTER, hrc5000_reg);
    OS_EXIT_CRITICAL(state);

    fm_dev_val = *hrc5000_reg;
#else
#endif		// #if defined D13.020 || S13.020
}


/*----------------------------------------------------------------*/
/* This initializes the putc call.				  */
/*----------------------------------------------------------------*/
void hrc5000_buffer_init(void)
{
  //Initialize or wipe out the old read buffer content.
  for(int i=0;i<HRC5000_BUFFER_SIZE;i++)
    hrc5000_reg[i]=0;
}

/*----------------------------------------------------------------*/
/* This flushes the HRC5000 register message buffer.		  */
/*----------------------------------------------------------------*/
void hrc5000_buffer_flush(void)
{
  hrc5000_buffer_init();
}


/****************************************************************END OF FILE****/

