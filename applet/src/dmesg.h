/* \file dmesg.h
   \brief Kernel logging functions and buffers.
*/

//1kb buffer.  Verify that this is empty with reads.
#define DMESG_START 0x2001d500
#define DMESG_SIZE 1024


extern char dmesg_start[];
extern char *dmesg_tx_buf;
extern int dmesg_wcurs;

//Basic functions.
void md380_putc ( void* p, char c);
void dmesg_init();
void dmesg_flush();

//Convenience functions.
void printhex(char *buf, int len);

