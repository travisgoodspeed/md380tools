# flash.r by Travis Goodspeed

# This is a Radare2 script for annotating the Tytera MD380
# application, version 2.032, which begins at 0x0800C000 in Flash
# memory.  Preceding it is a bootloader, which can be either the
# official factory bootloader or an aftermarket one.


# Begin by opening the application in R2 with this script.
# r2 -a arm -m 0x0800C000 -b 16 -i flash.r ../../firmware/D002.032.bin

# MD5 (../../firmware/D002.032.bin) = 8295594d00cb705eac7cd812642fccf2


## These annotations describe the functions that handle incoming
## audio, which is necessary to 

CCa 0x803ec86 This function handles incoming audio calls.
af+ 0x803ec86 740 dmr_audio_start

CCa 0x0803ee36 Nop this to crudely match on all public calls.
CCa 0x0803ef10 Nop this to crudely match on all private calls.

CCa 0x0803ee14 R5 will count through each public talk group.
CCa 0x0803ee1e If none matched, jump to where we'll mute the audio.
CCa 0x0803ee80 Here, we've tried to match all of the talk groups.
CCa 0x0803ee84 Skip ahead a bit if a match has been found.

CCa 0x0803ee66 Value of 9 enables audio reception.
CCa 0x0803ee8a Value of 8 enables light, but audio is muted.

af+ 0x08026ab6 4 GPIO_SetBits()
CCa 0x08026ab6 ... maybe STM32LIB 
af+ 0x08026aba 4 GPIO_ResetBits
CCa 0x08026aba ... maybe STM32LIB  


af+ 0x8042468 30 uC-OS-II-Idle()
CCa 0x8042468 runs with 1MHz
af+ 0x804921c 122 uC-OS-II-Tmr()
af+ 0x804403c 310 Start()
af+ 0x803dbc4 28 Sys-Inter()
af+ 0x8044548 226 RTC-Timer()
af+ 0x803b524 2286 Call_Process()
af+ 0x8044674 626 FMTx_Process()
af+ 0x80448e8 42 RF-Pll()
af+ 0x8044900 20 PC-Tune()
af+ 0x809573c 366 LED-Process()
af+ 0x802a960 4084 Beep-Process()
af+ 0x802bb40 372 Af-Mute()
af+ 0x803dbe2 52 TimeSlot-Inter()
af+ 0x802bcb8 340 Set-Vocoder()
af+ 0x803c244 772 ChAccess-Pr()
af+ 0x803be24 992 State-Change()
af+ 0x804af68 568 Tone-fft()




af+ 0x08041df8 8 OS_ENTER_CRITICAL()
af+ 0x08041e00 6 OS_EXIT_CRITICAL()

CCa 0x8030262 Call OS_ENTER_CRITICAL()
CCa 0x80302d6 Call OS_ENTER_CRITICAL()
CCa 0x803032c Call OS_ENTER_CRITICAL()
CCa 0x80303c6 Call OS_ENTER_CRITICAL()
CCa 0x80329da Call OS_ENTER_CRITICAL()
CCa 0x803da10 Call OS_ENTER_CRITICAL()
CCa 0x803da3e Call OS_ENTER_CRITICAL()
CCa 0x803daaa Call OS_ENTER_CRITICAL()
CCa 0x803dafc Call OS_ENTER_CRITICAL()
CCa 0x803db78 Call OS_ENTER_CRITICAL()
CCa 0x8041ece Call OS_ENTER_CRITICAL()
CCa 0x8041f1e Call OS_ENTER_CRITICAL()
CCa 0x8041fb2 Call OS_ENTER_CRITICAL()
CCa 0x8041fec Call OS_ENTER_CRITICAL()
CCa 0x8042082 Call OS_ENTER_CRITICAL()
CCa 0x80420f8 Call OS_ENTER_CRITICAL()
CCa 0x80423f0 Call OS_ENTER_CRITICAL()
CCa 0x804246a Call OS_ENTER_CRITICAL()
CCa 0x80424a4 Call OS_ENTER_CRITICAL()
CCa 0x804255c Call OS_ENTER_CRITICAL()
CCa 0x8045a9c Call OS_ENTER_CRITICAL()
CCa 0x8045b7a Call OS_ENTER_CRITICAL()
CCa 0x8045c1c Call OS_ENTER_CRITICAL()
CCa 0x8045c8a Call OS_ENTER_CRITICAL()
CCa 0x8045e42 Call OS_ENTER_CRITICAL()
CCa 0x8045f2e Call OS_ENTER_CRITICAL()
CCa 0x804bb50 Call OS_ENTER_CRITICAL()
CCa 0x804bbc8 Call OS_ENTER_CRITICAL()
CCa 0x804bc10 Call OS_ENTER_CRITICAL()
CCa 0x804bc92 Call OS_ENTER_CRITICAL()
CCa 0x804bcd8 Call OS_ENTER_CRITICAL()
CCa 0x809381e Call OS_ENTER_CRITICAL()
CCa 0x8093b36 Call OS_ENTER_CRITICAL()

