/*! \file tooldfu.h
  \brief Custom DFU commands for the MD380.

This header defines commands for the first byte of the download's
data, which is sent to Block 1.  Return data is sent as an upload
transaction from Block 1.

For example, the TDFU_DMESG command is sent first as a DNLOAD of one
byte to Block 1, which sets the DFU target address to the DMESG
buffer.  Later reads grab 1024 bytes as an UPLOAD from Block 1,
fetching the raw buffer.
*/



//Memory commands
#define TDFU_DMESG                   0x00
#define TDFU_SPIFLASHREAD            0x01 //u32 address
#define TDFU_SPIFLASHWRITE           0x02 //u32 address, u32 size, u8 val[]  (don't work with many calls)
#define TDFU_SPIFLASHERASE64K        0x03 //u32 address
#define TDFU_SPIFLASHWRITE_NEW       0x04 //u32 address, u32 size, u8 val[]
#define TDFU_SPIFLASHGETID           0x05 // (void) -> 4 Byte ID
#define TDFU_SPIFLASHSECURITYREGREAD 0x08 // (void)
#define TDFU_SYSLOG                  0x09 //syslog_dump_dmesg()

//Radio Commands
#define TDFU_C5000_WRITEREG 0x10 //u8 reg, u8 val
#define TDFU_C5000_READREG  0x11 //u8 register


//Graphics Commands
#define TDFU_PRINT 0x80 //(u8 x, u8 y, wchar_t str[])
#define TDFU_BOX   0x81 //(u8 x, u8 y, u8 xp, u8 yp)
