/*! \file dispinit.c
  \brief display init Hook functions, similar the display init from S13.032.
*/

#include <string.h>

#include "md380.h"

// new display reset, old was only SetBit
void display_reset()
{
  md380_GPIO_SetBits(0x40020C00, 0x2000);
  OSTimeDly(2);
  md380_GPIO_ResetBits(0x40020C00, 0x2000);
  OSTimeDly(10);
  md380_GPIO_SetBits(0x40020C00, 0x2000);
  OSTimeDly(100);
}

void display_init(void)
{
  display_reset();
  if ( (md380_radio_config_bank2[0x1d] & 3) != 2 && (md380_radio_config_bank2[0x1d] & 3) != 3 ) {
    md380_Write_Command_2display(0x11);
    OSTimeDly(120);
    md380_Write_Command_2display(0xB1);
    md380_Write_Data_2display(5);
    md380_Write_Data_2display(0x3C);
    md380_Write_Data_2display(0x3C);
    md380_Write_Command_2display(0xB2);
    md380_Write_Data_2display(5);
    md380_Write_Data_2display(0x3C);
    md380_Write_Data_2display(0x3C);
    md380_Write_Command_2display(0xB3);
    md380_Write_Data_2display(5);
    md380_Write_Data_2display(0x3C);
    md380_Write_Data_2display(0x3C);
    md380_Write_Data_2display(5);
    md380_Write_Data_2display(0x3C);
    md380_Write_Data_2display(0x3C);
    md380_Write_Command_2display(0xB4);
    md380_Write_Data_2display(3);
    md380_Write_Command_2display(0xC0);
    md380_Write_Data_2display(0x28);
    md380_Write_Data_2display(8);
    md380_Write_Data_2display(4);
    md380_Write_Command_2display(0xC1);
    md380_Write_Data_2display(0xC0);
    md380_Write_Command_2display(0xC2);
    md380_Write_Data_2display(0xD);
    md380_Write_Data_2display(0);
    md380_Write_Command_2display(0xC3);
    md380_Write_Data_2display(0x8D);
    md380_Write_Data_2display(0x2A);
    md380_Write_Command_2display(0xC4);
    md380_Write_Data_2display(0x8D);
    md380_Write_Data_2display(0xEE);
    md380_Write_Command_2display(0xC5);
    md380_Write_Data_2display(0x1A);
    md380_Write_Command_2display(0x36);
    md380_Write_Data_2display(8);
    md380_Write_Command_2display(0xE0);
    md380_Write_Data_2display(4);
    md380_Write_Data_2display(0xC);
    md380_Write_Data_2display(7);
    md380_Write_Data_2display(0xA);
    md380_Write_Data_2display(0x2E);
    md380_Write_Data_2display(0x30);
    md380_Write_Data_2display(0x25);
    md380_Write_Data_2display(0x2A);
    md380_Write_Data_2display(0x28);
    md380_Write_Data_2display(0x26);
    md380_Write_Data_2display(0x2E);
    md380_Write_Data_2display(0x3A);
    md380_Write_Data_2display(0);
    md380_Write_Data_2display(1);
    md380_Write_Data_2display(3);
    md380_Write_Data_2display(0x13);
    md380_Write_Command_2display(0xE1);
    md380_Write_Data_2display(4);
    md380_Write_Data_2display(0x16);
    md380_Write_Data_2display(6);
    md380_Write_Data_2display(0xD);
    md380_Write_Data_2display(0x2D);
    md380_Write_Data_2display(0x26);
    md380_Write_Data_2display(0x23);
    md380_Write_Data_2display(0x27);
    md380_Write_Data_2display(0x27);
    md380_Write_Data_2display(0x25);
    md380_Write_Data_2display(0x2D);
    md380_Write_Data_2display(0x3B);
    md380_Write_Data_2display(0);
    md380_Write_Data_2display(1);
    md380_Write_Data_2display(4);
    md380_Write_Data_2display(0x13);
    md380_Write_Command_2display(0x3A);
    md380_Write_Data_2display(5);
    md380_Write_Command_2display(0x36);
    if ( (md380_radio_config_bank2[0x1d] & 3) == 1 )
      md380_Write_Data_2display(200);
    else
      md380_Write_Data_2display(8);
    md380_Write_Command_2display(0x29);
    md380_Write_Command_2display(0x2C);
  }else{
    md380_Write_Command_2display(0x3A);
    md380_Write_Data_2display(5);
    md380_Write_Command_2display(0x36);
    if ( (md380_radio_config_bank2[0x1d] & 3) == 3 )
      md380_Write_Data_2display(8);
    else
      md380_Write_Data_2display(0x48);
    md380_Write_Command_2display(0xFE);
    md380_Write_Command_2display(0xEF);
    md380_Write_Command_2display(0xB4);
    md380_Write_Data_2display(0);
    md380_Write_Command_2display(0xFF);
    md380_Write_Data_2display(0x16);
    md380_Write_Command_2display(0xfd);
    if ( (md380_radio_config_bank2[0x1d] & 3) == 3 )
      md380_Write_Data_2display(0x40);
    else
      md380_Write_Data_2display(0x4F);
    md380_Write_Command_2display(0xA4);
    md380_Write_Data_2display(0x70);
    md380_Write_Command_2display(0xE7);
    md380_Write_Data_2display(0x94);
    md380_Write_Data_2display(0x88);
    md380_Write_Command_2display(0xEA);
    md380_Write_Data_2display(0x3A);
    md380_Write_Command_2display(0xED);
    md380_Write_Data_2display(0x11);
    md380_Write_Command_2display(0xE4);
    md380_Write_Data_2display(0xC5);
    md380_Write_Command_2display(0xE2);
    md380_Write_Data_2display(0x80);
    md380_Write_Command_2display(0xA3);
    md380_Write_Data_2display(18);
    md380_Write_Command_2display(0xE3);
    md380_Write_Data_2display(7);
    md380_Write_Command_2display(0xE5);
    md380_Write_Data_2display(0x10);
    md380_Write_Command_2display(0xF0);
    md380_Write_Data_2display(0);
    md380_Write_Command_2display(0xF1);
    md380_Write_Data_2display(0x55);
    md380_Write_Command_2display(0xF2);
    md380_Write_Data_2display(5);
    md380_Write_Command_2display(0xF3);
    md380_Write_Data_2display(0x53);
    md380_Write_Command_2display(0xF4);
    md380_Write_Data_2display(0);
    md380_Write_Command_2display(0xF5);
    md380_Write_Data_2display(0);
    md380_Write_Command_2display(0xF7);
    md380_Write_Data_2display(0x27);
    md380_Write_Command_2display(0xF8);
    md380_Write_Data_2display(0x22);
    md380_Write_Command_2display(0xF9);
    md380_Write_Data_2display(0x77);
    md380_Write_Command_2display(0xFA);
    md380_Write_Data_2display(0x35);
    md380_Write_Command_2display(0xFB);
    md380_Write_Data_2display(0);
    md380_Write_Command_2display(0xFC);
    md380_Write_Data_2display(0);
    md380_Write_Command_2display(0xFE);
    md380_Write_Command_2display(0xEF);
    md380_Write_Command_2display(0xE9);
    md380_Write_Data_2display(0);
    OSTimeDly(20);
    md380_Write_Command_2display(0x11);
    OSTimeDly(130);
    md380_Write_Command_2display(0x29);
    md380_Write_Command_2display(0x2C);
  }
}