CCa 0x803027c Call OS_EXIT_CRITICAL()
CCa 0x80302ec Call OS_EXIT_CRITICAL()
CCa 0x8030324 Call OS_EXIT_CRITICAL()
CCa 0x8030394 Call OS_EXIT_CRITICAL()
CCa 0x80303e2 Call OS_EXIT_CRITICAL()
CCa 0x80303f6 Call OS_EXIT_CRITICAL()
CCa 0x8030402 Call OS_EXIT_CRITICAL()
CCa 0x8032a1e Call OS_EXIT_CRITICAL()
CCa 0x803da22 Call OS_EXIT_CRITICAL()
CCa 0x803da58 Call OS_EXIT_CRITICAL()
CCa 0x803dabe Call OS_EXIT_CRITICAL()
CCa 0x803daf4 Call OS_EXIT_CRITICAL()
CCa 0x803db4c Call OS_EXIT_CRITICAL()
CCa 0x803db94 Call OS_EXIT_CRITICAL()
CCa 0x803dbb2 Call OS_EXIT_CRITICAL()
CCa 0x803dbbc Call OS_EXIT_CRITICAL()
CCa 0x8041ed4 Call OS_EXIT_CRITICAL()
CCa 0x8041f9e Call OS_EXIT_CRITICAL()
CCa 0x8041fd8 Call OS_EXIT_CRITICAL()
CCa 0x804201c Call OS_EXIT_CRITICAL()
CCa 0x8042026 Call OS_EXIT_CRITICAL()
CCa 0x804202c Call OS_EXIT_CRITICAL()
CCa 0x8042032 Call OS_EXIT_CRITICAL()
CCa 0x8042098 Call OS_EXIT_CRITICAL()
CCa 0x80420ec Call OS_EXIT_CRITICAL()
CCa 0x8042442 Call OS_EXIT_CRITICAL()
CCa 0x804247c Call OS_EXIT_CRITICAL()
CCa 0x80424bc Call OS_EXIT_CRITICAL()
CCa 0x80425b6 Call OS_EXIT_CRITICAL()
CCa 0x80425c0 Call OS_EXIT_CRITICAL()
CCa 0x8045ad8 Call OS_EXIT_CRITICAL()
CCa 0x8045afe Call OS_EXIT_CRITICAL()
CCa 0x8045b22 Call OS_EXIT_CRITICAL()
CCa 0x8045b48 Call OS_EXIT_CRITICAL()
CCa 0x8045b4e Call OS_EXIT_CRITICAL()
CCa 0x8045ba4 Call OS_EXIT_CRITICAL()
CCa 0x8045bae Call OS_EXIT_CRITICAL()
CCa 0x8045c62 Call OS_EXIT_CRITICAL()
CCa 0x8045c82 Call OS_EXIT_CRITICAL()
CCa 0x8045cca Call OS_EXIT_CRITICAL()
CCa 0x8045d00 Call OS_EXIT_CRITICAL()
CCa 0x8045d20 Call OS_EXIT_CRITICAL()
CCa 0x8045d4a Call OS_EXIT_CRITICAL()
CCa 0x8045d6a Call OS_EXIT_CRITICAL()
CCa 0x8045d92 Call OS_EXIT_CRITICAL()
CCa 0x8045db2 Call OS_EXIT_CRITICAL()
CCa 0x8045dba Call OS_EXIT_CRITICAL()
CCa 0x8045e00 Call OS_EXIT_CRITICAL()
CCa 0x8045e1a Call OS_EXIT_CRITICAL()
CCa 0x8045e72 Call OS_EXIT_CRITICAL()
CCa 0x8045f12 Call OS_EXIT_CRITICAL()
CCa 0x8045f20 Call OS_EXIT_CRITICAL()
CCa 0x8045f38 Call OS_EXIT_CRITICAL()
CCa 0x804bb62 Call OS_EXIT_CRITICAL()
CCa 0x804bb86 Call OS_EXIT_CRITICAL()
CCa 0x804bbdc Call OS_EXIT_CRITICAL()
CCa 0x804bbe8 Call OS_EXIT_CRITICAL()
CCa 0x804bc22 Call OS_EXIT_CRITICAL()
CCa 0x804bc4a Call OS_EXIT_CRITICAL()
CCa 0x804bca8 Call OS_EXIT_CRITICAL()
CCa 0x804bcb4 Call OS_EXIT_CRITICAL()
CCa 0x804bcf6 Call OS_EXIT_CRITICAL()
CCa 0x804bd04 Call OS_EXIT_CRITICAL()
CCa 0x804bd10 Call OS_EXIT_CRITICAL()
CCa 0x8093830 Call OS_EXIT_CRITICAL()
CCa 0x8093b44 Call OS_EXIT_CRITICAL()


