/* \file dmesg.h
   \brief Kernel logging functions and buffers.
*/

//1kb buffer.  Verify that this is empty with reads.
#define DMESG_START  0x2001f700  //0x2001d500
#define DMESG_SIZE 1024

//This buffer is in TCRAM, cannot be DMAd.
extern char dmesg_start[];
//dmesg_start is copied here in SRAM for DMA over USB.
extern char *dmesg_tx_buf;
//Pointer to the current byte.  Loops as a ring buffer when full.
extern int dmesg_wcurs;

//Basic functions.
void md380_putc ( void* p, char c);
void dmesg_init();
void dmesg_flush();

//Convenience functions.
void printhex(void *buf, int len);

void printhex2(const char *buf, int len);
