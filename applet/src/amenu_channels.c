// File:    md380tools/applet/src/amenu_codeplug.c
// Author:  Wolf (DL4YHF) [initial version]
//
// Date:    2017-04-29
//  Highly experimental 'alternative zone list' and similar codeplug-related displays.
//  Most list-like displays are implemented as a callback function 
//  for the 'application menu' (app_menu.c) .

#include "config.h"

#include <stm32f4xx.h>
#include <string.h>
#include "irq_handlers.h"
#include "lcd_driver.h"
#include "app_menu.h" // 'simple' alternative menu activated by red BACK-button
#include "printf.h"
#include "spiflash.h" // md380_spiflash_read()
#include "codeplug.h" // codeplug memory addresses, struct- and array-sizes
#if defined(FW_D13_020) || defined(FW_S13_020)
	#include "amenu_channels.h" // header for THIS module (to check prototypes,etc)
	#include "amenu_codeplug.h"
#endif

#include "syslog.h"

channel_t selChan;
channel_easy selChanE;
int selIndex = 0;
int numChannels = 0;
contact_t cont;

#if defined(FW_D13_020) || defined(FW_S13_020)
const am_stringtable_t am_stringtab_onoff[] =
{
	{ 0,      "Off" },
	{ 1,      "On" },
	{ 0,      NULL } // <- marks the end of a string table
};
#endif

/*
const am_tonetab_t am_tone_num[] =
{
	{1, 67.0}
	{2, 71.9}
	{3, 74.4}
	{4, 77.0}
	{5, 79.7}
	{6, 82.5}
	{7, 85.4}
	{8, 88.5}
	{9, 91.5}
	{10, 94.8}
	{11, 97.4}
	{12, 100.0}
	{13, 103.5}
	{14, 107.2}
	{15, 110.9}
	{16, 114.8}
	{17, 118.8}
	{18, 123.0}
	{19, 127.3}
	{20, 131.8}
	{21, 136.5}
	{22, 141.3}
	{23, 146.2}
	{24, 151.4}
	{25, 156.7}
	{26, 162.2}
	{27, 167.9}
	{28, 173.8}
	{29, 179.9}
	{30, 186.2}
	{31, 192.8}
	{32, 203.5}
	{33, 210.7}
	{34, 218.1}
	{35, 225.7}
	{36, 233.6}
	{37, 241.8}
	{38, 250.3}
	{ 0,      NULL } // <- marks the end of a string table
};
*/

uint8_t getCC(channel_t* ch)
{
	return ((((uint8_t*)ch)[1] & 0xF0) >> 4) & 0xF;
}
void setCC(channel_t* ch, uint8_t cc)
{
	((uint8_t*)ch)[1] = (((uint8_t*)ch)[1] & 0x0F) | ((cc << 4) & 0xF0);
}
/*
uint8_t getType(channel_t* ch)
{
	return ((((uint8_t*)ch)[0] & 0x01) != 0 ? 1 : 0);
}
*/
uint8_t getType(channel_t* ch)
{
	return ((((uint8_t*)ch)[0] & 0x01) == 0 ? 0 : 1);		// 0=digital 1=analog
}

void setType(channel_t* ch, uint8_t type)
{
	((uint8_t*)ch)[0] = (((uint8_t*)ch)[1] & (~0x01)) | (type ? 0x01: 0);
}

uint8_t getAutoScan(channel_t* ch)
{
	return ((ch->settings[0] & 0x10) != 0 ? 1 : 0);
}
void setAutoScan(channel_t* ch, uint8_t val)
{
	ch->settings[0] = (ch->settings[0] & (~0x10)) | (val ? 0x10 : 0);
}

uint8_t getSlot(channel_t* ch)
{
	return ((((uint8_t*)ch)[1] & 0x04) != 0 ? 1 : 2);
}
void setSlot(channel_t* ch, uint8_t slot)
{
	((uint8_t*)ch)[1] = (((uint8_t*)ch)[1] & (~0x04)) | (slot <= 1 ? 0x04 : 0);
}