CCa 0x0801931c GPIO_SetBits(GPIOC, 0x40)
CCa 0x080193a6 GPIO_SetBits(GPIOC, 0x40)
CCa 0x0801942e GPIO_SetBits(GPIOC, 0x40)
CCa 0x08019718 GPIO_SetBits(GPIOC, 0x40)
CCa 0x0801ca5c GPIO_SetBits(GPIOD, 0x40)
CCa 0x0801f228 GPIO_SetBits(GPIOC, 0x40)
CCa 0x0801f334 GPIO_SetBits(GPIOC, 0x40)
CCa 0x0801f3b0 GPIO_SetBits(GPIOC, 0x40)
CCa 0x0801f40c GPIO_SetBits(GPIOC, 0x40)
CCa 0x0801f878 GPIO_SetBits(GPIOC, 0x40)
CCa 0x0801ffda GPIO_SetBits(GPIOC, 0x40)
CCa 0x080201c0 GPIO_SetBits(GPIOC, 0x40)
CCa 0x08027c22 GPIO_SetBits(GPIOC, 0x40)
CCa 0x08027c2e GPIO_SetBits(GPIOC, 0x40)
CCa 0x08028120 GPIO_SetBits(GPIOC, 0x40)
CCa 0x0802823a GPIO_SetBits(GPIOC, 0x40)
CCa 0x08028246 GPIO_SetBits(GPIOC, 0x40)
CCa 0x080282b6 GPIO_SetBits(GPIOC, 0x40)
CCa 0x080282c2 GPIO_SetBits(GPIOC, 0x40)
CCa 0x0802fb0e GPIO_SetBits(GPIOD, 0x80)

af+ 0x08026abe 14 GPIO_WriteBit()
CCa 0x08026abe ... maybe STM32LIB .. (r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)

af+ 0x0804ff74 244 Function_Called_Big_I2C_Function()
af+ 0x08050068 270 Function_Function_Called_Big_I2C_Function()
af+ 0x08046e00 156 Function_Function_Function_Called_Big_I2C_Function()
af+ 0x08032a3c 146 Function_Function_Function_Function_Called_Big_I2C_Function()
CCa 0x08032a3c Called from Task Start()


af+ 0x080355be 32 RCC_APB1_peripheral_clock_enable_register
af+ 0x080355de 32 RCC_APB2_peripheral_clock_enable_register

af+ 0x0804767c 62 dma_cmd_?()

af+ 0x08041f10 148 function_whith_lot_of_memmove_in_OS_ENTER_CRITICAL_1()
af+ 0x0804248c 318 function_whith_lot_of_memmove_in_OS_ENTER_CRITICAL_2()

af+ 0x0804b90c 8  short_function_1()
af+ 0x0804b914 36 short_function_2()
af+ 0x0804b976 2 short_function_3()
af+ 0x0804b974 2 short_function_4()


af+ 0x08042448 30 mem_transfet


af+ 0x08041ccc 28 f_0x400_0xfbff()


af+ 0x0804696e 186 Not_Big_I2C_Function2()
af+ 0x0804690c 22 Not_Big_I2C_Function3() 


af+ 0x0804696e 186 Not_Big_I2C_Function3()
CCa 0x8046a12 access auf 0x4000540 I2C1
CCa 0x8046a1c access auf 0x4000540 I2C1




af+ 0x08046a28 306 Big_I2C_Function()
CCa 0x8046a36 access auf 0x4000540 I2C1
CCa 0x8046a44 access auf 0x4000540 I2C1
CCa 0x8046a62 access auf 0x4000540 I2C1
CCa 0x8046a70 access auf 0x4000540 I2C1
CCa 0x8046a8c access auf 0x4000540 I2C1
CCa 0x8046a9a access auf 0x4000540 I2C1
CCa 0x8046aa8 access auf 0x4000540 I2C1
CCa 0x8046ac2 access auf 0x4000540 I2C1
CCa 0x8046ad0 access auf 0x4000540 I2C1
CCa 0x8046aee access auf 0x4000540 I2C1
CCa 0x8046afc access auf 0x4000540 I2C1
CCa 0x8046b18 access auf 0x4000540 I2C1
CCa 0x8046b32 access auf 0x4000540 I2C1
CCa 0x8046b4c access auf 0x4000540 I2C1

af+ 0x08046b5a 236  Verry_Big_Function_with_I2C1()
CCa 0x8046b68 access auf 0x4000540 I2C1
CCa 0x8046b76 access auf 0x4000540 I2C1
CCa 0x8046b94 access auf 0x4000540 I2C1
CCa 0x8046ba2 access auf 0x4000540 I2C1
CCa 0x8046bbe access auf 0x4000540 I2C1
CCa 0x8046bcc access auf 0x4000540 I2C1
CCa 0x8046bda access auf 0x4000540 I2C1
CCa 0x8046c02 access auf 0x4000540 I2C1
CCa 0x8046c10 access auf 0x4000540 I2C1
CCa 0x8046c1e access auf 0x4000540 I2C1
CCa 0x8046c38 access auf 0x4000540 I2C1

