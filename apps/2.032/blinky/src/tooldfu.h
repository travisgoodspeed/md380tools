/*! \file tooldfu.h
  \brief Custom DFU commands for the MD380.
*/


//Memory commands
#define TDFU_PEEK  0x01 //Returns value at an address.
#define TDFU_POKE  0x02 //Pokes adress to be a value.
#define TDFU_CALL  0x03 //Calls a function at an address.
#define TDFU_EXEC  0x04 //Executes shellcode in the packet with BX.

//Graphics Commands
#define TDFU_PRINT 0x80 //(u8 x, u8 y, wchar_t str[])
#define TDFU_BOX   0x81 //(u8 x, u8 y, u8 xp, u8 yp)
