/* \file dmesg.h
   \brief Kernel logging functions and buffers.
*/

extern char *dmesg_start;
extern char *dmesg_end;
extern int dmesg_wcurs;

void md380_putc ( void* p, char c);
void dmesg_init();
void dmesg_flush();