af+ 0x804fcb0 234 I2C1_Function_1()
af+ 0x804fda8 28 I2C1_Function_2()
af+ 0x804fdc4 28 I2C1_Function_3()
af+ 0x804fde0 28 I2C1_Function_4()
af+ 0x804fdfc 22 I2C1_Function_5()
af+ 0x804fe12 6 I2C1_Function_6()
af+ 0x804fe18 6 I2C1_Function_7()
af+ 0x804fe1e 52 I2C1_Function_8()
af+ 0x804fe52 66 I2C1_Function_9()

af+ 0x08046ca0 22 PWR_GetFlagStatus()
CCa 0x08046ca0 ... maybe STM32LIB

af+ 0x08046cb6 14 Function_PWR_CR__()
CCa 0x08046cb6 maybe Function ???

CCa 0x08046ca2 PWR_CSR
CCa 0x08046cb6 PWR_CR
CCa 0x08046cbe PWR_CR

af+ 0x08026a9a 22 GPIO_ReadInputDataBit()
CCa 0x08026a9a ... maybe STM32LIB  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)


CCa 0x0800df84 Function with check LCD_CS ....   why ....  when (set/noset) jump over function



CCa 0x08095736 Somsing with leds

CCa 0x0803a2c6 mybee vox

af+ 0x0804d268 208 Read_Channel_Switch()
af+ 0x0804d338 136 This_function_called_Read_Channel_Switch()

CCa 0x0804d29a add 1 for ECN0
CCa 0x0804d2ca add 2 for ECN1
CCa 0x0804d2fa add 4 for ECN2
CCa 0x0804d32a add 8 for ECN3
CCa 0x0804d336 end of Function  read and .. channal switch

CCa 0x080475c2 Function Test gpio with address from address

CCa 0x0804b5a4 Set BSHIFT
CCa 0x0804b5d4 Reset BSHIFT
CCa 0x0804b584 Reset BSHIFT
CCa 0x0804b5b2 Set PLL_DAT/DMR_SDI
CCa 0x0804b5bc Reset PLL_DAT/DMR_SDI

CCa 0x080443f8 Crate Process -LED Process- Thread Start  addr 0x809573d
CCa 0x0809573d LED Process()

CCa 0x08095740 from Codeplug Led Indikatore Enable 0x2001c658  ... maybe
CCa 0x08095752 GPIO_WriteBit(GPIOE, 1, 1) - RX_LED ON
CCa 0x08095752 GPIO_WriteBit(GPIOE, 1, 0) - RX_LED OFF
CCa 0x08095770 GPIO_WriteBit(GPIOE, 1, 0) - RX_LED OFF
CCa 0x0809577a GPIO_WriteBit(GPIOE, 2, 0) - TX_LED OFF
CCa 0x080957c6 GPIO_WriteBit(GPIOE, 2, 0) - TX_LED OFF
CCa 0x080957d0 GPIO_WriteBit(GPIOE, 1, 1) - RX_LED ON
CCa 0x080957ec GPIO_WriteBit(GPIOE, 1, 0) - RX_LED OFF
CCa 0x080957f6 GPIO_WriteBit(GPIOE, 2, 1) - TX_LED ON
CCa 0x08095842 GPIO_WriteBit(GPIOE, 1, 0) - RX_LED OFF
CCa 0x0809584c GPIO_WriteBit(GPIOE, 2, 0) - TX_LED OFF
CCa 0x0809586c GPIO_WriteBit(GPIOE, 1, 0) - RX_LED OFF
CCa 0x08095876 GPIO_WriteBit(GPIOE, 2, 0) - TX_LED OFF
CCa 0x08095884 GPIO_WriteBit(GPIOE, 1, 1) - RX_LED ON
CCa 0x0809588e GPIO_WriteBit(GPIOE, 2, 1) - TX_LED ON
CCa 0x0809589a GPIO_WriteBit(GPIOE, 1, 0) - RX_LED OFF
CCa 0x080958a4 GPIO_WriteBit(GPIOE, 2, 0) - TX_LED OFF


af+ 0x08049e9c 10 ADC_SoftwareStartConv()
CCa 0x08049e9c ... maybe STM32LIB ..Function start adc(r0=base address) Bit 30 SWSTART: Start conversion of regular channels

CCa 0x0803d026 Switch LAMP on ....

af+ 0x080426c8 536 Init_ADC()
CCa 0x080426c8 Function ... Init ADC1 (Bat) with dma  value via dma DMA2 DMA_S0CRin 0x2001cfcc

af+ 0x0804bd2c 86 DMA_Init()  
CCa 0x0804bd2c ... maybe STM32LIB 

af+ 0x0804bd82 26 DMA_Cmd()
CCa 0x0804bd82 ... maybe STM32LIB

af+ 0x08026136 114 RTC_Init()
CCa 0x8026136 ... maybe STM32LIB ErrorStatus RTC_Init(RTC_InitTypeDef* RTC_InitStruct) \n Beginn Function ... Set RTC_CR (Hour format RTC_CR and RTC_PRER ) r0  arg[0] arg[1] arg[2]\narg[0] RTC_CR\arg[1] RTC_PRER\n arg[2] RTC_PRER 

