Pins of the C5000 that are routed to the microcontroller
not yet complete!
The LCD module is believed to be similar to the ILI9481.

| Pin | Name |I/O| Datasheet Discription | Routed to STM32 | Comment
|-----|------|---|-----------------------|-----------------|---------|
| 5 | MIC2_P | I | Mic2 input P | PC8 | 2T/5T/DTMF
| 47 | PWD | I | Shutdown | PE6 | DMR_SLEEP
# Surprisingly the following 4 SPI pins are not connected to a hardware SPI on the STM32
| 58 | U_CS | I | SPI0 CS | PE2 | DMR_CS
| 57 | U_SCLK | I | SPI0 CLK | PE3 | DMR_SCLK | The clock for the PLL is on the same pin!
| 56 | U_SDI | I | SPI0 SDI | PE5 | DMR_SDI
| 55 | U_SDO | O | SPI0 SDO | PE4 | DMR_SDO


