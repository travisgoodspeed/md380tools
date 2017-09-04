/*
 *  system_hrc5000.h
 * 
 */

#ifndef SYSTEM_HRC5000_H
#define SYSTEM_HRC5000_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MIC_GAIN_REGISTER	0x0F
#define FM_SET_REGISTER  	0x34
#define FM_DEV_REGISTER		0x35

#define FM_BPF_MASK	  	0x7
#define FM_COMP_MASK	  	0x6
#define FM_PREEMP_MASK	  	0x5
#define FM_BANDW_MASK	  	0x4
#define FM_DEFAULT	  	0xF0

#define FM_DEV0			0x00	// level -3
#define FM_DEV1			0x10	// level -2
#define FM_DEV2			0x20	// level -1
#define FM_DEV3			0x28	// default
#define FM_DEV4			0x40	// level +1
#define FM_DEV5			0x60	// level +2
#define FM_DEV6			0x80	// level +3
#define FM_DEV_DEFAULT		0x28

#define HRC5000_BUFFER_SIZE 	16

extern char hrc5000_reg[];
extern char *hrc5000_register;

void hrc5000_check(void);
void hrc5000_fm_set(void);
void hrc5000_fm_read(void);
void hrc5000_fm_reset(void);
void hrc5000_dev_set(void);
void hrc5000_dev_read(void);

void hrc5000_buffer_init(void);
void hrc5000_buffer_flush(void);

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_HRC5000_H */