af+ 0x080261b8 82 RTC_EnterInitMode()
CCa 0x080261b8 ... maybe STM32LIB ErrorStatus RTC_EnterInitMode(void)

af+ 0x0802620a 18 RTC_ExitInitMode()
CCa 0x0802620a ... maybe STM32LIB Fuction void RTC_ExitInitMode(void)

af+ 0x0802621c 100 RTC_WaitForSynchro()
CCa 0x0802621c ... maybe STM32LIB ErrorStatus RTC_WaitForSynchro(void)

af+ 0x08026280 202 RTC_SetTime()
CCa 0x08026280 ... maybe STM32LIB ErrorStatus RTC_SetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct)

af+ 0x0802634a 80 RTC_GetTime()
CCa 0x0802634a ... maybe STM32LIB void RTC_GetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct)

af+ 0x0802639a 198 RTC_SetDate()
CCa 0x0802639a ... maybe STM32LIB ErrorStatus RTC_SetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct)

af+ 0x08026460 76 RTC_GetDate()
CCa 0x08026460 ... maybe STM32LIB void RTC_GetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct)

af+ 0x080264ac 56 RTC_WakeUpClockConfig()
CCa 0x080264ac ... maybe STM32LIB void RTC_WakeUpClockConfig(uint32_t RTC_WakeUpClock)

af+ 0x080264e4 32 RTC_SetWakeUpCounter()
CCa 0x080264e4 ... maybe STM32LIB void RTC_SetWakeUpCounter(uint32_t RTC_WakeUpCounter)

af+ 0x08026504 122 RTC_RefClockCmd()
CCa 0x08026504 ... maybe STM32LIB ErrorStatus RTC_RefClockCmd(FunctionalState NewState)

af+ 0x08026596 24 something_to_store_to_RTC_backup_registers()
CCa 0x08026596 something to store to RTC backup registers (RTC_BKPxR)

af+ 0x080265ba 84 RTC_ITConfig()
CCa 0x080265ba ... maybe STM32LIB void RTC_ITConfig(uint32_t RTC_IT, FunctionalState NewState)

af+ 0x08026616 28 RTC_ClearFlag()
CCa 0x08026616 ... maybe STM32LIB void RTC_ClearFlag(uint32_t RTC_FLAG)


af+ 0x08026662 24 RTC_Bcd2ToByte()
CCa 0x08026662 ... maybe STM32LIB

CCa 0x08044620 set A7 POW_C

af+ 0x08026ab0 6  GPIO_ReadInputData()
CCa 0x08026ab0 GPIO_ReadInputData(GPIO_TypeDef* GPIOx)


CCa 0x8026194 Call RTC_ExitInitMode()
CCa 0x802631e Call RTC_ExitInitMode()
CCa 0x8026434 Call RTC_ExitInitMode()
CCa 0x801760c Call RTC_SetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct)
CCa 0x808d0a2 Call RTC_SetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct)
CCa 0x800d940 Call RTC_GetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct)
CCa 0x8017218 Call RTC_GetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct)
CCa 0x808d624 Call RTC_GetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct)
CCa 0x8017644 Call RTC_SetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct)
CCa 0x808d0c8 Call RTC_SetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct)
CCa 0x800d8d0 Call RTC_GetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct)
CCa 0x801727c Call RTC_GetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct)
CCa 0x808d5f6 Call RTC_GetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct)
CCa 0x804f318 Call RTC_WakeUpClockConfig(uint32_t RTC_WakeUpClock)
CCa 0x804f31e Call RTC_SetWakeUpCounter(uint32_t RTC_WakeUpCounter)
CCa 0x804f2e0 Call RTC_RefClockCmd(FunctionalState NewState)
CCa 0x804f324 Call RTC_RefClockCmd(FunctionalState NewState)

CCa 0x0802631e call RTC_ExitInitMode(void)
CCa 0x0802632c call RTC_WaitForSynchro

CCa 0x0800df8e call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x080475e4 call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x08047620 call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804b5e2 call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804c24a call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804c258 call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804c266 call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804c2c0 call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804c2ce call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804c2dc call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804ccca call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804ccd6 call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804cce2 call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804cd26 call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804cd32 call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804cd3e call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804ce18 call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804d272 call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804d27e call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804d28a call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804d2a2 call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804d2ae call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804d2ba call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804d2d2 call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804d2de call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804d2ea call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804d302 call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804d30e call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804d31a call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)

