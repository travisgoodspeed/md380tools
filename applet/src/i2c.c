/*! \file i2c.c
  \brief i2c Hooks and diagnostic functions.
*/

#include <string.h>

#include "printf.h"
#include "dmesg.h"
#include "md380.h"
#include "version.h"
#include "config.h"
#include "printf.h"


extern void sleep(__IO uint32_t ms);

void I2C_GenerateSTART_hook(I2C_TypeDef* I2Cx, FunctionalState NewState) {
  void *return_addr;
  __asm__("mov %0,r14" : "=r" (return_addr));

#ifdef I2CPRINT  
  sleep(50);
  printf("%s RA: 0x%x S:%x\n", "GSA", return_addr,NewState);        
#endif
  md380_I2C_GenerateSTART(I2Cx, NewState);
}

void I2C_GenerateSTOP_hook(I2C_TypeDef* I2Cx, FunctionalState NewState) {
#ifdef I2CPRINT  
  printf("\n%s S:%x\n", "GST", NewState); 
#endif
  md380_I2C_GenerateSTOP(I2Cx, NewState);
}

uint8_t I2C_ReceiveData_hook(I2C_TypeDef* I2Cx) {
  uint8_t data;

  data=md380_I2C_ReceiveData(I2Cx);
#ifdef I2CPRINT  
  printf("r:%02x ", data);
#endif
  return (data);
}

void I2C_Send7bitAddress_hook(I2C_TypeDef* I2Cx, uint8_t Address, uint8_t I2C_Direction) {
#ifdef I2CPRINT  
  printf("s7 %02x %02x\n", Address, I2C_Direction);  
#endif
  md380_I2C_Send7bitAddress(I2Cx, Address, I2C_Direction);
}

void I2C_SendData_hook(I2C_TypeDef* I2Cx, uint8_t Data) {
#ifdef I2CPRINT  
   printf("s:%02x ", Data);
#endif
   md380_I2C_SendData(I2Cx, Data);
}