int readFrequency(channel_t* ch, frequency_t* freq, char fRx)
{
	for (int i = 0; i < 8; i++) {
		if ((i + 1) % 2 != 0) {
			freq->digits[i] = (ch->settings[(fRx ? 16 : 20) + (i / 2)]) & 0xF;
		}
		else {
			freq->digits[i] = (ch->settings[(fRx ? 16 : 20) + (i / 2)] >> 4) & 0xF;
		}
	}
	sprintf(freq->text, "%d%d%d.%d%d%d%d%d\0\0", freq->digits[7], freq->digits[6], freq->digits[5], freq->digits[4], freq->digits[3], freq->digits[2], freq->digits[1], freq->digits[0]);
	return 0;
}



int readTone(channel_t* chan, tone_t* tone, char fEnc)
{
	tone_t tt;
	if (*(uint8_t*)&chan->settings[(fEnc ? 26 : 24)] == 0xFFFF) {
		tone->fType = 0;
		sprintf(tone->text, "None");
		return 0;
	}
	for (int i = 0; i < 4; i++) {
		if ((i + 1) % 2 != 0) {
			tt.digits[i] = chan->settings[(fEnc ? 26 : 24) + (i / 2)] & 0xF;
		}
		else {
			tt.digits[i] = (chan->settings[(fEnc ? 26 : 24) + (i / 2)] >> 4) & 0xF;
		}
	}
	for (int i = 0; i < 4; i++)
	{
		tone->digits[i] = tt.digits[3 - i];
	}
	switch (tone->digits[0]) {
		case 8:
			tone->fType = 'N';
			break;
		case 12:
			tone->fType = 'I';
			break;
		default:
			tone->fType = 'C';
			break;
	}

	if (tone->fType == 'C') {
		tone->freq = ((float)tone->digits[0] * 100.0f) + ((float)tone->digits[1] * 10.0f) + ((float)tone->digits[2] * 1.0f) + ((float)tone->digits[3] * 0.1f);
	}
	else {
		tone->freq = 0.0f;
	}

	if (tone->fType != 'C') {
		sprintf(tone->text, "D%d%d%d%c\0\0", tone->digits[1], tone->digits[2], tone->digits[3], tone->fType);
	}
	else {
		if (!tone->digits[0] || tone->digits[0] > 9) {
			sprintf(tone->text, "%d%d.%d", tone->digits[1], tone->digits[2], tone->digits[3]);
		}
		else {
			sprintf(tone->text, "%d%d%d.%d", tone->digits[0], tone->digits[1], tone->digits[2], tone->digits[3]);
		}
	}
	return 0;
}

int ParseChannel(channel_t* chan, channel_easy* chanE)
{
	
	memcpy(chanE->name, chan->name, 32);
	chanE->name[15] = '\0';

	chanE->bIsDigital = getType(chan);
/*
	chanE->TOT = getTOT(chan);
	chanE->EmergencyIndex = getEmergencyIndex(chan);
	chanE->GroupListIndex = getGroupListIndex(chan);
	chanE->ScanListIndex = getScanListIndex(chan);
	chanE->ContactIndex = getContactIndex(chan);
	chanE->AutoScan = getAutoScan(chan);
*/
	readFrequency(chan, &chanE->rxFreq, 1);
	readFrequency(chan, &chanE->txFreq, 0);

	if (chanE->bIsDigital) {
		chanE->CC = 0;
		chanE->Slot = 0;
		chanE->ContactIndex = 0;

		readTone(chan, &chanE->DecTone, FALSE);
		readTone(chan, &chanE->EncTone, TRUE);
	}
	else {
		chanE->CC = getCC(chan);
		chanE->Slot = getSlot(chan);
		sprintf(chanE->DecTone.text, "N/A");
		sprintf(chanE->EncTone.text, "N/A");
	}
	return 0;
}