CCa 0x08030f40 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x08030f64 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0803dc3a call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0803dc48 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0803dd70 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0803e0b2 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0803e0be call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0803e0e2 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0803e100 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0803e110 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0803e1f4 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0803e296 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0803e2a2 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0803e50e call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0803e51a call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0803e542 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0803e54e call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0803e5d8 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0803e5e4 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0803e69a call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0803e6a4 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x080458de call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x080458e8 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x08045952 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0804595c call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0804598a call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x08045994 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0804d57e call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0804d608 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0804e9b8 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0804eb1e call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0804ec78 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0804ed92 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x08093918 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x08095752 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x08095764 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x08095770 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0809577a call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x080957c6 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x080957d0 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x080957ec call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x080957f6 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x08095842 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0809584c call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0809586c call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x08095876 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x08095884 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0809588e call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0809589a call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x080958a4 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)

CCa 0x800ded4 config_byte_LED_enable_and_more
CCa 0x8012b9e config_byte_LED_enable_and_more
CCa 0x8012bae config_byte_LED_enable_and_more
CCa 0x8012bbe config_byte_LED_enable_and_more
CCa 0x8012bce config_byte_LED_enable_and_more
CCa 0x80154c2 config_byte_LED_enable_and_more
CCa 0x8016c02 config_byte_LED_enable_and_more
CCa 0x8016d26 config_byte_LED_enable_and_more
CCa 0x8016db0 config_byte_LED_enable_and_more
CCa 0x8016e5a config_byte_LED_enable_and_more
CCa 0x8017002 config_byte_LED_enable_and_more
CCa 0x8017bba config_byte_LED_enable_and_more
CCa 0x8017bc4 config_byte_LED_enable_and_more
CCa 0x8017bde config_byte_LED_enable_and_more
CCa 0x8017be8 config_byte_LED_enable_and_more
CCa 0x8017c02 config_byte_LED_enable_and_more
CCa 0x8017c0c config_byte_LED_enable_and_more
CCa 0x8017c18 config_byte_LED_enable_and_more
CCa 0x8017c22 config_byte_LED_enable_and_more
CCa 0x8017c2e config_byte_LED_enable_and_more
CCa 0x8017c38 config_byte_LED_enable_and_more
CCa 0x8017c44 config_byte_LED_enable_and_more
CCa 0x8017c4e config_byte_LED_enable_and_more
CCa 0x8017c5a config_byte_LED_enable_and_more
CCa 0x8017c64 config_byte_LED_enable_and_more
CCa 0x8017c70 config_byte_LED_enable_and_more
CCa 0x8017c7a config_byte_LED_enable_and_more
CCa 0x8017e80 config_byte_LED_enable_and_more
CCa 0x8017e8a config_byte_LED_enable_and_more
CCa 0x8017ea4 config_byte_LED_enable_and_more
CCa 0x8017eae config_byte_LED_enable_and_more
CCa 0x8017ec8 config_byte_LED_enable_and_more
CCa 0x8017ed2 config_byte_LED_enable_and_more
CCa 0x8017ede config_byte_LED_enable_and_more
CCa 0x8017ee8 config_byte_LED_enable_and_more
CCa 0x8017ef4 config_byte_LED_enable_and_more
CCa 0x8017efe config_byte_LED_enable_and_more
CCa 0x8017f0a config_byte_LED_enable_and_more
CCa 0x8017f14 config_byte_LED_enable_and_more
CCa 0x8017f22 config_byte_LED_enable_and_more
CCa 0x8017f2c config_byte_LED_enable_and_more
CCa 0x8017f38 config_byte_LED_enable_and_more
CCa 0x8017f42 config_byte_LED_enable_and_more
CCa 0x8018142 config_byte_LED_enable_and_more
CCa 0x8018256 config_byte_LED_enable_and_more
CCa 0x80182de config_byte_LED_enable_and_more
CCa 0x801851a config_byte_LED_enable_and_more
CCa 0x8018610 config_byte_LED_enable_and_more
CCa 0x8018c9e config_byte_LED_enable_and_more
CCa 0x8019188 config_byte_LED_enable_and_more
CCa 0x8019326 config_byte_LED_enable_and_more
CCa 0x80193b0 config_byte_LED_enable_and_more
CCa 0x8019438 config_byte_LED_enable_and_more
CCa 0x8019722 config_byte_LED_enable_and_more
CCa 0x801978e config_byte_LED_enable_and_more
CCa 0x80198a6 config_byte_LED_enable_and_more
CCa 0x80198ae config_byte_LED_enable_and_more
CCa 0x8019930 config_byte_LED_enable_and_more
CCa 0x8019938 config_byte_LED_enable_and_more
CCa 0x80199a6 config_byte_LED_enable_and_more
CCa 0x8019b34 config_byte_LED_enable_and_more
CCa 0x8019bc8 config_byte_LED_enable_and_more
CCa 0x8019c50 config_byte_LED_enable_and_more
CCa 0x8019cd2 config_byte_LED_enable_and_more
CCa 0x8019d60 config_byte_LED_enable_and_more
CCa 0x801a02a config_byte_LED_enable_and_more
CCa 0x801a544 config_byte_LED_enable_and_more
CCa 0x801a54c config_byte_LED_enable_and_more
CCa 0x801a554 config_byte_LED_enable_and_more
CCa 0x801a55c config_byte_LED_enable_and_more
CCa 0x801a564 config_byte_LED_enable_and_more
CCa 0x801a574 config_byte_LED_enable_and_more
CCa 0x801a584 config_byte_LED_enable_and_more
CCa 0x801a594 config_byte_LED_enable_and_more
CCa 0x801a5f0 config_byte_LED_enable_and_more
CCa 0x801a782 config_byte_LED_enable_and_more
CCa 0x801a78c config_byte_LED_enable_and_more
CCa 0x801a828 config_byte_LED_enable_and_more
CCa 0x801a832 config_byte_LED_enable_and_more
CCa 0x801a9ba config_byte_LED_enable_and_more
CCa 0x801ad0a config_byte_LED_enable_and_more
CCa 0x801afbe config_byte_LED_enable_and_more
CCa 0x801afcc config_byte_LED_enable_and_more
CCa 0x801afda config_byte_LED_enable_and_more
CCa 0x801afe8 config_byte_LED_enable_and_more
CCa 0x801f232 config_byte_LED_enable_and_more
CCa 0x801f33e config_byte_LED_enable_and_more
CCa 0x801f3ba config_byte_LED_enable_and_more
CCa 0x801f416 config_byte_LED_enable_and_more
CCa 0x801f558 config_byte_LED_enable_and_more
CCa 0x801f578 config_byte_LED_enable_and_more
CCa 0x801f7da config_byte_LED_enable_and_more
CCa 0x801f814 config_byte_LED_enable_and_more
CCa 0x801f882 config_byte_LED_enable_and_more
CCa 0x801f88c config_byte_LED_enable_and_more
CCa 0x801f996 config_byte_LED_enable_and_more
CCa 0x801fc04 config_byte_LED_enable_and_more
CCa 0x801fc9a config_byte_LED_enable_and_more
CCa 0x801fd12 config_byte_LED_enable_and_more
CCa 0x801ffe4 config_byte_LED_enable_and_more
CCa 0x80201ca config_byte_LED_enable_and_more
CCa 0x80207ce config_byte_LED_enable_and_more
CCa 0x80208e8 config_byte_LED_enable_and_more
CCa 0x80218ae config_byte_LED_enable_and_more
CCa 0x80218c0 config_byte_LED_enable_and_more
CCa 0x802352a config_byte_LED_enable_and_more
CCa 0x8023536 config_byte_LED_enable_and_more
CCa 0x8027c18 config_byte_LED_enable_and_more
CCa 0x8027c38 config_byte_LED_enable_and_more
CCa 0x802812a config_byte_LED_enable_and_more
CCa 0x8028230 config_byte_LED_enable_and_more
CCa 0x8028250 config_byte_LED_enable_and_more
CCa 0x80282ac config_byte_LED_enable_and_more
CCa 0x80282cc config_byte_LED_enable_and_more
CCa 0x8028fda config_byte_LED_enable_and_more
CCa 0x80293a2 config_byte_LED_enable_and_more
CCa 0x80294e8 config_byte_LED_enable_and_more
CCa 0x80295a2 config_byte_LED_enable_and_more
CCa 0x80295ce config_byte_LED_enable_and_more
CCa 0x8029774 config_byte_LED_enable_and_more
CCa 0x80299d4 config_byte_LED_enable_and_more
CCa 0x80299fc config_byte_LED_enable_and_more
CCa 0x8029a04 config_byte_LED_enable_and_more
CCa 0x8029a0a config_byte_LED_enable_and_more
CCa 0x8029a12 config_byte_LED_enable_and_more
CCa 0x802a8a8 config_byte_LED_enable_and_more
CCa 0x802a91a config_byte_LED_enable_and_more
CCa 0x802af8a config_byte_LED_enable_and_more
CCa 0x802af98 config_byte_LED_enable_and_more
CCa 0x80308da config_byte_LED_enable_and_more
CCa 0x8030d56 config_byte_LED_enable_and_more
CCa 0x8030d66 config_byte_LED_enable_and_more
CCa 0x8030fa8 config_byte_LED_enable_and_more
CCa 0x8031018 config_byte_LED_enable_and_more
CCa 0x8031020 config_byte_LED_enable_and_more
CCa 0x80372ba config_byte_LED_enable_and_more
CCa 0x80372f4 config_byte_LED_enable_and_more
CCa 0x80375d2 config_byte_LED_enable_and_more
CCa 0x803760c config_byte_LED_enable_and_more
CCa 0x8037774 config_byte_LED_enable_and_more
CCa 0x80377a4 config_byte_LED_enable_and_more
CCa 0x8037824 config_byte_LED_enable_and_more
CCa 0x8037854 config_byte_LED_enable_and_more
CCa 0x803bb72 config_byte_LED_enable_and_more
CCa 0x803bb7c config_byte_LED_enable_and_more
CCa 0x803c04e config_byte_LED_enable_and_more
CCa 0x803c672 config_byte_LED_enable_and_more
CCa 0x803c6c0 config_byte_LED_enable_and_more
CCa 0x803cb6a config_byte_LED_enable_and_more
CCa 0x803cb78 config_byte_LED_enable_and_more
CCa 0x803cc2c config_byte_LED_enable_and_more
CCa 0x803cc3a config_byte_LED_enable_and_more
CCa 0x803d02e config_byte_LED_enable_and_more
CCa 0x803d0b8 config_byte_LED_enable_and_more
CCa 0x803d106 config_byte_LED_enable_and_more
CCa 0x803ddce config_byte_LED_enable_and_more
CCa 0x803dddc config_byte_LED_enable_and_more
CCa 0x803ddec config_byte_LED_enable_and_more
CCa 0x803eefe config_byte_LED_enable_and_more
CCa 0x803f0c6 config_byte_LED_enable_and_more
CCa 0x803f1d8 config_byte_LED_enable_and_more
CCa 0x803f2ca config_byte_LED_enable_and_more
CCa 0x803f3d6 config_byte_LED_enable_and_more
CCa 0x803f42c config_byte_LED_enable_and_more
CCa 0x803f462 config_byte_LED_enable_and_more
CCa 0x803f506 config_byte_LED_enable_and_more
CCa 0x803f5a0 config_byte_LED_enable_and_more
CCa 0x803f716 config_byte_LED_enable_and_more
CCa 0x803f75a config_byte_LED_enable_and_more
CCa 0x803f78c config_byte_LED_enable_and_more
CCa 0x803f7e8 config_byte_LED_enable_and_more
CCa 0x803f87e config_byte_LED_enable_and_more
CCa 0x803f902 config_byte_LED_enable_and_more
CCa 0x803f934 config_byte_LED_enable_and_more
CCa 0x803f95c config_byte_LED_enable_and_more
CCa 0x803f98c config_byte_LED_enable_and_more
CCa 0x803f9ce config_byte_LED_enable_and_more
CCa 0x803fa1c config_byte_LED_enable_and_more
CCa 0x803fa44 config_byte_LED_enable_and_more
CCa 0x803fa88 config_byte_LED_enable_and_more
CCa 0x803fb1c config_byte_LED_enable_and_more
CCa 0x8040152 config_byte_LED_enable_and_more
CCa 0x80401ce config_byte_LED_enable_and_more
CCa 0x80401e6 config_byte_LED_enable_and_more
CCa 0x80401fe config_byte_LED_enable_and_more
CCa 0x8040216 config_byte_LED_enable_and_more
CCa 0x804022c config_byte_LED_enable_and_more
CCa 0x8040244 config_byte_LED_enable_and_more
CCa 0x8040260 config_byte_LED_enable_and_more
CCa 0x80409e4 config_byte_LED_enable_and_more
CCa 0x8040a44 config_byte_LED_enable_and_more
CCa 0x8040a8c config_byte_LED_enable_and_more
CCa 0x8040bea config_byte_LED_enable_and_more
CCa 0x8040d7e config_byte_LED_enable_and_more
CCa 0x8040dec config_byte_LED_enable_and_more
CCa 0x8041120 config_byte_LED_enable_and_more
CCa 0x8041148 config_byte_LED_enable_and_more
CCa 0x8043570 config_byte_LED_enable_and_more
CCa 0x8043cee config_byte_LED_enable_and_more
CCa 0x8043cfa config_byte_LED_enable_and_more
CCa 0x8043d04 config_byte_LED_enable_and_more
CCa 0x8044086 config_byte_LED_enable_and_more
CCa 0x804454a config_byte_LED_enable_and_more
CCa 0x80447a2 config_byte_LED_enable_and_more
CCa 0x80447ae config_byte_LED_enable_and_more
CCa 0x804a53e config_byte_LED_enable_and_more
CCa 0x804a548 config_byte_LED_enable_and_more
CCa 0x804a552 config_byte_LED_enable_and_more
CCa 0x804a61a config_byte_LED_enable_and_more
CCa 0x804a624 config_byte_LED_enable_and_more
CCa 0x804a62e config_byte_LED_enable_and_more
CCa 0x804a9cc config_byte_LED_enable_and_more
CCa 0x804a9d4 config_byte_LED_enable_and_more
CCa 0x804aa90 config_byte_LED_enable_and_more
CCa 0x804aa98 config_byte_LED_enable_and_more
CCa 0x804abe6 config_byte_LED_enable_and_more
CCa 0x804abee config_byte_LED_enable_and_more
CCa 0x804c478 config_byte_LED_enable_and_more
CCa 0x808d6a2 config_byte_LED_enable_and_more
CCa 0x808d96e config_byte_LED_enable_and_more
CCa 0x808d976 config_byte_LED_enable_and_more
CCa 0x808d984 config_byte_LED_enable_and_more
CCa 0x808d9c8 config_byte_LED_enable_and_more
CCa 0x8095740 config_byte_LED_enable_and_more
CCa 0x80957ac config_byte_LED_enable_and_more
