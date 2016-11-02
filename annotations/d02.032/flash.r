# flash.r by Travis Goodspeed

# This is a Radare2 script for annotating the Tytera MD380
# application, version 2.032, which begins at 0x0800C000 in Flash
# memory.  Preceding it is a bootloader, which can be either the
# official factory bootloader or an aftermarket one.


# Begin by opening the application in R2 with this script.
# radare2 -a arm -m 0x0800C000 -b 16 -i flash.r ../../firmware/D002.032.bin

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


CCa 0x08014520 Create_Menu_Entry_RX_QRG_2
CCa 0x08014524 Create_Menu_Entry_RX_QRG_3
CCa 0x080154de md380_usbbuf
CCa 0x0802631e call RTC_ExitInitMode(void)
CCa 0x0802632c call RTC_WaitForSynchro
CCa 0x0802634a ... maybe STM32LIB void RTC_GetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct)
CCa 0x0802639a ... maybe STM32LIB ErrorStatus RTC_SetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct)
CCa 0x08026460 ... maybe STM32LIB void RTC_GetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct)
CCa 0x080264ac ... maybe STM32LIB void RTC_WakeUpClockConfig(uint32_t RTC_WakeUpClock)
CCa 0x080264e4 ... maybe STM32LIB void RTC_SetWakeUpCounter(uint32_t RTC_WakeUpCounter)
CCa 0x08026504 ... maybe STM32LIB ErrorStatus RTC_RefClockCmd(FunctionalState NewState)
CCa 0x08026596 something to store to RTC backup registers (RTC_BKPxR)
CCa 0x080265ba ... maybe STM32LIB void RTC_ITConfig(uint32_t RTC_IT, FunctionalState NewState)
CCa 0x08026616 ... maybe STM32LIB void RTC_ClearFlag(uint32_t RTC_FLAG)
CCa 0x08026662 ... maybe STM32LIB
CCa 0x08026a9a ... maybe STM32LIB  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x08026ab0 GPIO_ReadInputData(GPIO_TypeDef* GPIOx)
CCa 0x08026ab6 ... maybe STM32LIB
CCa 0x08026aba ... maybe STM32LIB
CCa 0x08026abe ... maybe STM32LIB .. (r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x08027c22 GPIO_SetBits(GPIOC, 0x40)
CCa 0x08027c2e GPIO_SetBits(GPIOC, 0x40)
CCa 0x08028120 GPIO_SetBits(GPIOC, 0x40)
CCa 0x0802823a GPIO_SetBits(GPIOC, 0x40)
CCa 0x08028246 GPIO_SetBits(GPIOC, 0x40)
CCa 0x080282b6 GPIO_SetBits(GPIOC, 0x40)
CCa 0x080282c2 GPIO_SetBits(GPIOC, 0x40)
CCa 0x0802ab68 beginn roger beep
CCa 0x0802acd8 beginn dmr sync
CCa 0x0802fb0e GPIO_SetBits(GPIOD, 0x80)
CCa 0x08030f40 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x08030f64 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x08032a3c Called from Task Start()
CCa 0x0803a2c6 mybee vox
CCa 0x0803d026 Switch LAMP on ....
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
CCa 0x0803eea0 dmr_apply_squelch_hook
CCa 0x0803ef64 dmr_apply_privsquelch_hook
CCa 0x080426c8 Function ... Init ADC1 (Bat) with dma  value via dma DMA2 DMA_S0CRin 0x2001cfcc
CCa 0x080443f8 Create Process -LED Process- Thread Start  addr 0x809573d
CCa 0x08044620 set A7 POW_C
CCa 0x080458de call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x080458e8 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x08045952 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0804595c call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0804598a call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x08045994 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x08046ca0 ... maybe STM32LIB
CCa 0x08046ca2 PWR_CSR
CCa 0x08046cb6 maybe Function
CCa 0x08046cb6 PWR_CR
CCa 0x08046cbe PWR_CR
CCa 0x080475c2 Function Test gpio with address from address
CCa 0x080475e4 call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x08047620 call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x08049e9c ... maybe STM32LIB ..Function start adc(r0=base address) Bit 30 SWSTART: Start conversion of regular channels
CCa 0x0804b584 Reset BSHIFT
CCa 0x0804b5a4 Set BSHIFT
CCa 0x0804b5b2 Set PLL_DAT/DMR_SDI
CCa 0x0804b5bc Reset PLL_DAT/DMR_SDI
CCa 0x0804b5d4 Reset BSHIFT
CCa 0x0804b5e2 call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804bd2c ... maybe STM32LIB
CCa 0x0804bd82 ... maybe STM32LIB
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
CCa 0x0804d29a add 1 for ECN0
CCa 0x0804d2a2 call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804d2ae call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804d2ba call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804d2ca add 2 for ECN1
CCa 0x0804d2d2 call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804d2de call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804d2ea call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804d2fa add 4 for ECN2
CCa 0x0804d302 call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804d30e call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804d31a call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804d32a add 8 for ECN3
CCa 0x0804d336 end of Function  read and .. channal switch
CCa 0x0804d57e call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0804d608 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0804e9b8 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0804eb1e call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0804ec78 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0804ed92 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0808ccc2 md380_dfu_state
CCa 0x0808ccce md380_blockadr
CCa 0x0808cce4 md380_packetlen
CCa 0x0808cd00 md380_packet
CCa 0x0808cd1a md380_dfutargetadr
CCa 0x0808cd1a md380_dfu_target_adr
CCa 0x0808d234 !!! md380_thingy2 + 1 !!!!!!!!
CCa 0x08093918 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0809573d LED Process()
CCa 0x08095740 from Codeplug Led Indikatore Enable 0x2001c658  ... maybe
CCa 0x08095752 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x08095752 GPIO_WriteBit(GPIOE, 1, 0) - RX_LED OFF
CCa 0x08095752 GPIO_WriteBit(GPIOE, 1, 1) - RX_LED ON
CCa 0x08095764 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x08095770 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x08095770 GPIO_WriteBit(GPIOE, 1, 0) - RX_LED OFF
CCa 0x0809577a call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0809577a GPIO_WriteBit(GPIOE, 2, 0) - TX_LED OFF
CCa 0x080957c6 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x080957c6 GPIO_WriteBit(GPIOE, 2, 0) - TX_LED OFF
CCa 0x080957d0 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x080957d0 GPIO_WriteBit(GPIOE, 1, 1) - RX_LED ON
CCa 0x080957ec call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x080957ec GPIO_WriteBit(GPIOE, 1, 0) - RX_LED OFF
CCa 0x080957f6 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x080957f6 GPIO_WriteBit(GPIOE, 2, 1) - TX_LED ON
CCa 0x08095842 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x08095842 GPIO_WriteBit(GPIOE, 1, 0) - RX_LED OFF
CCa 0x0809584c call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0809584c GPIO_WriteBit(GPIOE, 2, 0) - TX_LED OFF
CCa 0x0809586c call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0809586c GPIO_WriteBit(GPIOE, 1, 0) - RX_LED OFF
CCa 0x08095876 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x08095876 GPIO_WriteBit(GPIOE, 2, 0) - TX_LED OFF
CCa 0x08095884 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x08095884 GPIO_WriteBit(GPIOE, 1, 1) - RX_LED ON
CCa 0x0809588e call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0809588e GPIO_WriteBit(GPIOE, 2, 1) - TX_LED ON
CCa 0x0809589a call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0809589a GPIO_WriteBit(GPIOE, 1, 0) - RX_LED OFF
CCa 0x080958a4 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x080958a4 GPIO_WriteBit(GPIOE, 2, 0) - TX_LED OFF
CCa 0x080969e4 md380_usbstring
CCa 0x800c000 0x2001dc10
CCa 0x800c004 0x80fa969
CCa 0x800c008 0x80937f1
CCa 0x800c00c 0x80937f9
CCa 0x800c010 0x8093801
CCa 0x800c014 0x8093809
CCa 0x800c018 0x8093811
CCa 0x800c030 0x809381b hase
CCa 0x800c03c 0x0809381d F_5003 Ticker
CCa 0x800c060 0x08093899 F_5007
CCa 0x800c0e8 0x809383f F_5004
CCa 0x800c0f0 F_5036 0x08093b25
CCa 0x800c108 F_5010 08093975
CCa 0x800c158 0x80fdd95 .. 0x80fdd94 while 1
CCa 0x800c160 0x80fdd9d .. 0x80fdd9c while 1
af+ 0x800c188 1448 Create_MainMenyEntry
CCa 0x800c18a md380_menu_0x200011e4
CCa 0x800c192 md380_menu_id
CCa 0x800c1aa copy Volume_R_Value
CCa 0x800c1ae VolumeKnop
CCa 0x800c1ce md380_menu_0x200011e4
CCa 0x800c1d6 md380_menu_memory
CCa 0x800c1ea md380_menu_mem_base
CCa 0x800c270 ... C.o.n.t.a.c.t.s.
CCa 0x800c272 md380_menu_id
CCa 0x800c282 Test Can this Channel Scan 
CCa 0x800c29a 3 Menu entry shown but not aktive
CCa 0x800c2b4 ... S.c.a.n
CCa 0x800c2b6 md380_menu_id
CCa 0x800c2e8 ... S.c.a.n
CCa 0x800c2ea md380_menu_id
CCa 0x800c31a ... Z.o.n.e
CCa 0x800c31c md380_menu_id
CCa 0x800c33a ... 0x802bfd4 Create_Menu_Entry_Message
CCa 0x800c34c ... M.e.s.s.a.g.e.s.
CCa 0x800c34e md380_menu_id
CCa 0x800c37e ... C.a.l.l...Log
CCa 0x800c380 md380_menu_id
CCa 0x800c39e 0x80126a8 -> Create_Menu_Utilies
CCa 0x800c3b0 ... U.t.i.l.i.t.i.e.s
CCa 0x800c3b2 md380_menu_id
CCa 0x800c3c0 md380_menu_0x200011e4
CCa 0x800c3ca md380_menu_id
CCa 0x800c3ec md380_menu_0x200011e4
CCa 0x800c462 ... C.o.n.t.a.c.t.s
CCa 0x800c464 md380_menu_id
CCa 0x800c49e ... S.c.a.n
CCa 0x800c4a0 md380_menu_id
CCa 0x800c4ca md380_menu_id
CCa 0x800c4f2 md380_menu_id
CCa 0x800c51a md380_menu_id
CCa 0x800c542 md380_menu_id
CCa 0x800c56a md380_menu_id
CCa 0x800c576 md380_menu_0x200011e4
CCa 0x800c57e md380_menu_id
CCa 0x800c598 md380_menu_0x200011e4
CCa 0x800c5e8 md380_menu_id
CCa 0x800c60c md380_menu_id
CCa 0x800c642 md380_menu_id
CCa 0x800c66c md380_menu_id
CCa 0x800c694 md380_menu_id
CCa 0x800c6bc md380_menu_id
CCa 0x800c6e4 md380_menu_id
CCa 0x800c6f0 md380_menu_0x200011e4
CCa 0x800c6f8 md380_menu_id
CCa 0x800c712 md380_menu_0x200011e4
CCa 0x800c71e md380_menu_mem_base
af+ 0x800c730 86 F_249_Create_MenuEntry
CCa 0x800c73c md380_menu_mem_base
CCa 0x800c74a md380_menu_mem_base
CCa 0x800c756 md380_menu_mem_base
CCa 0x800c762 md380_menu_mem_base
CCa 0x800c76e md380_menu_mem_base
CCa 0x800c77a md380_menu_mem_base
af+ 0x800c7e8 2980 F_4140
af+ 0x800d390 378 F_4495
CCa 0x800d392 md380_menu_0x200011e4
af+ 0x800d518 320 F_4141
CCa 0x800d55c md380_menu_0x200011e4
CCa 0x800d566 md380_menu_0x200011e4
CCa 0x800d57a md380_menu_0x200011e4
af+ 0x800d66c 178 F_4139
af+ 0x800d730 2 F_4147
af+ 0x800d732 188 F_4283
CCa 0x800d74e md380_menu_id
CCa 0x800d758 md380_menu_mem_base
af+ 0x800d7f4 114 Clear_Display
af+ 0x800d86c 30 F_4039_something_write_to_screen
af+ 0x800d88a 36 gfx_drawtext
af+ 0x800d8ae 254 F_785_Print_Date_and_Time
CCa 0x800d8ca 0x2001d1ac Date
CCa 0x800d8d0 Call RTC_GetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct)
CCa 0x800d8d4 20xx year 
CCa 0x800d8e0 0x2001d1ac Date
CCa 0x800d8f6 0x2001d1ac Date
CCa 0x800d90c 0x2001d1ac Date
CCa 0x800d93a 0x2001d1a8 Time
CCa 0x800d940 Call RTC_GetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct)
CCa 0x800d944 0x2001d1a8 Time
CCa 0x800d95a 0x2001d1a8 Time
CCa 0x800d970 0x2001d1a8 Time
CCa 0x800d986 0x9d 157
CCa 0x800d988 0x60 96
CCa 0x800d98a 0x5f 95
af+ 0x800d9b0 18 F_4153
af+ 0x800d9cc 124 F_4220
af+ 0x800da54 124 F_786
af+ 0x800dad4 386 F_4154
CCa 0x800daf0 md380_menu_0x2001d3f0
CCa 0x800dbbe md380_menu_0x2001d3ee
af+ 0x800dc56 230 F_4284
af+ 0x800dd40 284 F_1056
af+ 0x800de5c 96 F_972
af+ 0x800debc 126 F_787
CCa 0x800ded4 SystemConfig
CCa 0x800ded8 ConfigData + 0x17 (byte) F_787
af+ 0x800df84 996 Volume_Menu
CCa 0x800df8e call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x800dfa4 md380_menu_0x200011e4
CCa 0x800dfae md380_menu_0x200011e4
CCa 0x800dfb6 md380_menu_0x200011e4
CCa 0x800dfbe md380_menu_memory
CCa 0x800dfd6 ... V.o.l.u.m.e
CCa 0x800dfdc md380_menu_id
CCa 0x800dfe2 md380_menu_mem_base
CCa 0x800dff8 md380_menu_0x200011e4
CCa 0x800e016 glob. String ..Volume (0-9)
CCa 0x800e01a md380_menu_id
CCa 0x800e028 Default 1 sec
CCa 0x800e038 glob. String ..Volume (0-9)
CCa 0x800e03c md380_menu_id
CCa 0x800e044 md380_menu_0x200011e4
CCa 0x800e04c md380_menu_0x200011e4
CCa 0x800e052 md380_menu_0x200011e4
CCa 0x800e05c md380_menu_id
CCa 0x800e060 md380_menu_0x200011e4
CCa 0x800e068 md380_menu_memory
CCa 0x800e0ac set glob. String ..Volume (0-9)
CCa 0x800e0cc V
CCa 0x800e0d2 o
CCa 0x800e0f8 act Volume 0-9
CCa 0x800e0f8 VolumeKnop
CCa 0x800e228 C.o.n.f.i.r.m
CCa 0x800e244 ..B.a.c.k
CCa 0x800e2b0 C.o.n.t.a.c.t.s.
CCa 0x800e2cc compare bothsVolume_R_Value
CCa 0x800e2cc VolumeKnop
CCa 0x800e2d2 VolumeKnop
CCa 0x800e2de VolumeKnop
CCa 0x800e2fc VolumeKnop
CCa 0x800e30a VolumeKnop
CCa 0x800e30e VolumeKnop
CCa 0x800e31a VolumeKnop
CCa 0x800e338 VolumeKnop
CCa 0x800e346 copy Volume_R_Value
CCa 0x800e34a VolumeKnop
CCa 0x800e3a8 md380_menu_id
af+ 0x800e3c4 1820 F_4142
CCa 0x800e40c md380_menu_0x200011e4
CCa 0x800e418 md380_menu_0x200011e4
CCa 0x800e42c md380_menu_0x200011e4
CCa 0x800e568 md380_menu_0x200011e4
CCa 0x800e570 md380_menu_0x200011e4
CCa 0x800e576 md380_menu_0x200011e4
CCa 0x800e580 md380_menu_id
CCa 0x800e586 md380_menu_0x200011e4
CCa 0x800e58e md380_menu_memory
CCa 0x800e63e md380_menu_0x200011e4
CCa 0x800e652 md380_menu_0x200011e4
CCa 0x800e658 md380_menu_0x200011e4
CCa 0x800e662 md380_menu_id
CCa 0x800e668 md380_menu_0x200011e4
CCa 0x800e670 md380_menu_memory
CCa 0x800e8c8 md380_menu_0x200011e4
af+ 0x800eb10 414 F_4144
CCa 0x800eb5e md380_menu_0x200011e4
af+ 0x800ecc0 418 F_4145
CCa 0x800ed0a md380_menu_0x200011e4
af+ 0x800ee94 1422 F_4143
CCa 0x800eef2 md380_menu_0x200011e4
CCa 0x800eefe md380_menu_0x200011e4
CCa 0x800ef50 md380_menu_0x200011e4
CCa 0x800ef58 md380_menu_0x200011e4
CCa 0x800ef5e md380_menu_0x200011e4
CCa 0x800ef68 md380_menu_id
CCa 0x800ef6c md380_menu_0x200011e4
CCa 0x800ef74 md380_menu_memory
CCa 0x800f008 md380_menu_0x200011e4
CCa 0x800f010 md380_menu_0x200011e4
CCa 0x800f016 md380_menu_0x200011e4
CCa 0x800f038 md380_menu_0x200011e4
CCa 0x800f040 md380_menu_0x200011e4
CCa 0x800f046 md380_menu_0x200011e4
CCa 0x800f050 md380_menu_id
CCa 0x800f054 md380_menu_0x200011e4
CCa 0x800f05c md380_menu_memory
af+ 0x800f422 48 F_251
CCa 0x800f434 md380_menu_memory
af+ 0x800f452 18 Menu_Back
af+ 0x800f464 32 F_86
af+ 0x800f488 294 F_4155
CCa 0x800f48e md380_menu_0x200011e4
CCa 0x800f494 md380_menu_memory
CCa 0x800f4b0 md380_menu_id
CCa 0x800f4b8 md380_menu_mem_base
CCa 0x800f506 md380_menu_id
CCa 0x800f534 md380_menu_id
CCa 0x800f57e md380_menu_id
CCa 0x800f590 md380_menu_id
CCa 0x800f59a md380_menu_mem_base
CCa 0x800f5b0 md380_menu_0x200011e4
CCa 0x800f5b6 md380_menu_memory
CCa 0x800f5c6 md380_menu_id
CCa 0x800f5ce md380_menu_mem_base
CCa 0x800f600 md380_menu_id
CCa 0x800f60e md380_menu_id
CCa 0x800f618 md380_menu_mem_base
CCa 0x800f640 md380_menu_0x200011e4
CCa 0x800f646 md380_menu_memory
CCa 0x800f674 md380_menu_id
CCa 0x800f67c md380_menu_mem_base
CCa 0x800f6b4 ... E.m.p.t.y
CCa 0x800f6b8 md380_menu_id
CCa 0x800f6ca md380_menu_id
CCa 0x800f6d2 md380_menu_mem_base
CCa 0x800f704 md380_menu_id
CCa 0x800f712 md380_menu_id
CCa 0x800f71c md380_menu_mem_base
CCa 0x800f808 md380_menu_0x200011e4
CCa 0x800f810 md380_menu_memory
CCa 0x800f844 md380_menu_id
CCa 0x800f84c md380_menu_mem_base
CCa 0x800f888 md380_menu_id
CCa 0x800f89a md380_menu_id
CCa 0x800f8a2 md380_menu_mem_base
CCa 0x800f8d4 md380_menu_id
CCa 0x800f8e2 md380_menu_id
CCa 0x800f8ec md380_menu_mem_base
CCa 0x800f98c md380_menu_0x200011e4
CCa 0x800f994 md380_menu_memory
CCa 0x800f9c6 md380_menu_id
CCa 0x800f9ce md380_menu_mem_base
CCa 0x800fa0a md380_menu_id
CCa 0x800fa3a md380_menu_id
CCa 0x800fa6e md380_menu_id
CCa 0x800faa2 md380_menu_id
CCa 0x800fad2 md380_menu_id
CCa 0x800fb00 md380_menu_id
CCa 0x800fb2e md380_menu_id
CCa 0x800fb3e md380_menu_id
CCa 0x800fb46 md380_menu_mem_base
CCa 0x800fbac md380_menu_0x200011e4
CCa 0x800fbb4 md380_menu_memory
CCa 0x800fbd8 md380_menu_id
CCa 0x800fbde md380_menu_mem_base
CCa 0x800fc08 md380_menu_id
af+ 0x800fc12 34 F_4286
CCa 0x800fc4a md380_menu_0x200011e4
CCa 0x800fc52 md380_menu_memory
CCa 0x800fc6a md380_menu_id
CCa 0x800fc70 md380_menu_mem_base
CCa 0x800fc9a md380_menu_id
CCa 0x800fd16 md380_menu_0x200011e4
CCa 0x800fd1e md380_menu_memory
CCa 0x800fd36 md380_menu_id
CCa 0x800fd3c md380_menu_mem_base
CCa 0x800fd66 md380_menu_id
CCa 0x800fdc6 md380_menu_0x200011e4
CCa 0x800fdce md380_menu_memory
CCa 0x800fde6 md380_menu_id
CCa 0x800fdec md380_menu_mem_base
CCa 0x800fe16 md380_menu_id
CCa 0x800feee md380_menu_0x200011e4
CCa 0x800fef4 md380_menu_memory
CCa 0x800ff12 md380_menu_id
CCa 0x800ff1a md380_menu_mem_base
CCa 0x800ff44 md380_menu_id
CCa 0x8010002 md380_menu_0x200011e4
CCa 0x8010008 md380_menu_memory
CCa 0x8010026 md380_menu_id
CCa 0x801002e md380_menu_mem_base
CCa 0x8010072 md380_menu_id
CCa 0x80100a0 ... R.X
CCa 0x80100a4 md380_menu_id
CCa 0x80100da md380_menu_0x200011e4
CCa 0x80100e2 md380_menu_memory
CCa 0x8010100 md380_menu_id
CCa 0x8010108 md380_menu_mem_base
CCa 0x801013e ... Y.e.s
CCa 0x8010140 md380_menu_id
CCa 0x801016c ... N.o.
CCa 0x801016e md380_menu_id
CCa 0x80103ca md380_menu_0x200011e4
CCa 0x80103d2 md380_menu_memory
CCa 0x80103e2 md380_menu_id
CCa 0x80103ea md380_menu_mem_base
CCa 0x801046a md380_menu_id
CCa 0x8010498 md380_menu_id
CCa 0x80104e0 ... E.d.i.t
CCa 0x80104e2 md380_menu_id
CCa 0x8010514 ... E.d.i.t
CCa 0x8010516 md380_menu_id
CCa 0x8010546 ... R.i.n.g
CCa 0x8010548 md380_menu_id
CCa 0x801058c ... P.r.o.g.r.a.m
CCa 0x801058e md380_menu_id
CCa 0x80105d2 ... D.e.l.e.t.e
CCa 0x80105d4 md380_menu_id
CCa 0x8010604 md380_menu_id
CCa 0x8010648 ... R.a.d.i.o
CCa 0x801064a md380_menu_id
CCa 0x801068e ... R.e.m.o.t.e
CCa 0x8010690 md380_menu_id
CCa 0x80106d4 ... R.a.d.i.o
CCa 0x80106d6 md380_menu_id
CCa 0x8010716 ... R.a.d.i.o
CCa 0x8010718 md380_menu_id
CCa 0x801072c md380_menu_id
CCa 0x8010736 md380_menu_mem_base
CCa 0x8010784 ... V.i.e.w
CCa 0x8010786 md380_menu_id
CCa 0x8010796 md380_menu_id
CCa 0x80107a0 md380_menu_mem_base
CCa 0x80107d2 ... V.i.e.w
CCa 0x80107d4 md380_menu_id
CCa 0x80107fe md380_menu_id
CCa 0x801080e md380_menu_id
CCa 0x8010816 md380_menu_mem_base
af+ 0x8010838 260 F_4221
af+ 0x8010950 124 F_4156
af+ 0x80109d0 150 F_4349
CCa 0x8010afc md380_menu_0x200011e4
CCa 0x8010b02 md380_menu_memory
CCa 0x8010b10 md380_menu_id
CCa 0x8010b16 md380_menu_mem_base
CCa 0x8010b44 md380_menu_id
af+ 0x8010b6a 96 F_4157
CCa 0x8010bd6 md380_menu_0x200011e4
CCa 0x8010bde md380_menu_memory
CCa 0x8010bfa md380_menu_id
CCa 0x8010c00 md380_menu_mem_base
CCa 0x8010c2e md380_menu_id
CCa 0x8010d84 md380_menu_0x200011e4
CCa 0x8010d8c md380_menu_memory
CCa 0x8010daa md380_menu_id
CCa 0x8010db0 md380_menu_mem_base
CCa 0x8010ddc md380_menu_id
CCa 0x8010de8 md380_menu_id
CCa 0x8010df0 md380_menu_mem_base
CCa 0x8010ff8 md380_menu_0x200011e4
CCa 0x8011000 md380_menu_memory
CCa 0x8011050 md380_menu_id
CCa 0x8011058 md380_menu_mem_base
CCa 0x8011092 md380_menu_id
CCa 0x80110c6 md380_menu_id
CCa 0x80110ce md380_menu_mem_base
CCa 0x8011108 md380_menu_id
CCa 0x8011160 md380_menu_id
CCa 0x8011168 md380_menu_mem_base
CCa 0x80111a2 md380_menu_id
CCa 0x80111cc md380_menu_id
CCa 0x80111de md380_menu_id
CCa 0x80111e8 md380_menu_mem_base
CCa 0x801120a md380_menu_id
CCa 0x8011212 md380_menu_mem_base
CCa 0x8011244 md380_menu_id
CCa 0x8011252 md380_menu_0x200011e4
CCa 0x8011258 md380_menu_memory
CCa 0x8011270 md380_menu_id
CCa 0x8011278 md380_menu_mem_base
CCa 0x80112aa md380_menu_id
CCa 0x80112fe md380_menu_0x200011e4
CCa 0x8011304 md380_menu_0x200011e4
CCa 0x8011308 md380_menu_0x200011e4
CCa 0x8011310 md380_menu_id
CCa 0x8011316 md380_menu_0x200011e4
CCa 0x801131c md380_menu_memory
CCa 0x8011334 md380_menu_id
CCa 0x801133c md380_menu_mem_base
CCa 0x8011372 md380_menu_id
CCa 0x801139a md380_menu_id
CCa 0x80113ac md380_menu_id
CCa 0x80113b6 md380_menu_mem_base
CCa 0x80113de md380_menu_0x200011e4
CCa 0x80113e4 md380_menu_memory
CCa 0x80113fa md380_menu_id
CCa 0x8011402 md380_menu_mem_base
CCa 0x8011432 md380_menu_id
CCa 0x801143c md380_menu_0x200011e4
CCa 0x8011442 md380_menu_memory
CCa 0x8011468 md380_menu_0x2001d3f4
CCa 0x8011470 mn_editbuffer_poi
CCa 0x8011474 md380_menu_edit_buf
CCa 0x8011480 mn_editbuffer_poi
CCa 0x8011494 md380_menu_0x2001d3ed
CCa 0x801149c md380_menu_0x2001d3ee
CCa 0x80114a4 md380_menu_0x2001d3ef
CCa 0x80114ac md380_menu_0x2001d3f1
CCa 0x80114b4 md380_menu_0x2001d3f0
CCa 0x80114bc md380_menu_0x200011e4
CCa 0x80114c2 md380_menu_memory
CCa 0x80114da md380_menu_id
CCa 0x80114e2 md380_menu_mem_base
CCa 0x801150a mn_editbuffer_poi
CCa 0x8011510 md380_menu_id
CCa 0x8011560 md380_menu_edit_buf
CCa 0x8011582 md380_menu_0x200011e4
CCa 0x801158a md380_menu_memory
CCa 0x80115a6 md380_menu_id
CCa 0x80115ae md380_menu_mem_base
CCa 0x80115e8 md380_menu_id
CCa 0x80115f4 mn_editbuffer_poi
CCa 0x8011600 md380_menu_0x200011e4
CCa 0x8011608 md380_menu_memory
CCa 0x8011624 md380_menu_id
CCa 0x801162c md380_menu_mem_base
CCa 0x8011666 md380_menu_id
CCa 0x8011672 md380_menu_0x200011e4
CCa 0x801167a md380_menu_memory
CCa 0x8011694 md380_menu_id
CCa 0x801169c md380_menu_mem_base
CCa 0x80116d4 md380_menu_id
CCa 0x80116fa mn_editbuffer_poi
CCa 0x8011750 mn_editbuffer_poi
CCa 0x8011762 mn_editbuffer_poi
CCa 0x8011784 mn_editbuffer_poi
CCa 0x8011794 mn_editbuffer_poi
CCa 0x80117ca md380_menu_0x2001d3f4
CCa 0x80117d2 mn_editbuffer_poi
CCa 0x80117d6 md380_menu_edit_buf
CCa 0x80117e2 mn_editbuffer_poi
CCa 0x80117f6 md380_menu_0x2001d3ed
CCa 0x80117fe md380_menu_0x2001d3ee
CCa 0x8011806 md380_menu_0x2001d3ef
CCa 0x801180e md380_menu_0x2001d3f1
CCa 0x8011816 md380_menu_0x2001d3f0
CCa 0x801181e md380_menu_0x200011e4
CCa 0x8011826 md380_menu_memory
CCa 0x8011844 md380_menu_id
CCa 0x801184c md380_menu_mem_base
CCa 0x8011876 mn_editbuffer_poi
CCa 0x801187c md380_menu_id
CCa 0x801189e md380_menu_edit_buf
CCa 0x8011912 md380_menu_0x200011e4
CCa 0x801191a md380_menu_memory
CCa 0x801193a md380_menu_id
CCa 0x8011942 md380_menu_mem_base
CCa 0x801197e md380_menu_id
CCa 0x801198a md380_menu_0x200011e4
CCa 0x8011992 md380_menu_memory
CCa 0x801199e mn_editbuffer_poi
CCa 0x80119d6 mn_editbuffer_poi
CCa 0x80119e6 mn_editbuffer_poi
CCa 0x80119f6 mn_editbuffer_poi
CCa 0x8011a42 md380_menu_id
CCa 0x8011a48 md380_menu_mem_base
CCa 0x8011a8c md380_menu_id
CCa 0x8011abc md380_menu_id
CCa 0x8011ad8 md380_menu_id
CCa 0x8011ade md380_menu_mem_base
CCa 0x8011b18 md380_menu_id
CCa 0x8011b3a mn_editbuffer_poi
CCa 0x8011bb8 md380_menu_0x200011e4
CCa 0x8011bc0 md380_menu_memory
CCa 0x8011bde md380_menu_id
CCa 0x8011be4 md380_menu_mem_base
CCa 0x8011c12 md380_menu_id
CCa 0x8011c2a md380_menu_0x200011e4
CCa 0x8011c32 md380_menu_0x200011e4
CCa 0x8011c38 md380_menu_0x200011e4
CCa 0x8011c40 md380_menu_id
CCa 0x8011c44 md380_menu_0x200011e4
CCa 0x8011c4a md380_menu_memory
CCa 0x8011c64 md380_menu_id
CCa 0x8011c6a md380_menu_mem_base
CCa 0x8011ca8 md380_menu_id
CCa 0x8011cd2 md380_menu_id
CCa 0x8011d14 md380_menu_0x200011e4
CCa 0x8011d1a md380_menu_memory
CCa 0x8011d34 md380_menu_id
CCa 0x8011d3c md380_menu_mem_base
CCa 0x8011d64 md380_menu_id
CCa 0x8011d76 md380_menu_0x200011e4
CCa 0x8011d7c md380_menu_0x200011e4
CCa 0x8011d80 md380_menu_0x200011e4
CCa 0x8011d88 md380_menu_id
CCa 0x8011d8e md380_menu_0x200011e4
CCa 0x8011d94 md380_menu_memory
CCa 0x8011dac md380_menu_id
CCa 0x8011db4 md380_menu_mem_base
CCa 0x8011df2 md380_menu_id
CCa 0x8011e1e md380_menu_id
CCa 0x8011e5c md380_menu_0x200011e4
CCa 0x8011e62 md380_menu_memory
CCa 0x8011e7a md380_menu_id
CCa 0x8011e82 md380_menu_mem_base
CCa 0x8011eaa md380_menu_id
CCa 0x8011ed4 md380_menu_0x2001d3f4
CCa 0x8011ee2 md380_menu_0x200011e4
CCa 0x8011ee8 md380_menu_0x200011e4
CCa 0x8011eec md380_menu_0x200011e4
CCa 0x8011ef4 md380_menu_id
CCa 0x8011efa md380_menu_0x200011e4
CCa 0x8011f00 md380_menu_memory
CCa 0x8011f16 md380_menu_id
CCa 0x8011f1e md380_menu_mem_base
CCa 0x8011f5a md380_menu_id
CCa 0x8011f84 md380_menu_id
CCa 0x8011fde md380_menu_0x200011e4
CCa 0x8011fe6 md380_menu_memory
CCa 0x8012002 md380_menu_id
CCa 0x801200a md380_menu_mem_base
CCa 0x8012038 md380_menu_id
CCa 0x801204a md380_menu_0x200011e4
CCa 0x8012052 md380_menu_0x200011e4
CCa 0x8012058 md380_menu_0x200011e4
CCa 0x8012062 md380_menu_id
CCa 0x8012068 md380_menu_0x200011e4
CCa 0x8012070 md380_menu_memory
CCa 0x801208a md380_menu_id
CCa 0x8012092 md380_menu_mem_base
CCa 0x80120d6 md380_menu_id
CCa 0x8012106 md380_menu_id
CCa 0x8012154 md380_menu_0x200011e4
CCa 0x801215c md380_menu_memory
CCa 0x8012178 md380_menu_id
CCa 0x8012180 md380_menu_mem_base
CCa 0x80121ae md380_menu_id
CCa 0x80121c6 md380_menu_0x200011e4
CCa 0x80121ce md380_menu_0x200011e4
CCa 0x80121d4 md380_menu_0x200011e4
CCa 0x80121de md380_menu_id
CCa 0x80121e4 md380_menu_0x200011e4
CCa 0x80121ec md380_menu_memory
CCa 0x801220a md380_menu_id
CCa 0x8012212 md380_menu_mem_base
CCa 0x8012258 md380_menu_id
CCa 0x801228a md380_menu_id
CCa 0x8012298 md380_menu_0x200011e4
CCa 0x80122a0 md380_menu_0x200011e4
CCa 0x80122a6 md380_menu_0x200011e4
CCa 0x80122b0 md380_menu_id
CCa 0x80122b6 md380_menu_0x200011e4
CCa 0x80122be md380_menu_memory
CCa 0x80122da md380_menu_id
CCa 0x80122e2 md380_menu_mem_base
CCa 0x801231c md380_menu_id
CCa 0x801232a md380_menu_0x200011e4
CCa 0x8012332 md380_menu_0x200011e4
CCa 0x8012338 md380_menu_0x200011e4
CCa 0x8012342 md380_menu_id
CCa 0x8012348 md380_menu_0x200011e4
CCa 0x8012350 md380_menu_memory
CCa 0x801236e md380_menu_id
CCa 0x8012376 md380_menu_mem_base
CCa 0x80123b2 md380_menu_id
af+ 0x80123c4 186 F_Menu_Zone
CCa 0x80123c6 md380_menu_0x200011e4
CCa 0x80123ce md380_menu_memory
CCa 0x80123ea Z.o.n.e
CCa 0x80123ec md380_menu_id
CCa 0x80123f4 md380_menu_mem_base
CCa 0x801244c 0x74f8dfb6
CCa 0x8012450 0xd0f8dfb5
CCa 0x8012454 act Zone
CCa 0x8012458 0x00000006
CCa 0x8012458 md380_menu_id
CCa 0x8012464 md380_menu_id
CCa 0x801246c md380_menu_mem_base
af+ 0x8012528 148 F_253
CCa 0x8012528 ... number of zones?
CCa 0x801252a ret value r0, r1
CCa 0x8012538 0x149e0 begin zone list
CCa 0x8012570 0x40 Next Zone entry
af+ 0x80125c0 98 F_4287
CCa 0x8012644 md380_menu_0x200011e4
CCa 0x801264a md380_menu_memory
CCa 0x8012658 md380_menu_id
CCa 0x8012660 md380_menu_mem_base
CCa 0x8012696 md380_menu_id
af+ 0x80126a8 256 Create_Menu_Utilies
CCa 0x80126ac md380_menu_0x200011e4
CCa 0x80126ac menu_depth
CCa 0x80126b2 md380_menu_memory
CCa 0x80126c8 ... U.t.i.l.i.t.i.e.s
CCa 0x80126cc md380_menu_id
CCa 0x80126d2 md380_menu_mem_base
CCa 0x80126f6 0x8016684 .. Create_Menu_Entry_RadioSettings
CCa 0x8012706 ... R.a.d.i.o...S.e.t.t.i.n.g.s
CCa 0x801270a .. 6
CCa 0x801270a md380_menu_id
CCa 0x8012720 0x80152ec .. Create_Menu_Entry_RadioInfo
CCa 0x8012730 ... R.a.d.i.o...I.n.f.o
CCa 0x8012734 .. 6
CCa 0x8012734 md380_menu_id
CCa 0x801274c Is Menu ProgramRadio allowedly
CCa 0x8012766 0x80127d0 .. Create_Menu_Entry_ProgramRadio
CCa 0x8012776 ... P.r.o.g.r.a.m...R.a.d.i.o
CCa 0x801277a md380_menu_id
CCa 0x801278a md380_menu_id
CCa 0x8012792 md380_menu_mem_base
af+ 0x80127d0 880 Create_Menu_Entry_ProgramRadio
CCa 0x80127d4 Program Password
CCa 0x80127f4 md380_menu_0x200011e4
CCa 0x80127fc md380_menu_memory
CCa 0x8012808 md380_menu_0x200011e4
CCa 0x8012812 md380_menu_id
CCa 0x8012824 ... E.d.i.t...C.h.a
CCa 0x801282a md380_menu_id
CCa 0x8012832 md380_menu_mem_base
CCa 0x8012868 ... R.x...F.r.e.q.u.
CCa 0x801286c md380_menu_id
CCa 0x8012896 ... T.x...F.r.e.q.
CCa 0x801289a md380_menu_id
CCa 0x80128c8 ... C.h.a.n.n.e.l..N.a.m.e
CCa 0x80128cc md380_menu_id
CCa 0x80128fa ... T.i.m.e...O.u.t
CCa 0x80128fe md380_menu_id
CCa 0x801293a ... C.T.C./.D.C.S
CCa 0x801293e md380_menu_id
CCa 0x801296e ... C.T.C./.D.C.S
CCa 0x8012972 md380_menu_id
CCa 0x80129ae ... C.o.l.o.r...C.o.
CCa 0x80129b2 md380_menu_id
CCa 0x80129e0 ... R.e.p.e.a.t.e.r...S.l.o.t
CCa 0x80129e4 md380_menu_id
CCa 0x8012a14 ... C.o.l.o.r...C.o.
CCa 0x8012a18 md380_menu_id
CCa 0x8012a46 ... R.e.p.e.a.t.e.r...S.l.o.t
CCa 0x8012a4a md380_menu_id
CCa 0x8012a60 md380_menu_id
CCa 0x8012a6a md380_menu_mem_base
CCa 0x8012a7e md380_menu_0x200011e4
CCa 0x8012a86 md380_menu_0x2001d3f4
CCa 0x8012a8e mn_editbuffer_poi
CCa 0x8012a92 md380_menu_edit_buf
CCa 0x8012a9e mn_editbuffer_poi
CCa 0x8012ab2 md380_menu_0x2001d3ed
CCa 0x8012aba md380_menu_0x2001d3ee
CCa 0x8012ac2 md380_menu_0x2001d3ef
CCa 0x8012aca md380_menu_0x2001d3f1
CCa 0x8012ad2 md380_menu_0x2001d3f0
CCa 0x8012ada md380_menu_0x200011e4
CCa 0x8012ae2 md380_menu_memory
CCa 0x8012afa ... E.n.t.e.r...P.a.s.s.w.o.r.d
CCa 0x8012b00 md380_menu_id
CCa 0x8012b06 md380_menu_mem_base
CCa 0x8012b2c 0x8012b55 .. 0x08012b54 Create_Menu_Entry_ProgramRadio_with_password_set
CCa 0x8012b30 mn_editbuffer_poi
CCa 0x8012b36 md380_menu_id
af+ 0x8012b54 854 Create_Menu_Entry_ProgramRadio_with_password_set
CCa 0x8012b56 md380_menu_edit_buf
CCa 0x8012b5a md380_menu_0x200011e4
CCa 0x8012b62 md380_menu_memory
CCa 0x8012b6e md380_menu_0x200011e4
CCa 0x8012b78 md380_menu_id
CCa 0x8012b9e SystemConfig
CCa 0x8012ba2 ConfigData + 1c (byte) F_5075
CCa 0x8012bae SystemConfig
CCa 0x8012bb2 ConfigData + 1d (byte) F_5075
CCa 0x8012bbe SystemConfig
CCa 0x8012bc2 ConfigData + 1e (byte) F_5075
CCa 0x8012bce SystemConfig
CCa 0x8012bd2 ConfigData + 1f (byte) F_5075
CCa 0x8012bde md380_menu_0x2001d3ee
CCa 0x8012bfe md380_menu_id
CCa 0x8012c04 md380_menu_mem_base
CCa 0x8012c40 md380_menu_id
CCa 0x8012c6e md380_menu_id
CCa 0x8012ca0 md380_menu_id
CCa 0x8012cd2 md380_menu_id
CCa 0x8012d12 md380_menu_id
CCa 0x8012d46 md380_menu_id
CCa 0x8012d86 md380_menu_id
CCa 0x8012db8 md380_menu_id
CCa 0x8012dec md380_menu_id
CCa 0x8012e1e md380_menu_id
CCa 0x8012e32 md380_menu_id
CCa 0x8012e3a md380_menu_mem_base
CCa 0x8012e58 ... W.r.o.n.g
CCa 0x8012e5e md380_menu_id
CCa 0x8012e64 md380_menu_mem_base
CCa 0x8012e9c ... P.a.s.s.w.o.r.d.
CCa 0x8012ea0 md380_menu_id
CCa 0x8012ec4 md380_menu_0x200011e4
CCa 0x8012ecc md380_menu_memory
CCa 0x8012eec md380_menu_id
CCa 0x8012ef4 md380_menu_mem_base
CCa 0x8012f30 md380_menu_id
CCa 0x8012f60 md380_menu_id
CCa 0x8012f94 md380_menu_id
CCa 0x8012fc8 md380_menu_id
CCa 0x8012fda md380_menu_id
CCa 0x8012fe4 md380_menu_mem_base
CCa 0x8013012 md380_menu_0x200011e4
CCa 0x801301a md380_menu_memory
CCa 0x8013038 md380_menu_id
CCa 0x8013040 md380_menu_mem_base
CCa 0x801306a md380_menu_id
CCa 0x8013078 md380_menu_id
CCa 0x8013082 md380_menu_mem_base
CCa 0x8013242 md380_menu_0x200011e4
CCa 0x8013248 md380_menu_memory
CCa 0x8013250 md380_menu_0x200011e4
CCa 0x8013256 md380_menu_memory
CCa 0x8013260 md380_menu_id
CCa 0x8013268 md380_menu_mem_base
CCa 0x801329c md380_menu_id
CCa 0x80132da md380_menu_0x200011e4
CCa 0x80132e0 md380_menu_memory
CCa 0x80132ee md380_menu_id
CCa 0x80132f6 md380_menu_mem_base
CCa 0x801332a md380_menu_id
CCa 0x80133ea md380_menu_0x200011e4
CCa 0x80133f2 md380_menu_memory
CCa 0x8013410 md380_menu_id
CCa 0x8013418 md380_menu_mem_base
CCa 0x8013446 md380_menu_id
CCa 0x8013454 md380_menu_id
CCa 0x801345e md380_menu_mem_base
CCa 0x80135c8 md380_menu_0x2001d3f4
CCa 0x80136ee md380_menu_0x200011e4
CCa 0x80136f6 md380_menu_memory
CCa 0x8013700 md380_menu_0x200011e4
CCa 0x8013708 md380_menu_memory
CCa 0x8013714 md380_menu_id
CCa 0x801371a md380_menu_mem_base
CCa 0x8013754 md380_menu_id
CCa 0x8013786 md380_menu_0x200011e4
CCa 0x801378e md380_menu_memory
CCa 0x801379e md380_menu_id
CCa 0x80137a4 md380_menu_mem_base
CCa 0x80137e0 md380_menu_id
CCa 0x80138ac md380_menu_0x200011e4
CCa 0x80138b4 md380_menu_memory
CCa 0x80138d4 md380_menu_id
CCa 0x80138da md380_menu_mem_base
CCa 0x8013906 md380_menu_id
CCa 0x8013912 md380_menu_id
CCa 0x801391a md380_menu_mem_base
CCa 0x8013ade md380_menu_0x200011e4
CCa 0x8013ae4 md380_menu_memory
CCa 0x8013aec md380_menu_0x200011e4
CCa 0x8013af2 md380_menu_memory
CCa 0x8013afc md380_menu_id
CCa 0x8013b04 md380_menu_mem_base
CCa 0x8013b3e md380_menu_id
CCa 0x8013b7a md380_menu_0x200011e4
CCa 0x8013b80 md380_menu_memory
CCa 0x8013b8e md380_menu_id
CCa 0x8013b96 md380_menu_mem_base
CCa 0x8013bca md380_menu_id
CCa 0x8013c62 md380_menu_0x200011e4
CCa 0x8013c68 md380_menu_memory
CCa 0x8013c84 md380_menu_id
CCa 0x8013c8c md380_menu_mem_base
CCa 0x8013cb6 md380_menu_id
CCa 0x8013cc4 md380_menu_id
CCa 0x8013cce md380_menu_mem_base
CCa 0x8013f4a md380_menu_0x200011e4
CCa 0x8013f52 md380_menu_memory
CCa 0x8013f5c md380_menu_0x200011e4
CCa 0x8013f64 md380_menu_memory
CCa 0x8013f70 md380_menu_id
CCa 0x8013f78 md380_menu_mem_base
CCa 0x8013fb2 md380_menu_id
CCa 0x8013fe6 md380_menu_0x200011e4
CCa 0x8013fee md380_menu_memory
CCa 0x8013ffe md380_menu_id
CCa 0x8014006 md380_menu_mem_base
CCa 0x8014040 md380_menu_id
CCa 0x8014128 md380_menu_0x200011e4
CCa 0x8014130 md380_menu_memory
CCa 0x8014150 md380_menu_id
CCa 0x8014156 md380_menu_mem_base
CCa 0x8014182 md380_menu_id
CCa 0x801418e md380_menu_id
CCa 0x8014196 md380_menu_mem_base
CCa 0x80141e6 md380_menu_0x200011e4
CCa 0x80141ee md380_menu_memory
CCa 0x80141fc md380_menu_id
CCa 0x8014202 md380_menu_mem_base
CCa 0x801423e md380_menu_id
CCa 0x8014246 md380_menu_0x200011e4
CCa 0x801424e md380_menu_memory
CCa 0x8014290 md380_menu_0x200011e4
CCa 0x8014298 md380_menu_memory
CCa 0x80142b8 md380_menu_id
CCa 0x80142be md380_menu_mem_base
CCa 0x80142e8 md380_menu_id
CCa 0x80142f4 md380_menu_id
CCa 0x80142fc md380_menu_mem_base
CCa 0x801435a md380_menu_0x200011e4
CCa 0x8014360 md380_menu_memory
CCa 0x801436e md380_menu_id
CCa 0x8014376 md380_menu_mem_base
CCa 0x80143aa md380_menu_id
CCa 0x80143bc md380_menu_0x200011e4
CCa 0x80143c2 md380_menu_memory
af+ 0x80143f4 104 Create_Menu_Entry_RX_QRG_shown
CCa 0x80143f6 md380_menu_0x200011e4
CCa 0x80143fc md380_menu_memory
CCa 0x8014416 md380_menu_id
CCa 0x801441e md380_menu_mem_base
CCa 0x8014448 md380_menu_id
af+ 0x8014464 214 Create_Menu_Entry_RX_QRG_1
CCa 0x8014466 md380_menu_0x200011e4
CCa 0x801446c md380_menu_0x200011e4
CCa 0x8014470 md380_menu_0x2001d3f4
CCa 0x8014478 mn_editbuffer_poi
CCa 0x801447c md380_menu_edit_buf
CCa 0x8014488 mn_editbuffer_poi
CCa 0x80144a8 md380_menu_edit_buf
CCa 0x80144b0 md380_menu_0x2001d3ed
CCa 0x80144b8 md380_menu_0x2001d3ee
CCa 0x80144c0 md380_menu_0x2001d3ef
CCa 0x80144c8 md380_menu_0x2001d3f1
CCa 0x80144d0 md380_menu_0x2001d3f0
CCa 0x80144d8 md380_menu_0x200011e4
CCa 0x80144de md380_menu_memory
CCa 0x80144f8 md380_menu_id
CCa 0x8014500 md380_menu_mem_base
CCa 0x8014528 mn_editbuffer_poi
CCa 0x801452e md380_menu_id
af+ 0x801453a 126 Create_Menu_Entry_RX_QRG_2
CCa 0x801454a md380_menu_0x2001d3f4
CCa 0x801455c md380_menu_0x2001d3ed
CCa 0x8014564 md380_menu_0x2001d3ee
CCa 0x8014570 md380_menu_0x2001d3ef
CCa 0x801457c md380_menu_0x2001d3f1
CCa 0x8014584 md380_menu_0x2001d3f0
CCa 0x8014592 mn_editbuffer_poi
af+ 0x80145b8 102 Create_Menu_Entry_RX_QRG_3
CCa 0x80145ba md380_menu_0x200011e4
CCa 0x80145c0 md380_menu_memory
CCa 0x80145d8 md380_menu_id
CCa 0x80145e0 md380_menu_mem_base
CCa 0x80145f8 with timeout
CCa 0x8014612 md380_menu_id
af+ 0x8014648 622 Create_Menu_Entry_RX_QRG_4
CCa 0x8014652 mn_editbuffer_poi
CCa 0x8014660 mn_editbuffer_poi
CCa 0x8014672 md380_menu_0x2001d3ef
CCa 0x8014742 mn_editbuffer_poi
CCa 0x801474e mn_editbuffer_poi
CCa 0x801475e mn_editbuffer_poi
CCa 0x801476c mn_editbuffer_poi
CCa 0x801478e mn_editbuffer_poi
CCa 0x801479a mn_editbuffer_poi
CCa 0x80147aa mn_editbuffer_poi
CCa 0x80147b8 mn_editbuffer_poi
CCa 0x80147e4 mn_editbuffer_poi
CCa 0x80147f4 mn_editbuffer_poi
CCa 0x8014806 mn_editbuffer_poi
CCa 0x8014810 mn_editbuffer_poi
CCa 0x8014820 mn_editbuffer_poi
CCa 0x801482a mn_editbuffer_poi
CCa 0x801483a mn_editbuffer_poi
CCa 0x8014844 mn_editbuffer_poi
CCa 0x8014854 mn_editbuffer_poi
CCa 0x801485e mn_editbuffer_poi
CCa 0x80148b8 md380_menu_0x200011e4
CCa 0x80148c0 md380_menu_memory
CCa 0x80148dc md380_menu_id
CCa 0x80148e4 md380_menu_mem_base
CCa 0x8014912 md380_menu_id
CCa 0x801492e md380_menu_0x200011e4
CCa 0x8014936 md380_menu_0x200011e4
CCa 0x801493c md380_menu_0x2001d3f4
CCa 0x8014944 mn_editbuffer_poi
CCa 0x8014948 md380_menu_edit_buf
CCa 0x8014954 mn_editbuffer_poi
CCa 0x8014974 md380_menu_edit_buf
CCa 0x801497c md380_menu_0x2001d3ed
CCa 0x8014984 md380_menu_0x2001d3ee
CCa 0x801498c md380_menu_0x2001d3ef
CCa 0x8014994 md380_menu_0x2001d3f1
CCa 0x801499c md380_menu_0x2001d3f0
CCa 0x80149a4 md380_menu_0x200011e4
CCa 0x80149ac md380_menu_memory
CCa 0x80149ca md380_menu_id
CCa 0x80149d2 md380_menu_mem_base
CCa 0x80149fc mn_editbuffer_poi
CCa 0x8014a02 md380_menu_id
CCa 0x8014a1c md380_menu_0x2001d3f4
CCa 0x8014a2e md380_menu_0x2001d3ed
CCa 0x8014a36 md380_menu_0x2001d3ee
CCa 0x8014a42 md380_menu_0x2001d3ef
CCa 0x8014a4e md380_menu_0x2001d3f1
CCa 0x8014a56 md380_menu_0x2001d3f0
CCa 0x8014a64 mn_editbuffer_poi
CCa 0x8014a9a md380_menu_0x200011e4
CCa 0x8014aa2 md380_menu_memory
CCa 0x8014ac0 md380_menu_id
CCa 0x8014ac6 md380_menu_mem_base
CCa 0x8014b00 md380_menu_id
CCa 0x8014b20 mn_editbuffer_poi
CCa 0x8014b2c mn_editbuffer_poi
CCa 0x8014b3c md380_menu_0x2001d3ef
CCa 0x8014c0a mn_editbuffer_poi
CCa 0x8014c14 mn_editbuffer_poi
CCa 0x8014c22 mn_editbuffer_poi
CCa 0x8014c2e mn_editbuffer_poi
CCa 0x8014c4e mn_editbuffer_poi
CCa 0x8014c58 mn_editbuffer_poi
CCa 0x8014c66 mn_editbuffer_poi
CCa 0x8014c72 mn_editbuffer_poi
CCa 0x8014c9c mn_editbuffer_poi
CCa 0x8014caa mn_editbuffer_poi
CCa 0x8014cba mn_editbuffer_poi
CCa 0x8014cc2 mn_editbuffer_poi
CCa 0x8014cd0 mn_editbuffer_poi
CCa 0x8014cd8 mn_editbuffer_poi
CCa 0x8014ce6 mn_editbuffer_poi
CCa 0x8014cee mn_editbuffer_poi
CCa 0x8014cfc mn_editbuffer_poi
CCa 0x8014d04 mn_editbuffer_poi
CCa 0x8014d6e md380_menu_0x2001d3f4
CCa 0x8014d74 mn_editbuffer_poi
CCa 0x8014d76 md380_menu_edit_buf
CCa 0x8014d80 mn_editbuffer_poi
CCa 0x8014d98 mn_editbuffer_poi
CCa 0x8014db8 md380_menu_0x2001d3ed
CCa 0x8014dbe md380_menu_0x2001d3ee
CCa 0x8014dc2 md380_menu_0x2001d3ef
CCa 0x8014dc6 md380_menu_0x2001d3f1
CCa 0x8014dcc md380_menu_0x2001d3f0
CCa 0x8014dd2 md380_menu_0x200011e4
CCa 0x8014dda md380_menu_memory
CCa 0x8014df8 md380_menu_id
CCa 0x8014e00 md380_menu_mem_base
CCa 0x8014e2a mn_editbuffer_poi
CCa 0x8014e2e md380_menu_id
CCa 0x8014e60 mn_editbuffer_poi
CCa 0x8014e66 mn_editbuffer_poi
CCa 0x8014e72 md380_menu_0x200011e4
CCa 0x8014e7a md380_menu_memory
CCa 0x8014e98 md380_menu_id
CCa 0x8014ea0 md380_menu_mem_base
CCa 0x8014edc md380_menu_id
CCa 0x8014ee8 md380_menu_0x200011e4
CCa 0x8014ef0 md380_menu_memory
CCa 0x8014f0e md380_menu_id
CCa 0x8014f16 md380_menu_mem_base
CCa 0x8014f4e S.a.v.e.d...I.n.b.o.x
CCa 0x8014f50 md380_menu_id
CCa 0x8014f7a mn_editbuffer_poi
CCa 0x8014fd0 mn_editbuffer_poi
CCa 0x8014fe2 mn_editbuffer_poi
CCa 0x8015004 mn_editbuffer_poi
CCa 0x8015014 mn_editbuffer_poi
CCa 0x8015060 md380_menu_0x200011e4
CCa 0x8015066 md380_menu_memory
CCa 0x8015080 md380_menu_id
CCa 0x8015088 md380_menu_mem_base
CCa 0x80150b2 md380_menu_id
CCa 0x80150c0 md380_menu_id
CCa 0x80150ca md380_menu_mem_base
CCa 0x80151ba md380_menu_0x200011e4
CCa 0x80151c0 md380_menu_memory
CCa 0x80151c8 md380_menu_0x200011e4
CCa 0x80151ce md380_menu_memory
CCa 0x80151d8 md380_menu_id
CCa 0x80151e0 md380_menu_mem_base
CCa 0x8015212 md380_menu_id
CCa 0x801524a md380_menu_0x200011e4
CCa 0x8015250 md380_menu_memory
CCa 0x801525e md380_menu_id
CCa 0x8015266 md380_menu_mem_base
CCa 0x8015298 md380_menu_id
CCa 0x80152a2 md380_menu_0x200011e4
CCa 0x80152a8 md380_menu_memory
af+ 0x80152ec 198 Create_Menu_Entry_RadioInfo
CCa 0x80152ee md380_menu_0x200011e4
CCa 0x80152f6 md380_menu_memory
CCa 0x8015312 md380_menu_id
CCa 0x801531a md380_menu_mem_base
CCa 0x8015354 md380_menu_id
CCa 0x8015382 md380_menu_id
CCa 0x8015394 md380_menu_id
CCa 0x801539e md380_menu_mem_base
CCa 0x80153be md380_menu_0x200011e4
CCa 0x80153c6 md380_menu_memory
CCa 0x80153e4 md380_menu_id
CCa 0x80153ec md380_menu_mem_base
CCa 0x8015418 md380_menu_id
af+ 0x8015464 122 F_5076
CCa 0x801546a md380_menu_0x200011e4
CCa 0x8015472 md380_menu_memory
CCa 0x8015482 md380_menu_id
CCa 0x801548a md380_menu_mem_base
CCa 0x80154b6 md380_menu_id
CCa 0x80154c2 SystemConfig
CCa 0x80154c6 ConfigData + 0x4  (long) MyDMRID
CCa 0x80154cc itoa(DMRID, *data) -> len .. max 0a digits
af+ 0x80154de 58 F_4158
af+ 0x8015518 86 F_4159
CCa 0x8015576 md380_menu_0x2001d3f4
CCa 0x801557e mn_editbuffer_poi
CCa 0x8015580 md380_menu_edit_buf
CCa 0x801558c mn_editbuffer_poi
CCa 0x80155aa md380_menu_0x2001d3ed
CCa 0x80155b2 md380_menu_0x2001d3ee
CCa 0x80155ba md380_menu_0x2001d3ef
CCa 0x80155c2 md380_menu_0x2001d3f1
CCa 0x80155ca md380_menu_0x2001d3f0
CCa 0x80155d2 md380_menu_0x200011e4
CCa 0x80155da md380_menu_memory
CCa 0x80155f8 md380_menu_id
CCa 0x8015600 md380_menu_mem_base
CCa 0x8015628 mn_editbuffer_poi
CCa 0x801562c md380_menu_id
CCa 0x8015648 md380_menu_0x2001d3f4
CCa 0x801565a md380_menu_0x2001d3ed
CCa 0x8015662 md380_menu_0x2001d3ee
CCa 0x801566e md380_menu_0x2001d3ef
CCa 0x801567a md380_menu_0x2001d3f1
CCa 0x8015682 md380_menu_0x2001d3f0
CCa 0x8015690 mn_editbuffer_poi
CCa 0x80156ee mn_editbuffer_poi
CCa 0x8015726 mn_editbuffer_poi
CCa 0x8015736 mn_editbuffer_poi
CCa 0x8015746 md380_menu_edit_buf
CCa 0x80157cc md380_menu_0x200011e4
CCa 0x80157d4 md380_menu_memory
CCa 0x80157f2 md380_menu_id
CCa 0x80157fa md380_menu_mem_base
CCa 0x8015836 md380_menu_id
CCa 0x8015842 mn_editbuffer_poi
CCa 0x801585c md380_menu_0x200011e4
CCa 0x8015864 md380_menu_memory
CCa 0x80158a0 md380_menu_id
CCa 0x80158a8 md380_menu_mem_base
CCa 0x80158ec md380_menu_id
CCa 0x801591e md380_menu_id
CCa 0x801592a md380_menu_0x2001d3f4
CCa 0x8015932 mn_editbuffer_poi
CCa 0x8015936 md380_menu_edit_buf
CCa 0x8015942 mn_editbuffer_poi
CCa 0x8015962 md380_menu_0x2001d3ee
CCa 0x801596e md380_menu_0x2001d3ef
CCa 0x8015984 mn_editbuffer_poi
CCa 0x8015998 md380_menu_0x2001d3ed
CCa 0x80159a0 md380_menu_0x2001d3ee
CCa 0x80159a8 md380_menu_0x2001d3ef
CCa 0x80159b0 md380_menu_0x2001d3f1
CCa 0x80159b8 md380_menu_0x2001d3f0
CCa 0x80159c0 md380_menu_0x200011e4
CCa 0x80159c6 md380_menu_memory
CCa 0x80159e0 md380_menu_id
CCa 0x80159e8 md380_menu_mem_base
CCa 0x8015a12 mn_editbuffer_poi
CCa 0x8015a18 md380_menu_id
CCa 0x8015a3a md380_menu_0x2001d3f4
CCa 0x8015a4c mn_editbuffer_poi
CCa 0x8015a68 mn_editbuffer_poi
CCa 0x8015b08 md380_menu_edit_buf
CCa 0x8015b2a md380_menu_0x200011e4
CCa 0x8015b30 md380_menu_memory
CCa 0x8015b48 md380_menu_id
CCa 0x8015b50 md380_menu_mem_base
CCa 0x8015b84 md380_menu_id
CCa 0x8015b90 mn_editbuffer_poi
CCa 0x8015b9c md380_menu_0x200011e4
CCa 0x8015ba2 md380_menu_memory
CCa 0x8015bba md380_menu_id
CCa 0x8015bc2 md380_menu_mem_base
CCa 0x8015bf6 md380_menu_id
CCa 0x8015c02 md380_menu_0x200011e4
CCa 0x8015c08 md380_menu_memory
CCa 0x8015c1e md380_menu_id
CCa 0x8015c26 md380_menu_mem_base
CCa 0x8015c54 md380_menu_id
CCa 0x8015c62 md380_menu_id
CCa 0x8015c6c md380_menu_mem_base
CCa 0x8015c82 md380_menu_0x200011e4
CCa 0x8015c88 md380_menu_memory
CCa 0x8015c9e md380_menu_id
CCa 0x8015ca6 md380_menu_mem_base
CCa 0x8015ce6 md380_menu_id
CCa 0x8015d18 md380_menu_id
CCa 0x8015dc8 mn_editbuffer_poi
CCa 0x8015dd8 mn_editbuffer_poi
CCa 0x8015df6 mn_editbuffer_poi
CCa 0x8015e06 mn_editbuffer_poi
CCa 0x8015e26 mn_editbuffer_poi
CCa 0x8015e34 mn_editbuffer_poi
CCa 0x8015e92 md380_menu_0x200011e4
CCa 0x8015e9a md380_menu_0x2001d3f4
CCa 0x8015ea0 mn_editbuffer_poi
CCa 0x8015ea2 md380_menu_edit_buf
CCa 0x8015eac mn_editbuffer_poi
CCa 0x8015ebe md380_menu_0x2001d3ed
CCa 0x8015ec4 md380_menu_0x2001d3ee
CCa 0x8015eca md380_menu_0x2001d3ef
CCa 0x8015ed0 md380_menu_0x2001d3f1
CCa 0x8015ed6 md380_menu_0x2001d3f0
CCa 0x8015eec md380_menu_0x200011e4
CCa 0x8015ef4 md380_menu_memory
CCa 0x8015f28 md380_menu_id
CCa 0x8015f30 md380_menu_mem_base
CCa 0x8015f5a mn_editbuffer_poi
CCa 0x8015f5e md380_menu_id
CCa 0x8015f94 md380_menu_0x2001d3f4
CCa 0x8015fb0 md380_menu_edit_buf
CCa 0x8015fd4 mn_editbuffer_poi
CCa 0x8015ffe md380_menu_0x200011e4
CCa 0x8016006 md380_menu_memory
CCa 0x801601e mn_editbuffer_poi
CCa 0x801602e mn_editbuffer_poi
CCa 0x801603e mn_editbuffer_poi
CCa 0x8016086 md380_menu_id
CCa 0x801608e md380_menu_mem_base
CCa 0x80160d0 md380_menu_id
CCa 0x8016102 md380_menu_id
CCa 0x8016114 md380_menu_id
CCa 0x801611e md380_menu_mem_base
CCa 0x801612e mn_editbuffer_poi
CCa 0x8016136 md380_menu_id
CCa 0x801613e md380_menu_mem_base
CCa 0x80161a2 md380_menu_id
CCa 0x80161e6 md380_menu_id
CCa 0x801622e md380_menu_id
CCa 0x8016276 md380_menu_id
CCa 0x80162ba md380_menu_id
CCa 0x80162ca md380_menu_id
CCa 0x80162d2 md380_menu_mem_base
CCa 0x80162f0 mn_editbuffer_poi
CCa 0x8016380 md380_menu_0x200011e4
CCa 0x8016386 md380_menu_memory
CCa 0x801639e md380_menu_id
CCa 0x80163a6 md380_menu_mem_base
CCa 0x80163cc mn_editbuffer_poi
CCa 0x80163d0 md380_menu_id
CCa 0x8016414 md380_menu_0x200011e4
CCa 0x801641a md380_menu_memory
CCa 0x8016432 md380_menu_id
CCa 0x801643a md380_menu_mem_base
CCa 0x8016460 mn_editbuffer_poi
CCa 0x8016464 md380_menu_id
CCa 0x80164b4 md380_menu_0x200011e4
CCa 0x80164ba md380_menu_memory
CCa 0x80164d2 md380_menu_id
CCa 0x80164da md380_menu_mem_base
CCa 0x8016500 mn_editbuffer_poi
CCa 0x8016504 md380_menu_id
CCa 0x801654c md380_menu_0x200011e4
CCa 0x8016552 md380_menu_memory
CCa 0x801656a md380_menu_id
CCa 0x8016572 md380_menu_mem_base
CCa 0x8016598 mn_editbuffer_poi
CCa 0x801659c md380_menu_id
CCa 0x80165e8 md380_menu_0x200011e4
CCa 0x80165ee md380_menu_memory
CCa 0x8016606 md380_menu_id
CCa 0x801660e md380_menu_mem_base
CCa 0x8016634 mn_editbuffer_poi
CCa 0x8016638 md380_menu_id
af+ 0x8016684 1312 Create_Menu_Entry_RadioSettings
CCa 0x8016688 md380_menu_0x200011e4
CCa 0x8016690 md380_menu_memory
CCa 0x80166c6 md380_menu_id
CCa 0x80166ce md380_menu_mem_base
CCa 0x8016744 ... T.a.l.k.a.r.o.u.n.d
CCa 0x8016748 md380_menu_id
CCa 0x8016778 ... T.a.l.k.a.r.o.u.n.d
CCa 0x801677c md380_menu_id
CCa 0x80167bc ... T.o.n.e.s./.A.l.e.r.t.s
CCa 0x80167c0 md380_menu_id
CCa 0x801680e ... P.o.w.e.r
CCa 0x8016812 md380_menu_id
CCa 0x8016858 ... B.a.c.k.l.i.g.h.t
CCa 0x801685c md380_menu_id
CCa 0x80168ac ... S.q.u.e.l.c.h.
CCa 0x80168b0 md380_menu_id
CCa 0x80168d6 4 to sp,8
CCa 0x80168da 0 to sp,4
CCa 0x80168de 0x98 to sp
CCa 0x80168e0 0x800f453 .. 0x800f452 F_5143() to r3
CCa 0x80168e4 .. Create_Menu_Entry_Intro_Screen(),r2
CCa 0x80168f6 ... I.n.t.r.o...S.c.r.e.e.n
CCa 0x80168fa md380_menu_id
CCa 0x8016902  (byte)[0x2001d3c2]+5 to r0 .. [0x2001d3c2]+1 = 11
CCa 0x8016940 ... K.e.y.p.a.d...L.o.c.k.
CCa 0x8016944 md380_menu_id
CCa 0x8016978 ... L.a.n.g.u.a.g.e
CCa 0x801697c md380_menu_id
CCa 0x80169c2 ... L.E.D...I.n.d.i.c.a.t.o.r
CCa 0x80169c6 md380_menu_id
CCa 0x8016a12 ... V.O.X.
CCa 0x8016a16 md380_menu_id
CCa 0x8016a40 md380_menu_id
CCa 0x8016a82 ... P.a.s.s.w.d
CCa 0x8016a86 md380_menu_id
CCa 0x8016ab6 ... S.i.t.e...R.o.a.m.i.n.g
CCa 0x8016aba md380_menu_id
CCa 0x8016aee ... R.e.c.o.r.d
CCa 0x8016af2 md380_menu_id
CCa 0x8016b12 0x8017028 Create_Menu_Entry_RadioSettingsDateTime
CCa 0x8016b24 ... C.l.o.c.k
CCa 0x8016b28 md380_menu_id
CCa 0x8016b6e ... M.o.d.e
CCa 0x8016b72 md380_menu_id
CCa 0x8016b84 md380_menu_id
CCa 0x8016b8e md380_menu_mem_base
af+ 0x8016bb8 238 F_5077
CCa 0x8016bbc md380_menu_0x200011e4
CCa 0x8016bc4 md380_menu_memory
CCa 0x8016be4 md380_menu_id
CCa 0x8016bec md380_menu_mem_base
CCa 0x8016c02 SystemConfig
CCa 0x8016c06 ConfigData + 0x17 (byte) F_5077
CCa 0x8016c44 md380_menu_id
CCa 0x8016c74 md380_menu_id
CCa 0x8016c86 md380_menu_id
CCa 0x8016c90 md380_menu_mem_base
af+ 0x8016cb0 132 F_5078
CCa 0x8016cb2 md380_menu_0x200011e4
CCa 0x8016cba md380_menu_memory
CCa 0x8016cd8 md380_menu_id
CCa 0x8016ce0 md380_menu_mem_base
CCa 0x8016d1c md380_menu_id
CCa 0x8016d26 SystemConfig
CCa 0x8016d2c Set ConfigData + 0x17 (Byte) F_5078
af+ 0x8016d3c 130 F_5079
CCa 0x8016d3e md380_menu_0x200011e4
CCa 0x8016d46 md380_menu_memory
CCa 0x8016d64 md380_menu_id
CCa 0x8016d6c md380_menu_mem_base
CCa 0x8016da6 md380_menu_id
CCa 0x8016db0 SystemConfig
CCa 0x8016db6 Set ConfigData + 0x17 (Byte) F_5079
af+ 0x8016dcc 152 Create_Menu_Entry_VOX
CCa 0x8016dd0 md380_menu_0x200011e4
CCa 0x8016dd6 md380_menu_memory
CCa 0x8016df2 md380_menu_id
CCa 0x8016df8 md380_menu_mem_base
CCa 0x8016e26 md380_menu_id
CCa 0x8016e32 md380_menu_id
CCa 0x8016e3a md380_menu_mem_base
CCa 0x8016e5a SystemConfig
CCa 0x8016e60 ConfigData + 0x17 (Byte) F_5080
CCa 0x8016f04 md380_menu_0x200011e4
CCa 0x8016f0a md380_menu_memory
CCa 0x8016f12 md380_menu_0x200011e4
CCa 0x8016f18 md380_menu_memory
CCa 0x8016f22 md380_menu_id
CCa 0x8016f28 md380_menu_mem_base
CCa 0x8016f5c md380_menu_id
af+ 0x8016f90 146 F_5081
CCa 0x8016f92 md380_menu_0x200011e4
CCa 0x8016f98 md380_menu_memory
CCa 0x8016fa6 md380_menu_id
CCa 0x8016fac md380_menu_mem_base
CCa 0x8016fe0 md380_menu_id
CCa 0x8016ff4 md380_menu_0x200011e4
CCa 0x8016ffa md380_menu_memory
CCa 0x8017002 SystemConfig
CCa 0x8017006 Set ConfigData + 0xb (Byte) F_5081
af+ 0x8017028 180 Create_Menu_Entry_RadioSettingsDateTime
CCa 0x801702a md380_menu_0x200011e4
CCa 0x8017030 md380_menu_memory
CCa 0x8017048 md380_menu_id
CCa 0x801704e md380_menu_mem_base
CCa 0x8017070 0x80171e8 Edit_Time_Back
CCa 0x8017074 0x80170f0 Edit_Time
CCa 0x8017082 ... Time
CCa 0x8017086 md380_menu_id
CCa 0x801709a 0x80171f2
CCa 0x801709e 0x8017178
CCa 0x80170ac ... Date
CCa 0x80170b0 md380_menu_id
CCa 0x80170c0 md380_menu_id
CCa 0x80170c8 md380_menu_mem_base
af+ 0x80170f0 106 Edit_Time
CCa 0x80170f2 md380_menu_0x200011e4
CCa 0x80170fa md380_menu_memory
CCa 0x8017110 T.i.m.e
CCa 0x8017116 .. 0x1c
CCa 0x8017116 md380_menu_id
CCa 0x801711c md380_menu_mem_base
CCa 0x8017144 0x8017204 Edit_Time2_Confirm
CCa 0x8017148 0x80173d8 Edit_Time3_Back
CCa 0x801714c ... 0.1.:.1.3.:.4.2
CCa 0x8017150 md380_menu_id
CCa 0x801715c md380_menu_memory
CCa 0x801717a md380_menu_0x200011e4
CCa 0x8017182 md380_menu_memory
CCa 0x80171a0 md380_menu_id
CCa 0x80171a8 md380_menu_mem_base
CCa 0x80171dc md380_menu_id
af+ 0x80171e8 10 Edit_Time_Back
af+ 0x8017204 92 Edit_Time2_Confirm
CCa 0x8017212 0x2001d1a8 RTC_TimeTypeDef* RTC_TimeStruct
CCa 0x8017212 0x2001d1a8 Time
CCa 0x8017218 Call RTC_GetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct)
CCa 0x801721c 0x2001d1a8 Time
CCa 0x8017220 h in r1
CCa 0x8017232 0x2001d1a8 Time
CCa 0x8017236 m in r1
CCa 0x8017248 0x2001d1a8 Time
CCa 0x801724c s in r1
af+ 0x8017268 104 F_4289
CCa 0x8017276 0x2001d1ac Date
CCa 0x801727c Call RTC_GetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct)
CCa 0x801728c 0x2001d1ac Date
CCa 0x80172a2 0x2001d1ac Date
CCa 0x80172b8 0x2001d1ac Date
af+ 0x80172ec 236 F_973
af+ 0x80173d8 134 Edit_Time3_Back
CCa 0x80173da md380_menu_0x200011e4
CCa 0x80173e2 md380_menu_memory
CCa 0x80173f4 md380_menu_0x2001d3ef
CCa 0x80173fc md380_menu_0x2001d3f1
CCa 0x8017404 md380_menu_0x2001d3f0
CCa 0x8017420 md380_menu_id
CCa 0x8017428 md380_menu_mem_base
CCa 0x8017448 Back
CCa 0x801744c 0x80174fc Edit_Time_Confirm
CCa 0x8017450 ... 0.1.:.2.8.:.0.0
CCa 0x8017452 md380_menu_id
af+ 0x8017470 134 Create_Menu_Entry_QuickText
CCa 0x8017472 md380_menu_0x200011e4
CCa 0x801747a md380_menu_memory
CCa 0x801748c md380_menu_0x2001d3ef
CCa 0x8017494 md380_menu_0x2001d3f1
CCa 0x801749c md380_menu_0x2001d3f0
CCa 0x80174b8 md380_menu_id
CCa 0x80174c0 md380_menu_mem_base
CCa 0x80174e8 Q.u.i.c.k...T.e.x.t
CCa 0x80174ea md380_menu_id
af+ 0x80174fc 104 Edit_Time_Confirm
CCa 0x80174fe md380_menu_0x200011e4
CCa 0x8017504 md380_menu_memory
CCa 0x801751c md380_menu_id
CCa 0x8017524 md380_menu_mem_base
CCa 0x8017544 0x80175dc Edit_Time5_SetToRTC 
CCa 0x8017548 Back
CCa 0x8017558 md380_menu_id
CCa 0x8017572 md380_menu_0x200011e4
CCa 0x8017578 md380_menu_memory
CCa 0x8017590 md380_menu_id
CCa 0x8017598 md380_menu_mem_base
CCa 0x80175cc md380_menu_id
af+ 0x80175dc 54 Edit_Time5_SetToRTC
CCa 0x80175e0 0x2001d1a8 Time
CCa 0x8017606 0x2001d1a8 Time
CCa 0x801760c Call RTC_SetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct)
af+ 0x8017612 56 F_4083
CCa 0x801763e 0x2001d1ac Date
CCa 0x8017644 Call RTC_SetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct)
af+ 0x8017650 124 F_4085
af+ 0x80176cc 216 Create_Menu_Entry_TurnOnOff
CCa 0x80176ce md380_menu_0x200011e4
CCa 0x80176d4 md380_menu_memory
CCa 0x80176ec md380_menu_id
CCa 0x80176f4 md380_menu_mem_base
CCa 0x8017744 ... T.u.r.n...O.n
CCa 0x8017748 md380_menu_id
CCa 0x8017770 ... T.u.r.n...O.f.f
CCa 0x8017774 md380_menu_id
CCa 0x8017786 md380_menu_id
CCa 0x8017790 md380_menu_mem_base
CCa 0x80177c0 mn_editbuffer_poi
CCa 0x80177dc md380_menu_0x200011e4
CCa 0x80177e2 md380_menu_memory
CCa 0x80177fa md380_menu_id
CCa 0x8017802 md380_menu_mem_base
CCa 0x801784a md380_menu_id
CCa 0x801789a md380_menu_0x200011e4
CCa 0x80178a2 md380_menu_memory
CCa 0x80178c0 md380_menu_id
CCa 0x80178c8 md380_menu_mem_base
CCa 0x8017912 md380_menu_id
af+ 0x8017938 250 Create_Menu_Entry_Talkaround
CCa 0x801793a md380_menu_0x200011e4
CCa 0x8017942 md380_menu_memory
CCa 0x8017976 md380_menu_id
CCa 0x801797c md380_menu_mem_base
CCa 0x80179d2 md380_menu_id
CCa 0x8017a00 md380_menu_id
CCa 0x8017a10 md380_menu_id
CCa 0x8017a18 md380_menu_mem_base
af+ 0x8017a60 590 F_5082
CCa 0x8017a62 md380_menu_0x200011e4
CCa 0x8017a6a md380_menu_memory
CCa 0x8017a76 md380_menu_0x200011e4
CCa 0x8017a7e md380_menu_memory
CCa 0x8017ad4 md380_menu_id
CCa 0x8017ada md380_menu_mem_base
CCa 0x8017b28 ... T.u.r.n...O.n
CCa 0x8017b2c md380_menu_id
CCa 0x8017b5c md380_menu_id
CCa 0x8017bba SystemConfig
CCa 0x8017bbe Get ConfigData + 0x1 F_5082
CCa 0x8017bc4 SystemConfig
CCa 0x8017bd0 Get ConfigData + 0x1 F_5082
CCa 0x8017bde SystemConfig
CCa 0x8017be2 Get ConfigData + 0x1 F_5082
CCa 0x8017be8 SystemConfig
CCa 0x8017bec Set ConfigData + 0x1 F_5082
CCa 0x8017c02 SystemConfig
CCa 0x8017c06 Get ConfigData + 0x1 F_5082
CCa 0x8017c0c SystemConfig
CCa 0x8017c10 Set ConfigData + 0x1 F_5082
CCa 0x8017c18 SystemConfig
CCa 0x8017c1c Get ConfigData + 0x0 F_5082
CCa 0x8017c22 SystemConfig
CCa 0x8017c2e SystemConfig
CCa 0x8017c32 Get ConfigData + 0x2 F_5082
CCa 0x8017c34 Bit 5
CCa 0x8017c38 SystemConfig
CCa 0x8017c3c Set ConfigData + 0x2 F_5082
CCa 0x8017c44 SystemConfig
CCa 0x8017c48 Get ConfigData + 0x1 F_5082
CCa 0x8017c4a Bit 3
CCa 0x8017c4e SystemConfig
CCa 0x8017c52 Set ConfigData + 0x1 F_5082
CCa 0x8017c5a SystemConfig
CCa 0x8017c5e Get ConfigData + 0x2 F_5082
CCa 0x8017c64 SystemConfig
CCa 0x8017c70 SystemConfig
CCa 0x8017c74 Get ConfigData + 0x0 F_5082
CCa 0x8017c7a SystemConfig
CCa 0x8017c7e Set ConfigData + 0x0 F_5082
af+ 0x8017cd4 674 F_5083
CCa 0x8017cd6 md380_menu_0x200011e4
CCa 0x8017cde md380_menu_memory
CCa 0x8017cea md380_menu_0x200011e4
CCa 0x8017cf2 md380_menu_memory
CCa 0x8017d8e md380_menu_id
CCa 0x8017d96 md380_menu_mem_base
CCa 0x8017dea md380_menu_id
CCa 0x8017e1c md380_menu_id
CCa 0x8017e80 SystemConfig
CCa 0x8017e84 Get ConfigData + 0x1 F_5083
CCa 0x8017e8a SystemConfig
CCa 0x8017e8e Set ConfigData + 0x1 F_5083
CCa 0x8017ea4 SystemConfig
CCa 0x8017ea8 Get ConfigData + 0x1 F_5083
CCa 0x8017eae SystemConfig
CCa 0x8017eb2 Set ConfigData + 0x1 F_5083
CCa 0x8017ec8 SystemConfig
CCa 0x8017ecc Get ConfigData + 0x1 F_5083
CCa 0x8017ed2 SystemConfig
CCa 0x8017ed6 Set ConfigData + 0x1 F_5083
CCa 0x8017ede SystemConfig
CCa 0x8017ee2 Get ConfigData + 0x0 F_5083
CCa 0x8017ee8 SystemConfig
CCa 0x8017eec Set ConfigData + 0x0 F_5083
CCa 0x8017ef4 SystemConfig
CCa 0x8017ef8 Get ConfigData + 0x2 F_5083
CCa 0x8017efa Unset Bit 5
CCa 0x8017efe SystemConfig
CCa 0x8017f02 Set ConfigData + 0x2 F_5083
CCa 0x8017f0a SystemConfig
CCa 0x8017f0e Get ConfigData + 0x1 F_5083
CCa 0x8017f14 SystemConfig
CCa 0x8017f18 Set ConfigData + 0x1 F_5083
CCa 0x8017f22 SystemConfig
CCa 0x8017f26 Get ConfigData + 0x2 F_5083
CCa 0x8017f28 Unset Bit 4
CCa 0x8017f2c SystemConfig
CCa 0x8017f30 Set ConfigData + 0x2 F_5083
CCa 0x8017f38 SystemConfig
CCa 0x8017f3c Get ConfigData + 0x0 F_5083
CCa 0x8017f42 SystemConfig
CCa 0x8017f46 Set ConfigData + 0x0 F_5083
af+ 0x8017f78 394 Create_Menu_Entry_Tones_Alerts
CCa 0x8017f7a md380_menu_0x200011e4
CCa 0x8017f80 md380_menu_memory
CCa 0x8017f9c ... T.o.n.e.s./.A.l.e.r.t.s
CCa 0x8017fac md380_menu_id
CCa 0x8017fb4 md380_menu_mem_base
CCa 0x8017fe8 ... A.l.l...T.o.n.e.s
CCa 0x8017fec md380_menu_id
CCa 0x8018014 ... T.a.l.k...P.e.r.m.i.t
CCa 0x8018018 md380_menu_id
CCa 0x8018044 ... C.a.l.l...R.i.n.g.e.r.s
CCa 0x8018048 md380_menu_id
CCa 0x8018072 ... K.e.y.p.a.d...T.o.n.e.s
CCa 0x8018076 md380_menu_id
CCa 0x80180a0 ... E.s.c.a.l.e.r.t
CCa 0x80180a4 md380_menu_id
CCa 0x80180ce ... V.o.l
CCa 0x80180d2 md380_menu_id
CCa 0x80180e4 md380_menu_id
CCa 0x80180ee md380_menu_mem_base
af+ 0x8018104 220 F_5085
CCa 0x8018106 md380_menu_0x200011e4
CCa 0x801810c md380_menu_memory
CCa 0x8018124 md380_menu_id
CCa 0x801812c md380_menu_mem_base
CCa 0x8018142 SystemConfig
CCa 0x8018146 Get ConfigData + 0x1 F_5085
CCa 0x801817c ... T.u.r.n...O.n
CCa 0x8018180 md380_menu_id
CCa 0x80181a4 ... T.u.r.n...O.f.f
CCa 0x80181a8 md380_menu_id
CCa 0x80181c2 md380_menu_id
CCa 0x80181cc md380_menu_mem_base
af+ 0x80181f0 398 F_5086
CCa 0x80181f2 md380_menu_0x200011e4
CCa 0x80181fa md380_menu_memory
CCa 0x801822c md380_menu_id
CCa 0x8018234 md380_menu_mem_base
CCa 0x8018256 SystemConfig
CCa 0x8018258 Get ConfigData + 0x1 F_5086
CCa 0x8018296 md380_menu_id
CCa 0x80182c2 md380_menu_id
CCa 0x80182de SystemConfig
CCa 0x80182e0 Get ConfigData + 0x1 F_5086
CCa 0x801831a md380_menu_id
CCa 0x8018346 md380_menu_id
CCa 0x8018358 md380_menu_id
CCa 0x8018362 md380_menu_mem_base
af+ 0x8018394 296 F_5087
CCa 0x8018396 md380_menu_0x200011e4
CCa 0x801839e md380_menu_memory
CCa 0x80183bc md380_menu_id
CCa 0x80183c4 md380_menu_mem_base
CCa 0x80183fe md380_menu_id
CCa 0x801842c md380_menu_id
CCa 0x801845e md380_menu_id
CCa 0x8018490 md380_menu_id
CCa 0x80184a0 md380_menu_id
CCa 0x80184a8 md380_menu_mem_base
af+ 0x80184c0 254 F_5088
CCa 0x80184c2 md380_menu_0x200011e4
CCa 0x80184ca md380_menu_memory
CCa 0x80184fe md380_menu_id
CCa 0x8018504 md380_menu_mem_base
CCa 0x801851a SystemConfig
CCa 0x801851c Get ConfigData + 0x2 F_5088
CCa 0x801855e md380_menu_id
CCa 0x801858c md380_menu_id
CCa 0x80185a2 md380_menu_id
CCa 0x80185aa md380_menu_mem_base
af+ 0x80185cc 236 F_5089
CCa 0x80185ce md380_menu_0x200011e4
CCa 0x80185d6 md380_menu_memory
CCa 0x80185f4 md380_menu_id
CCa 0x80185fa md380_menu_mem_base
CCa 0x8018610 SystemConfig
CCa 0x8018614 Get ConfigData + 0x1 F_5089
CCa 0x8018656 md380_menu_id
CCa 0x8018684 md380_menu_id
CCa 0x801869c md380_menu_id
CCa 0x80186a4 md380_menu_mem_base
af+ 0x80186b8 170 F_5090
CCa 0x80186bc md380_menu_0x200011e4
CCa 0x80186c4 md380_menu_memory
CCa 0x80186de ... V.o.l.u.m.e
CCa 0x80186e4 md380_menu_id
CCa 0x80186ea md380_menu_mem_base
CCa 0x8018716 md380_menu_id
CCa 0x8018722 md380_menu_id
CCa 0x801872a md380_menu_mem_base
af+ 0x801876c 152 F_5091
CCa 0x801876e md380_menu_0x200011e4
CCa 0x8018776 md380_menu_memory
CCa 0x801878e ... V.o.l.u.m.e
CCa 0x8018794 md380_menu_id
CCa 0x801879c md380_menu_mem_base
CCa 0x80187d4 ... S.e.l.e.c.t.e.d
CCa 0x80187d8 md380_menu_id
CCa 0x80187e2 md380_menu_0x200011e4
CCa 0x80187ea md380_menu_memory
af+ 0x8018804 248 F_5092
af+ 0x8018900 192 F_5093
CCa 0x801890c md380_menu_0x200011e4
CCa 0x8018912 md380_menu_memory
CCa 0x801892a md380_menu_id
CCa 0x8018932 md380_menu_mem_base
CCa 0x8018960 md380_menu_id
CCa 0x801897c md380_menu_id
CCa 0x8018986 md380_menu_mem_base
af+ 0x80189d8 350 F_5094
af+ 0x8018b3c 134 F_5095
CCa 0x8018b3e md380_menu_0x200011e4
CCa 0x8018b44 md380_menu_memory
CCa 0x8018b52 md380_menu_id
CCa 0x8018b5a md380_menu_mem_base
CCa 0x8018b8e md380_menu_id
CCa 0x8018b98 md380_menu_0x200011e4
CCa 0x8018b9e md380_menu_memory
af+ 0x8018bc2 132 F_5096
CCa 0x8018bc4 md380_menu_0x200011e4
CCa 0x8018bca md380_menu_memory
CCa 0x8018bd2 md380_menu_0x200011e4
CCa 0x8018bd8 md380_menu_memory
CCa 0x8018be2 md380_menu_id
CCa 0x8018bea md380_menu_mem_base
CCa 0x8018c1e md380_menu_id
af+ 0x8018c58 228 F_5097
CCa 0x8018c5a md380_menu_0x200011e4
CCa 0x8018c62 md380_menu_memory
CCa 0x8018c80 md380_menu_id
CCa 0x8018c88 md380_menu_mem_base
CCa 0x8018c9e SystemConfig
CCa 0x8018ca0 Get ConfigData + 0x0 F_5097
CCa 0x8018cda md380_menu_id
CCa 0x8018d06 md380_menu_id
CCa 0x8018d1e md380_menu_id
CCa 0x8018d28 md380_menu_mem_base
af+ 0x8018d3c 176 F_5098
CCa 0x8018d48 md380_menu_0x200011e4
CCa 0x8018d50 md380_menu_memory
CCa 0x8018d70 md380_menu_id
CCa 0x8018d78 md380_menu_mem_base
CCa 0x8018da0 md380_menu_id
CCa 0x8018dac md380_menu_id
CCa 0x8018db4 md380_menu_mem_base
af+ 0x8018e00 190 F_5099
CCa 0x8018e0c md380_menu_0x200011e4
CCa 0x8018e14 md380_menu_memory
CCa 0x8018e34 md380_menu_id
CCa 0x8018e3a md380_menu_mem_base
CCa 0x8018e64 md380_menu_id
CCa 0x8018e7e md380_menu_id
CCa 0x8018e86 md380_menu_mem_base
af+ 0x8018ecc 250 Create_Menu_Entry_Power
CCa 0x8018ece md380_menu_0x200011e4
CCa 0x8018ed6 md380_menu_memory
CCa 0x8018f0a md380_menu_id
CCa 0x8018f10 md380_menu_mem_base
CCa 0x8018f6c md380_menu_id
CCa 0x8018f9a md380_menu_id
CCa 0x8018faa md380_menu_id
CCa 0x8018fb2 md380_menu_mem_base
af+ 0x8018fcc 154 F_5101
CCa 0x8018fce md380_menu_0x200011e4
CCa 0x8018fd6 md380_menu_memory
CCa 0x8018ff4 md380_menu_id
CCa 0x8018ffa md380_menu_mem_base
CCa 0x8019034 md380_menu_id
af+ 0x801906c 154 F_5102
CCa 0x801906e md380_menu_0x200011e4
CCa 0x8019076 md380_menu_memory
CCa 0x8019094 md380_menu_id
CCa 0x801909a md380_menu_mem_base
CCa 0x80190d4 md380_menu_id
af+ 0x8019128 394 Create_Menu_Entry_Backlight
CCa 0x801912c md380_menu_0x200011e4
CCa 0x8019134 md380_menu_memory
CCa 0x801916a md380_menu_id
CCa 0x8019172 md380_menu_mem_base
CCa 0x8019188 SystemConfig
CCa 0x801918c Get ConfigData + 0x15 F_5103
CCa 0x80191ec md380_menu_id
CCa 0x801921c md380_menu_id
CCa 0x8019250 md380_menu_id
CCa 0x8019280 md380_menu_id
CCa 0x8019292 md380_menu_id
CCa 0x801929c md380_menu_mem_base
af+ 0x80192b2 130 F_5104
CCa 0x80192b4 md380_menu_0x200011e4
CCa 0x80192ba md380_menu_memory
CCa 0x80192d2 md380_menu_id
CCa 0x80192da md380_menu_mem_base
CCa 0x8019312 md380_menu_id
CCa 0x801931c GPIO_SetBits(GPIOC, 0x40)
CCa 0x8019326 SystemConfig
CCa 0x801932c Set ConfigData + 0x15 F_5104
af+ 0x801933c 130 F_5105
CCa 0x801933e md380_menu_0x200011e4
CCa 0x8019344 md380_menu_memory
CCa 0x801935c md380_menu_id
CCa 0x8019364 md380_menu_mem_base
CCa 0x801939c md380_menu_id
CCa 0x80193a6 GPIO_SetBits(GPIOC, 0x40)
CCa 0x80193b0 SystemConfig
af+ 0x80193c8 126 F_5106
CCa 0x80193ca md380_menu_0x200011e4
CCa 0x80193d0 md380_menu_memory
CCa 0x80193e8 md380_menu_id
CCa 0x80193f0 md380_menu_mem_base
CCa 0x8019424 md380_menu_id
CCa 0x801942e GPIO_SetBits(GPIOC, 0x40)
CCa 0x8019438 SystemConfig
af+ 0x801944c 232 Create_Menu_Entry_Squelch
CCa 0x801944e md380_menu_0x200011e4
CCa 0x8019454 md380_menu_memory
CCa 0x8019480 md380_menu_id
CCa 0x8019488 md380_menu_mem_base
CCa 0x80194da md380_menu_id
CCa 0x8019504 md380_menu_id
CCa 0x8019516 md380_menu_id
CCa 0x8019520 md380_menu_mem_base
af+ 0x8019534 164 F_5108
CCa 0x8019536 md380_menu_0x200011e4
CCa 0x801953c md380_menu_0x200011e4
CCa 0x8019540 md380_menu_0x200011e4
CCa 0x8019548 md380_menu_id
CCa 0x801954e md380_menu_0x200011e4
CCa 0x8019554 md380_menu_memory
CCa 0x801956c md380_menu_id
CCa 0x8019574 md380_menu_mem_base
CCa 0x80195b4 md380_menu_id
af+ 0x80195f0 182 F_5109
CCa 0x80195f2 md380_menu_0x200011e4
CCa 0x80195fa md380_menu_0x200011e4
CCa 0x8019600 md380_menu_0x200011e4
CCa 0x801960a md380_menu_id
CCa 0x8019610 md380_menu_0x200011e4
CCa 0x8019618 md380_menu_memory
CCa 0x8019636 md380_menu_id
CCa 0x801963e md380_menu_mem_base
CCa 0x8019682 md380_menu_id
af+ 0x80196a6 138 F_5110
CCa 0x80196a8 md380_menu_0x200011e4
CCa 0x80196b0 md380_menu_memory
CCa 0x80196ce md380_menu_id
CCa 0x80196d6 md380_menu_mem_base
CCa 0x801970e md380_menu_id
CCa 0x8019718 GPIO_SetBits(GPIOC, 0x40)
CCa 0x8019722 SystemConfig
CCa 0x8019728 Set ConfigData + 0x15 F_5110
af+ 0x8019734 250 Create_Menu_Entry_Intro_Screen
CCa 0x8019736 md380_menu_0x200011e4
CCa 0x801973e md380_menu_memory
CCa 0x801975e ... I.n.t.r.o...S.c.r.e.e.n
CCa 0x801976a ... I.n.t.r.o...S.c.r.e.e.n
CCa 0x8019770 md380_menu_id
CCa 0x8019778 md380_menu_mem_base
CCa 0x801978e SystemConfig
CCa 0x8019792 Get ConfigData + 0x2 F_5111
CCa 0x801979a Set old Valvue selected
CCa 0x80197b0 1 or 0 ... to SP,8
CCa 0x80197b4 0 to SP,4 
CCa 0x80197b8 0x8b to SP
CCa 0x80197ba 0x800f453 .. 0x800f452 F_5143()  to r3 - Back
CCa 0x80197be 0x8019834 .. SetConfig0x02Bit4andmore(),r2
CCa 0x80197d0 [((0x2001d1a0 lsl 2) +[0x20000000])+0x3b0] to r1 .. [0x200003b0] = 0x080fa348 .. [0x080fa348] = "P.i.c.t.u.r.e"
CCa 0x80197d4 md380_menu_id
CCa 0x80197d6 (byte)[0x2001d3c2] to r0 .. [0x2001d3c2] = 6 ???
CCa 0x80197dc 1 to SP,8
CCa 0x80197e2 0 to SP,4
CCa 0x80197e6 0x8b to SP
CCa 0x80197e8 0x800f453 .. 0x800f452 F_5143() to r3 - Back
CCa 0x80197ec 0x80198c0 .. UnsetConfig0x02Bit4andmore(),r2
CCa 0x80197fe [((0x2001d1a0 lsl 2) +[0x20000000])+0x3b4] to r1 .. [0x200003b4] = 0x080d1f48 .. [0x080d1f48] ="C.h.a.r...S.t.r.i.n.g" 
CCa 0x8019802 md380_menu_id
CCa 0x8019808 (byte)[0x2001d3c2]+1 to r0 .. [0x2001d3c2]+1 = 7 ???
CCa 0x8019812 md380_menu_id
CCa 0x801981a md380_menu_mem_base
af+ 0x8019834 132 SetConfig0x02Bit4andmore_Introscreen
CCa 0x8019836 md380_menu_0x200011e4
CCa 0x801983e md380_menu_memory
CCa 0x8019848 md380_menu_id
CCa 0x801984e md380_menu_mem_base
CCa 0x8019884 Back
CCa 0x8019888 Back
CCa 0x801989e md380_menu_id
CCa 0x80198a6 SystemConfig
CCa 0x80198a8 Get ConfigData + 0x2 F_5112
CCa 0x80198aa SetBit 4
CCa 0x80198ae SystemConfig
CCa 0x80198b0 Set ConfigData + 0x2 F_5112
af+ 0x80198c0 130 UnsetConfig0x02Bit4andmore_Introscreen
CCa 0x80198c2 md380_menu_0x200011e4
CCa 0x80198ca md380_menu_memory
CCa 0x80198d4 md380_menu_id
CCa 0x80198da md380_menu_mem_base
CCa 0x80198dc mla 0x20019e68 , 0x14, 6 ,0x20019df0  .. 0x14*6+0x20019df0  ... 0x20019e68 ... 0x0809bb8c                            
CCa 0x80198ec add ,0x20000000 ,0 , lsl 2  ... 080d1b58 ... thinking :(
CCa 0x8019902 para 6 from F_249_Create_MenuEntry
CCa 0x8019906 para 5 from F_249_Create_MenuEntry
CCa 0x801990a para 4 from F_249_Create_MenuEntry
CCa 0x801990e Back
CCa 0x8019912 Back
CCa 0x8019924 ... T.u.r.n...O.n
CCa 0x8019928 md380_menu_id
CCa 0x8019930 SystemConfig
CCa 0x8019932 Get ConfigData + 0x2 F_5113
CCa 0x8019934 Unset Bit 4 
CCa 0x8019938 SystemConfig
CCa 0x801993a Set ConfigData + 0x2 F_5113
af+ 0x8019948 380 Create_Menu_Entry_KeypadLock
CCa 0x801994c md380_menu_0x200011e4
CCa 0x8019954 md380_menu_memory
CCa 0x801998a md380_menu_id
CCa 0x8019990 md380_menu_mem_base
CCa 0x80199a6 SystemConfig
CCa 0x80199a8 Get ConfigData + 0x16 F_5114
CCa 0x8019a04 md380_menu_id
CCa 0x8019a32 md380_menu_id
CCa 0x8019a64 md380_menu_id
CCa 0x8019a96 md380_menu_id
CCa 0x8019aa6 md380_menu_id
CCa 0x8019aae md380_menu_mem_base
af+ 0x8019ac4 126 F_5115
CCa 0x8019ac6 md380_menu_0x200011e4
CCa 0x8019ace md380_menu_memory
CCa 0x8019aec md380_menu_id
CCa 0x8019af2 md380_menu_mem_base
CCa 0x8019b2c md380_menu_id
CCa 0x8019b34 SystemConfig
CCa 0x8019b3a Set ConfigData + 0x16 F_5115
af+ 0x8019b54 130 F_5116
CCa 0x8019b56 md380_menu_0x200011e4
CCa 0x8019b5e md380_menu_memory
CCa 0x8019b7c md380_menu_id
CCa 0x8019b84 md380_menu_mem_base
CCa 0x8019bbe md380_menu_id
CCa 0x8019bc8 SystemConfig
CCa 0x8019bce Set ConfigData + 0x16 F_5116
af+ 0x8019be0 126 F_5117
CCa 0x8019be2 md380_menu_0x200011e4
CCa 0x8019be8 md380_menu_memory
CCa 0x8019c04 md380_menu_id
CCa 0x8019c0c md380_menu_mem_base
CCa 0x8019c46 md380_menu_id
CCa 0x8019c50 SystemConfig
CCa 0x8019c56 Set ConfigData + 0x16 F_5117
af+ 0x8019c68 120 F_5118
CCa 0x8019c6a md380_menu_0x200011e4
CCa 0x8019c70 md380_menu_memory
CCa 0x8019c88 md380_menu_id
CCa 0x8019c90 md380_menu_mem_base
CCa 0x8019cc8 md380_menu_id
CCa 0x8019cd2 SystemConfig
CCa 0x8019cd8 Set ConfigData + 0x16 F_5118
af+ 0x8019ce0 272 F_4160
CCa 0x8019ce2 md380_menu_0x200011e4
CCa 0x8019cea md380_menu_0x200011e4
CCa 0x8019cf0 md380_menu_0x200011e4
CCa 0x8019cf6 md380_menu_memory
CCa 0x8019d0e md380_menu_id
CCa 0x8019d16 md380_menu_mem_base
CCa 0x8019d4e md380_menu_id
CCa 0x8019d60 SystemConfig
CCa 0x8019d64 ConfigData + 0x16 (byte) F_4160
CCa 0x8019d74 md380_menu_0x200011e4
CCa 0x8019d7a md380_menu_0x200011e4
CCa 0x8019d7e md380_menu_0x200011e4
CCa 0x8019d86 md380_menu_id
CCa 0x8019d8c md380_menu_0x200011e4
CCa 0x8019d92 md380_menu_memory
af+ 0x8019dfc 208 Create_Menu_Entry_Language
CCa 0x8019dfe md380_menu_0x200011e4
CCa 0x8019e04 md380_menu_memory
CCa 0x8019e1c md380_menu_id
CCa 0x8019e24 md380_menu_mem_base
CCa 0x8019e72 md380_menu_id
CCa 0x8019e9c md380_menu_id
CCa 0x8019eae md380_menu_id
CCa 0x8019eb8 md380_menu_mem_base
af+ 0x8019ecc 120 F_5120
CCa 0x8019ece md380_menu_0x200011e4
CCa 0x8019ed4 md380_menu_memory
CCa 0x8019eee md380_menu_id
CCa 0x8019ef6 md380_menu_mem_base
CCa 0x8019f24 md380_menu_id
af+ 0x8019f4c 122 F_5121
CCa 0x8019f4e md380_menu_0x200011e4
CCa 0x8019f56 md380_menu_memory
CCa 0x8019f72 md380_menu_id
CCa 0x8019f7a md380_menu_mem_base
CCa 0x8019fa8 md380_menu_id
af+ 0x8019fd0 256 Create_Menu_Entry_LEDIndicator
CCa 0x8019fd2 md380_menu_0x200011e4
CCa 0x8019fda md380_menu_memory
CCa 0x801a00c md380_menu_id
CCa 0x801a014 md380_menu_mem_base
CCa 0x801a02a SystemConfig
CCa 0x801a02e Get ConfigData + 0x0 F_5122
CCa 0x801a06a md380_menu_id
CCa 0x801a098 md380_menu_id
CCa 0x801a0b2 md380_menu_id
CCa 0x801a0bc md380_menu_mem_base
af+ 0x801a0e0 304 Create_Menu_Entry_wtf_5235235
CCa 0x801a0e2 md380_menu_0x200011e4
CCa 0x801a0ea md380_menu_memory
CCa 0x801a108 md380_menu_id
CCa 0x801a110 md380_menu_mem_base
CCa 0x801a154 md380_menu_id
CCa 0x801a182 md380_menu_id
CCa 0x801a1b4 md380_menu_id
CCa 0x801a1e6 md380_menu_id
CCa 0x801a1f6 md380_menu_id
CCa 0x801a1fe md380_menu_mem_base
af+ 0x801a210 198 F_5124
CCa 0x801a216 md380_menu_0x200011e4
CCa 0x801a21e md380_menu_0x2001d3f4
CCa 0x801a226 mn_editbuffer_poi
CCa 0x801a22a md380_menu_edit_buf
CCa 0x801a236 mn_editbuffer_poi
CCa 0x801a24a md380_menu_0x2001d3ed
CCa 0x801a252 md380_menu_0x2001d3ee
CCa 0x801a25a md380_menu_0x2001d3ef
CCa 0x801a262 md380_menu_0x2001d3f1
CCa 0x801a26a md380_menu_0x2001d3f0
CCa 0x801a272 md380_menu_0x200011e4
CCa 0x801a27a md380_menu_memory
CCa 0x801a298 md380_menu_id
CCa 0x801a29e md380_menu_mem_base
CCa 0x801a2c6 mn_editbuffer_poi
CCa 0x801a2cc md380_menu_id
af+ 0x801a2d6 26 F_5125
CCa 0x801a2e6 md380_menu_0x2001d3f4
af+ 0x801a2f4 114 F_5126
CCa 0x801a2f6 md380_menu_0x200011e4
CCa 0x801a2fe md380_menu_memory
CCa 0x801a31c md380_menu_id
CCa 0x801a322 md380_menu_mem_base
CCa 0x801a35c md380_menu_id
af+ 0x801a374 114 F_5127
CCa 0x801a376 md380_menu_0x200011e4
CCa 0x801a37e md380_menu_memory
CCa 0x801a39c md380_menu_id
CCa 0x801a3a2 md380_menu_mem_base
CCa 0x801a3dc md380_menu_id
af+ 0x801a3ec 114 F_5128
CCa 0x801a3ee md380_menu_0x200011e4
CCa 0x801a3f6 md380_menu_memory
CCa 0x801a414 md380_menu_id
CCa 0x801a41a md380_menu_mem_base
CCa 0x801a454 md380_menu_id
af+ 0x801a45e 146 F_5129
CCa 0x801a460 md380_menu_0x200011e4
CCa 0x801a468 md380_menu_memory
CCa 0x801a486 md380_menu_id
CCa 0x801a48c md380_menu_mem_base
CCa 0x801a4c8 md380_menu_id
CCa 0x801a4d4 md380_menu_id
CCa 0x801a4dc md380_menu_mem_base
af+ 0x801a4fc 520 Ask/Test_PowerOnPassword
CCa 0x801a4fe md380_menu_edit_buf
CCa 0x801a502 md380_menu_0x200011e4
CCa 0x801a508 md380_menu_memory
CCa 0x801a512 md380_menu_0x200011e4
CCa 0x801a51a md380_menu_id
CCa 0x801a544 SystemConfig
CCa 0x801a546 Get ConfigData + 0x18 F_5130
CCa 0x801a54c SystemConfig
CCa 0x801a54e Get ConfigData + 0x19 F_5130
CCa 0x801a554 SystemConfig
CCa 0x801a556 Get ConfigData + 0x1a F_5130
CCa 0x801a55c SystemConfig
CCa 0x801a55e Get ConfigData + 0x1b F_5130
CCa 0x801a564 SystemConfig
CCa 0x801a566 Get ConfigData + 0x18 F_5130
CCa 0x801a574 SystemConfig
CCa 0x801a576 Get ConfigData + 0x19 F_5130
CCa 0x801a584 SystemConfig
CCa 0x801a586 Get ConfigData + 0x1a F_5130
CCa 0x801a594 SystemConfig
CCa 0x801a596 Get ConfigData + 0x1b F_5130
CCa 0x801a5a4 md380_menu_0x2001d3ee
CCa 0x801a5d2 md380_menu_id
CCa 0x801a5da md380_menu_mem_base
CCa 0x801a5f0 SystemConfig
CCa 0x801a5f2 Get ConfigData + 0x1 F_5130
CCa 0x801a61e md380_menu_id
CCa 0x801a64c md380_menu_id
CCa 0x801a678 md380_menu_id
CCa 0x801a68e md380_menu_id
CCa 0x801a698 md380_menu_mem_base
CCa 0x801a6b8 md380_menu_id
CCa 0x801a6c0 md380_menu_mem_base
CCa 0x801a6f8 md380_menu_id
af+ 0x801a714 174 F_5131
CCa 0x801a718 md380_menu_0x200011e4
CCa 0x801a71e md380_menu_0x200011e4
CCa 0x801a722 md380_menu_0x200011e4
CCa 0x801a728 md380_menu_memory
CCa 0x801a732 md380_menu_0x200011e4
CCa 0x801a73a md380_menu_id
CCa 0x801a764 md380_menu_id
CCa 0x801a76c md380_menu_mem_base
CCa 0x801a782 SystemConfig
CCa 0x801a786 Get ConfigData + 0x1 F_5131
CCa 0x801a78c SystemConfig
CCa 0x801a790 Set ConfigData + 0x1 F_5131
CCa 0x801a7b2 md380_menu_id
af+ 0x801a7cc 156 F_5132
CCa 0x801a7ce md380_menu_0x200011e4
CCa 0x801a7d4 md380_menu_0x200011e4
CCa 0x801a7d8 md380_menu_0x200011e4
CCa 0x801a7de md380_menu_memory
CCa 0x801a80a md380_menu_id
CCa 0x801a812 md380_menu_mem_base
CCa 0x801a828 SystemConfig
CCa 0x801a82c Get ConfigData + 0x1 F_5132
CCa 0x801a832 SystemConfig
CCa 0x801a836 Set ConfigData + 0x1 F_5132
CCa 0x801a858 md380_menu_id
af+ 0x801a870 178 F_5133
CCa 0x801a874 md380_menu_0x200011e4
CCa 0x801a87c md380_menu_0x2001d3f4
CCa 0x801a882 mn_editbuffer_poi
CCa 0x801a884 md380_menu_edit_buf
CCa 0x801a88e mn_editbuffer_poi
CCa 0x801a8a0 md380_menu_0x2001d3ed
CCa 0x801a8a6 md380_menu_0x2001d3ee
CCa 0x801a8ac md380_menu_0x2001d3ef
CCa 0x801a8b2 md380_menu_0x2001d3f1
CCa 0x801a8b8 md380_menu_0x2001d3f0
CCa 0x801a8be md380_menu_0x200011e4
CCa 0x801a8c6 md380_menu_memory
CCa 0x801a8e0 md380_menu_id
CCa 0x801a8e8 md380_menu_mem_base
CCa 0x801a912 mn_editbuffer_poi
CCa 0x801a916 md380_menu_id
CCa 0x801a924 md380_menu_0x200011e4
af+ 0x801a938 322 F_5134
CCa 0x801a93c md380_menu_0x200011e4
CCa 0x801a944 md380_menu_0x200011e4
CCa 0x801a94a md380_menu_0x200011e4
CCa 0x801a952 md380_menu_memory
CCa 0x801a95e md380_menu_0x200011e4
CCa 0x801a968 md380_menu_id
CCa 0x801a99c md380_menu_id
CCa 0x801a9a4 md380_menu_mem_base
CCa 0x801a9ba SystemConfig
CCa 0x801a9be Get ConfigData + 0x1 F_5134
CCa 0x801a9ec md380_menu_id
CCa 0x801aa1c md380_menu_id
CCa 0x801aa4a md380_menu_id
CCa 0x801aa5c md380_menu_id
CCa 0x801aa66 md380_menu_mem_base
CCa 0x801aab8 md380_menu_0x2001d3f4
af+ 0x801aabc 408 F_5135
CCa 0x801aabe md380_menu_edit_buf
CCa 0x801aac2 md380_menu_0x200011e4
CCa 0x801aaca md380_menu_memory
CCa 0x801aad6 md380_menu_0x2001d3ee
CCa 0x801ab04 md380_menu_0x200011e4
CCa 0x801ab0c md380_menu_0x2001d3f4
CCa 0x801ab14 mn_editbuffer_poi
CCa 0x801ab18 md380_menu_edit_buf
CCa 0x801ab24 mn_editbuffer_poi
CCa 0x801ab38 md380_menu_0x2001d3ed
CCa 0x801ab40 md380_menu_0x2001d3ee
CCa 0x801ab48 md380_menu_0x2001d3ef
CCa 0x801ab50 md380_menu_0x2001d3f1
CCa 0x801ab58 md380_menu_0x2001d3f0
CCa 0x801ab74 md380_menu_id
CCa 0x801ab7c md380_menu_mem_base
CCa 0x801aba6 mn_editbuffer_poi
CCa 0x801abac md380_menu_id
CCa 0x801abb8 md380_menu_id
CCa 0x801abc0 md380_menu_mem_base
CCa 0x801ac16 md380_menu_id
CCa 0x801ac48 md380_menu_id
af+ 0x801ac60 452 F_5144
CCa 0x801ac62 md380_menu_edit_buf
CCa 0x801ac66 md380_menu_0x200011e4
CCa 0x801ac6e md380_menu_memory
CCa 0x801ac7a md380_menu_0x2001d3ee
CCa 0x801acb8 md380_menu_id
CCa 0x801acc0 ... E.n.t.r.y
CCa 0x801acc0 md380_menu_mem_base
CCa 0x801ad0a SystemConfig
CCa 0x801ad46 ... C.h.a.n.g.e.d
CCa 0x801ad4a md380_menu_id
CCa 0x801ad7c md380_menu_id
CCa 0x801ad88 md380_menu_id
CCa 0x801ad90 md380_menu_mem_base
CCa 0x801ade6 md380_menu_id
CCa 0x801ae18 md380_menu_id
af+ 0x801ae24 282 F_5136
CCa 0x801ae26 md380_menu_0x200011e4
CCa 0x801ae2e md380_menu_id
CCa 0x801ae3a md380_menu_0x200011e4
CCa 0x801ae42 md380_menu_0x2001d3f4
CCa 0x801ae4a mn_editbuffer_poi
CCa 0x801ae4e md380_menu_edit_buf
CCa 0x801ae5a mn_editbuffer_poi
CCa 0x801ae6e md380_menu_0x2001d3ed
CCa 0x801ae76 md380_menu_0x2001d3ee
CCa 0x801ae7e md380_menu_0x2001d3ef
CCa 0x801ae86 md380_menu_0x2001d3f1
CCa 0x801ae8e md380_menu_0x2001d3f0
CCa 0x801ae96 md380_menu_0x200011e4
CCa 0x801ae9e md380_menu_memory
CCa 0x801aecc md380_menu_mem_base
CCa 0x801af0a mn_editbuffer_poi
CCa 0x801af10 md380_menu_id
CCa 0x801af18 md380_menu_0x200011e4
CCa 0x801af22 md380_menu_id
CCa 0x801af34 md380_menu_0x200011e4
af+ 0x801af58 298 F_5137
CCa 0x801af5c md380_menu_0x200011e4
CCa 0x801af62 md380_menu_memory
CCa 0x801af6c md380_menu_0x200011e4
CCa 0x801af74 md380_menu_id
CCa 0x801afbe SystemConfig
CCa 0x801afc0 Get ConfigData + 0x18 F_5137
CCa 0x801afcc SystemConfig
CCa 0x801afce Get ConfigData + 0x19 F_5137
CCa 0x801afda SystemConfig
CCa 0x801afdc Get ConfigData + 0x1a F_5137
CCa 0x801afe8 SystemConfig
CCa 0x801afea Get ConfigData + 0x1b F_5137
CCa 0x801aff6 md380_menu_0x2001d3ee
CCa 0x801b034 md380_menu_id
CCa 0x801b03c md380_menu_mem_base
CCa 0x801b074 md380_menu_id
af+ 0x801b082 28 F_5138
af+ 0x801b09e 188 F_4146
CCa 0x801b0a0 md380_menu_0x200011e4
CCa 0x801b0a8 md380_menu_id
CCa 0x801b0ae md380_menu_0x200011e4
CCa 0x801b0b6 md380_menu_memory
CCa 0x801b0de md380_menu_mem_base
CCa 0x801b12c md380_menu_id
CCa 0x801b134 md380_menu_0x200011e4
CCa 0x801b13e md380_menu_id
CCa 0x801b150 md380_menu_0x200011e4
af+ 0x801b164 98 F_5139
CCa 0x801b166 md380_menu_0x200011e4
CCa 0x801b16c md380_menu_memory
CCa 0x801b184 md380_menu_id
CCa 0x801b18a md380_menu_mem_base
CCa 0x801b1bc md380_menu_id
af+ 0x801b1c6 156 F_4162
CCa 0x801b1c8 md380_menu_0x200011e4
CCa 0x801b1ce md380_menu_id
CCa 0x801b1d4 md380_menu_0x200011e4
CCa 0x801b1da md380_menu_memory
CCa 0x801b1fc md380_menu_mem_base
CCa 0x801b23c md380_menu_id
CCa 0x801b244 md380_menu_0x200011e4
CCa 0x801b24c md380_menu_id
CCa 0x801b25a md380_menu_0x200011e4
af+ 0x801b270 136 F_5140
CCa 0x801b272 md380_menu_0x200011e4
CCa 0x801b278 md380_menu_memory
CCa 0x801b290 md380_menu_id
CCa 0x801b296 md380_menu_mem_base
CCa 0x801b2c6 md380_menu_id
CCa 0x801b2ea md380_menu_id
af+ 0x801b300 106 F_5141
CCa 0x801b302 md380_menu_0x200011e4
CCa 0x801b308 md380_menu_0x200011e4
CCa 0x801b30c md380_menu_0x200011e4
CCa 0x801b312 md380_menu_memory
CCa 0x801b328 md380_menu_id
CCa 0x801b32e md380_menu_mem_base
CCa 0x801b360 md380_menu_id
af+ 0x801b388 54 F_5142
CCa 0x801b3d4 md380_menu_id
CCa 0x801b3e8 md380_menu_0x2001d3ee
CCa 0x801b41c mn_editbuffer_poi
af+ 0x801b440 30 F_974
af+ 0x801b45e 24 F_975
af+ 0x801b476 118 F_1091
af+ 0x801b4ec 10 F_976
af+ 0x801b4f6 34 F_1057
af+ 0x801b518 68 F_977
af+ 0x801b55c 68 F_788
af+ 0x801b5a0 80 F_789
af+ 0x801b5f0 142 F_1092
af+ 0x801b67e 88 F_1058
af+ 0x801b6d6 44 F_978
af+ 0x801b702 36 F_979
af+ 0x801b726 20 F_790
af+ 0x801b73a 46 F_980
af+ 0x801b768 10 F_1093
af+ 0x801b772 54 F_981
af+ 0x801b7a8 54 F_1094
af+ 0x801b7de 36 F_1125
af+ 0x801b802 178 F_982
af+ 0x801b8b4 152 F_983
af+ 0x801b94c 132 F_984
af+ 0x801b9d0 38 F_1059
af+ 0x801b9f6 20 F_985
af+ 0x801ba0a 24 F_1151
af+ 0x801ba22 18 F_791
af+ 0x801ba34 120 F_1060
af+ 0x801baac 12 F_986
af+ 0x801bab8 44 F_1061
af+ 0x801bae4 332 F_1062
af+ 0x801bc30 28 F_987
af+ 0x801bc4c 208 F_792
af+ 0x801bd1c 68 F_988
af+ 0x801bd60 10 F_4163
af+ 0x801bd6a 418 F_793
af+ 0x801bf0c 92 F_254
af+ 0x801bf68 288 F_255
af+ 0x801c088 16 F_4426
af+ 0x801c098 130 F_4350
af+ 0x801c11a 124 F_4427
af+ 0x801c1ac 106 F_4351
af+ 0x801c216 84 F_4352
af+ 0x801c26a 56 F_4290
af+ 0x801c2f4 156 F_4222
af+ 0x801c390 92 F_4164
af+ 0x801c3ec 78 F_4150
af+ 0x801c43a 20 F_4149
af+ 0x801c44e 70 F_4151
af+ 0x801c494 8 F_989
af+ 0x801c49c 14 F_4152
af+ 0x801c4b0 94 F_4086
af+ 0x801c50e 120 F_794
af+ 0x801c5e0 8 gfx_set_bg_color
af+ 0x801c5e8 8 gfx_set_fg_color
af+ 0x801c5f0 214 F_4165
af+ 0x801c6c6 330 F_4166
af+ 0x801c810 328 F_4167
af+ 0x801c958 164 F_4132
af+ 0x801c9fc 70 F_4133
af+ 0x801ca42 36 F_1063
CCa 0x801ca4a GPIO D3 K3 .. config as input
CCa 0x801ca5c GPIO D3 K3 .. config as output
af+ 0x801ca66 12 F_1095
af+ 0x801ca72 32 F_1064
af+ 0x801ca92 52 F_990
af+ 0x801cac6 52 F_4134
af+ 0x801cb04 30 F_797_gfx_blockfill
af+ 0x801cb22 168 F_4135
af+ 0x801cbca 2 F_991
af+ 0x801cbcc 10 F_992
af+ 0x801cbd8 40 F_4168
af+ 0x801cc00 722 F_4136
CCa 0x801cc46 md380_menu_0x2001d3ef
CCa 0x801ce56 md380_menu_0x2001d3ef
CCa 0x801ce64 md380_menu_0x2001d3ef
af+ 0x801ced2 40 F_993
af+ 0x801cf1c 18 gfx_drawtext2
af+ 0x801cf2e 18 F_799
af+ 0x801cf40 16 F_4169
af+ 0x801cf50 30 F_4291
af+ 0x801cf70 2172 F_4223
CCa 0x801cf7c md380_menu_0x2001d3f1
CCa 0x801cf86 md380_menu_0x2001d3f0
CCa 0x801cfb0 md380_menu_0x2001d3f1
CCa 0x801cfb8 md380_menu_0x2001d3f1
CCa 0x801cfc2 md380_menu_0x2001d3f0
CCa 0x801cfd0 md380_menu_0x2001d3f0
CCa 0x801cfdc md380_menu_0x2001d3f1
CCa 0x801d04e md380_menu_0x2001d3ef
CCa 0x801d058 md380_menu_0x2001d3f1
CCa 0x801d068 md380_menu_0x2001d3ef
CCa 0x801d074 mn_editbuffer_poi
CCa 0x801d082 mn_editbuffer_poi
CCa 0x801d090 md380_menu_0x2001d3ee
CCa 0x801d09c md380_menu_0x2001d3ee
CCa 0x801d0a2 mn_editbuffer_poi
CCa 0x801d0b2 md380_menu_0x2001d3ef
CCa 0x801d0ba md380_menu_0x2001d3ef
CCa 0x801d0c0 md380_menu_0x2001d3ee
CCa 0x801d0c8 md380_menu_0x2001d3ee
CCa 0x801d0d2 md380_menu_0x2001d3f1
CCa 0x801d0f2 md380_menu_0x2001d3ef
CCa 0x801d0fc md380_menu_0x2001d3ef
CCa 0x801d104 md380_menu_0x2001d3ef
CCa 0x801d10a md380_menu_0x2001d3f1
CCa 0x801d11c md380_menu_0x2001d3f1
CCa 0x801d13c md380_menu_0x2001d3ef
CCa 0x801d142 md380_menu_0x2001d3ee
CCa 0x801d14c md380_menu_0x2001d3ef
CCa 0x801d154 md380_menu_0x2001d3ef
CCa 0x801d15a md380_menu_0x2001d3f1
CCa 0x801d1b6 md380_menu_0x2001d3f0
CCa 0x801d1c4 md380_menu_0x2001d3f0
CCa 0x801d1d0 md380_menu_0x2001d3ef
CCa 0x801d1dc md380_menu_0x2001d3f1
CCa 0x801d1ec md380_menu_0x2001d3ef
CCa 0x801d1f8 mn_editbuffer_poi
CCa 0x801d206 mn_editbuffer_poi
CCa 0x801d214 md380_menu_0x2001d3ee
CCa 0x801d220 md380_menu_0x2001d3ee
CCa 0x801d226 mn_editbuffer_poi
CCa 0x801d236 md380_menu_0x2001d3ef
CCa 0x801d23e md380_menu_0x2001d3ef
CCa 0x801d244 md380_menu_0x2001d3ee
CCa 0x801d24c md380_menu_0x2001d3ee
CCa 0x801d256 md380_menu_0x2001d3ef
CCa 0x801d260 md380_menu_0x2001d3f1
CCa 0x801d270 md380_menu_0x2001d3ef
CCa 0x801d27c mn_editbuffer_poi
CCa 0x801d28a mn_editbuffer_poi
CCa 0x801d298 md380_menu_0x2001d3ee
CCa 0x801d2a4 md380_menu_0x2001d3ee
CCa 0x801d2aa mn_editbuffer_poi
CCa 0x801d2ba md380_menu_0x2001d3ef
CCa 0x801d2c2 md380_menu_0x2001d3ef
CCa 0x801d2c8 md380_menu_0x2001d3ee
CCa 0x801d2d0 md380_menu_0x2001d3ee
CCa 0x801d2e0 md380_menu_0x2001d3f1
CCa 0x801d2f0 md380_menu_0x2001d3f0
CCa 0x801d2fe md380_menu_0x2001d3f0
CCa 0x801d308 md380_menu_0x2001d3f0
CCa 0x801d310 md380_menu_0x2001d3f0
CCa 0x801d318 md380_menu_0x2001d3f0
CCa 0x801d364 md380_menu_0x2001d3f0
CCa 0x801d37a md380_menu_0x2001d3ee
CCa 0x801d380 md380_menu_0x2001d3ed
CCa 0x801d394 md380_menu_0x2001d3ef
CCa 0x801d39c md380_menu_0x2001d3ef
CCa 0x801d3a2 md380_menu_0x2001d3ee
CCa 0x801d3aa md380_menu_0x2001d3ee
CCa 0x801d3b0 md380_menu_0x2001d3f1
CCa 0x801d3be md380_menu_0x2001d3ee
CCa 0x801d3ca mn_editbuffer_poi
CCa 0x801d3da mn_editbuffer_poi
CCa 0x801d3e6 md380_menu_0x2001d3ef
CCa 0x801d3f2 md380_menu_0x2001d3ef
CCa 0x801d3f8 mn_editbuffer_poi
CCa 0x801d41e md380_menu_0x2001d3f0
CCa 0x801d42e md380_menu_0x2001d3f1
CCa 0x801d444 md380_menu_0x2001d3ee
CCa 0x801d44a md380_menu_0x2001d3ed
CCa 0x801d45e md380_menu_0x2001d3ef
CCa 0x801d466 md380_menu_0x2001d3ef
CCa 0x801d46c md380_menu_0x2001d3ee
CCa 0x801d474 md380_menu_0x2001d3ee
CCa 0x801d47a md380_menu_0x2001d3f1
CCa 0x801d486 md380_menu_0x2001d3ee
CCa 0x801d48e md380_menu_0x2001d3ef
CCa 0x801d49c mn_editbuffer_poi
CCa 0x801d4ac mn_editbuffer_poi
CCa 0x801d4c4 md380_menu_0x2001d3f1
CCa 0x801d4ca md380_menu_0x2001d3f1
CCa 0x801d4d6 md380_menu_0x2001d3f1
CCa 0x801d4dc md380_menu_0x2001d3f0
CCa 0x801d4e8 md380_menu_0x2001d3f0
CCa 0x801d530 md380_menu_0x2001d3ef
CCa 0x801d544 md380_menu_0x2001d3ef
CCa 0x801d54a mn_editbuffer_poi
CCa 0x801d560 md380_menu_0x2001d3ef
CCa 0x801d568 md380_menu_0x2001d3ef
CCa 0x801d56e md380_menu_0x2001d3ee
CCa 0x801d576 md380_menu_0x2001d3ee
CCa 0x801d57c md380_menu_0x2001d3f1
CCa 0x801d58a md380_menu_0x2001d3ee
CCa 0x801d594 mn_editbuffer_poi
CCa 0x801d5de md380_menu_0x2001d3ef
CCa 0x801d5ea mn_editbuffer_poi
CCa 0x801d5f6 md380_menu_0x2001d3ef
CCa 0x801d5fe md380_menu_0x2001d3ef
CCa 0x801d604 md380_menu_0x2001d3ee
CCa 0x801d60c md380_menu_0x2001d3ee
CCa 0x801d612 md380_menu_0x2001d3f1
CCa 0x801d620 md380_menu_0x2001d3ee
CCa 0x801d634 md380_menu_0x2001d3ef
CCa 0x801d63a mn_editbuffer_poi
CCa 0x801d65c md380_menu_0x2001d3ef
CCa 0x801d662 mn_editbuffer_poi
CCa 0x801d684 md380_menu_0x2001d3f1
CCa 0x801d696 md380_menu_0x2001d3ef
CCa 0x801d69c mn_editbuffer_poi
CCa 0x801d6b4 md380_menu_0x2001d3f1
CCa 0x801d6c8 md380_menu_0x2001d3ee
CCa 0x801d6ce md380_menu_0x2001d3ed
CCa 0x801d6e2 md380_menu_0x2001d3ef
CCa 0x801d6ea md380_menu_0x2001d3ef
CCa 0x801d6f0 md380_menu_0x2001d3ee
CCa 0x801d6f8 md380_menu_0x2001d3ee
CCa 0x801d6fe md380_menu_0x2001d3f1
CCa 0x801d70a md380_menu_0x2001d3ee
CCa 0x801d716 mn_editbuffer_poi
CCa 0x801d726 mn_editbuffer_poi
CCa 0x801d732 md380_menu_0x2001d3ef
CCa 0x801d73e md380_menu_0x2001d3ef
CCa 0x801d744 mn_editbuffer_poi
CCa 0x801d758 md380_menu_0x2001d3f1
CCa 0x801d7f8 md380_menu_0x2001d3f0
af+ 0x801d7fc 1204 F_4170
CCa 0x801d842 md380_menu_0x200011e4
CCa 0x801d904 mn_editbuffer_poi
CCa 0x801d90a md380_menu_0x2001d3ef
CCa 0x801d910 md380_menu_0x2001d3ee
CCa 0x801d9ca mn_editbuffer_poi
CCa 0x801d9ce md380_menu_0x2001d3ef
CCa 0x801d9d2 md380_menu_0x2001d3ee
CCa 0x801da42 md380_menu_0x2001d3ef
CCa 0x801da52 md380_menu_0x2001d3f1
CCa 0x801da6e md380_menu_0x2001d3ef
CCa 0x801da82 md380_menu_0x2001d3ef
CCa 0x801da8c md380_menu_0x2001d3ef
CCa 0x801daca md380_menu_0x2001d3ef
CCa 0x801dae6 md380_menu_0x2001d3f1
CCa 0x801db10 md380_menu_0x2001d3ef
CCa 0x801db4e md380_menu_0x2001d3ef
CCa 0x801db62 md380_menu_0x2001d3ef
CCa 0x801db6c md380_menu_0x2001d3ef
CCa 0x801dbaa md380_menu_0x2001d3ef
CCa 0x801dbc6 md380_menu_0x2001d3f1
CCa 0x801dc26 md380_menu_0x2001d3f1
CCa 0x801dc3a md380_menu_0x2001d3ef
CCa 0x801dc74 md380_menu_0x2001d3ef
CCa 0x801dca0 md380_menu_0x2001d3f1
af+ 0x801dcf0 1436 F_4171
CCa 0x801dcfa md380_menu_0x2001d3f1
CCa 0x801dd04 md380_menu_0x2001d3f0
CCa 0x801dd12 md380_menu_0x2001d3f0
CCa 0x801dd1e md380_menu_0x2001d3f1
CCa 0x801dd64 md380_menu_0x2001d3ef
CCa 0x801dd6e md380_menu_0x2001d3ef
CCa 0x801dd76 md380_menu_0x2001d3ef
CCa 0x801dd7c md380_menu_0x2001d3f1
CCa 0x801dd84 md380_menu_0x2001d3ef
CCa 0x801dd8e md380_menu_0x2001d3ef
CCa 0x801dd98 md380_menu_0x2001d3ef
CCa 0x801dda0 md380_menu_0x2001d3ef
CCa 0x801dda6 md380_menu_0x2001d3ef
CCa 0x801ddb0 md380_menu_0x2001d3ef
CCa 0x801ddd8 md380_menu_0x2001d3ef
CCa 0x801dde2 md380_menu_0x2001d3ef
CCa 0x801ddea md380_menu_0x2001d3ef
CCa 0x801ddf0 md380_menu_0x2001d3f1
CCa 0x801ddf8 md380_menu_0x2001d3ef
CCa 0x801de02 md380_menu_0x2001d3ef
CCa 0x801de0c md380_menu_0x2001d3ef
CCa 0x801de14 md380_menu_0x2001d3ef
CCa 0x801de1a md380_menu_0x2001d3ef
CCa 0x801de24 md380_menu_0x2001d3ef
CCa 0x801de4c md380_menu_0x2001d3ef
CCa 0x801de56 md380_menu_0x2001d3ef
CCa 0x801de5e md380_menu_0x2001d3ef
CCa 0x801de64 md380_menu_0x2001d3f1
CCa 0x801de6c md380_menu_0x2001d3ef
CCa 0x801de76 md380_menu_0x2001d3ef
CCa 0x801de80 md380_menu_0x2001d3ef
CCa 0x801de88 md380_menu_0x2001d3ef
CCa 0x801de8e md380_menu_0x2001d3ef
CCa 0x801de98 md380_menu_0x2001d3ef
CCa 0x801dec0 md380_menu_0x2001d3ef
CCa 0x801deca md380_menu_0x2001d3ef
CCa 0x801ded2 md380_menu_0x2001d3ef
CCa 0x801ded8 md380_menu_0x2001d3f1
CCa 0x801dee0 md380_menu_0x2001d3ef
CCa 0x801deea md380_menu_0x2001d3ef
CCa 0x801def4 md380_menu_0x2001d3ef
CCa 0x801defc md380_menu_0x2001d3ef
CCa 0x801df02 md380_menu_0x2001d3ef
CCa 0x801df0c md380_menu_0x2001d3ef
CCa 0x801df42 md380_menu_0x2001d3f1
CCa 0x801df4c md380_menu_0x2001d3ef
CCa 0x801df54 md380_menu_0x2001d3ef
CCa 0x801df5a md380_menu_0x2001d3f1
CCa 0x801df6a md380_menu_0x2001d3f0
CCa 0x801df78 md380_menu_0x2001d3f0
CCa 0x801df90 md380_menu_0x2001d3ef
CCa 0x801df96 mn_editbuffer_poi
CCa 0x801dfac md380_menu_0x2001d3ef
CCa 0x801dfb6 md380_menu_0x2001d3ef
CCa 0x801dfba mn_editbuffer_poi
CCa 0x801dfcc md380_menu_0x2001d3ef
CCa 0x801dfda md380_menu_0x2001d3ef
CCa 0x801dfde mn_editbuffer_poi
CCa 0x801dff0 md380_menu_0x2001d3ef
CCa 0x801dffc md380_menu_0x2001d3ef
CCa 0x801e000 mn_editbuffer_poi
CCa 0x801e010 md380_menu_0x2001d3ef
CCa 0x801e018 md380_menu_0x2001d3ef
CCa 0x801e02a md380_menu_0x2001d3ef
CCa 0x801e02e mn_editbuffer_poi
CCa 0x801e040 md380_menu_0x2001d3ef
CCa 0x801e044 mn_editbuffer_poi
CCa 0x801e054 md380_menu_0x2001d3ef
CCa 0x801e05c md380_menu_0x2001d3ef
CCa 0x801e060 mn_editbuffer_poi
CCa 0x801e072 md380_menu_0x2001d3ef
CCa 0x801e076 mn_editbuffer_poi
CCa 0x801e086 md380_menu_0x2001d3ef
CCa 0x801e08e md380_menu_0x2001d3ef
CCa 0x801e092 mn_editbuffer_poi
CCa 0x801e0a4 md380_menu_0x2001d3ef
CCa 0x801e0a8 mn_editbuffer_poi
CCa 0x801e0b8 md380_menu_0x2001d3ef
CCa 0x801e0c0 md380_menu_0x2001d3ef
CCa 0x801e0c8 md380_menu_0x2001d3ef
CCa 0x801e0cc mn_editbuffer_poi
CCa 0x801e0da md380_menu_0x2001d3ef
CCa 0x801e0de mn_editbuffer_poi
CCa 0x801e0ec md380_menu_0x2001d3ef
CCa 0x801e0fa md380_menu_0x2001d3ef
CCa 0x801e102 md380_menu_0x2001d3ef
CCa 0x801e110 md380_menu_0x2001d3f1
CCa 0x801e136 md380_menu_0x2001d3f1
CCa 0x801e13e md380_menu_0x2001d3ef
CCa 0x801e144 md380_menu_0x2001d3ef
CCa 0x801e148 md380_menu_0x2001d3f1
CCa 0x801e154 md380_menu_0x2001d3f0
CCa 0x801e160 md380_menu_0x2001d3f0
CCa 0x801e172 md380_menu_0x2001d3ef
CCa 0x801e176 mn_editbuffer_poi
CCa 0x801e18e md380_menu_0x2001d3ef
CCa 0x801e196 md380_menu_0x2001d3ef
CCa 0x801e19e md380_menu_0x2001d3ef
CCa 0x801e1a2 mn_editbuffer_poi
CCa 0x801e1b0 md380_menu_0x2001d3ef
CCa 0x801e1b4 mn_editbuffer_poi
CCa 0x801e1c2 md380_menu_0x2001d3ef
CCa 0x801e1d0 md380_menu_0x2001d3ef
CCa 0x801e1d8 md380_menu_0x2001d3ef
CCa 0x801e1e6 md380_menu_0x2001d3f1
CCa 0x801e204 md380_menu_0x2001d3ef
CCa 0x801e20c md380_menu_0x2001d3ef
CCa 0x801e220 md380_menu_0x2001d3ef
CCa 0x801e226 md380_menu_0x2001d3ef
CCa 0x801e252 md380_menu_0x2001d3ef
CCa 0x801e25a md380_menu_0x2001d3ef
CCa 0x801e26e md380_menu_0x2001d3ef
CCa 0x801e274 md380_menu_0x2001d3ef
CCa 0x801e2a8 md380_menu_0x200011e4
CCa 0x801e2bc md380_menu_0x200011e4
af+ 0x801e2d8 3408 F_4224
CCa 0x801e2da md380_menu_0x2001d3ef
CCa 0x801e2e6 md380_menu_0x2001d3ef
CCa 0x801e2fa md380_menu_0x2001d3ef
CCa 0x801e30e md380_menu_0x2001d3ef
CCa 0x801e322 md380_menu_0x2001d3ef
CCa 0x801e336 md380_menu_0x2001d3ef
CCa 0x801e33c mn_editbuffer_poi
CCa 0x801e358 md380_menu_0x2001d3ef
CCa 0x801e36c md380_menu_0x2001d3ef
CCa 0x801e37e md380_menu_0x2001d3ef
CCa 0x801e392 md380_menu_0x2001d3ef
CCa 0x801e398 mn_editbuffer_poi
CCa 0x801e3cc md380_menu_0x2001d3ef
CCa 0x801e3de md380_menu_0x2001d3ef
CCa 0x801e3f0 md380_menu_0x2001d3ef
CCa 0x801e3fc md380_menu_0x2001d3ef
CCa 0x801e410 md380_menu_0x2001d3ef
CCa 0x801e424 md380_menu_0x2001d3ef
CCa 0x801e438 md380_menu_0x2001d3ef
CCa 0x801e44c md380_menu_0x2001d3ef
CCa 0x801e452 mn_editbuffer_poi
CCa 0x801e46e md380_menu_0x2001d3ef
CCa 0x801e482 md380_menu_0x2001d3ef
CCa 0x801e494 md380_menu_0x2001d3ef
CCa 0x801e4b2 md380_menu_0x2001d3ef
CCa 0x801e4b8 mn_editbuffer_poi
CCa 0x801e4e2 md380_menu_0x2001d3ef
CCa 0x801e4f4 md380_menu_0x2001d3ef
CCa 0x801e506 md380_menu_0x2001d3ef
CCa 0x801e512 md380_menu_0x2001d3ef
CCa 0x801e518 mn_editbuffer_poi
CCa 0x801e52a md380_menu_0x2001d3ef
CCa 0x801e530 mn_editbuffer_poi
CCa 0x801e540 md380_menu_0x2001d3ef
CCa 0x801e552 md380_menu_0x2001d3ef
CCa 0x801e558 mn_editbuffer_poi
CCa 0x801e56a md380_menu_0x2001d3ef
CCa 0x801e57c md380_menu_0x2001d3ef
CCa 0x801e582 mn_editbuffer_poi
CCa 0x801e592 md380_menu_0x2001d3ef
CCa 0x801e5a4 md380_menu_0x2001d3ef
CCa 0x801e5aa mn_editbuffer_poi
CCa 0x801e5bc md380_menu_0x2001d3ef
CCa 0x801e5c2 mn_editbuffer_poi
CCa 0x801e5d4 md380_menu_0x2001d3ef
CCa 0x801e5e6 md380_menu_0x2001d3ef
CCa 0x801e5f6 md380_menu_0x2001d3ef
CCa 0x801e5fc mn_editbuffer_poi
CCa 0x801e610 md380_menu_0x2001d3ef
CCa 0x801e638 md380_menu_0x2001d3ef
CCa 0x801e64e md380_menu_0x2001d3ef
CCa 0x801e660 md380_menu_0x2001d3ef
CCa 0x801e6a0 md380_menu_0x2001d3ef
CCa 0x801e6b6 md380_menu_0x2001d3ef
CCa 0x801e6c8 md380_menu_0x2001d3ef
CCa 0x801e6dc md380_menu_0x2001d3ef
CCa 0x801e6f0 md380_menu_0x2001d3ef
CCa 0x801e702 md380_menu_0x2001d3ef
CCa 0x801e716 md380_menu_0x2001d3ef
CCa 0x801e72a md380_menu_0x2001d3ef
CCa 0x801e73e md380_menu_0x2001d3ef
CCa 0x801e752 md380_menu_0x2001d3ef
CCa 0x801e758 mn_editbuffer_poi
CCa 0x801e76a md380_menu_0x2001d3ef
CCa 0x801e77c md380_menu_0x2001d3ef
CCa 0x801e78e md380_menu_0x2001d3ef
CCa 0x801e7a0 md380_menu_0x2001d3ef
CCa 0x801e7a6 mn_editbuffer_poi
CCa 0x801e7b8 md380_menu_0x2001d3ef
CCa 0x801e7ca md380_menu_0x2001d3ef
CCa 0x801e7de md380_menu_0x2001d3ef
CCa 0x801e7f2 md380_menu_0x2001d3ef
CCa 0x801e804 md380_menu_0x2001d3ef
CCa 0x801e810 md380_menu_0x2001d3ef
CCa 0x801e826 md380_menu_0x2001d3ef
CCa 0x801e82c mn_editbuffer_poi
CCa 0x801e83e md380_menu_0x2001d3ef
CCa 0x801e844 mn_editbuffer_poi
CCa 0x801e854 md380_menu_0x2001d3ef
CCa 0x801e86a md380_menu_0x2001d3ef
CCa 0x801e870 mn_editbuffer_poi
CCa 0x801e882 md380_menu_0x2001d3ef
CCa 0x801e888 mn_editbuffer_poi
CCa 0x801e898 md380_menu_0x2001d3ef
CCa 0x801e89e mn_editbuffer_poi
CCa 0x801e8b2 md380_menu_0x2001d3ef
CCa 0x801e8de md380_menu_0x2001d3ef
CCa 0x801e8f4 md380_menu_0x2001d3ef
CCa 0x801e906 md380_menu_0x2001d3ef
CCa 0x801e946 md380_menu_0x2001d3ef
CCa 0x801e95c md380_menu_0x2001d3ef
CCa 0x801e96e md380_menu_0x2001d3ef
CCa 0x801e982 md380_menu_0x2001d3ef
CCa 0x801e996 md380_menu_0x2001d3ef
CCa 0x801e9a8 md380_menu_0x2001d3ef
CCa 0x801e9bc md380_menu_0x2001d3ef
CCa 0x801e9d0 md380_menu_0x2001d3ef
CCa 0x801e9e4 md380_menu_0x2001d3ef
CCa 0x801e9f8 md380_menu_0x2001d3ef
CCa 0x801e9fe mn_editbuffer_poi
CCa 0x801ea0e md380_menu_0x2001d3ef
CCa 0x801ea14 mn_editbuffer_poi
CCa 0x801ea24 md380_menu_0x2001d3ef
CCa 0x801ea2a mn_editbuffer_poi
CCa 0x801ea3a md380_menu_0x2001d3ef
CCa 0x801ea40 mn_editbuffer_poi
CCa 0x801ea50 md380_menu_0x2001d3ef
CCa 0x801ea66 md380_menu_0x2001d3ef
CCa 0x801ea7a md380_menu_0x2001d3ef
CCa 0x801ea8e md380_menu_0x2001d3ef
CCa 0x801eaa0 md380_menu_0x2001d3ef
CCa 0x801eaac md380_menu_0x2001d3ef
CCa 0x801eab2 mn_editbuffer_poi
CCa 0x801eac2 md380_menu_0x2001d3ef
CCa 0x801ead4 md380_menu_0x2001d3ef
CCa 0x801eae4 md380_menu_0x2001d3ef
CCa 0x801eafa md380_menu_0x2001d3ef
CCa 0x801eb10 md380_menu_0x2001d3ef
CCa 0x801eb16 mn_editbuffer_poi
CCa 0x801eb44 md380_menu_0x2001d3ef
CCa 0x801eb66 md380_menu_0x2001d3ef
CCa 0x801eb6c mn_editbuffer_poi
CCa 0x801eb7a md380_menu_0x2001d3ef
CCa 0x801eb8c md380_menu_0x2001d3ef
CCa 0x801eb92 mn_editbuffer_poi
CCa 0x801eba0 md380_menu_0x2001d3ef
CCa 0x801ebb2 md380_menu_0x2001d3ef
CCa 0x801ebb8 mn_editbuffer_poi
CCa 0x801ebca md380_menu_0x2001d3ef
CCa 0x801ebe0 md380_menu_0x2001d3ef
CCa 0x801ebf6 md380_menu_0x2001d3ef
CCa 0x801ec0c md380_menu_0x2001d3ef
CCa 0x801ec22 md380_menu_0x2001d3ef
CCa 0x801ec38 md380_menu_0x2001d3ef
CCa 0x801ec4e md380_menu_0x2001d3ef
CCa 0x801ec64 md380_menu_0x2001d3ef
CCa 0x801ec7a md380_menu_0x2001d3ef
CCa 0x801ec90 md380_menu_0x2001d3ef
CCa 0x801eca6 md380_menu_0x2001d3ef
CCa 0x801ecb8 md380_menu_0x2001d3ef
CCa 0x801ecbe mn_editbuffer_poi
CCa 0x801ecce md380_menu_0x2001d3ef
CCa 0x801ecde md380_menu_0x2001d3ef
CCa 0x801ece4 mn_editbuffer_poi
CCa 0x801ecf8 md380_menu_0x2001d3ef
CCa 0x801ed0e md380_menu_0x2001d3ef
CCa 0x801ed24 md380_menu_0x2001d3ef
CCa 0x801ed36 md380_menu_0x2001d3ef
CCa 0x801ed48 md380_menu_0x2001d3ef
CCa 0x801ed5a md380_menu_0x2001d3ef
CCa 0x801ed5e mn_editbuffer_poi
CCa 0x801ed6c md380_menu_0x2001d3ef
CCa 0x801ed78 md380_menu_0x2001d3ef
CCa 0x801ed82 md380_menu_0x2001d3ef
CCa 0x801ed94 md380_menu_0x2001d3ef
CCa 0x801ed98 mn_editbuffer_poi
CCa 0x801eda8 md380_menu_0x2001d3ef
CCa 0x801edac mn_editbuffer_poi
CCa 0x801edba md380_menu_0x2001d3ef
CCa 0x801edcc md380_menu_0x2001d3ef
CCa 0x801edde md380_menu_0x2001d3ef
CCa 0x801edf0 md380_menu_0x2001d3ef
CCa 0x801ee02 md380_menu_0x2001d3ef
CCa 0x801ee14 md380_menu_0x2001d3ef
CCa 0x801ee26 md380_menu_0x2001d3ef
CCa 0x801ee38 md380_menu_0x2001d3ef
CCa 0x801ee4a md380_menu_0x2001d3ef
CCa 0x801ee5c md380_menu_0x2001d3ef
CCa 0x801ee6e md380_menu_0x2001d3ef
CCa 0x801ee82 md380_menu_0x2001d3ef
CCa 0x801ee86 mn_editbuffer_poi
CCa 0x801ee98 md380_menu_0x2001d3ef
CCa 0x801ee9c mn_editbuffer_poi
CCa 0x801eeac md380_menu_0x2001d3ef
CCa 0x801eebe md380_menu_0x2001d3ef
CCa 0x801eed0 md380_menu_0x2001d3ef
CCa 0x801eee2 md380_menu_0x2001d3ef
CCa 0x801eef4 md380_menu_0x2001d3ef
CCa 0x801ef06 md380_menu_0x2001d3ef
CCa 0x801ef1a md380_menu_0x2001d3ef
CCa 0x801ef1e mn_editbuffer_poi
CCa 0x801ef2e md380_menu_0x2001d3ef
CCa 0x801ef32 mn_editbuffer_poi
CCa 0x801ef52 md380_menu_0x2001d3ef
CCa 0x801ef64 md380_menu_0x2001d3ef
CCa 0x801ef68 mn_editbuffer_poi
CCa 0x801ef78 md380_menu_0x2001d3ef
CCa 0x801ef7c mn_editbuffer_poi
CCa 0x801efb4 md380_menu_0x2001d3ef
CCa 0x801efc6 md380_menu_0x2001d3ef
CCa 0x801efca mn_editbuffer_poi
CCa 0x801efda md380_menu_0x2001d3ef
CCa 0x801efde mn_editbuffer_poi
CCa 0x801efee md380_menu_0x2001d3ef
CCa 0x801f000 md380_menu_0x2001d3ef
CCa 0x801f004 mn_editbuffer_poi
CCa 0x801f014 md380_menu_0x2001d3ef
CCa 0x801f018 mn_editbuffer_poi
af+ 0x801f03c 8 F_4292
af+ 0x801f044 5240 F_4225
CCa 0x801f228 GPIO_SetBits(GPIOC, 0x40) .. Bit 3 .. C3  2T/5T / RF_RX_INTER
CCa 0x801f232 SystemConfig
CCa 0x801f236 ConfigData + 0x15 (byte) F_4225
CCa 0x801f334 GPIO_SetBits(GPIOC, 0x40)
CCa 0x801f33e SystemConfig
CCa 0x801f342 ConfigData + 0x15 (byte) F_4225
CCa 0x801f3b0 GPIO_SetBits(GPIOC, 0x40)
CCa 0x801f3ba SystemConfig
CCa 0x801f3be ConfigData + 0x15 (byte) F_4225
CCa 0x801f3ce md380_menu_0x200011e4
CCa 0x801f40c GPIO_SetBits(GPIOC, 0x40)
CCa 0x801f416 SystemConfig
CCa 0x801f41a ConfigData + 0x15 (byte) F_4225
CCa 0x801f558 SystemConfig
CCa 0x801f55c ConfigData + 0xa  (byte) F_4225
CCa 0x801f578 SystemConfig
CCa 0x801f57c ConfigData + 0x9  (byte) F_4225
CCa 0x801f5b6 sprite telephone
CCa 0x801f6de md380_menu_0x200011e4
CCa 0x801f7da SystemConfig
CCa 0x801f7de ConfigData + 0x16 (byte) F_4225
CCa 0x801f814 SystemConfig
CCa 0x801f818 ConfigData + 0x16 (byte) F_4225
CCa 0x801f878 GPIO_SetBits(GPIOC, 0x40)
CCa 0x801f882 SystemConfig
CCa 0x801f886 ConfigData + 0x0  (byte) F_4225
CCa 0x801f88c SystemConfig
CCa 0x801f890 ConfigData + 0x0  (byte) F_4225
CCa 0x801f996 SystemConfig
CCa 0x801f99a ConfigData + 0x15 (byte) F_4225
CCa 0x801fc04 SystemConfig
CCa 0x801fc06 ConfigData + 0x1  (byte) F_4225
CCa 0x801fc9a SystemConfig
CCa 0x801fc9c ConfigData + 0x16 (byte) F_4225
CCa 0x801fd12 SystemConfig
CCa 0x801fd14 ConfigData + 0x16 (byte) F_4225
CCa 0x801ffda GPIO_SetBits(GPIOC, 0x40)
CCa 0x801ffe4 SystemConfig
CCa 0x801ffe8 ConfigData + 0x15 (byte) F_4225
CCa 0x80201a8 md380_menu_edit_buf
CCa 0x80201c0 GPIO_SetBits(GPIOC, 0x40)
CCa 0x80201ca SystemConfig
CCa 0x80201ce ConfigData + 0x15 (byte) F_4225
CCa 0x802045c 0x2001cfe4 signal strength (byte) 0-5
CCa 0x802046a 0x2001cfe4 signal strength (byte) 0-5
CCa 0x8020472 Ant symbol with strength
CCa 0x8020486 Ant symbol with strength
af+ 0x80204dc 282 F_4293
CCa 0x8020544 md380_menu_0x200011e4
CCa 0x802054c md380_menu_id
CCa 0x8020554 md380_menu_0x200011e4
CCa 0x802055c md380_menu_memory
CCa 0x802057c md380_menu_mem_base
CCa 0x80205c6 md380_menu_id
CCa 0x80205d0 md380_menu_0x200011e4
CCa 0x80205da md380_menu_id
CCa 0x80205ee md380_menu_0x200011e4
af+ 0x8020604 152 F_4294
af+ 0x802069c 44 F_4295
af+ 0x80206c8 58 F_4296
CCa 0x80206f0 E.m.e.r.g.e.n.c.y
af+ 0x8020702 74 F_4297
CCa 0x802072a A.l.a.r.m...R.c.v.d
af+ 0x802074c 58 F_4298
af+ 0x802079c 294 F_800_something_with_ant
CCa 0x80207ce SystemConfig
CCa 0x80207d0 ConfigData + 0x1  (byte) F_800
CCa 0x80208a8 Ant symbol with strength
CCa 0x80208ba Ant symbol with strength
CCa 0x80208e8 SystemConfig
af+ 0x8020974 16 gfx_select_font
af+ 0x8020988 8 F_4172
af+ 0x8020990 8 F_4226
af+ 0x802099c 154 F_994
af+ 0x8020a36 10 F_4353
af+ 0x8020a40 18 gfx_Single_Char_Out_X_Y_Char
af+ 0x8020a58 3020 F_28
af+ 0x8021624 48 F_30
af+ 0x8021654 30 F_31
af+ 0x8021672 26 F_32
af+ 0x802168c 484 aes_startup_check
af+ 0x8021870 18 F_804
af+ 0x8021882 18 Get_Welcome_Line1_from_spi_flash
af+ 0x8021894 18 Get_Welcome_Line2_from_spi_flash
af+ 0x80218a6 18 Read_Config_Data_from_SPIFlash
CCa 0x80218ae SystemConfig
af+ 0x80218b8 18 Write_Config_Data_to_SPIFlash
CCa 0x80218c0 SystemConfig
af+ 0x80218d4 474 F_806
af+ 0x8021ac0 18 F_807
af+ 0x8021ad2 26 F_808
af+ 0x8021aec 22 F_995
af+ 0x8021b02 36 F_809
af+ 0x8021b26 36 F_4300
af+ 0x8021b4a 36 F_4173
af+ 0x8021b6e 116 F_810
af+ 0x8021be2 26 F_811
af+ 0x8021bfc 156 F_812
af+ 0x8021c98 362 F_260
af+ 0x8021e02 26 F_4301
af+ 0x8021e1c 26 F_261
af+ 0x8021e4c 26 F_813_SPIF_ZoneName2Ram_0x40_ID
CCa 0x8021e4c get aktive zone name to ram
CCa 0x8021e56 SPI 0x149a0 + 0x40 begin zone names
CCa 0x8021e5c set akt. Zone_Name
af+ 0x8021e66 26 F_814_SPIF_ZoneName2Ram_0x20_ID
CCa 0x8021e66 get aktive zone name to ram 
CCa 0x8021e70 SPI 0x149a0 + 0x40 begin zone names
CCa 0x8021e76 set akt. Zone_Name
af+ 0x8021e80 24 F_815
af+ 0x8021e98 24 F_816
af+ 0x8021eb0 26 F_4302_SMS_QuickText2Ram_ID
af+ 0x8021eca 16 F_Get_From_SPIF
af+ 0x8021edc 18 Edit_Message_Confirm_Store_Message_to_spi_1
af+ 0x8021eee 26 F_4354
af+ 0x8021f08 26 Edit_Message_Confirm_Store_Message_to_spi_2
af+ 0x8021f22 18 F_817_SPIF_Number_of_zones
af+ 0x8021f34 18 F_818
af+ 0x8021f46 18 F_1065_spiflash_read_c8_from_0x416d0
af+ 0x8021f58 18 F_4428
af+ 0x8021f6a 32 F_4355
CCa 0x8021f80 md380_menu_edit_buf
af+ 0x8021f8a 44 F_4429
CCa 0x8021fac md380_menu_edit_buf
af+ 0x8021fb6 84 F_4356
af+ 0x802200a 18 F_4430
af+ 0x802201c 18 F_4357
af+ 0x802202e 32 F_4358_SPIF_Read_Send_SMS
CCa 0x8022044 md380_menu_edit_buf
af+ 0x802204e 44 F_4359
af+ 0x802207a 126 F_4360
af+ 0x80220f8 26 F_819
af+ 0x8022112 26 F_262
af+ 0x802212c 24 F_4303
af+ 0x8022144 26 F_263
af+ 0x802215e 20 F_4361
af+ 0x8022172 28 F_4431
af+ 0x80221d6 18 F_264
af+ 0x8022268 80 F_4362
af+ 0x8022334 80 F_4363
af+ 0x80223fc 18 F_4432
af+ 0x802240e 18 F_4364
af+ 0x8022420 24 F_4365
af+ 0x8022438 18 F_4433
af+ 0x802244a 18 F_4366
af+ 0x8022460 24 F_4367
af+ 0x802247c 36 F_4434
af+ 0x80224a0 80 F_4368
af+ 0x8022564 18 F_4435
af+ 0x802257c 18 F_4369
af+ 0x802259c 24 F_4370
af+ 0x80225b8 18 F_820
af+ 0x80225e8 118 F_4371
af+ 0x802268c 28 F_4436
af+ 0x80226cc 16 F_4372
af+ 0x80226e4 18 F_4093_spi_read_0x2000_from_0x80000_to_CCM
af+ 0x80226f6 18 F_4064
af+ 0x8022708 18 F_4094_spi_read_0x3dc0_from_0x84000_to_CCM
af+ 0x802271a 18 F_4065
af+ 0x80227e8 158 F_265
af+ 0x8022886 86 gfx_drawbmp
af+ 0x80228e0 84 F_4119
af+ 0x8022934 72 F_4120
af+ 0x802297c 314 F_4121
af+ 0x8022ab6 76 F_4122
af+ 0x8022b02 342 F_4123
af+ 0x8022c58 250 F_4124
af+ 0x8022d52 32 F_4125
af+ 0x8022d72 220 F_4126
af+ 0x8022e4e 248 F_4127
af+ 0x8022f46 24 F_4128
af+ 0x8022f74 42 F_4174
af+ 0x8022f9e 42 F_4175
af+ 0x8022fc8 16 F_4176
af+ 0x8022fd8 32 F_4177
af+ 0x8022ff8 86 F_4178
af+ 0x802304e 24 F_4179
af+ 0x802306c 42 F_4227
af+ 0x80230b8 54 F_4180
af+ 0x80230f4 36 F_4373
af+ 0x8023118 10 F_4228
af+ 0x8023122 22 F_4456
af+ 0x8023138 26 F_4374
af+ 0x8023152 84 F_4457
af+ 0x80231a6 84 F_4304
af+ 0x80231fa 62 F_4437
af+ 0x8023238 68 F_4305
af+ 0x802327c 74 F_4306
af+ 0x80232c6 84 F_4438
af+ 0x802331a 68 F_4229
af+ 0x802335e 1102 F_4474
CCa 0x802352a SystemConfig
CCa 0x802352e ConfigData + 0xb  (byte) F_4474
CCa 0x8023536 SystemConfig
CCa 0x802353a ConfigData + 0xb  (byte) F_4474
af+ 0x80237ac 1002 F_4458
CCa 0x80237fa seen picture  2 picture .. 1 ok 2 ok 3 wrong palet (menu)
CCa 0x8023824 2 people picture or 4 windows or envelope closed or phone
CCa 0x802384e 4 windows and more (menu)
CCa 0x802386c numbers (Menu)
CCa 0x8023934 picture (N) .. Scan or (1) (2)
CCa 0x802397c record picture
CCa 0x80239a0 envelope closed blue
CCa 0x80239c4 envelope closed yellow
CCa 0x8023a10 envelope closed yellow
CCa 0x8023a24 envelope open yellow
CCa 0x8023a5c envelope closed yellow
CCa 0x8023a9a 1 people
CCa 0x8023ae4 1 people or TG
CCa 0x8023ae4 TalkGroupSymbol in contact list
CCa 0x8023b2a record picture
CCa 0x8023b66 TG or  1 people
af+ 0x8023bdc 408 F_4439
af+ 0x8023d74 96 F_4230
af+ 0x8023dd4 22 F_4440
af+ 0x8023dea 26 F_4231
af+ 0x8023e04 90 F_4232
af+ 0x8023e5e 56 F_4441
af+ 0x8023e96 64 F_4442
af+ 0x8023ed6 82 F_4375
af+ 0x8023f28 140 F_4376
af+ 0x8023fb4 22 F_4307
af+ 0x8024392 104 F_4181
af+ 0x80244b2 112 F_4233
af+ 0x8024538 104 F_4377
af+ 0x80245b6 118 F_4378
af+ 0x802466c 4992 F_4308
CCa 0x80248ec md380_menu_edit_buf
CCa 0x8024a06 md380_menu_edit_buf
af+ 0x8025a10 174 F_4182
af+ 0x8025ae0 28 F_4234
af+ 0x8025afc 18 F_4183
af+ 0x8025b0e 20 F_4148
af+ 0x8025b22 62 F_4184
af+ 0x8025b60 34 F_4185
af+ 0x8025b82 46 F_4186
af+ 0x8025bb0 6 F_4187
af+ 0x8025bbc 30 F_4188
af+ 0x8025dbc 94 F_4235
af+ 0x8025e1a 24 F_4189
af+ 0x8025e40 42 F_4236
af+ 0x8025e6c 362 F_1066
af+ 0x8025fdc 76 F_996
af+ 0x8026028 110 F_4066
af+ 0x8026096 148 F_267
af+ 0x802612a 12 F_821
CCa 0x8026136 ... maybe STM32LIB ErrorStatus RTC_Init(RTC_InitTypeDef* RTC_InitStruct) \n Beginn Function ... Set RTC_CR (Hour format RTC_CR and RTC_PRER ) r0  arg[0] arg[1] arg[2]\narg[0] RTC_CR\arg[1] RTC_PRER\n arg[2] RTC_PRER
af+ 0x8026138 112 RTC_Init
CCa 0x8026194 Call RTC_ExitInitMode()
af+ 0x80261a8 14 F_998
af+ 0x80261b6 84 RTC_EnterInitMode
CCa 0x80261b8 ... maybe STM32LIB ErrorStatus RTC_EnterInitMode(void)
af+ 0x802620a 18 RTC_ExitInitMode
CCa 0x802620a ... maybe STM32LIB Fuction void RTC_ExitInitMode(void)
af+ 0x802621c 100 RTC_WaitForSynchro
CCa 0x802621c ... maybe STM32LIB ErrorStatus RTC_WaitForSynchro(void)
af+ 0x8026280 202 RTC_SetTime
CCa 0x8026280 ... maybe STM32LIB ErrorStatus RTC_SetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct)
CCa 0x802631e Call RTC_ExitInitMode()
af+ 0x802634a 80 RTC_GetTime
af+ 0x802639a 198 RTC_SetDate
CCa 0x8026434 Call RTC_ExitInitMode()
af+ 0x8026460 76 RTC_GetDate
af+ 0x80264ac 56 RTC_WakeUpClockConfig
af+ 0x80264e4 32 RTC_SetWakeUpCounter
af+ 0x8026504 122 RTC_RefClockCmd
af+ 0x8026594 2 F_822
af+ 0x8026596 24 something_to_store_to_RTC_backup_registers
af+ 0x80265b8 86 F_999
af+ 0x80265ba 84 RTC_ITConfig
af+ 0x8026614 30 RTC_ClearFlag
af+ 0x802664c 22 F_92
af+ 0x8026662 24 RTC_Bcd2ToByte
af+ 0x802667a 32 F_4379
af+ 0x802669c 70 F_276
af+ 0x80266e2 100 F_277
af+ 0x8026754 106 F_278
af+ 0x80267be 266 F_94
af+ 0x80268c8 122 F_1097..Mybee.itoa()..check
CCa 0x80268c8 r0 = iValue, r1 
af+ 0x8026942 112 Edit_Message_Confirm_Send_4
af+ 0x80269f4 166 F_95
af+ 0x8026a9a 22 GPIO_ReadInputDataBit
af+ 0x8026ab0 6  GPIO_ReadInputData
af+ 0x8026ab6 4 GPIO_SetBits
af+ 0x8026aba 4 GPIO_ResetBits
af+ 0x8026abe 14 GPIO_WriteBit
af+ 0x8026acc 82 F_96
af+ 0x8026b20 596 F_4309
CCa 0x8026ba8 blue background
CCa 0x8026c46 smarl arrow right picture
CCa 0x8026d2a smarl arrow right picture
af+ 0x8026d7c 1270 F_4310
CCa 0x8026d7e md380_menu_0x2001d3f0
CCa 0x8026e8e blue background
CCa 0x8026f22 smarl arrow right picture
CCa 0x802703a smarl arrow right picture
CCa 0x802711c smarl arrow left picture
CCa 0x80271a4 smarl arrow right picture
af+ 0x8027278 1228 F_4118
CCa 0x802727a md380_menu_0x2001d3f0
CCa 0x80272f8 blue background
CCa 0x8027304 smarl arrow left picture
CCa 0x8027428 smarl arrow right picture
CCa 0x80274b4 smarl arrow right picture
CCa 0x80275a4 smarl arrow right picture
CCa 0x8027606 smarl arrow left picture
CCa 0x8027684 smarl arrow right picture
af+ 0x802778c 522 F_4311
CCa 0x802778e md380_menu_0x2001d3f0
CCa 0x80278e8 md380_menu_0x2001d3ee
CCa 0x80278ec md380_menu_0x2001d3ed
CCa 0x80278fc md380_menu_0x2001d3ef
CCa 0x8027902 md380_menu_0x2001d3ef
CCa 0x8027906 md380_menu_0x2001d3ee
CCa 0x802790c md380_menu_0x2001d3ee
CCa 0x8027910 md380_menu_0x2001d3f1
CCa 0x8027916 md380_menu_0x2001d3ee
CCa 0x8027920 mn_editbuffer_poi
CCa 0x802792e mn_editbuffer_poi
CCa 0x8027938 md380_menu_0x2001d3ef
CCa 0x8027948 md380_menu_0x2001d3ef
CCa 0x802794c mn_editbuffer_poi
af+ 0x80279a8 170 F_4312
af+ 0x8027a52 66 F_823
af+ 0x8027aa8 58 F_4519_wtf
af+ 0x8027ae8 2094 F_4520
CCa 0x8027bae ... D.i.s.a.b.l.e.d
CCa 0x8027c18 SystemConfig
CCa 0x8027c1c ConfigData + 0x15 (byte) F_4520
CCa 0x8027c38 SystemConfig
CCa 0x8027c3c ConfigData + 0x15 (byte) F_4520
CCa 0x8027ca0 md380_menu_0x2001d3ed
CCa 0x8027ca8 md380_menu_0x2001d3ee
CCa 0x8027cb0 md380_menu_0x2001d3ef
CCa 0x8027e56 md380_menu_0x2001d3ed
CCa 0x8027e5e md380_menu_0x2001d3ee
CCa 0x8027e66 md380_menu_0x2001d3ef
CCa 0x80280b4 md380_menu_0x200011e4
CCa 0x80280be md380_menu_0x200011e4
CCa 0x80280d2 md380_menu_0x200011e4
CCa 0x80280de md380_menu_0x200011e4
CCa 0x80280e6 md380_menu_0x200011e4
CCa 0x80280ec md380_menu_0x200011e4
CCa 0x80280f6 md380_menu_id
CCa 0x80280fc md380_menu_0x200011e4
CCa 0x8028104 md380_menu_memory
CCa 0x802812a SystemConfig
CCa 0x802812e ConfigData + 0x15 (byte) F_4520
CCa 0x80281d8 ... C.o.n.f.i.r.m
CCa 0x80281f8 ... B.a.c.k.
CCa 0x8028230 SystemConfig
CCa 0x8028234 ConfigData + 0x15 (byte) F_4520
CCa 0x8028250 SystemConfig
CCa 0x8028254 ConfigData + 0x15 (byte) F_4520
CCa 0x802828e last pressed key
CCa 0x80282ac SystemConfig
CCa 0x80282b0 ConfigData + 0x15 (byte) F_4520
CCa 0x80282cc SystemConfig
CCa 0x80282d0 ConfigData + 0x15 (byte) F_4520
af+ 0x8028316 52 F_4313
af+ 0x802834a 396 F_4190
af+ 0x80284d6 442 F_4237
CCa 0x8028514 get channel nr
CCa 0x8028522 get akt. Zone_Name
af+ 0x80286e8 580 F_286
CCa 0x80287ac ... M.e.n.u
CCa 0x80287c6 2 house picture
CCa 0x80287d6 0x1f 31
CCa 0x80287d8 0x9d 157
CCa 0x80287da 0x10 16
CCa 0x80287e2 0x39 57
CCa 0x80287e4 0x2c 44
CCa 0x80287e6 0x35 53
CCa 0x80287ee 0x30 48
CCa 0x80287f0 0x18 24
CCa 0x80287f2 0x20 32
CCa 0x80287fa 0x30 48
CCa 0x80287fc 0x9d 157
CCa 0x80287fe 0x20 32
CCa 0x8028800 0x91 145
CCa 0x8028806 0x39 57
CCa 0x8028808 0x9d 157
CCa 0x802880a 0x30 48
CCa 0x8028812 0x4a 74
CCa 0x8028814 0x9d 157
CCa 0x8028816 0x3a 58
CCa 0x8028818 0x22 34
CCa 0x8028838 0x9d 157
CCa 0x802883e get akt. Zone_Name
CCa 0x8028848 0x4b 75
CCa 0x802884a 0x69 105
CCa 0x802885a ... C.H.
CCa 0x8028862 get channel nr
CCa 0x8028868 test for 2 digit channel
CCa 0x8028870 get channel nr
CCa 0x8028876 +0x30 for ascii
CCa 0x802888e get channel nr
af+ 0x8028960 674 F_287
CCa 0x8028a14 ... M.e.n.u
CCa 0x8028a5a speaker picture
CCa 0x8028a76 2 people picture
CCa 0x8028aea get akt. Zone_Name
CCa 0x8028aea get akt. Zone_Name
CCa 0x8028b06 ... C.H.
CCa 0x8028b0e get channel nr
CCa 0x8028b1c get channel nr
CCa 0x8028b3a get channel nr
CCa 0x8028b8a 2 house picture
af+ 0x8028c20 530 F_4191
af+ 0x8028e32 28 F_4192
af+ 0x8028e58 330 F_4238
af+ 0x8028fa2 112 F_4239
CCa 0x8028fda SystemConfig
CCa 0x8028fde ConfigData + 0x0  (byte) F_4239
af+ 0x8029020 56 F_4240
af+ 0x80290c8 1854 F_4137
CCa 0x8029162 md380_menu_0x2001d3ee
CCa 0x8029168 md380_menu_0x2001d3ed
CCa 0x8029188 md380_menu_0x2001d3ef
CCa 0x8029190 md380_menu_0x2001d3ef
CCa 0x8029196 md380_menu_0x2001d3ee
CCa 0x802919e md380_menu_0x2001d3ee
CCa 0x80291a4 md380_menu_0x2001d3ef
CCa 0x80291aa mn_editbuffer_poi
CCa 0x8029242 md380_menu_0x2001d3ef
CCa 0x8029248 mn_editbuffer_poi
CCa 0x8029256 md380_menu_0x2001d3ef
CCa 0x8029312 md380_menu_0x2001d3ed
CCa 0x802931a md380_menu_0x2001d3ee
CCa 0x8029322 md380_menu_0x2001d3ef
CCa 0x8029332 mn_editbuffer_poi
CCa 0x80293a2 SystemConfig
CCa 0x80293a6 Get ConfigData + 0x2  (byte) F_4137
CCa 0x80294e8 SystemConfig
CCa 0x80294ec Get ConfigData + 0x2  (byte) F_4137
CCa 0x8029556 md380_menu_0x2001d3ee
CCa 0x802955c md380_menu_0x2001d3ed
CCa 0x802956c md380_menu_0x2001d3ef
CCa 0x8029574 md380_menu_0x2001d3ef
CCa 0x802957a md380_menu_0x2001d3ee
CCa 0x8029582 md380_menu_0x2001d3ee
CCa 0x8029588 md380_menu_0x2001d3ef
CCa 0x802958e mn_editbuffer_poi
CCa 0x80295a2 SystemConfig
CCa 0x80295a6 Get ConfigData + 0x2  (byte) F_4137
CCa 0x80295ce SystemConfig
CCa 0x80295d2 Get ConfigData + 0x2  (byte) F_4137
CCa 0x80295d2 Get ConfigData + 0x2  (byte) F_4137
CCa 0x80295d4 Check if  keypad tone enabled
CCa 0x80295f6 Send make keypad tone to beep prozess
CCa 0x80295fa char * 0x2001d345  last pressed key
CCa 0x8029600 green key
CCa 0x8029604 up key
CCa 0x8029608 down key
CCa 0x802960c red key
CCa 0x802969a md380_menu_0x2001d3ee
CCa 0x80296ae md380_menu_0x200011e4
CCa 0x8029712 md380_menu_0x200011e4
CCa 0x802974a md380_menu_0x200011e4
CCa 0x8029774 SystemConfig
CCa 0x8029778 Get ConfigData + 0x2  (byte) F_4137
af+ 0x8029810 104 F_4241
af+ 0x8029898 156 F_4242
af+ 0x8029938 120 F_4243
af+ 0x80299bc 118 F_4244
CCa 0x80299d4 SystemConfig
CCa 0x80299d6 ConfigData + 0x1  (byte) ubfx r0, r0, 2, 1 .. lsls r0, r0, 0x1f F_4244
CCa 0x80299fc SystemConfig
CCa 0x80299fe ConfigData + 0x1  (byte) F_4244
CCa 0x8029a04 SystemConfig
CCa 0x8029a06 ConfigData + 0x1  (byte) F_4244
CCa 0x8029a0a SystemConfig
CCa 0x8029a0c ConfigData + 0x1  (byte) F_4244
CCa 0x8029a12 SystemConfig
CCa 0x8029a14 ConfigData + 0x1  (byte) F_4244
af+ 0x8029a3c 138 F_4245
af+ 0x8029acc 138 F_4246
af+ 0x8029b64 134 F_4247
af+ 0x8029bf4 130 F_4248
af+ 0x8029c94 166 F_4249
af+ 0x8029d50 196 F_4250
af+ 0x8029e24 130 F_4251
af+ 0x8029ea6 174 F_4252
af+ 0x8029f54 176 F_4253
af+ 0x802a018 288 F_4254
af+ 0x802a138 122 F_4255
CCa 0x802a186 md380_menu_0x200011e4
af+ 0x802a1c0 324 F_288
CCa 0x802a314 md380_menu_0x200011e4
af+ 0x802a398 2 F_4256
af+ 0x802a39c 96 F_4095
af+ 0x802a3fc 168 F_4087
af+ 0x802a4a4 110 F_4380
af+ 0x802a68a 368 F_4381
af+ 0x802a824 88 F_4088
af+ 0x802a87c 24 F_4040
CCa 0x802a8a8 SystemConfig
af+ 0x802a8b0 48 F_289
af+ 0x802a8e0 56 F_99
af+ 0x802a918 36 F_291
CCa 0x802a91a SystemConfig
CCa 0x802a91c ConfigData + 0x1  (byte) ubfx r0, r0, 2, 1 .. lsls r0, r0, 0x1f F_291
af+ 0x802a93c 24 F_292
af+ 0x802a960 4084 Beep_Process
CCa 0x802aa5c 0x24 end of rx (Roger beep)
CCa 0x802af8a SystemConfig
CCa 0x802af8e ConfigData + 0xf  (byte) Beep_Process
CCa 0x802af98 SystemConfig
CCa 0x802af9c ConfigData + 0xf  (byte) Beep_Process
af+ 0x802b958 236 F_5027
af+ 0x802ba44 52 F_293
af+ 0x802ba78 16 bp_set_freq
af+ 0x802ba88 32 F_295
af+ 0x802baa8 80 F_296
af+ 0x802baf8 72 F_297
af+ 0x802bb40 372 Af_Mute
af+ 0x802bcb8 340 Set_Vocoder
af+ 0x802be0c 340 F_107
af+ 0x802bf76 2 F_108_Nice
af+ 0x802bfd4 394 Create_Menu_Entry_Message
CCa 0x802bfda md380_menu_0x200011e4
CCa 0x802bfe2 md380_menu_memory
CCa 0x802c000 md380_menu_id
CCa 0x802c008 md380_menu_mem_base
CCa 0x802c044 0x802ec54 Create_Menu_Entry_Inbox
CCa 0x802c056 ... I.n.b.o.x
CCa 0x802c05a md380_menu_id
CCa 0x802c076 0x802c47a Create_Menu_Entry_Write
CCa 0x802c088 ... W.r.i.t.e
CCa 0x802c08c md380_menu_id
CCa 0x802c0ac 0x802c168 Create_Menu_Entry_Quick
CCa 0x802c0be ... Q.u.i.c.k
CCa 0x802c0c2 md380_menu_id
CCa 0x802c0e2 0x8038bd8 Create_Menu_Entry_Send
CCa 0x802c0f4 ... S.e.n.t
CCa 0x802c0f8 md380_menu_id
CCa 0x802c118 0x802d1e4 Create_Menu_Entry_Draft
CCa 0x802c12a ... D.r.a.f.t.s
CCa 0x802c12e md380_menu_id
CCa 0x802c140 md380_menu_id
CCa 0x802c14a md380_menu_mem_base
af+ 0x802c168 146 Create_Menu_Entry_Quick
CCa 0x802c16a md380_menu_0x200011e4
CCa 0x802c172 md380_menu_memory
CCa 0x802c17c mn_editbuffer_poi
CCa 0x802c180 md380_menu_edit_buf
CCa 0x802c18c md380_menu_id
CCa 0x802c194 md380_menu_mem_base
CCa 0x802c1c8 mn_editbuffer_poi
CCa 0x802c1ce md380_menu_id
CCa 0x802c1dc md380_menu_id
CCa 0x802c1e6 md380_menu_mem_base
af+ 0x802c250 104 F_4382
CCa 0x802c2ec md380_menu_edit_buf
CCa 0x802c3b6 md380_menu_0x200011e4
CCa 0x802c3c8 md380_menu_0x2001d3f4
CCa 0x802c3d2 mn_editbuffer_poi
CCa 0x802c3e8 md380_menu_0x2001d3ee
CCa 0x802c3ee md380_menu_0x2001d3ef
CCa 0x802c3f4 md380_menu_0x2001d3ed
CCa 0x802c3fc md380_menu_0x2001d3f1
CCa 0x802c404 md380_menu_0x2001d3f0
CCa 0x802c40c md380_menu_0x200011e4
CCa 0x802c414 md380_menu_memory
CCa 0x802c432 md380_menu_id
CCa 0x802c43a md380_menu_mem_base
CCa 0x802c468 mn_editbuffer_poi
CCa 0x802c46e md380_menu_id
af+ 0x802c47a 228 Create_Menu_Entry_Write
CCa 0x802c480 md380_menu_0x200011e4
CCa 0x802c488 md380_menu_0x2001d3f4
CCa 0x802c490 mn_editbuffer_poi
CCa 0x802c494 md380_menu_edit_buf
CCa 0x802c4a0 mn_editbuffer_poi
CCa 0x802c4b4 md380_menu_0x2001d3ed
CCa 0x802c4bc md380_menu_0x2001d3ee
CCa 0x802c4c4 md380_menu_0x2001d3ef
CCa 0x802c4cc md380_menu_0x2001d3f1
CCa 0x802c4d4 md380_menu_0x2001d3f0
CCa 0x802c4e4 md380_menu_0x200011e4
CCa 0x802c4ec md380_menu_memory
CCa 0x802c50a md380_menu_id
CCa 0x802c512 md380_menu_mem_base
CCa 0x802c53c 0x802c55e Create_Menu_Entry_Edit
CCa 0x802c54e ... E.d.i.t.
CCa 0x802c552 md380_menu_id
af+ 0x802c55e 66 Create_Menu_Entry_Edit
CCa 0x802c566 mn_editbuffer_poi
CCa 0x802c586 md380_menu_0x2001d3ee
CCa 0x802c592 md380_menu_0x2001d3ef
af+ 0x802c5a0 192 Edit_Message_Clear
CCa 0x802c5a2 md380_menu_0x200011e4
CCa 0x802c5aa md380_menu_memory
CCa 0x802c5c8 md380_menu_id
CCa 0x802c5d0 md380_menu_mem_base
CCa 0x802c60a md380_menu_id
CCa 0x802c634 md380_menu_id
CCa 0x802c644 md380_menu_id
CCa 0x802c64c md380_menu_mem_base
af+ 0x802c660 394 Edit_Message_Menu_Entry
CCa 0x802c662 md380_menu_0x200011e4
CCa 0x802c668 md380_menu_memory
CCa 0x802c68a md380_menu_id
CCa 0x802c690 md380_menu_mem_base
CCa 0x802c6b6 0x802d108 Edit_Message_Confirm
CCa 0x802c6c4 ... Y.e.s
CCa 0x802c6c6 md380_menu_id
CCa 0x802c6de 0x800f452 ... Back
CCa 0x802c6ec ... N.o.
CCa 0x802c6ee md380_menu_id
CCa 0x802c700 md380_menu_id
CCa 0x802c708 md380_menu_mem_base
CCa 0x802c728 md380_menu_id
CCa 0x802c72e md380_menu_mem_base
CCa 0x802c754 0x802c818 Edit_Message_Confirm_Send
CCa 0x802c762 ... S.e.n.d
CCa 0x802c766 md380_menu_id
CCa 0x802c77e 0x802d108 Edit_Message_Confirm
CCa 0x802c78c ... S.a.v.e
CCa 0x802c790 md380_menu_id
CCa 0x802c7ac 0x802c5a0 Edit_Message_Clear
CCa 0x802c7ba ... C.l.e.a.r
CCa 0x802c7be md380_menu_id
CCa 0x802c7ce md380_menu_id
CCa 0x802c7d6 md380_menu_mem_base
CCa 0x802c7fa md380_menu_0x2001d3f4
af+ 0x802c818 50 Edit_Message_Confirm_Send
CCa 0x802c81a md380_menu_0x200011e4
CCa 0x802c822 md380_menu_memory
CCa 0x802c82a md380_menu_memory
af+ 0x802c84a 176 Edit_Message_Confirm_Send_2
CCa 0x802c84c md380_menu_0x200011e4
CCa 0x802c852 md380_menu_memory
CCa 0x802c86a md380_menu_id
CCa 0x802c870 md380_menu_mem_base
CCa 0x802c896 0x802c8fa Edit_Message_Confirm_Send_Contacts
CCa 0x802c8a4 .. C.o.n.t.a.c.t.s
CCa 0x802c8a6 md380_menu_id
CCa 0x802c8be 0x802c99c Edit_Message_Confirm_Send_Manual
CCa 0x802c8cc .. M.a.n.u.a.l
CCa 0x802c8ce md380_menu_id
CCa 0x802c8de md380_menu_id
CCa 0x802c8e6 md380_menu_mem_base
af+ 0x802c8fa 126  Edit_Message_Confirm_Send_Contacts
CCa 0x802c8fe md380_menu_0x200011e4
CCa 0x802c904 md380_menu_memory
CCa 0x802c912 md380_menu_id
CCa 0x802c918 md380_menu_mem_base
CCa 0x802c94a Ponter to contact list
CCa 0x802c94e md380_menu_id
CCa 0x802c95a md380_menu_id
CCa 0x802c962 md380_menu_mem_base
af+ 0x802c99c 204 Edit_Message_Confirm_Send_Manual
CCa 0x802c99e md380_menu_0x2001d3f4
CCa 0x802c9a6 mn_editbuffer_poi
CCa 0x802c9a8 md380_menu_edit_buf
CCa 0x802c9b2 mn_editbuffer_poi
CCa 0x802c9c4 md380_menu_0x2001d3ed
CCa 0x802c9cc md380_menu_0x2001d3ee
CCa 0x802c9d4 md380_menu_0x2001d3ef
CCa 0x802c9dc md380_menu_0x2001d3f1
CCa 0x802c9e4 md380_menu_0x2001d3f0
CCa 0x802c9ec md380_menu_0x200011e4
CCa 0x802c9f4 md380_menu_memory
CCa 0x802ca28 md380_menu_id
CCa 0x802ca30 md380_menu_mem_base
CCa 0x802ca54 0x802ca88 Edit_Message_Confirm_Send_3
CCa 0x802ca58 mn_editbuffer_poi
CCa 0x802ca58 .. Pointer to Message 
CCa 0x802ca5c md380_menu_id
af+ 0x802ca88 302 Edit_Message_Confirm_Send_3
CCa 0x802ca8c md380_menu_0x200011e4
CCa 0x802ca94 md380_menu_0x200011e4
CCa 0x802caa0 md380_menu_0x200011e4
CCa 0x802caaa md380_menu_id
CCa 0x802cab0 mn_editbuffer_poi
CCa 0x802cae8 mn_editbuffer_poi
CCa 0x802caf8 mn_editbuffer_poi
CCa 0x802cb08 mn_editbuffer_poi
CCa 0x802cb58 mn_editbuffer_poi
CCa 0x802cb78 mn_editbuffer_poi
CCa 0x802cb92 md380_menu_0x2001d3ee
CCa 0x802cb9a md380_menu_0x2001d3ef
CCa 0x802cba2 md380_menu_0x2001d3ed
CCa 0x802cba8 md380_menu_0x2001d3f0
af+ 0x802cbbc 114 Edit_Message_Confirm_Send_5
CCa 0x802cbbe md380_menu_0x200011e4
CCa 0x802cbc6 md380_menu_memory
CCa 0x802cbe4 md380_menu_id
CCa 0x802cbec md380_menu_mem_base
CCa 0x802cc0e 0x800f452 ... Back
CCa 0x802cc1e ... N.u.m.b.e.r
CCa 0x802cc22 md380_menu_id
CCa 0x802cc3c md380_menu_0x2001d3f4
CCa 0x802cd7a md380_menu_0x200011e4
CCa 0x802cd82 md380_menu_0x200011e4
CCa 0x802cd88 md380_menu_0x200011e4
CCa 0x802cd92 md380_menu_id
CCa 0x802cdbc md380_menu_mem_base
af+ 0x802cdc8 194 Edit_Message_Confirm_Send_1
CCa 0x802ce1c md380_menu_0x2001d3f4
CCa 0x802ce24 md380_menu_0x200011e4
CCa 0x802ce2c md380_menu_memory
CCa 0x802ce4a md380_menu_id
CCa 0x802ce52 md380_menu_mem_base
CCa 0x802ce7a .. NULLTEXT
CCa 0x802ce7e md380_menu_id
CCa 0x802ceb8 md380_menu_0x200011e4
CCa 0x802cec0 md380_menu_0x200011e4
CCa 0x802cec6 md380_menu_0x200011e4
CCa 0x802ced0 md380_menu_id
CCa 0x802ced6 md380_menu_0x200011e4
CCa 0x802cede md380_menu_memory
CCa 0x802ceea md380_menu_id
CCa 0x802cef2 md380_menu_mem_base
CCa 0x802cf14 md380_menu_memory
CCa 0x802cf32 md380_menu_0x200011e4
CCa 0x802cf3a md380_menu_0x200011e4
CCa 0x802cf4c md380_menu_memory
CCa 0x802cf68 md380_menu_0x200011e4
CCa 0x802cf70 md380_menu_memory
CCa 0x802cf7a md380_menu_0x200011e4
CCa 0x802cf80 md380_menu_0x2001d3ee
CCa 0x802cf8c md380_menu_0x2001d3ef
CCa 0x802cf98 md380_menu_0x2001d3ed
CCa 0x802cfa0 md380_menu_0x2001d3f0
CCa 0x802cfaa md380_menu_memory
CCa 0x802cfb4 md380_menu_0x200011e4
CCa 0x802cfbc md380_menu_0x2001d3ee
CCa 0x802cfc8 md380_menu_0x2001d3ef
CCa 0x802cfd4 md380_menu_0x2001d3ed
CCa 0x802cfdc md380_menu_0x2001d3f0
CCa 0x802cfe6 md380_menu_memory
CCa 0x802cff0 md380_menu_0x200011e4
CCa 0x802cff8 md380_menu_0x2001d3ee
CCa 0x802d004 md380_menu_0x2001d3ef
CCa 0x802d010 md380_menu_0x2001d3ed
CCa 0x802d018 md380_menu_0x2001d3f0
CCa 0x802d026 md380_menu_memory
CCa 0x802d05a md380_menu_0x200011e4
CCa 0x802d068 mn_editbuffer_poi
CCa 0x802d0ba md380_menu_id
CCa 0x802d0f8 md380_menu_id
af+ 0x802d108 204 Edit_Message_Confirm
CCa 0x802d10c md380_menu_memory
CCa 0x802d11e md380_menu_memory
CCa 0x802d128 md380_menu_0x2001d3f4
CCa 0x802d130 md380_menu_memory
CCa 0x802d14a md380_menu_edit_buf
CCa 0x802d152 md380_menu_0x200011e4
CCa 0x802d158 md380_menu_memory
CCa 0x802d16a md380_menu_0x200011e4
CCa 0x802d170 md380_menu_memory
CCa 0x802d188 md380_menu_id
CCa 0x802d190 md380_menu_mem_base
CCa 0x802d1c4 ... D.r.a.f.t.s
CCa 0x802d1c8 md380_menu_id
af+ 0x802d1e4 408 Create_Menu_Entry_Draft
CCa 0x802d1e8 md380_menu_0x200011e4
CCa 0x802d1ee md380_menu_memory
CCa 0x802d1f8 mn_editbuffer_poi
CCa 0x802d1fa md380_menu_edit_buf
CCa 0x802d21e md380_menu_id
CCa 0x802d226 md380_menu_mem_base
CCa 0x802d25e md380_menu_id
CCa 0x802d284 md380_menu_id
CCa 0x802d28c md380_menu_mem_base
CCa 0x802d2c4 md380_menu_id
CCa 0x802d2e0 md380_menu_id
CCa 0x802d2e8 md380_menu_mem_base
CCa 0x802d320 md380_menu_id
CCa 0x802d34c md380_menu_id
CCa 0x802d35e md380_menu_id
CCa 0x802d368 md380_menu_mem_base
CCa 0x802d38e md380_menu_0x200011e4
CCa 0x802d394 md380_menu_memory
CCa 0x802d3a2 md380_menu_id
CCa 0x802d3aa md380_menu_mem_base
CCa 0x802d3da mn_editbuffer_poi
CCa 0x802d3de md380_menu_id
CCa 0x802d3ec md380_menu_id
CCa 0x802d3f6 md380_menu_mem_base
CCa 0x802d42c md380_menu_0x200011e4
CCa 0x802d434 md380_menu_0x200011e4
CCa 0x802d43a md380_menu_0x200011e4
CCa 0x802d444 md380_menu_id
CCa 0x802d448 md380_menu_0x200011e4
CCa 0x802d450 md380_menu_memory
CCa 0x802d45c mn_editbuffer_poi
CCa 0x802d45e md380_menu_edit_buf
CCa 0x802d478 md380_menu_id
CCa 0x802d47e md380_menu_mem_base
CCa 0x802d4ba md380_menu_id
CCa 0x802d4e8 md380_menu_id
CCa 0x802d4f8 md380_menu_id
CCa 0x802d500 md380_menu_mem_base
af+ 0x802d524 166 Edit_Message_Confirm_2
af+ 0x802d5d0 92 F_4383
af+ 0x802d62c 162 Edit_Message_Confirm_3
CCa 0x802d6bc md380_menu_edit_buf
af+ 0x802d714 212 Edit_Message_Confirm_1
CCa 0x802d83e md380_menu_edit_buf
CCa 0x802d958 md380_menu_0x200011e4
CCa 0x802d960 md380_menu_0x2001d3f4
CCa 0x802d96a mn_editbuffer_poi
CCa 0x802d980 md380_menu_0x2001d3ee
CCa 0x802d984 md380_menu_0x2001d3ef
CCa 0x802d98a md380_menu_0x2001d3ed
CCa 0x802d990 md380_menu_0x2001d3f1
CCa 0x802d998 md380_menu_0x2001d3f0
CCa 0x802d99e md380_menu_0x200011e4
CCa 0x802d9a6 md380_menu_memory
CCa 0x802d9b6 md380_menu_id
CCa 0x802d9be md380_menu_mem_base
CCa 0x802d9ec mn_editbuffer_poi
CCa 0x802d9f2 md380_menu_id
CCa 0x802da10 mn_editbuffer_poi
CCa 0x802da54 md380_menu_0x2001d3ee
CCa 0x802da5a md380_menu_0x200011e4
CCa 0x802da62 md380_menu_memory
CCa 0x802da6c mn_editbuffer_poi
CCa 0x802da70 md380_menu_edit_buf
CCa 0x802da88 md380_menu_id
CCa 0x802da8e md380_menu_mem_base
CCa 0x802dace md380_menu_id
CCa 0x802dafc md380_menu_id
CCa 0x802db2e md380_menu_id
CCa 0x802db3e md380_menu_id
CCa 0x802db46 md380_menu_mem_base
CCa 0x802db6a md380_menu_0x200011e4
CCa 0x802db70 md380_menu_memory
CCa 0x802db88 md380_menu_id
CCa 0x802db8e md380_menu_mem_base
CCa 0x802dbc4 md380_menu_id
CCa 0x802dbec md380_menu_id
CCa 0x802dbfc md380_menu_id
CCa 0x802dc04 md380_menu_mem_base
CCa 0x802dc1e md380_menu_0x200011e4
CCa 0x802dc24 md380_menu_memory
CCa 0x802dc3c md380_menu_id
CCa 0x802dc44 md380_menu_mem_base
CCa 0x802dc7c md380_menu_id
CCa 0x802dc96 md380_menu_0x200011e4
CCa 0x802dc9c md380_menu_memory
CCa 0x802dcb4 md380_menu_id
CCa 0x802dcbc md380_menu_mem_base
CCa 0x802dcf2 md380_menu_id
CCa 0x802dd1c md380_menu_id
CCa 0x802dd2e md380_menu_id
CCa 0x802dd38 md380_menu_mem_base
CCa 0x802dd72 md380_menu_0x200011e4
CCa 0x802dd7c md380_menu_memory
CCa 0x802dd98 md380_menu_0x200011e4
CCa 0x802dd9e md380_menu_memory
CCa 0x802ddb6 md380_menu_id
CCa 0x802ddbe md380_menu_mem_base
CCa 0x802ddf6 md380_menu_id
CCa 0x802de46 md380_menu_0x2001d3ef
CCa 0x802de7c md380_menu_0x2001d3ef
CCa 0x802de8a md380_menu_edit_buf
CCa 0x802de92 md380_menu_0x2001d3f4
CCa 0x802de9a mn_editbuffer_poi
CCa 0x802deae md380_menu_0x2001d3ee
CCa 0x802deb4 md380_menu_0x2001d3ef
CCa 0x802debc md380_menu_0x2001d3ed
CCa 0x802dec4 md380_menu_0x2001d3f1
af+ 0x802dee4 428 F_4314
CCa 0x802df52 md380_menu_0x200011e4
CCa 0x802df5a md380_menu_id
CCa 0x802df62 md380_menu_0x200011e4
CCa 0x802df6a md380_menu_memory
CCa 0x802df8a md380_menu_mem_base
CCa 0x802dfda md380_menu_id
CCa 0x802e00a md380_menu_id
CCa 0x802e03e md380_menu_id
CCa 0x802e04c md380_menu_0x200011e4
CCa 0x802e056 md380_menu_id
CCa 0x802e068 md380_menu_0x200011e4
CCa 0x802e076 md380_menu_mem_base
CCa 0x802e09e md380_menu_0x200011e4
CCa 0x802e0a6 md380_menu_memory
CCa 0x802e0c4 md380_menu_id
CCa 0x802e0cc md380_menu_mem_base
CCa 0x802e104 md380_menu_id
CCa 0x802e132 md380_menu_id
CCa 0x802e144 md380_menu_id
CCa 0x802e14e md380_menu_mem_base
CCa 0x802e162 md380_menu_0x200011e4
CCa 0x802e16a md380_menu_0x200011e4
CCa 0x802e170 md380_menu_0x200011e4
CCa 0x802e178 md380_menu_memory
CCa 0x802e196 md380_menu_id
CCa 0x802e19e md380_menu_mem_base
CCa 0x802e1f8 md380_menu_id
af+ 0x802e240 888 F_4315
CCa 0x802e442 check for private call
CCa 0x802e452 singel person picture
CCa 0x802e4a0 2 people picture
CCa 0x802e4b0 sprite incomming telephone
CCa 0x802e516 2 people picture
CCa 0x802e566 outgoing telefone
CCa 0x802e582 sprite incomming telephone
af+ 0x802e5b8 24 F_4316
CCa 0x802e5f0 mn_editbuffer_poi
CCa 0x802e5f4 md380_menu_edit_buf
CCa 0x802e5fa md380_menu_0x2001d3f4
CCa 0x802e638 mn_editbuffer_poi
CCa 0x802e64e md380_menu_0x2001d3ee
CCa 0x802e652 md380_menu_0x2001d3ef
CCa 0x802e658 md380_menu_0x2001d3ed
CCa 0x802e660 md380_menu_0x2001d3f1
CCa 0x802e668 md380_menu_0x2001d3f0
CCa 0x802e670 md380_menu_0x200011e4
CCa 0x802e678 md380_menu_memory
CCa 0x802e688 md380_menu_id
CCa 0x802e68e md380_menu_mem_base
CCa 0x802e6bc mn_editbuffer_poi
CCa 0x802e6c2 md380_menu_id
CCa 0x802e6f6 md380_menu_0x2001d3ef
CCa 0x802e71e md380_menu_0x2001d3ef
CCa 0x802e724 md380_menu_0x2001d3f4
CCa 0x802e72c md380_menu_0x2001d3ef
CCa 0x802e732 md380_menu_0x2001d3ed
CCa 0x802e738 md380_menu_0x2001d3f1
af+ 0x802e74c 184 F_4384
af+ 0x802e80c 184 F_4443
af+ 0x802e8d8 166 F_1001
af+ 0x802e988 92 F_4385
af+ 0x802e9f0 54 F_824
af+ 0x802ea44 202 F_4386
af+ 0x802eb68 212 F_4444
af+ 0x802ec54 504 Create_Menu_Entry_Inbox
CCa 0x802ec58 md380_menu_0x200011e4
CCa 0x802ec60 md380_menu_memory
CCa 0x802ec6c mn_editbuffer_poi
CCa 0x802ec70 md380_menu_edit_buf
CCa 0x802ec98 md380_menu_id
CCa 0x802eca0 md380_menu_mem_base
CCa 0x802ecdc md380_menu_id
CCa 0x802ecee md380_menu_id
CCa 0x802ecf8 md380_menu_mem_base
CCa 0x802ed26 md380_menu_id
CCa 0x802ed2e md380_menu_mem_base
CCa 0x802ed6a md380_menu_id
CCa 0x802ed7a md380_menu_id
CCa 0x802ed84 md380_menu_mem_base
CCa 0x802eda8 md380_menu_id
CCa 0x802edb0 md380_menu_mem_base
CCa 0x802edec md380_menu_id
CCa 0x802ee1c md380_menu_id
CCa 0x802ee2e md380_menu_id
CCa 0x802ee38 md380_menu_mem_base
CCa 0x802ee5e md380_menu_0x200011e4
CCa 0x802ee66 md380_menu_memory
CCa 0x802ee74 md380_menu_id
CCa 0x802ee7c md380_menu_mem_base
CCa 0x802eeac mn_editbuffer_poi
CCa 0x802eeb0 md380_menu_id
CCa 0x802eebe md380_menu_id
CCa 0x802eec8 md380_menu_mem_base
CCa 0x802eef2 md380_menu_0x200011e4
CCa 0x802eefa md380_menu_0x200011e4
CCa 0x802ef00 md380_menu_0x200011e4
CCa 0x802ef0a md380_menu_id
CCa 0x802ef10 md380_menu_0x200011e4
CCa 0x802ef18 md380_menu_memory
CCa 0x802ef22 mn_editbuffer_poi
CCa 0x802ef24 md380_menu_edit_buf
CCa 0x802ef3e md380_menu_id
CCa 0x802ef46 md380_menu_mem_base
CCa 0x802ef82 md380_menu_id
CCa 0x802efb2 md380_menu_id
CCa 0x802efc4 md380_menu_id
CCa 0x802efce md380_menu_mem_base
CCa 0x802f1c2 md380_menu_0x2001d3f4
CCa 0x802f200 mn_editbuffer_poi
CCa 0x802f216 md380_menu_0x2001d3ee
CCa 0x802f21c md380_menu_0x2001d3ef
CCa 0x802f224 md380_menu_0x2001d3ed
CCa 0x802f22c md380_menu_0x2001d3f1
CCa 0x802f234 md380_menu_0x2001d3f0
CCa 0x802f23c md380_menu_0x200011e4
CCa 0x802f244 md380_menu_memory
CCa 0x802f254 md380_menu_id
CCa 0x802f25c md380_menu_mem_base
CCa 0x802f28a mn_editbuffer_poi
CCa 0x802f290 md380_menu_id
CCa 0x802f2a4 mn_editbuffer_poi
CCa 0x802f2c2 mn_editbuffer_poi
CCa 0x802f2de md380_menu_0x200011e4
CCa 0x802f2e6 md380_menu_memory
CCa 0x802f304 md380_menu_id
CCa 0x802f30c md380_menu_mem_base
CCa 0x802f340 md380_menu_id
CCa 0x802f364 md380_menu_id
CCa 0x802f374 md380_menu_id
CCa 0x802f37c md380_menu_mem_base
CCa 0x802f396 md380_menu_0x200011e4
CCa 0x802f39c md380_menu_memory
CCa 0x802f3b4 md380_menu_id
CCa 0x802f3bc md380_menu_mem_base
CCa 0x802f3f0 md380_menu_id
CCa 0x802f402 md380_menu_0x200011e4
CCa 0x802f408 md380_menu_memory
CCa 0x802f410 mn_editbuffer_poi
CCa 0x802f414 md380_menu_edit_buf
CCa 0x802f418 mn_editbuffer_poi
CCa 0x802f420 md380_menu_id
CCa 0x802f426 md380_menu_mem_base
CCa 0x802f462 md380_menu_id
CCa 0x802f48c md380_menu_id
CCa 0x802f4ba md380_menu_id
CCa 0x802f4ca md380_menu_id
CCa 0x802f4d2 md380_menu_mem_base
CCa 0x802f52a md380_menu_0x2001d3ef
CCa 0x802f560 md380_menu_0x2001d3ef
CCa 0x802f57c md380_menu_0x2001d3f4
CCa 0x802f586 mn_editbuffer_poi
CCa 0x802f59c md380_menu_0x2001d3ee
CCa 0x802f5a2 md380_menu_0x2001d3ef
CCa 0x802f5aa md380_menu_0x2001d3ed
CCa 0x802f5b2 md380_menu_0x2001d3f1
CCa 0x802f5c2 md380_menu_0x200011e4
CCa 0x802f5c8 md380_menu_memory
CCa 0x802f5e0 md380_menu_id
CCa 0x802f5e6 md380_menu_mem_base
CCa 0x802f61a md380_menu_id
CCa 0x802f63e md380_menu_id
CCa 0x802f64e md380_menu_id
CCa 0x802f656 md380_menu_mem_base
CCa 0x802f6a8 md380_menu_0x200011e4
CCa 0x802f6ba md380_menu_memory
CCa 0x802f6d8 md380_menu_0x200011e4
CCa 0x802f6e0 md380_menu_memory
CCa 0x802f6fe md380_menu_id
CCa 0x802f706 md380_menu_mem_base
CCa 0x802f742 md380_menu_id
CCa 0x802f75a md380_menu_0x200011e4
CCa 0x802f762 md380_menu_memory
CCa 0x802f794 md380_menu_id
CCa 0x802f79a md380_menu_mem_base
CCa 0x802f7cc md380_menu_id
CCa 0x802f7f2 md380_menu_id
CCa 0x802f802 md380_menu_id
CCa 0x802f80a md380_menu_mem_base
CCa 0x802f820 md380_menu_0x200011e4
CCa 0x802f83e md380_menu_0x200011e4
CCa 0x802f846 md380_menu_0x200011e4
CCa 0x802f84e md380_menu_id
CCa 0x802f852 md380_menu_0x200011e4
CCa 0x802f858 md380_menu_memory
CCa 0x802f866 mn_editbuffer_poi
CCa 0x802f87e mn_editbuffer_poi
CCa 0x802f884 md380_menu_id
CCa 0x802f88a md380_menu_mem_base
CCa 0x802f8bc md380_menu_id
CCa 0x802f8e2 md380_menu_id
CCa 0x802f90c md380_menu_id
CCa 0x802f922 md380_menu_id
CCa 0x802f92a md380_menu_mem_base
CCa 0x802f93e md380_menu_0x200011e4
CCa 0x802f944 md380_menu_0x200011e4
CCa 0x802f948 md380_menu_0x200011e4
CCa 0x802f950 md380_menu_id
CCa 0x802f95c md380_menu_0x200011e4
CCa 0x802f962 md380_menu_0x200011e4
CCa 0x802f966 md380_menu_0x200011e4
CCa 0x802f96e md380_menu_id
CCa 0x802f980 md380_menu_0x200011e4
CCa 0x802f986 md380_menu_0x200011e4
CCa 0x802f98a md380_menu_0x200011e4
CCa 0x802f992 md380_menu_id
af+ 0x802f9bc 172 F_4138
CCa 0x802f9be md380_menu_0x200011e4
CCa 0x802f9c4 md380_menu_id
CCa 0x802f9ca md380_menu_0x2001d3f4
CCa 0x802f9d0 mn_editbuffer_poi
CCa 0x802f9d2 md380_menu_edit_buf
CCa 0x802f9d6 md380_menu_0x2001d3f1
CCa 0x802f9dc md380_menu_0x2001d3f0
CCa 0x802f9e2 md380_menu_0x200011e4
CCa 0x802f9e8 md380_menu_memory
CCa 0x802fa08 md380_menu_mem_base
CCa 0x802fa3e mn_editbuffer_poi
CCa 0x802fa42 md380_menu_id
CCa 0x802fa4a md380_menu_0x200011e4
CCa 0x802fa52 md380_menu_id
CCa 0x802fa60 md380_menu_0x200011e4
CCa 0x802fa90 md380_menu_0x2001d3f4
af+ 0x802fb00 130 F_300
af+ 0x802fb82 52 spiflash_sektor_erase4k
af+ 0x802fbb6 52 spiflash_block_erase64k
af+ 0x802fbea 76 spiflash_program_page
CCa 0x802fbfa Page Program CMD (02h)
af+ 0x802fc36 332 F_1069_spiflash_multiple_spiflash_program_page
af+ 0x802fd82 70 spiflash_read
CCa 0x802fd8e Read Data CMD 03h
CCa 0x802fdae dummy_value
af+ 0x802fdc8 58 spi1_sendrecv
af+ 0x802fe02 18 spiflash_write_enable
af+ 0x802fe14 34 spiflash_wait_for_read_status_register
af+ 0x802fe36 28 enable_spi_flash_and_sem
CCa 0x802fe48 enable Flash_CS0
af+ 0x802fe52 24 disable_spi_flash_and_sem
CCa 0x802fe56 disable Flash_CS0
af+ 0x802fe6a 704 spiflash_write
CCa 0x802fe6a same_parameter_as_spiflash_read
af+ 0x803013c 52 spiflash_Erase_Security_Registers_44h
af+ 0x8030170 76 spiflash_Program_Security_Registers_42h
af+ 0x80301bc 78 spiflash_Read_Security_Registers_48h
af+ 0x803020c 66 F_4193
af+ 0x8030250 76 F_310
CCa 0x8030262 Call OS_ENTER_CRITICAL()
CCa 0x803027c Call OS_EXIT_CRITICAL()
af+ 0x803029c 258 OSMboxPend
CCa 0x80302d6 Call OS_ENTER_CRITICAL()
CCa 0x80302ec Call OS_EXIT_CRITICAL()
CCa 0x8030324 Call OS_EXIT_CRITICAL()
CCa 0x803032c Call OS_ENTER_CRITICAL()
CCa 0x8030394 Call OS_EXIT_CRITICAL()
af+ 0x80303b4 86 OSMboxPost
CCa 0x80303c6 Call OS_ENTER_CRITICAL()
CCa 0x80303e2 Call OS_EXIT_CRITICAL()
CCa 0x80303f6 Call OS_EXIT_CRITICAL()
CCa 0x8030402 Call OS_EXIT_CRITICAL()
af+ 0x803040c 26 F_4497
af+ 0x8030426 478 F_4492
CCa 0x8030572 ... nice wtf
af+ 0x8030604 104 F_4493
af+ 0x803066c 42 F_4491
af+ 0x8030696 118 F_4490
af+ 0x803070c 44 F_4489
af+ 0x8030738 248 F_4445
af+ 0x8030830 36 F_4511
af+ 0x8030854 14 F_4498
af+ 0x8030862 12 F_4499
af+ 0x803086e 72 F_4485
af+ 0x80308d8 38 F_4494
CCa 0x80308da SystemConfig
CCa 0x80308de ConfigData + 0x1  (byte) ubfx r0, r0, 1, 1 .. lsls r0, r0, 0x1f F_4494
af+ 0x80308fe 142 F_33
af+ 0x803098c 198 F_314
af+ 0x8030a52 284 F_4041
af+ 0x8030b6e 50 F_110
af+ 0x8030ba0 184 F_316
af+ 0x8030c58 70 F_317
af+ 0x8030c9e 80 F_08
af+ 0x8030cee 68 F_09
af+ 0x8030d32 260 F_4500
CCa 0x8030d56 SystemConfig
CCa 0x8030d5a ConfigData + 0xe  (byte) F_4500
CCa 0x8030d66 SystemConfig
CCa 0x8030d6a ConfigData + 0xe  (byte) F_4500
af+ 0x8030e36 40 F_320
af+ 0x8030e5e 92 F_26
af+ 0x8030eba 92 F_322
CCa 0x8030eca GPIO C2 Reset MODER2_0 Input
CCa 0x8030ed4 GPIO C4 Reset MODER4_1 Input
CCa 0x8030ee0 GPIO A4 Reset MODER4_1 Input
CCa 0x8030eec GPIO A4 Reset MODER4_0 Input
af+ 0x8030f16 102 F_323
af+ 0x8030f7c 328 F_4501
CCa 0x8030fa8 SystemConfig
CCa 0x8030faa ConfigData + 0x10 (byte) F_4501
CCa 0x8031018 SystemConfig
CCa 0x803101a ConfigData + 0x11 (byte) F_4501
CCa 0x8031020 SystemConfig
CCa 0x8031022 ConfigData + 0x10 (byte) F_4501
af+ 0x803114c 10 F_1152
af+ 0x8031156 16 F_4486
af+ 0x8031166 152 F_1070
af+ 0x80311fe 38 F_1153
af+ 0x8031224 100 F_1154
af+ 0x8031288 170 F_1155
af+ 0x8031332 16 F_1156
af+ 0x8031342 276 F_1138
af+ 0x8031456 62 F_1157
af+ 0x8031494 22 F_1115
af+ 0x80314aa 24 F_825
af+ 0x80314c2 98 F_4459
af+ 0x8031538 20 F_4475
af+ 0x803154c 16 F_1003
af+ 0x803155c 18 F_4194
af+ 0x803156e 136 F_1071
af+ 0x80315f6 126 F_1116
af+ 0x8031674 94 F_826
af+ 0x8031694 62 F_67
af+ 0x80316ec 26 F_1072
CCa 0x80316f8 BLX F_1021 ...wtf 
af+ 0x8031710 66 F_827
af+ 0x8031752 30 F_1004
af+ 0x8031770 56 F_1073
af+ 0x80317a8 44 F_1099
af+ 0x80317d4 78 F_1100
af+ 0x8031822 18 F_1005
af+ 0x8031834 16 F_828
af+ 0x8031844 20 F_325
af+ 0x8031858 20 F_326
af+ 0x803186c 56 F_829
af+ 0x80318a4 90 F_4487
af+ 0x80318fe 70 F_5012
af+ 0x8031944 84 F_830
CCa 0x8031994 BLX F_1095 ...wtf
af+ 0x8031998 540 F_831
af+ 0x8031bb4 14 F_327
af+ 0x8031bc2 54 F_832
af+ 0x8031bf8 12 F_328
CCa 0x8031c00 BLX F_5049 ... wtf
af+ 0x8031c04 12 F_5068
af+ 0x8031c10 24 F_1006
af+ 0x8031c28 24 F_1007
af+ 0x8031c4c 54 F_4460
af+ 0x8031c8c 8 F_4387
af+ 0x8031c94 284 F_4317
af+ 0x8031db0 12 F_4257
af+ 0x8031dbc 14 F_4258
af+ 0x8031dd0 14 F_4195
af+ 0x8031dde 54 F_4196
af+ 0x8031e20 80 F_833
af+ 0x8031e70 110 F_329
af+ 0x8031ede 28 F_111
af+ 0x8031efa 32 F_331
af+ 0x8031f40 36 F_1074
af+ 0x8031f64 12 F_1008
af+ 0x8031f74 8 F_834_Write_Command_2display
af+ 0x8031f7c 6 F_835_Write_Data_2display
af+ 0x8031f82 12 F_1139_to_display
af+ 0x8031f8e 20 F_1101
CCa 0x8031f90 Set GPIO D8 to Alternate function (Flash CS1..wtf)
CCa 0x8031f9a 720895
af+ 0x8031fb0 20 F_1102_delay
af+ 0x8031fc4 392 F_1075_display_init
af+ 0x803214c 72 F_332
af+ 0x8032194 68 F_1117
af+ 0x80321d8 218 F_836
af+ 0x80322b2 216 F_837
af+ 0x803238a 258 F_4388
af+ 0x8032494 26 F_4318
af+ 0x80324ae 246 F_5069
af+ 0x80325d4 40 F_4259
af+ 0x8032620 6 F_838
af+ 0x8032628 136 F_3000
af+ 0x80326b0 16 F_1010
af+ 0x80326c4 26 F_1011
af+ 0x80326de 54 F_1148
CCa 0x80326fe BLX F_5069 ...wtf
CCa 0x8032700 0 value in my test case
af+ 0x8032714 66 F_1149
af+ 0x8032756 16 F_1012
af+ 0x8032766 14 F_1150
CCa 0x8032770 BLX F_5074 ...wtf
af+ 0x803277c 8 F_840
af+ 0x8032784 8 F_841
af+ 0x8032790 24 F_333
af+ 0x80327a8 90 F_4260
af+ 0x8032802 52 F_4319
af+ 0x8032838 52 F_112
af+ 0x803286c 320 F_1013_Something_with_display_ant_signal_strength
CCa 0x80328dc Ant symbol
CCa 0x803292a Ant symbol
CCa 0x8032950 clear ant_signal
CCa 0x8032972 Ant symbol
af+ 0x80329c0 104 OSTimeDly
CCa 0x80329da Call OS_ENTER_CRITICAL()
CCa 0x8032a1e Call OS_EXIT_CRITICAL()
af+ 0x8032a3c 146 licence_check
CCa 0x8032a5e 2 PWR_FLAG_SB: StandBy flag This flag indicates that the system was resumed from StandBy mode
af+ 0x8032ace 28 F_337
af+ 0x8032aea 10 F_114
af+ 0x8032af4 24 F_115
af+ 0x8032b0c 28 F_116
af+ 0x8032b28 82 F_4320
af+ 0x8032b7a 8 F_4321
af+ 0x8032b82 36 F_117
af+ 0x8032ba6 36 F_118
af+ 0x8032bf0 62 F_4322
af+ 0x8032c2e 18 F_343
af+ 0x8032c90 50 F_120
af+ 0x8032cc2 18 F_4042
CCa 0x8032d02 md380_menu_0x200011e4
CCa 0x8032d0a md380_menu_memory
CCa 0x8032d26 md380_menu_id
CCa 0x8032d2e md380_menu_mem_base
CCa 0x8032d94 md380_menu_id
CCa 0x8032dee md380_menu_id
CCa 0x8032e4c md380_menu_id
CCa 0x8032e84 md380_menu_id
CCa 0x8032e96 md380_menu_id
CCa 0x8032ea0 md380_menu_mem_base
CCa 0x8032ec4 md380_menu_0x200011e4
CCa 0x8032ecc md380_menu_memory
CCa 0x8032efa md380_menu_id
CCa 0x8032f02 md380_menu_mem_base
CCa 0x8032f3e md380_menu_id
CCa 0x8032f5e md380_menu_id
CCa 0x8032f66 md380_menu_mem_base
CCa 0x8032fa2 md380_menu_id
CCa 0x8032fd2 md380_menu_id
CCa 0x8032fe4 md380_menu_id
CCa 0x8032fee md380_menu_mem_base
CCa 0x8033008 md380_menu_0x200011e4
CCa 0x8033010 md380_menu_memory
CCa 0x8033026 md380_menu_id
CCa 0x803302e md380_menu_mem_base
CCa 0x8033062 md380_menu_id
CCa 0x8033070 md380_menu_id
CCa 0x803307a md380_menu_mem_base
CCa 0x8033094 md380_menu_0x200011e4
CCa 0x803309c md380_menu_memory
CCa 0x80330ca md380_menu_id
CCa 0x80330d2 md380_menu_mem_base
CCa 0x803310e md380_menu_id
CCa 0x803312e md380_menu_id
CCa 0x8033136 md380_menu_mem_base
CCa 0x8033172 md380_menu_id
CCa 0x80331a2 md380_menu_id
CCa 0x80331b4 md380_menu_id
CCa 0x80331be md380_menu_mem_base
CCa 0x80331d8 md380_menu_0x200011e4
CCa 0x80331e0 md380_menu_memory
CCa 0x80331f6 md380_menu_id
CCa 0x80331fe md380_menu_mem_base
CCa 0x8033248 md380_menu_id
CCa 0x8033256 md380_menu_id
CCa 0x8033260 md380_menu_mem_base
CCa 0x803327c md380_menu_0x200011e4
CCa 0x8033284 md380_menu_memory
CCa 0x8033290 mn_editbuffer_poi
CCa 0x8033294 md380_menu_edit_buf
CCa 0x80332a0 md380_menu_id
CCa 0x80332a8 md380_menu_mem_base
CCa 0x80332e4 md380_menu_id
CCa 0x8033314 md380_menu_id
CCa 0x8033326 md380_menu_id
CCa 0x8033330 md380_menu_mem_base
CCa 0x803334c md380_menu_0x200011e4
CCa 0x8033354 md380_menu_memory
CCa 0x8033360 mn_editbuffer_poi
CCa 0x8033364 md380_menu_edit_buf
CCa 0x8033370 md380_menu_id
CCa 0x8033378 md380_menu_mem_base
CCa 0x80333b4 md380_menu_id
CCa 0x80333e4 md380_menu_id
CCa 0x80333f6 md380_menu_id
CCa 0x80333fe md380_menu_mem_base
CCa 0x8033528 md380_menu_0x200011e4
CCa 0x803352e md380_menu_memory
CCa 0x803355c md380_menu_id
CCa 0x8033562 md380_menu_mem_base
CCa 0x8033598 md380_menu_id
CCa 0x80335c0 md380_menu_id
CCa 0x80335d0 md380_menu_id
CCa 0x80335d8 md380_menu_mem_base
CCa 0x80335f4 md380_menu_0x200011e4
CCa 0x80335fa md380_menu_memory
CCa 0x8033628 md380_menu_id
CCa 0x803362e md380_menu_mem_base
CCa 0x8033662 md380_menu_id
CCa 0x8033686 md380_menu_id
CCa 0x8033696 md380_menu_id
CCa 0x803369e md380_menu_mem_base
CCa 0x80336ba md380_menu_0x200011e4
CCa 0x80336c0 md380_menu_0x2001d3f4
CCa 0x80336c8 mn_editbuffer_poi
CCa 0x80336cc md380_menu_edit_buf
CCa 0x80336d8 mn_editbuffer_poi
CCa 0x80336ec md380_menu_0x2001d3ed
CCa 0x80336f4 md380_menu_0x2001d3ee
CCa 0x80336fc md380_menu_0x2001d3ef
CCa 0x8033704 md380_menu_0x2001d3f1
CCa 0x803370c md380_menu_0x2001d3f0
CCa 0x8033714 md380_menu_0x200011e4
CCa 0x803371a md380_menu_memory
CCa 0x8033732 md380_menu_id
CCa 0x803373a md380_menu_mem_base
CCa 0x8033764 mn_editbuffer_poi
CCa 0x803376a md380_menu_id
CCa 0x8033798 md380_menu_0x200011e4
CCa 0x80337a0 md380_menu_0x2001d3f4
CCa 0x80337a8 mn_editbuffer_poi
CCa 0x80337ac md380_menu_edit_buf
CCa 0x80337b8 mn_editbuffer_poi
CCa 0x80337cc md380_menu_0x2001d3ed
CCa 0x80337d4 md380_menu_0x2001d3ee
CCa 0x80337dc md380_menu_0x2001d3ef
CCa 0x80337e4 md380_menu_0x2001d3f1
CCa 0x80337ec md380_menu_0x2001d3f0
CCa 0x80337f4 md380_menu_0x200011e4
CCa 0x80337fc md380_menu_memory
CCa 0x803381c md380_menu_id
CCa 0x8033824 md380_menu_mem_base
CCa 0x803384e mn_editbuffer_poi
CCa 0x8033854 md380_menu_id
CCa 0x8033862 md380_menu_id
CCa 0x803386c md380_menu_mem_base
CCa 0x8033898 md380_menu_0x200011e4
CCa 0x80338a0 md380_menu_memory
CCa 0x80338d6 md380_menu_id
CCa 0x80338de md380_menu_mem_base
CCa 0x8033916 md380_menu_id
CCa 0x8033940 md380_menu_id
CCa 0x8033952 md380_menu_id
CCa 0x803395c md380_menu_mem_base
CCa 0x803397c md380_menu_0x200011e4
CCa 0x8033984 md380_menu_memory
CCa 0x80339ba md380_menu_id
CCa 0x80339c2 md380_menu_mem_base
CCa 0x80339fc md380_menu_id
CCa 0x8033a2a md380_menu_id
CCa 0x8033a3c md380_menu_id
CCa 0x8033a46 md380_menu_mem_base
CCa 0x8033a6c md380_menu_0x200011e4
CCa 0x8033a74 md380_menu_memory
CCa 0x8033a94 md380_menu_id
CCa 0x8033a9c md380_menu_mem_base
CCa 0x8033afc md380_menu_id
CCa 0x8033b2e md380_menu_id
CCa 0x8033b38 md380_menu_0x200011e4
CCa 0x8033b44 md380_menu_memory
CCa 0x8033b6c md380_menu_0x200011e4
CCa 0x8033b74 md380_menu_memory
CCa 0x8033b94 md380_menu_id
CCa 0x8033b9c md380_menu_mem_base
CCa 0x8033bfe md380_menu_id
CCa 0x8033c30 md380_menu_id
CCa 0x8033c3a md380_menu_0x200011e4
CCa 0x8033c46 md380_menu_memory
CCa 0x8033c74 md380_menu_0x200011e4
CCa 0x8033c7c md380_menu_memory
CCa 0x8033cb6 md380_menu_id
CCa 0x8033cbe md380_menu_mem_base
CCa 0x8033cfa md380_menu_id
CCa 0x8033d06 mn_editbuffer_poi
CCa 0x8033d26 md380_menu_id
CCa 0x8033d2e md380_menu_mem_base
CCa 0x8033d68 md380_menu_id
CCa 0x8033d78 md380_menu_0x200011e4
CCa 0x8033d80 md380_menu_memory
CCa 0x8033dba md380_menu_id
CCa 0x8033dc2 md380_menu_mem_base
CCa 0x8033dfe md380_menu_id
CCa 0x8033e0a mn_editbuffer_poi
CCa 0x8033e2c md380_menu_id
CCa 0x8033e34 md380_menu_mem_base
CCa 0x8033e6e md380_menu_id
af+ 0x8033e88 166 F_4389
af+ 0x8033f2e 92 F_4390
af+ 0x8033f94 158 F_4323
af+ 0x8034072 212 F_4391
CCa 0x803414e md380_menu_0x200011e4
CCa 0x8034154 md380_menu_memory
CCa 0x803416c md380_menu_id
CCa 0x8034174 md380_menu_mem_base
CCa 0x80341aa md380_menu_id
CCa 0x80341da md380_menu_0x200011e4
CCa 0x80341e0 md380_menu_memory
CCa 0x80341f8 md380_menu_id
CCa 0x8034200 md380_menu_mem_base
CCa 0x8034232 md380_menu_id
af+ 0x8034388 160 F_4392
af+ 0x8034428 90 F_4393
af+ 0x8034488 158 F_4324
af+ 0x8034566 210 F_4394
CCa 0x8034646 md380_menu_0x2001d3f4
af+ 0x803465c 230 F_4325
CCa 0x803465e md380_menu_0x200011e4
CCa 0x8034664 md380_menu_id
CCa 0x8034676 md380_menu_0x200011e4
CCa 0x803467c md380_menu_memory
CCa 0x803469e md380_menu_mem_base
CCa 0x80346d6 md380_menu_id
CCa 0x80346fc md380_menu_id
CCa 0x8034708 md380_menu_0x200011e4
CCa 0x8034710 md380_menu_id
CCa 0x803471e md380_menu_0x200011e4
CCa 0x803472a md380_menu_mem_base
CCa 0x80347b0 md380_menu_0x200011e4
CCa 0x80347b8 md380_menu_memory
CCa 0x80347e6 md380_menu_id
CCa 0x80347ee md380_menu_mem_base
CCa 0x803482a md380_menu_id
CCa 0x803484a md380_menu_id
CCa 0x8034852 md380_menu_mem_base
CCa 0x803488e md380_menu_id
CCa 0x80348be md380_menu_id
CCa 0x80348d0 md380_menu_id
CCa 0x80348da md380_menu_mem_base
CCa 0x80348f4 md380_menu_0x200011e4
CCa 0x80348fc md380_menu_memory
CCa 0x8034912 md380_menu_id
CCa 0x803491a md380_menu_mem_base
CCa 0x8034964 md380_menu_id
CCa 0x8034972 md380_menu_id
CCa 0x803497c md380_menu_mem_base
CCa 0x8034998 md380_menu_0x200011e4
CCa 0x80349a0 md380_menu_memory
CCa 0x80349d6 md380_menu_id
CCa 0x80349de md380_menu_mem_base
CCa 0x8034a18 md380_menu_id
CCa 0x8034a46 md380_menu_id
CCa 0x8034a58 md380_menu_id
CCa 0x8034a62 md380_menu_mem_base
CCa 0x8034a7a md380_menu_0x200011e4
CCa 0x8034a82 md380_menu_memory
CCa 0x8034aa0 md380_menu_id
CCa 0x8034aa8 md380_menu_mem_base
CCa 0x8034ae4 md380_menu_id
CCa 0x8034b80 md380_menu_0x200011e4
CCa 0x8034b88 md380_menu_memory
CCa 0x8034b94 mn_editbuffer_poi
CCa 0x8034b98 md380_menu_edit_buf
CCa 0x8034ba4 md380_menu_id
CCa 0x8034bac md380_menu_mem_base
CCa 0x8034be8 md380_menu_id
CCa 0x8034c18 md380_menu_id
CCa 0x8034c2a md380_menu_id
CCa 0x8034c34 md380_menu_mem_base
CCa 0x8034c50 md380_menu_0x200011e4
CCa 0x8034c58 md380_menu_memory
CCa 0x8034c8e md380_menu_id
CCa 0x8034c96 md380_menu_mem_base
CCa 0x8034cd0 md380_menu_id
CCa 0x8034cfe md380_menu_id
CCa 0x8034d10 md380_menu_id
CCa 0x8034d1a md380_menu_mem_base
CCa 0x8034d34 md380_menu_0x200011e4
CCa 0x8034d3c md380_menu_memory
CCa 0x8034d5c md380_menu_id
CCa 0x8034d64 md380_menu_mem_base
CCa 0x8034dc8 md380_menu_id
CCa 0x8034dfa md380_menu_id
CCa 0x8034e04 md380_menu_0x200011e4
CCa 0x8034e10 md380_menu_memory
CCa 0x8034e32 md380_menu_0x200011e4
CCa 0x8034e3a md380_menu_0x2001d3f4
CCa 0x8034e42 mn_editbuffer_poi
CCa 0x8034e46 md380_menu_edit_buf
CCa 0x8034e52 mn_editbuffer_poi
CCa 0x8034e66 md380_menu_0x2001d3ed
CCa 0x8034e6e md380_menu_0x2001d3ee
CCa 0x8034e76 md380_menu_0x2001d3ef
CCa 0x8034e7e md380_menu_0x2001d3f1
CCa 0x8034e86 md380_menu_0x2001d3f0
CCa 0x8034e8e md380_menu_0x200011e4
CCa 0x8034e96 md380_menu_memory
CCa 0x8034eb4 md380_menu_id
CCa 0x8034ebc md380_menu_mem_base
CCa 0x8034ee6 mn_editbuffer_poi
CCa 0x8034eec md380_menu_id
CCa 0x8034efc md380_menu_0x200011e4
CCa 0x8034f04 md380_menu_memory
CCa 0x8034f3e md380_menu_id
CCa 0x8034f46 md380_menu_mem_base
CCa 0x8034f82 md380_menu_id
CCa 0x8034f8c mn_editbuffer_poi
CCa 0x8034faa md380_menu_id
CCa 0x8034fb0 md380_menu_mem_base
CCa 0x8034fe2 md380_menu_id
af+ 0x8034fec 166 F_4395
af+ 0x8035092 88 F_4396
af+ 0x80350ea 134 F_4326
af+ 0x80351ae 210 F_4397
CCa 0x80353d4 md380_menu_edit_buf
af+ 0x80353f0 8 F_1080
af+ 0x80353f8 50 F_1014
af+ 0x803542a 10 F_1015
af+ 0x8035434 10 F_345
af+ 0x803543e 226 F_121
af+ 0x8035520 46 F_1016
af+ 0x803554e 8 F_1017
af+ 0x8035556 8 F_4089
af+ 0x803555e 32 F_347
af+ 0x803557e 32 F_842
af+ 0x803559e 32 F_843
af+ 0x80355be 32 RCC_APB1_peripheral_clock_enable_register
af+ 0x80355de 32 RCC_APB2_peripheral_clock_enable_register
af+ 0x80355fe 68 F_1018
af+ 0x803568c 8 F_350
af+ 0x8035694 10 F_351
af+ 0x80356a8 8 F_352
af+ 0x80356b0 136 aes_cipher
af+ 0x8035738 236 aes_loadkey
af+ 0x8035824 92 F_844
af+ 0x8035880 42 F_355
af+ 0x80358aa 68 F_356
af+ 0x80358ee 148 F_357
af+ 0x8035982 46 F_358
af+ 0x80359bc 8 F_845
af+ 0x80359c4 64 F_1019
af+ 0x8035a08 8 F_846
af+ 0x8035a10 2 F_1020
af+ 0x8035a14 202 F_4197
af+ 0x8035af0 26 F_4327
af+ 0x8035b10 82 F_4261
af+ 0x8035b62 10 F_4198
af+ 0x8035b6c 158 F_4199
af+ 0x8035c10 16 F_4262
af+ 0x8035c20 14 F_4200
af+ 0x8035c2e 14 F_4201
af+ 0x8035c3c 20 F_4202
af+ 0x8035c50 20 F_4203
af+ 0x8035c64 176 F_4204
af+ 0x8035d14 254 F_4205
af+ 0x8035e18 92 F_4206
af+ 0x8035e78 52 F_4207
af+ 0x8035eac 24 F_4208
af+ 0x8035ec4 10 F_4209
af+ 0x8035ece 60 F_4328
af+ 0x8035f0a 92 F_4263
af+ 0x8035f66 24 F_4210
af+ 0x8035f80 26 F_4264
af+ 0x8036006 40 F_4512
af+ 0x803602e 52 F_4502
af+ 0x803607e 28 F_4265
af+ 0x803609a 34 F_5172
af+ 0x80360bc 34 F_5173
af+ 0x80360de 32 F_4266
af+ 0x80360fe 44 F_5154
af+ 0x803612a 44 F_5155
af+ 0x8036156 12 F_4211
af+ 0x8036162 240 F_4212
af+ 0x8036252 46 F_4329
af+ 0x8036280 34 F_4503
af+ 0x80362a2 30 F_5148
af+ 0x80362c0 62 F_5149
af+ 0x80362fe 10 F_5150
af+ 0x8036308 50 F_5145
af+ 0x803633a 6 F_4461
af+ 0x8036344 22 F_1107
af+ 0x803635a 10 F_4213
af+ 0x8036364 10 F_4462
af+ 0x803636e 26 F_4214
af+ 0x8036388 72 F_4267
af+ 0x80363d0 10 F_5174
af+ 0x80363da 98 F_5156
af+ 0x803643c 8 F_5157
af+ 0x8036444 68 F_5158
af+ 0x8036488 152 F_4268
af+ 0x8036530 4 F_4330
af+ 0x8036534 104 F_4446
af+ 0x803659c 54 F_4504
af+ 0x80365d2 40 F_4269
af+ 0x80365fa 26 F_4398
af+ 0x8036614 198 F_4331
af+ 0x80366da 42 F_4476
af+ 0x8036704 628 F_5159
CCa 0x803670a md380_menu_0x200011e4
CCa 0x8036712 md380_menu_memory
CCa 0x803674e md380_menu_id
CCa 0x8036756 md380_menu_mem_base
CCa 0x80367da md380_menu_id
CCa 0x803680e md380_menu_id
CCa 0x803684e md380_menu_id
CCa 0x80368b6 md380_menu_id
CCa 0x80368ea md380_menu_id
CCa 0x8036946 md380_menu_id
CCa 0x8036958 md380_menu_id
CCa 0x8036962 md380_menu_mem_base
af+ 0x8036978 120 F_5160
CCa 0x803697a md380_menu_0x200011e4
CCa 0x8036982 md380_menu_memory
CCa 0x80369a0 md380_menu_id
CCa 0x80369a8 md380_menu_mem_base
CCa 0x80369e4 md380_menu_id
af+ 0x80369f0 144 F_5161
CCa 0x80369f2 md380_menu_0x200011e4
CCa 0x80369fa md380_menu_memory
CCa 0x8036a18 md380_menu_id
CCa 0x8036a20 md380_menu_mem_base
CCa 0x8036a58 md380_menu_id
CCa 0x8036a68 md380_menu_0x200011e4
CCa 0x8036a6e md380_menu_memory
CCa 0x8036a88 md380_menu_id
CCa 0x8036a8e md380_menu_mem_base
CCa 0x8036aba md380_menu_id
CCa 0x8036ac6 md380_menu_id
CCa 0x8036ace md380_menu_mem_base
af+ 0x8036b96 222 F_4477
af+ 0x8036c74 54 F_359
af+ 0x8036cac 140 F_4332
af+ 0x8036d88 2810 F_4505
CCa 0x80372ba SystemConfig
CCa 0x80372be ConfigData + 0x14 (byte) F_4505
CCa 0x80372f4 SystemConfig
CCa 0x80372f8 ConfigData + 0x13 (byte) F_4505
CCa 0x80375d2 SystemConfig
CCa 0x80375d6 ConfigData + 0x14 (byte) F_4505
CCa 0x803760c SystemConfig
CCa 0x8037610 ConfigData + 0x13 (byte) F_4505
CCa 0x8037774 SystemConfig
CCa 0x8037778 ConfigData + 0x14 (byte) F_4505
CCa 0x80377a4 SystemConfig
CCa 0x80377a8 ConfigData + 0x13 (byte) F_4505
CCa 0x8037824 SystemConfig
CCa 0x8037828 ConfigData + 0x14 (byte) F_4505
CCa 0x8037854 SystemConfig
CCa 0x8037858 ConfigData + 0x13 (byte) F_4505
af+ 0x80378b4 448 F_4513
af+ 0x8037a98 500 F_4514
af+ 0x8037c90 320 F_4515
af+ 0x8037dd0 64 F_4516
af+ 0x8037e10 176 F_4517
af+ 0x8037ec0 64 F_4518
CCa 0x8037f0c md380_menu_0x200011e4
CCa 0x8037f14 md380_menu_memory
CCa 0x8037f26 md380_menu_id
CCa 0x8037f2e md380_menu_mem_base
CCa 0x8037f6a md380_menu_id
CCa 0x8037f78 md380_menu_id
CCa 0x8037f82 md380_menu_mem_base
af+ 0x8038084 98 F_4399
CCa 0x80381e6 md380_menu_0x200011e4
CCa 0x80381ee md380_menu_memory
CCa 0x80381fa md380_menu_0x200011e4
CCa 0x8038202 md380_menu_memory
CCa 0x803824c md380_menu_id
CCa 0x8038254 md380_menu_mem_base
CCa 0x803829e md380_menu_id
CCa 0x80382e6 md380_menu_id
CCa 0x803830e md380_menu_id
CCa 0x8038362 md380_menu_id
CCa 0x8038394 md380_menu_id
CCa 0x80383bc md380_menu_id
CCa 0x8038410 md380_menu_id
CCa 0x8038442 md380_menu_id
CCa 0x803846a md380_menu_id
CCa 0x80384c2 md380_menu_id
CCa 0x80384ea md380_menu_id
CCa 0x803851c md380_menu_id
CCa 0x8038550 md380_menu_id
CCa 0x8038578 md380_menu_id
CCa 0x80385a0 md380_menu_memory
CCa 0x80385a4 md380_menu_id
CCa 0x80385e4 md380_menu_memory
CCa 0x80385ec md380_menu_memory
af+ 0x80385f0 20 F_4270
af+ 0x8038604 18 F_4333
af+ 0x8038616 16 F_4463
af+ 0x803862c 14 F_4271
af+ 0x8038640 78 F_4478
af+ 0x803868e 96 F_4464
af+ 0x80386f4 32 F_4334
af+ 0x8038728 8 F_4465
af+ 0x8038734 80 F_4447
af+ 0x8038784 24 F_4400
af+ 0x803879c 78 F_4479
af+ 0x80387ea 14 F_4466
af+ 0x80387f8 14 F_4467
af+ 0x8038806 10 F_4448
af+ 0x8038810 10 F_4449
af+ 0x803881c 18 F_1081
af+ 0x8038834 24 F_4272
af+ 0x803884c 12 F_5162
af+ 0x8038858 12 F_5163
af+ 0x8038864 18 F_5164
af+ 0x8038878 54 F_4335
af+ 0x80388b0 68 F_4336
af+ 0x80388f4 46 F_4401
af+ 0x8038924 166 F_4402
af+ 0x80389ca 92 F_4403
af+ 0x8038a26 158 F_4337
af+ 0x8038ac4 64 F_5165
af+ 0x8038b04 212 F_4404
af+ 0x8038bd8 442 Create_Menu_Entry_Send
CCa 0x8038bdc md380_menu_0x200011e4
CCa 0x8038be4 md380_menu_memory
CCa 0x8038bf0 mn_editbuffer_poi
CCa 0x8038bf4 md380_menu_edit_buf
CCa 0x8038c1c md380_menu_id
CCa 0x8038c24 md380_menu_mem_base
CCa 0x8038c60 md380_menu_id
CCa 0x8038c8a md380_menu_id
CCa 0x8038c92 md380_menu_mem_base
CCa 0x8038cce md380_menu_id
CCa 0x8038cee md380_menu_id
CCa 0x8038cf6 md380_menu_mem_base
CCa 0x8038d32 md380_menu_id
CCa 0x8038d62 md380_menu_id
CCa 0x8038d74 md380_menu_id
CCa 0x8038d7e md380_menu_mem_base
CCa 0x8038d96 md380_menu_0x200011e4
CCa 0x8038d9e md380_menu_memory
CCa 0x8038dae md380_menu_id
CCa 0x8038db6 md380_menu_mem_base
CCa 0x8038de6 mn_editbuffer_poi
CCa 0x8038dec md380_menu_id
CCa 0x8038dfa md380_menu_id
CCa 0x8038e04 md380_menu_mem_base
CCa 0x8038e1a md380_menu_0x200011e4
CCa 0x8038e22 md380_menu_0x200011e4
CCa 0x8038e28 md380_menu_0x200011e4
CCa 0x8038e32 md380_menu_id
CCa 0x8038e38 md380_menu_0x200011e4
CCa 0x8038e40 md380_menu_memory
CCa 0x8038e4a mn_editbuffer_poi
CCa 0x8038e4e md380_menu_edit_buf
CCa 0x8038e68 md380_menu_id
CCa 0x8038e70 md380_menu_mem_base
CCa 0x8038eac md380_menu_id
CCa 0x8038edc md380_menu_id
CCa 0x8038eee md380_menu_id
CCa 0x8038ef8 md380_menu_mem_base
CCa 0x803912c md380_menu_0x2001d3f4
CCa 0x8039136 mn_editbuffer_poi
CCa 0x803914c md380_menu_0x2001d3ee
CCa 0x8039152 md380_menu_0x2001d3ef
CCa 0x803915a md380_menu_0x2001d3ed
CCa 0x8039162 md380_menu_0x2001d3f1
CCa 0x803916a md380_menu_0x2001d3f0
CCa 0x8039172 md380_menu_0x200011e4
CCa 0x803917a md380_menu_memory
CCa 0x803918a md380_menu_id
CCa 0x8039192 md380_menu_mem_base
CCa 0x80391c0 mn_editbuffer_poi
CCa 0x80391c6 md380_menu_id
CCa 0x80391dc mn_editbuffer_poi
CCa 0x80391fc md380_menu_0x2001d3ee
CCa 0x8039208 md380_menu_0x2001d3ef
CCa 0x803921c md380_menu_0x200011e4
CCa 0x8039224 md380_menu_memory
CCa 0x8039244 md380_menu_id
CCa 0x803924c md380_menu_mem_base
CCa 0x8039286 md380_menu_id
CCa 0x80392b4 md380_menu_id
CCa 0x80392c6 md380_menu_id
CCa 0x80392d0 md380_menu_mem_base
CCa 0x80392ee md380_menu_0x200011e4
CCa 0x80392f6 md380_menu_memory
CCa 0x8039314 md380_menu_id
CCa 0x803931c md380_menu_mem_base
CCa 0x8039358 md380_menu_id
CCa 0x8039366 md380_menu_0x200011e4
CCa 0x803936c md380_menu_memory
CCa 0x8039374 mn_editbuffer_poi
CCa 0x8039376 md380_menu_edit_buf
CCa 0x8039380 md380_menu_id
CCa 0x8039388 md380_menu_mem_base
CCa 0x80393c4 md380_menu_id
CCa 0x80393ee md380_menu_id
CCa 0x803941c md380_menu_id
CCa 0x803944a md380_menu_id
CCa 0x803945a md380_menu_id
CCa 0x8039462 md380_menu_mem_base
CCa 0x80394ac md380_menu_0x2001d3ef
CCa 0x80394d4 md380_menu_0x2001d3ef
CCa 0x80394ea md380_menu_0x2001d3f4
CCa 0x80394f2 mn_editbuffer_poi
CCa 0x8039506 md380_menu_0x2001d3ee
CCa 0x803950a md380_menu_0x2001d3ef
CCa 0x8039510 md380_menu_0x2001d3ed
CCa 0x8039516 md380_menu_0x2001d3f1
CCa 0x8039524 md380_menu_0x200011e4
CCa 0x803952a md380_menu_memory
CCa 0x8039544 md380_menu_id
CCa 0x803954a md380_menu_mem_base
CCa 0x803957c md380_menu_id
CCa 0x80395a0 md380_menu_id
CCa 0x80395b0 md380_menu_id
CCa 0x80395b8 md380_menu_mem_base
CCa 0x80395e2 md380_menu_0x200011e4
CCa 0x80395f2 md380_menu_memory
CCa 0x803960c md380_menu_0x200011e4
CCa 0x8039612 md380_menu_memory
CCa 0x803962a md380_menu_id
CCa 0x8039630 md380_menu_mem_base
CCa 0x803965e ... D.e.l.e.t.e.d
CCa 0x8039662 md380_menu_id
CCa 0x8039680 md380_menu_0x200011e4
CCa 0x8039686 md380_menu_0x200011e4
CCa 0x803968c md380_menu_0x200011e4
CCa 0x8039690 md380_menu_0x200011e4
CCa 0x8039698 md380_menu_id
CCa 0x80396b0 md380_menu_0x200011e4
CCa 0x80396c4 mn_editbuffer_poi
CCa 0x80396d2 md380_menu_0x200011e4
CCa 0x80396d8 md380_menu_0x200011e4
CCa 0x80396dc md380_menu_0x200011e4
CCa 0x80396e4 md380_menu_id
CCa 0x8039704 md380_menu_memory
af+ 0x8039760 30 F_4405
af+ 0x8039780 32 F_5066
af+ 0x80397a0 56 F_5063
af+ 0x80397d8 294 F_5064
af+ 0x80398fe 58 F_5065
af+ 0x8039938 354 F_5062
af+ 0x8039a9a 36 F_5061
af+ 0x8039abe 36 F_5060
af+ 0x8039ae2 196 F_5059
af+ 0x8039ba6 42 F_5058
af+ 0x8039bd0 46 F_5057
af+ 0x8039bfe 36 F_5056
af+ 0x8039c22 144 main_menu
af+ 0x8039cb2 226 F_4274
CCa 0x8039d16 main_menu
af+ 0x8039da4 14 F_5146
af+ 0x8039db2 12 F_5147
af+ 0x8039dbe 34 F_5166
af+ 0x8039de0 72 F_5151
af+ 0x8039e28 54 F_4275
af+ 0x8039e5e 22 F_4506
af+ 0x8039e74 12 F_1108
af+ 0x8039e80 30 F_5152
af+ 0x8039ea0 100 F_1109
af+ 0x8039f04 66 F_1082
CCa 0x8039f24 BLX F_4543
af+ 0x8039f46 62 F_1083
af+ 0x8039f84 186 F_1084
af+ 0x803a04c 100 F_1110
af+ 0x803a0b0 36 F_1085
af+ 0x803a0d8 28 F_1111
af+ 0x803a0f4 108 F_4215
af+ 0x803a160 90 F_360
af+ 0x803a1ba 132 F_361
af+ 0x803a23e 104 F_362
af+ 0x803a2a6 32 F_5032
af+ 0x803a2c6 1506 F_5033
af+ 0x803a8a8 98 F_5037
af+ 0x803a90a 104 F_5038
af+ 0x803a972 572 F_5039
af+ 0x803abae 68 F_5041
af+ 0x803abf2 142 F_5044
af+ 0x803ac80 68 F_5042
af+ 0x803acd4 60 F_5045
af+ 0x803ad20 170 F_5043
af+ 0x803adfc 358 F_5046
af+ 0x803af70 92 F_847
af+ 0x803afd8 92 F_848
af+ 0x803b044 58 F_363
af+ 0x803b110 76 F_5030
af+ 0x803b15c 26 F_364
af+ 0x803b176 32 F_365
af+ 0x803b196 338 F_5031
af+ 0x803b2e8 90 F_849
af+ 0x803b342 100 F_850
af+ 0x803b3a6 74 F_366
af+ 0x803b3f0 36 F_367
af+ 0x803b414 20 F_368
af+ 0x803b428 70 F_851
af+ 0x803b46e 92 F_369
af+ 0x803b510 14 F_852
af+ 0x803b524 2286 Call_Process
CCa 0x803bb72 SystemConfig
CCa 0x803bb76 ConfigData + 0x1  (byte) Call_Process
CCa 0x803bb7c SystemConfig
CCa 0x803bb80 ConfigData + 0x8  (byte) Call_Process
af+ 0x803be24 992 State_Change
CCa 0x803c04e SystemConfig
CCa 0x803c052 ConfigData + 0x4  (long) MyDMRID State_Change
af+ 0x803c244 772 ChAccess_Pr
af+ 0x803c57c 86 F_39
af+ 0x803c5d8 90 F_853
af+ 0x803c632 52 F_371
af+ 0x803c666 50 F_372
CCa 0x803c672 SystemConfig
CCa 0x803c676 ConfigData + 0x4  (long) MyDMRID F_372
af+ 0x803c6b4 680 F_40
CCa 0x803c6c0 SystemConfig
CCa 0x803c6c4 ConfigData + 0x15 (byte) F_40
af+ 0x803c998 808 F_374
CCa 0x803cb6a ConfigData + 0x1  (byte) uxtb r0, r0 .. lsrs r0, r0, 6 check with 1 F_374
CCa 0x803cb6a SystemConfig
CCa 0x803cb6e ConfigData + 0x1  (byte) F_374
CCa 0x803cb78 SystemConfig
CCa 0x803cb7c ConfigData + 0x1  (byte) uxtb r0, r0 .. lsrs r0, r0, 6 check with 6 F_374
CCa 0x803cc2c SystemConfig
CCa 0x803cc30 ConfigData + 0x1  (byte) uxtb r0, r0 .. lsrs r0, r0, 6 check with 6 F_374
CCa 0x803cc3a SystemConfig
CCa 0x803cc3e ConfigData + 0x1  (byte) uxtb r0, r0 .. lsrs r0, r0, 6 check with 6 F_374
af+ 0x803cccc 590 F_375
af+ 0x803cf1a 62 F_41
af+ 0x803cf58 36 F_377
af+ 0x803cf7c 46 F_42
af+ 0x803cfb4 388 F_379
CCa 0x803d02e SystemConfig
CCa 0x803d030 ConfigData + 0x15 (byte) F_379
CCa 0x803d0b8 SystemConfig
CCa 0x803d0ba ConfigData + 0x15 (byte) F_379
CCa 0x803d106 SystemConfig
CCa 0x803d108 ConfigData + 0x15 (byte) F_379
af+ 0x803d140 102 F_380
af+ 0x803d1fc 40 F_381
af+ 0x803d224 70 F_382
af+ 0x803d26a 62 F_383
af+ 0x803d2a8 40 F_384
af+ 0x803d2d0 40 F_385
af+ 0x803d2f8 40 F_854
af+ 0x803d320 38 F_386
af+ 0x803d346 40 F_387
af+ 0x803d36e 44 F_855
af+ 0x803d39a 38 F_388
af+ 0x803d3c0 40 F_389
af+ 0x803d3e8 40 F_390
af+ 0x803d410 44 F_856
af+ 0x803d43c 374 F_391
af+ 0x803d5c0 146 F_136
af+ 0x803d660 94 F_137
af+ 0x803d6d8 214 F_138
af+ 0x803d7c0 282 F_395
af+ 0x803d8da 192 F_396
af+ 0x803da00 44 F_857
CCa 0x803da10 Call OS_ENTER_CRITICAL()
CCa 0x803da22 Call OS_EXIT_CRITICAL()
af+ 0x803da2c 76 OSSemCreate
CCa 0x803da3e Call OS_ENTER_CRITICAL()
CCa 0x803da58 Call OS_EXIT_CRITICAL()
af+ 0x803da78 218 OSSemPend
CCa 0x803da78 void OSSemPend (OS_EVENT *pevent, INT32U timeout, INT8U *perr)
CCa 0x803daaa Call OS_ENTER_CRITICAL()
CCa 0x803dabe Call OS_EXIT_CRITICAL()
CCa 0x803daf4 Call OS_EXIT_CRITICAL()
CCa 0x803dafc Call OS_ENTER_CRITICAL()
CCa 0x803db4c Call OS_EXIT_CRITICAL()
af+ 0x803db68 92 OSSemPost
CCa 0x803db78 Call OS_ENTER_CRITICAL()
CCa 0x803db94 Call OS_EXIT_CRITICAL()
CCa 0x803dbb2 Call OS_EXIT_CRITICAL()
CCa 0x803dbbc Call OS_EXIT_CRITICAL()
af+ 0x803dbc4 28 Sys_Inter
af+ 0x803dbe2 52 TimeSlot_Inter
af+ 0x803dc16 1206 F_43
CCa 0x803ddce SystemConfig
CCa 0x803ddd2 ConfigData + 0x4  (long) MyDMRID F_43
CCa 0x803dddc SystemConfig
CCa 0x803dde0 ConfigData + 0x4  (long) MyDMRID F_43
CCa 0x803ddec SystemConfig
CCa 0x803ddf0 ConfigData + 0x4  (long) MyDMRID F_43
af+ 0x803e0cc 476 F_44
af+ 0x803e2a8 38 F_402
af+ 0x803e2ce 38 F_68
af+ 0x803e2f4 42 F_404
af+ 0x803e31e 46 F_405
af+ 0x803e34c 52 F_48
af+ 0x803e380 332 F_407
af+ 0x803e4dc 68 F_408
af+ 0x803e528 44 F_409
af+ 0x803e594 86 F_410
af+ 0x803e600 218 F_411
af+ 0x803e6e4 196 F_412
af+ 0x803e7a8 340 F_413
af+ 0x803e900 868 F_414
af+ 0x803ec84 2 F_415
CCa 0x803eefe SystemConfig
CCa 0x803ef02 ConfigData + 0x4  (long) MyDMRID dmr_audio_start
af+ 0x803ef6c 92 F_416
af+ 0x803efc8 90 F_858
af+ 0x803f03c 728 dmr_sms_arrive
CCa 0x803f0c6 SystemConfig
CCa 0x803f0ca ConfigData + 0x4  (long) MyDMRID dmr_sms_arrive
CCa 0x803f1d8 SystemConfig
CCa 0x803f1dc ConfigData + 0x4  (long) MyDMRID dmr_sms_arrive
CCa 0x803f2ca SystemConfig
CCa 0x803f2ce ConfigData + 0x4  (long) MyDMRID dmr_sms_arrive
af+ 0x803f33c 818 dmr_call_end
CCa 0x803f3d6 SystemConfig
CCa 0x803f3d8 ConfigData + 0x1  (byte) dmr_call_end
CCa 0x803f42c SystemConfig
CCa 0x803f42e ConfigData + 0x4  (long) MyDMRID dmr_call_end
CCa 0x803f462 SystemConfig
CCa 0x803f464 ConfigData + 0x4  (long) MyDMRID dmr_call_end
CCa 0x803f506 SystemConfig
CCa 0x803f508 ConfigData + 0x1  (byte) dmr_call_end
CCa 0x803f5a0 SystemConfig
CCa 0x803f5a2 ConfigData + 0x4  (long) MyDMRID dmr_call_end
af+ 0x803f6d8 1134 F_419
CCa 0x803f716 SystemConfig
CCa 0x803f71a ConfigData + 0x4  (long) MyDMRID F_419
CCa 0x803f75a SystemConfig
CCa 0x803f75e ConfigData + 0x4  (long) MyDMRID F_419
CCa 0x803f78c SystemConfig
CCa 0x803f790 ConfigData + 0x4  (long) MyDMRID F_419
CCa 0x803f7e8 SystemConfig
CCa 0x803f7ec ConfigData + 0x4  (long) MyDMRID F_419
CCa 0x803f87e SystemConfig
CCa 0x803f882 ConfigData + 0x4  (long) MyDMRID F_419
CCa 0x803f902 SystemConfig
CCa 0x803f904 ConfigData + 0x4  (long) MyDMRID F_419
CCa 0x803f934 ConfigData + 0x4  (long) MyDMRID F_419
CCa 0x803f934 SystemConfig
CCa 0x803f95c SystemConfig
CCa 0x803f95e ConfigData + 0x4  (long) MyDMRID F_419
CCa 0x803f98c SystemConfig
CCa 0x803f98e ConfigData + 0x4  (long) MyDMRID F_419
CCa 0x803f9ce SystemConfig
CCa 0x803f9d0 ConfigData + 0x4  (long) MyDMRID F_419
CCa 0x803fa1c SystemConfig
CCa 0x803fa1e ConfigData + 0x4  (long) MyDMRID F_419
CCa 0x803fa44 SystemConfig
CCa 0x803fa46 ConfigData + 0x4  (long) MyDMRID F_419
CCa 0x803fa88 SystemConfig
CCa 0x803fa8a ConfigData + 0x4  (long) MyDMRID F_419
CCa 0x803fb1c SystemConfig
CCa 0x803fb1e ConfigData + 0x4  (long) MyDMRID F_419
af+ 0x803fbcc 5656 F_420
CCa 0x8040152 SystemConfig
CCa 0x8040156 ConfigData + 0x4  (long) MyDMRID F_420
CCa 0x80401ce SystemConfig
CCa 0x80401d2 ConfigData + 0x4  (long) MyDMRID F_420
CCa 0x80401e6 SystemConfig
CCa 0x80401ea ConfigData + 0x4  (long) MyDMRID F_420
CCa 0x80401fe SystemConfig
CCa 0x8040202 ConfigData + 0x4  (long) MyDMRID F_420
CCa 0x8040216 SystemConfig
CCa 0x804021a ConfigData + 0x4  (long) MyDMRID F_420
CCa 0x804022c SystemConfig
CCa 0x8040230 ConfigData + 0x4  (long) MyDMRID F_420
CCa 0x8040244 SystemConfig
CCa 0x8040248 ConfigData + 0x4  (long) MyDMRID F_420
CCa 0x8040260 SystemConfig
CCa 0x8040264 ConfigData + 0x4  (long) MyDMRID F_420
CCa 0x80409e4 SystemConfig
CCa 0x80409e8 ConfigData + 0x4  (long) MyDMRID F_420
CCa 0x8040a44 SystemConfig
CCa 0x8040a48 ConfigData + 0x4  (long) MyDMRID F_420
CCa 0x8040a8c SystemConfig
CCa 0x8040a90 ConfigData + 0x4  (long) MyDMRID F_420
CCa 0x8040bea SystemConfig
CCa 0x8040bec ConfigData + 0x4  (long) MyDMRID F_420
CCa 0x8040d7e SystemConfig
CCa 0x8040d80 ConfigData + 0x4  (long) MyDMRID F_420
CCa 0x8040dec SystemConfig
CCa 0x8040dee ConfigData + 0x4  (long) MyDMRID F_420
CCa 0x8041120 SystemConfig
CCa 0x8041124 ConfigData + 0x4  (long) MyDMRID F_420
CCa 0x8041148 SystemConfig
CCa 0x804114c ConfigData + 0x4  (long) MyDMRID F_420
af+ 0x80411e8 68 F_421
af+ 0x8041244 652 F_422
af+ 0x8041578 52 F_46
af+ 0x80415ac 390 F_424
af+ 0x804176c 106 F_425
af+ 0x80417d6 24 F_859
af+ 0x80417ee 6 F_426
af+ 0x80417f4 4 F_427
af+ 0x80417f8 4 F_428
af+ 0x80417fc 4 F_146_Nice
af+ 0x8041800 28 F_860
af+ 0x804181c 28 F_430
af+ 0x8041838 126 F_861
af+ 0x80418b6 140 F_862
af+ 0x8041942 138 F_863
af+ 0x80419cc 104 F_864
af+ 0x8041a54 34 F_865
af+ 0x8041a76 4 F_5040
af+ 0x8041a7a 4 F_5028
af+ 0x8041a7e 18 F_866
af+ 0x8041a90 20 F_867
af+ 0x8041aa4 18 F_868
af+ 0x8041ab6 34 F_431
af+ 0x8041ad8 24 F_432
af+ 0x8041b1e 8 F_433
af+ 0x8041b26 18 F_869
af+ 0x8041b38 32 F_4406
af+ 0x8041b58 2 F_1086
af+ 0x8041b5a 2 F_4507
af+ 0x8041b5c 64 F_870
af+ 0x8041b9c 256 F_434
af+ 0x8041cb0 28 F_871
af+ 0x8041ccc 28 F_435
af+ 0x8041ce8 86 F_436
af+ 0x8041d3e 4 SPI_I2S_ReceiveData
af+ 0x8041d42 4 SPI_I2S_SendData
af+ 0x8041d46 24 F_439
af+ 0x8041d5e 44 F_440
af+ 0x8041d8a 20 SPI_I2S_GetFlagStatus
af+ 0x8041df8 8 OS_ENTER_CRITICAL
af+ 0x8041e00 6 OS_EXIT_CRITICAL
af+ 0x8041e22 8 OS_TASK_SW
CCa 0x8041e24 PENDSVSET .. changes PendSV exception state to pending
af+ 0x8041e2a 8 OSIntCtxSw
CCa 0x8041e2c PENDSVSET .. changes PendSV exception state to pending
af+ 0x8041eac 100 OSMemNameSet_wtf
CCa 0x8041ece Call OS_ENTER_CRITICAL()
CCa 0x8041ed4 Call OS_EXIT_CRITICAL()
af+ 0x8041f10 148 OSIntExit
CCa 0x8041f1e Call OS_ENTER_CRITICAL()
CCa 0x8041f9e Call OS_EXIT_CRITICAL()
af+ 0x8041fa4 58 OSSchedLock
CCa 0x8041fb2 Call OS_ENTER_CRITICAL()
CCa 0x8041fd8 Call OS_EXIT_CRITICAL()
af+ 0x8041fde 90 OSSchedUnlock
CCa 0x8041fec Call OS_ENTER_CRITICAL()
CCa 0x804201c Call OS_EXIT_CRITICAL()
CCa 0x8042026 Call OS_EXIT_CRITICAL()
CCa 0x804202c Call OS_EXIT_CRITICAL()
CCa 0x8042032 Call OS_EXIT_CRITICAL()
af+ 0x804207a 182 OSTimeTick_maybee
CCa 0x8042082 Call OS_ENTER_CRITICAL()
CCa 0x8042098 Call OS_EXIT_CRITICAL()
CCa 0x80420ec Call OS_EXIT_CRITICAL()
CCa 0x80420f8 Call OS_ENTER_CRITICAL()
af+ 0x8042130 122 OS_EventTaskRdy
af+ 0x80421aa 140 OS_EventTaskWait_wtf_Multi
af+ 0x8042236 50 OS_EventTaskRemove_wtf_Multi
af+ 0x8042268 26 F_872
af+ 0x8042282 96 F_4114
af+ 0x80422e2 44 F_5013
af+ 0x804230e 52 F_5014
af+ 0x8042342 58 init_uC_OS-II_Idle_task
af+ 0x804237c 94 F_4116
af+ 0x80423da 18 F_4131
af+ 0x80423ec 92 OS_Sched
CCa 0x80423f0 Call OS_ENTER_CRITICAL()
CCa 0x8042442 Call OS_EXIT_CRITICAL()
af+ 0x8042448 30 OS_SchedNew
af+ 0x8042468 30 uC_OS_II_Idle
CCa 0x8042468 runs with 1MHz
CCa 0x804246a Call OS_ENTER_CRITICAL()
CCa 0x804247c Call OS_EXIT_CRITICAL()
af+ 0x804248c 318 OS_TCBInit
CCa 0x80424a4 Call OS_ENTER_CRITICAL()
CCa 0x80424bc Call OS_EXIT_CRITICAL()
CCa 0x804255c Call OS_ENTER_CRITICAL()
CCa 0x80425b6 Call OS_EXIT_CRITICAL()
CCa 0x80425c0 Call OS_EXIT_CRITICAL()
af+ 0x804262c 28 F_4508
af+ 0x804264c 28 F_4509
af+ 0x8042668 34 F_4338
af+ 0x804268a 30 F_4468
af+ 0x80426a8 30 Reboot_Radio
af+ 0x80426c8 536 Init_ADC
CCa 0x80426f4 Bat_Voltage
af+ 0x80428e0 332 F_455
CCa 0x8042940 Bat_Voltage
CCa 0x80429d2 Bat_Voltage
af+ 0x8042a2c 332 F_456
af+ 0x8042b78 700 F_153
af+ 0x8042e34 162 F_154
CCa 0x8042e34 VolumeKnop
CCa 0x8042e4c VolumeKnop
CCa 0x8042e52 VolumeKnop
CCa 0x8042ec8 VolumeKnop
CCa 0x8042ecc VolumeKnop
af+ 0x8042ed6 120 F_459
CCa 0x8042f0e Bat_Voltage
af+ 0x8042f4e 62 F_155
CCa 0x8042f7a VolumeKnop
CCa 0x8042f82 VolumeKnop
af+ 0x8042f8c 396 F_156
CCa 0x8042fa0 VolumeKnop
CCa 0x8042fec VolumeKnop
CCa 0x804301c VolumeKnop
CCa 0x8043066 VolumeKnop
CCa 0x80430ae VolumeKnop
CCa 0x80430be VolumeKnop
CCa 0x80430ec VolumeKnop
af+ 0x8043118 124 F_462
CCa 0x8043150 Bat_Voltage
af+ 0x8043194 54 F_157
af+ 0x8043204 934 F_158
CCa 0x8043570 SystemConfig
CCa 0x8043574 ConfigData + 0xe  (byte) F_158
af+ 0x80435b0 188 F_465
CCa 0x80435b4 Bat_Voltage
af+ 0x8043678 42 F_159
af+ 0x80436d4 834 F_160
af+ 0x8043a1c 110 F_468
CCa 0x8043a54 Bat_Voltage
af+ 0x8043aa0 74 F_161
af+ 0x8043b14 694 F_162
CCa 0x8043cee SystemConfig
CCa 0x8043cf0 ConfigData + 0xb  (byte) F_162
CCa 0x8043cfa SystemConfig
CCa 0x8043cfc ConfigData + 0xb  (byte) F_162
CCa 0x8043d04 SystemConfig
CCa 0x8043d06 ConfigData + 0xb  (byte) F_162
af+ 0x8043dd0 80 F_471
CCa 0x8043df8 Bat_Voltage
af+ 0x8043e28 64 F_163
af+ 0x8043e70 8 F_164
af+ 0x8043e78 268 Powering_Down
CCa 0x8043f32 P.o.w.e.r.i.n.g.
CCa 0x8043f4e D.o.w.n.
af+ 0x804403c 310 Start
CCa 0x8044086 SystemConfig
CCa 0x804408a ConfigData + 0x1  (byte) Start
af+ 0x8044172 298 F_474
CCa 0x8044184 _Start Mbox_
af+ 0x804429c 684 Start_multiple_tasks
CCa 0x80442c0 Create Process -Sys_Inter- Thread
CCa 0x80442f4 Create Process -RTC_Timer- Thread
CCa 0x8044328 Create Process -Call_Process- Thread
CCa 0x804435c Create Process -FMTx_Process- Thread
CCa 0x8044390 Create Process -RF_Pll- Thread
CCa 0x80443c4 Create Process -PC_Tune- Thread
CCa 0x804442c Create Process -Beep_Process- Thread
CCa 0x8044460 Create Process -Af_Mute- Thread
CCa 0x8044494 Create Process -TimeSlot- Thread
CCa 0x80444c8 Create Process -Set_Vocoder- Thread
CCa 0x80444fc Create Process -ChAccess_Rp- Thread
CCa 0x8044530 Create Process -State_Change- Thread
af+ 0x8044548 300 RTC_Timer
CCa 0x804454a SystemConfig
CCa 0x804454e Get ConfigData + 0x2  (byte) ubfx r0, r0, 4, 1 .. lsls r0, r0, 0x1f  RTC_Timer
CCa 0x8044550 Bit 4
CCa 0x804462a Loop RTC_Timer
CCa 0x804463a F_80_wtf an OS Function
CCa 0x804463e F_4500 -
CCa 0x8044642 F_4098 many function calls ... runs all functions
CCa 0x8044646 This_function_called_Read_Channel_Switch
CCa 0x804464a F_4520 something with keyboard
CCa 0x804464e F_4495 ?
CCa 0x8044658 F_4507 nothing
CCa 0x804465e F_4170 display?
CCa 0x8044662 F_4225 vip
CCa 0x8044666 F_4505
CCa 0x804466a F_4501
CCa 0x804466e dog_feeding
af+ 0x8044674 626 FMTx_Process
CCa 0x80447a2 SystemConfig
CCa 0x80447a4 ConfigData + 0x1  (byte) FMTx_Process
CCa 0x80447ae SystemConfig
CCa 0x80447b0 ConfigData + 0x1  (byte) FMTx_Process
af+ 0x80448e8 42 RF_Pll
af+ 0x8044900 20 PC_Tune
af+ 0x8044914 32 F_167
af+ 0x8044934 230 F_168
af+ 0x8044ab4 392 F_478
af+ 0x8044c3c 79 F_71
af+ 0x8044c82 678 F_480
af+ 0x8044f8c 118 F_481
af+ 0x8045002 366 F_482
af+ 0x8045198 108 F_483
af+ 0x8045218 96 F_484
af+ 0x80452b8 418 F_485
CCa 0x80453ca get akt. Zone_Name
af+ 0x80454b0 94 F_486
af+ 0x804550e 136 F_873
af+ 0x8045596 70 F_487
af+ 0x80455dc 154 F_169
af+ 0x8045676 124 F_489
af+ 0x80456f2 108 F_73
af+ 0x804575e 50 F_74
af+ 0x8045790 50 F_75
af+ 0x80457c2 246 F_493
af+ 0x80458b8 90 F_494
af+ 0x8045912 40 F_495
af+ 0x804593a 40 F_76
af+ 0x8045962 56 F_77
af+ 0x804599a 94 F_78
af+ 0x80459f8 20 F_79
af+ 0x8045a68 248 F_874
CCa 0x8045a9c Call OS_ENTER_CRITICAL()
CCa 0x8045ad8 Call OS_EXIT_CRITICAL()
CCa 0x8045afe Call OS_EXIT_CRITICAL()
CCa 0x8045b22 Call OS_EXIT_CRITICAL()
CCa 0x8045b48 Call OS_EXIT_CRITICAL()
CCa 0x8045b4e Call OS_EXIT_CRITICAL()
af+ 0x8045b60 90 F_500
CCa 0x8045b7a Call OS_ENTER_CRITICAL()
CCa 0x8045ba4 Call OS_EXIT_CRITICAL()
CCa 0x8045bae Call OS_EXIT_CRITICAL()
af+ 0x8045bc0 614 F_80_wtf
CCa 0x8045c1c Call OS_ENTER_CRITICAL()
CCa 0x8045c62 Call OS_EXIT_CRITICAL()
CCa 0x8045c82 Call OS_EXIT_CRITICAL()
CCa 0x8045c8a Call OS_ENTER_CRITICAL()
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
af+ 0x8045e26 288 F_502
CCa 0x8045e42 Call OS_ENTER_CRITICAL()
CCa 0x8045e72 Call OS_EXIT_CRITICAL()
CCa 0x8045f12 Call OS_EXIT_CRITICAL()
CCa 0x8045f20 Call OS_EXIT_CRITICAL()
CCa 0x8045f2e Call OS_ENTER_CRITICAL()
CCa 0x8045f38 Call OS_EXIT_CRITICAL()
af+ 0x8045f46 138 F_503
af+ 0x8045fd0 78 F_5015
af+ 0x804601e 88 F_504
af+ 0x8046098 38 F_505
af+ 0x80460be 14 F_1158
af+ 0x80460cc 94 F_1134
af+ 0x804612a 62 F_1123
af+ 0x8046168 14 F_4480
af+ 0x804617c 14 F_4481
af+ 0x8046190 14 F_4482
af+ 0x80461a4 4 F_4407
af+ 0x80461a8 4 F_4408
af+ 0x80461ac 6 F_4469
af+ 0x804622a 62 F_4409
af+ 0x8046268 248 F_4450
af+ 0x8046360 106 F_4410
af+ 0x80463ca 78 F_4411
af+ 0x8046418 294 F_4470
af+ 0x804653e 132 F_4451
af+ 0x80465c2 14 F_4412
af+ 0x80465e8 38 F_4413
af+ 0x8046614 40 F_4414
af+ 0x8046640 88 F_4415
af+ 0x804669c 52 F_4416
af+ 0x80466d4 48 F_4276
af+ 0x8046708 8 F_5049
af+ 0x8046710 8 F_5050
af+ 0x8046718 8 F_5051
af+ 0x8046720 18 F_1021
af+ 0x8046734 8 F_1022
af+ 0x8046740 2 F_875
af+ 0x8046742 12 F_5070
af+ 0x804674e 12 F_5071
af+ 0x8046790 378 F_1023
af+ 0x804690c 22 Not_Big_I2C_Function33
af+ 0x8046914 14 F_506
af+ 0x8046922 38 F_507
af+ 0x8046948 38 F_508
af+ 0x804696e 186 alpu_transfer_init
CCa 0x8046a12 access auf 0x4000540 I2C1
CCa 0x8046a1c access auf 0x4000540 I2C1
af+ 0x8046a28 306 alpu_transfer_read
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
af+ 0x8046b5a 236 alpu_transfer_write
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
af+ 0x8046c94 2 F_177_Nice
af+ 0x8046c98 8 F_1024
af+ 0x8046ca0 22 PWR_GetFlagStatus
af+ 0x8046cb6 14 Function_PWR_CR__
af+ 0x8046cd0 232 Delay
af+ 0x8046db8 10 Delay_char
af+ 0x8046dc2 62 maybe_get_random_from_rtc
af+ 0x8046e00 156 alpu_check_licence
af+ 0x8046eb0 2 F_180_Nice
af+ 0x8046eb4 8 F_520
af+ 0x8046ebc 8 F_521
af+ 0x8046ec4 8 F_522
af+ 0x8046ecc 20 F_523
af+ 0x8046ee0 38 F_524
af+ 0x8046f06 38 F_525
af+ 0x8046f2c 38 F_526
af+ 0x8046f52 38 F_527
af+ 0x8046f78 8 F_528
af+ 0x8046f80 14 F_529
af+ 0x8046f8e 14 F_530
af+ 0x8046f9c 8 F_531
af+ 0x8046fa4 8 F_532
af+ 0x8046fac 14 F_533
af+ 0x8046fba 14 F_534
af+ 0x8046fc8 716 F_181
af+ 0x804763e 62 F_536
af+ 0x804767c 62 dma_cmd
af+ 0x80476ba 106 F_538
af+ 0x8047724 2 F_182_Nice
af+ 0x80477dc 10 F_540
af+ 0x80477e6 14 F_541
af+ 0x80477f4 28 F_542
af+ 0x8047810 28 F_543
af+ 0x804782c 28 F_544
af+ 0x8047848 334 F_545
af+ 0x8047996 16 F_183
af+ 0x80479a6 16 F_184
af+ 0x80479b6 24 F_185
af+ 0x80479e8 38 F_549
af+ 0x8047a0e 22 F_186
af+ 0x8047a24 70 F_551
af+ 0x8047b0c 2 F_187_Nice
af+ 0x8047b10 182 F_188
af+ 0x8047c64 2 F_189_Nice
af+ 0x8047c68 16 F_555
af+ 0x8047c7c 734 Start_2_more_tasks__init_vocoder_tasks__Q
af+ 0x8047f5c 1700 F_4524
af+ 0x8048600 312 F_4525
af+ 0x8048738 104 F_4526
af+ 0x80487a8 158 F_557
af+ 0x8048c9c 202 ambe_unpack
af+ 0x8048d6c 88 F_4527
af+ 0x8048dc4 62 F_4528
af+ 0x8048e84 2 F_192_Nice
af+ 0x8048e88 120 F_560
af+ 0x8048f00 130 F_561
af+ 0x8048f82 184 F_562
af+ 0x804903a 14 OSTmrSignal___maybe
af+ 0x8049048 52 F_876
af+ 0x804907c 174 F_5016
CCa 0x804910e _uC/OS-II TmrLock_
CCa 0x804911a _uC/OS-II TmrSignal_
af+ 0x804912a 58 F_5023
af+ 0x8049164 110 F_194
af+ 0x80491d2 74 F_195
af+ 0x804921c 122 uC_OS_II_Tmr
af+ 0x80492d4 16 F_1112
af+ 0x80492e8 10 F_4417
af+ 0x80492f4 22 F_4277
af+ 0x8049310 36 F_4339
af+ 0x8049334 32 F_4278
af+ 0x804935c 76 F_4418
af+ 0x80493a8 100 F_4340
af+ 0x804940c 10 F_4279
af+ 0x8049418 36 F_4280
af+ 0x804943c 26 F_4281
af+ 0x8049458 34 F_4341
af+ 0x8049480 338 F_4342
af+ 0x80495d4 28 F_4044
af+ 0x80495f0 38 F_1136
af+ 0x804962c 28 F_4419
af+ 0x8049648 24 F_5167
af+ 0x8049660 48 F_4420
af+ 0x8049694 30 F_4343
af+ 0x80496b2 24 F_4344
af+ 0x80496ca 72 F_4471
af+ 0x8049712 86 F_4472
af+ 0x8049768 26 F_4421
af+ 0x8049782 18 F_4422
af+ 0x8049794 52 F_4488
af+ 0x80497c8 444 F_5168
af+ 0x8049984 120 F_5169
af+ 0x80499fc 66 F_5170
af+ 0x8049a3e 94 F_5171
af+ 0x8049a9c 178 F_4452
af+ 0x8049b5c 22 F_5153
af+ 0x8049b74 82 F_5017
af+ 0x8049bcc 78 F_5018
af+ 0x8049c20 46 F_4345
af+ 0x8049c4e 10 F_5024
af+ 0x8049c58 64 F_5025
af+ 0x8049c98 62 F_5026
af+ 0x8049cd8 102 F_877
af+ 0x8049d40 76 F_566
af+ 0x8049d8c 34 F_567
af+ 0x8049dae 26 F_568
af+ 0x8049dc8 212 F_569
af+ 0x8049e9c 10 ADC_SoftwareStartConv
af+ 0x8049ea6 6 F_5035
af+ 0x8049eac 26 F_571
af+ 0x8049ec6 26 F_572
af+ 0x8049ef0 22 F_5034
af+ 0x8049f08 532 F_1025
af+ 0x804a11c 98 F_1026
af+ 0x804a17e 16 F_878
af+ 0x804a18e 402 F_1087
af+ 0x804a420 90 F_573
af+ 0x804a47a 520 F_879
CCa 0x804a53e SystemConfig
CCa 0x804a542 ConfigData + 0x4  (long) MyDMRID F_879
CCa 0x804a548 SystemConfig
CCa 0x804a54c ConfigData + 0x4  (long) MyDMRID F_879
CCa 0x804a552 SystemConfig
CCa 0x804a556 ConfigData + 0x4  (long) MyDMRID F_879
CCa 0x804a61a SystemConfig
CCa 0x804a61e ConfigData + 0x4  (long) MyDMRID F_879
CCa 0x804a624 SystemConfig
CCa 0x804a628 ConfigData + 0x4  (long) MyDMRID F_879
CCa 0x804a62e SystemConfig
CCa 0x804a632 ConfigData + 0x4  (long) MyDMRID F_879
af+ 0x804a682 232 F_574
CCa 0x804a756 md380_menu_edit_buf
af+ 0x804a76a 52 F_575
af+ 0x804a79e 162 F_880
af+ 0x804a840 682 F_576
CCa 0x804a9cc SystemConfig
CCa 0x804a9ce ConfigData + 0x1  (byte) F_576
CCa 0x804a9d4 SystemConfig
CCa 0x804a9d6 ConfigData + 0x8  (byte) F_576
CCa 0x804aa90 SystemConfig
CCa 0x804aa92 ConfigData + 0x1  (byte) F_576
CCa 0x804aa98 SystemConfig
CCa 0x804aa9a ConfigData + 0x8  (byte) F_576
af+ 0x804aaea 340 F_577
CCa 0x804abe6 SystemConfig
CCa 0x804abe8 ConfigData + 0x1  (byte) F_577
CCa 0x804abee SystemConfig
CCa 0x804abf0 ConfigData + 0x8  (byte) F_577
af+ 0x804aca4 198 F_203
af+ 0x804ad6a 68 F_881
af+ 0x804adae 100 F_579
af+ 0x804ae14 92 init_Tone_fft_task
af+ 0x804ae70 126 F_581
af+ 0x804aeee 120 F_582
af+ 0x804af68 568 Tone_fft
af+ 0x804b1a0 734 F_205
af+ 0x804b508 120 F_584
af+ 0x804b580 114 F_17
af+ 0x804b5f2 28 F_586
af+ 0x804b60e 36 F_587
af+ 0x804b632 24 F_588
af+ 0x804b64a 20 F_16
af+ 0x804b66c 190 dmr_handle_data
af+ 0x804b7a6 8 F_5019
af+ 0x804b7ae 2 F_5020
af+ 0x804b7b0 8 F_206
af+ 0x804b7b8 8 OSTaskIdleHook
af+ 0x804b7c0 324 OSTaskStkInit
af+ 0x804b90c 8  short_function_1
af+ 0x804b914 36 short_function_2
af+ 0x804b970 2 F_594
af+ 0x804b974 2 short_function_4
af+ 0x804b976 2 short_function_3
af+ 0x804b978 228 F_5021
af+ 0x804bb40 180 OSTaskCreate
CCa 0x804bb50 Call OS_ENTER_CRITICAL()
CCa 0x804bb62 Call OS_EXIT_CRITICAL()
CCa 0x804bb86 Call OS_EXIT_CRITICAL()
CCa 0x804bbc8 Call OS_ENTER_CRITICAL()
CCa 0x804bbdc Call OS_EXIT_CRITICAL()
CCa 0x804bbe8 Call OS_EXIT_CRITICAL()
af+ 0x804bbf4 204 OSTaskCreateExt
CCa 0x804bc10 Call OS_ENTER_CRITICAL()
CCa 0x804bc22 Call OS_EXIT_CRITICAL()
CCa 0x804bc4a Call OS_EXIT_CRITICAL()
CCa 0x804bc92 Call OS_ENTER_CRITICAL()
CCa 0x804bca8 Call OS_EXIT_CRITICAL()
CCa 0x804bcb4 Call OS_EXIT_CRITICAL()
af+ 0x804bcc0 90 OSTaskNameSet
CCa 0x804bcd8 Call OS_ENTER_CRITICAL()
CCa 0x804bcf6 Call OS_EXIT_CRITICAL()
CCa 0x804bd04 Call OS_EXIT_CRITICAL()
CCa 0x804bd10 Call OS_EXIT_CRITICAL()
af+ 0x804bd2c 86 DMA_Init
af+ 0x804bd82 26 DMA_Cmd
af+ 0x804bd9c 64 F_208
af+ 0x804be68 14 F_209
af+ 0x804be7c 20 F_604
af+ 0x804bea0 292 F_605
af+ 0x804c01c 32 F_606
af+ 0x804c0fe 82 F_4090
af+ 0x804c150 6 F_607
af+ 0x804c156 58 F_4101
af+ 0x804c1a8 40 F_4097
af+ 0x804c1d0 112 F_4098
af+ 0x804c240 106 F_608
af+ 0x804c2aa 286 F_4102
af+ 0x804c3c8 64 F_609
af+ 0x804c408 2 F_4103
af+ 0x804c40a 78 F_610
af+ 0x804c458 518 F_4104
CCa 0x804c464 md380_menu_0x200011e4
CCa 0x804c478 SystemConfig
CCa 0x804c47c ConfigData + 0x16 (byte) F_4104
af+ 0x804c65e 74 F_611
af+ 0x804c6a8 478 F_4105
af+ 0x804c886 78 F_218
af+ 0x804c8d4 486 F_4106
af+ 0x804caba 78 F_219
af+ 0x804cb08 438 F_4107
af+ 0x804ccbe 2 F_220_Nice
af+ 0x804ccc0 2 F_4108
af+ 0x804ccc2 84 F_221
af+ 0x804cd16 240 F_4099
af+ 0x804ce10 258 F_4072
af+ 0x804cf18 48 F_4109
af+ 0x804cf80 614 F_4100
af+ 0x804d258 2 F_222_Nice
af+ 0x804d25a 2 F_4110
af+ 0x804d25c 2 F_223_Nice
af+ 0x804d25e 2 F_4111
af+ 0x804d260 2 F_224_Nice
af+ 0x804d262 2 F_4112
af+ 0x804d264 2 F_225_Nice
af+ 0x804d266 2 F_4113
af+ 0x804d268 208 Read_Channel_Switch
af+ 0x804d338 136 This_function_called_Read_Channel_Switch
CCa 0x804d35c get channel nr
CCa 0x804d3a4 set channel nr
af+ 0x804d3c0 36 F_622
CCa 0x804d3c6 set channel nr (init)
af+ 0x804d410 8 F_623
af+ 0x804d418 8 F_624
af+ 0x804d420 8 F_625
af+ 0x804d428 10 F_626
af+ 0x804d432 10 F_627
af+ 0x804d448 7540 F_628
af+ 0x804f1c0 124 F_82
af+ 0x804f28c 18 F_630
af+ 0x804f29e 150 F_883
CCa 0x804f2e0 Call RTC_RefClockCmd(FunctionalState NewState)
CCa 0x804f318 Call RTC_WakeUpClockConfig(uint32_t RTC_WakeUpClock)
CCa 0x804f31e Call RTC_SetWakeUpCounter(uint32_t RTC_WakeUpCounter)
CCa 0x804f324 Call RTC_RefClockCmd(FunctionalState NewState)
af+ 0x804f33c 2 F_631
af+ 0x804f340 10 F_884
af+ 0x804f34a 104 F_632
af+ 0x804f3b2 12 F_885
af+ 0x804f3dc 136 EXTI_Init
af+ 0x804f464 18 F_634
af+ 0x804f476 36 F_4217_Function_with_a_lot_of_ETXI
af+ 0x804f49a 6 F_229
af+ 0x804f4b4 80 F_230_Function_with_a_lot_of_ETXI
af+ 0x804f508 78 F_4074
af+ 0x804f556 138 F_4075
af+ 0x804f5e0 58 F_4076
af+ 0x804f61a 54 F_4073
af+ 0x804f650 78 F_231
af+ 0x804f6ac 224 F_886
af+ 0x804f78c 52 F_887
af+ 0x804f7c8 238 F_638
af+ 0x804f8b8 50 F_232
af+ 0x804f8ea 18 F_233
af+ 0x804f8fc 44 F_234
af+ 0x804f928 28 F_642
af+ 0x804f944 28 F_235
af+ 0x804f964 120 F_1159
af+ 0x804f9dc 32 F_1145
af+ 0x804f9fc 16 F_4453
af+ 0x804fa0c 34 F_4454
af+ 0x804fa2e 50 F_4045
af+ 0x804fa60 48 F_4077
af+ 0x804fb00 22 F_4078
af+ 0x804fb16 170 F_4079
af+ 0x804fbc0 10 F_4080
af+ 0x804fbca 90 F_4081
af+ 0x804fc28 40 F_4082
af+ 0x804fc50 22 F_5052
af+ 0x804fc66 6 F_5054
af+ 0x804fc6c 52 F_5053
af+ 0x804fca0 6 F_5072
af+ 0x804fca6 6 F_5073
af+ 0x804fcb0 234 I2C_Init
af+ 0x804fda8 28 I2C_Cmd
af+ 0x804fdc4 28 I2C_GenerateSTART
af+ 0x804fde0 28 I2C_GenerateSTOP
af+ 0x804fdfc 22 I2C_Send7bitAddress
af+ 0x804fe12 6 I2C_SendData
af+ 0x804fe18 6 I2C_ReceiveData
af+ 0x804fe1e 52 I2C_GetLastEvent
af+ 0x804fe52 66 I2C1_CheckEvent
af+ 0x804fe94 212 F_653
af+ 0x804ff74 244 alpu_xor
af+ 0x8050068 270 Function_Function_Called_Big_I2C_Function
af+ 0x8050176 2632 alpu_compare_cpu_crypt_vs_alpu_crypt
CCa 0x805017a first_8byte_seed_then_10byte_from_alpu
CCa 0x805023e secret
af+ 0x8050bcc 102 F_238
af+ 0x8050c40 2 F_239_Nice
af+ 0x8050c44 328 F_4522
af+ 0x8050d90 122 ambe_encode_thing__size_not_correct
af+ 0x8050e24 44 F_659
af+ 0x8050ed8 524 F_660
af+ 0x8051140 24 F_661
af+ 0x8051158 170 F_4529
af+ 0x8051248 66 ambe_decode_wav
af+ 0x80512f0 46 F_663
af+ 0x8051334 126 F_664
af+ 0x80513b4 226 F_4530
af+ 0x8051498 76 F_665
af+ 0x805151c 64 F_888
af+ 0x805155c 20 F_666
af+ 0x8051570 14 F_667
af+ 0x8051c58 60 F_4455
af+ 0x8051c94 38 F_4423
af+ 0x8051cba 226 F_4424
af+ 0x8051d9c 68 F_5022
af+ 0x8051de4 48 F_4425
af+ 0x8051e14 28 F_668
af+ 0x8051e30 418 F_669
af+ 0x8051fd4 582 F_670
af+ 0x805221c 144 F_671
af+ 0x80522ac 152 F_672
af+ 0x805235c 58 F_4473_wtf
af+ 0x80523a0 846 F_674
af+ 0x80526f0 878 F_675
af+ 0x8052a5e 266 F_676
af+ 0x8052b68 164 F_677
af+ 0x8052c0c 68 F_678
af+ 0x8052c50 4 F_679
af+ 0x8052e06 12 F_680
af+ 0x8052e9a 28 F_1027
af+ 0x8052eb6 16 F_1028
af+ 0x8052ee8 30 F_4483
af+ 0x8052f14 28 F_4484
af+ 0x8052f34 2 F_681
af+ 0x8052f38 84 F_682
af+ 0x8052f8c 82 F_683
af+ 0x8052fde 130 F_684
af+ 0x8053060 434 F_685
af+ 0x8053212 682 F_686
af+ 0x80534d4 98 F_687
af+ 0x8053538 98 F_688
af+ 0x805359c 70 F_689
af+ 0x80535e4 142 F_690
af+ 0x8053674 84 F_691
af+ 0x80536c8 78 F_692
af+ 0x8053718 158 F_693
af+ 0x80537b8 70 F_694
af+ 0x8053800 3606 F_4531
af+ 0x8054618 1484 F_4521
af+ 0x8054be4 214 F_4532
af+ 0x8054cbc 240 F_695
af+ 0x8054dac 98 F_4533
af+ 0x8054e10 548 F_4534
af+ 0x8055038 142 F_696
af+ 0x80550c8 150 F_697
af+ 0x8055168 6 F_698
af+ 0x8055170 108 F_699
af+ 0x8055258 254 F_700
af+ 0x805535c 68 F_701
af+ 0x80553a0 228 F_702
af+ 0x8055484 416 F_703
af+ 0x8055628 146 F_704
af+ 0x80556bc 64 F_705
af+ 0x80556fc 60 F_706
af+ 0x805573c 200 F_707
af+ 0x8055808 526 F_708
af+ 0x8055a20 46 F_709
af+ 0x8055a78 142 F_889
af+ 0x8055b64 28 F_890
af+ 0x8055b80 42 F_4535
af+ 0x8055bac 42 F_891
af+ 0x8055bd8 50 F_4536
af+ 0x8055c14 60 F_4537
af+ 0x8055c50 34 F_4538
af+ 0x8055c74 128 F_710
af+ 0x8055cf4 72 F_711
af+ 0x8055d3c 56 F_892
af+ 0x8055d74 1198 F_893
af+ 0x8056224 452 F_894
af+ 0x80563e8 50 F_712
af+ 0x805641c 188 F_713
af+ 0x8056580 62 F_714
af+ 0x80565c0 74 F_1029
af+ 0x8056640 58 F_715
af+ 0x8056680 120 F_716
af+ 0x8056700 130 F_895
af+ 0x8056800 120 F_717
af+ 0x8056880 48 F_718
af+ 0x80568c0 48 F_719
af+ 0x8056900 124 F_720
af+ 0x8056980 32 F_896
af+ 0x80569c0 130 F_721
af+ 0x8056a80 54 F_722
af+ 0x8056ac0 116 F_723
af+ 0x8056b40 44 F_724
af+ 0x8056b80 264 F_725
af+ 0x8056e4c 28 F_726
af+ 0x8056e68 104 F_727
af+ 0x8056ed4 42 F_728
af+ 0x8056efe 22 F_1088
af+ 0x8056f14 14 F_1030
af+ 0x8056f22 150 F_729
af+ 0x8056fb8 90 F_730
af+ 0x8057012 70 F_897
af+ 0x8057058 62 F_898
af+ 0x8057096 70 F_731
af+ 0x80570dc 70 F_899
af+ 0x8057122 20 F_1031
af+ 0x8057136 22 F_4067
af+ 0x805714c 22 F_4068
af+ 0x8057164 88 F_732
af+ 0x80571bc 80 F_733
af+ 0x805720c 360 F_734
af+ 0x8057374 296 F_900
af+ 0x805749c 88 F_901
af+ 0x80574f4 192 F_902
af+ 0x80575b4 56 F_903
af+ 0x80575ec 54 F_904
af+ 0x8057622 136 F_905
af+ 0x80576c0 52 F_906
af+ 0x80576f4 68 F_735
af+ 0x8057738 28 F_907
af+ 0x8057754 70 F_4046
af+ 0x805779a 20 F_4069
af+ 0x80577ae 40 usb_send_packet
af+ 0x80577d6 22 F_736
af+ 0x80577ec 40 F_4070
af+ 0x8057814 22 F_737
af+ 0x805782a 36 F_738
af+ 0x805784e 36 F_739
af+ 0x8057880 46 ambr_x01
af+ 0x80578c0 78 ambr_x02
af+ 0x8057940 118 ambr_001
# 0x8057940 --  39792 byte -Ambr Codec ---
af+ 0x80579c0 356 ambr_002
af+ 0x8057b40 162 ambr_003
af+ 0x8057c00 70 ambr_004
af+ 0x8057c80 70 ambr_005
af+ 0x8057d00 82 ambr_006
af+ 0x8057d5c 180 ambr_007
af+ 0x8057e10 36 ambr_008
af+ 0x8057e34 42 ambr_009
af+ 0x8057e80 30 ambr_010
af+ 0x8057ea0 32 ambr_011
af+ 0x8057ec0 38 ambr_012
af+ 0x8057f00 34 ambr_013
af+ 0x8057f24 20 ambr_014
af+ 0x8057f38 142 ambr_015
af+ 0x8057fc8 30 ambr_016
af+ 0x8057fe8 38 ambr_017
af+ 0x8058010 30 ambr_018
af+ 0x8058030 38 ambr_019
af+ 0x8058058 64 ambr_020
af+ 0x8058098 32 ambr_021
af+ 0x80580b8 162 ambr_022
af+ 0x805815c 20 ambr_023
af+ 0x8058170 68 ambr_024
af+ 0x80581b4 38 ambr_025
af+ 0x80581dc 62 ambr_026
af+ 0x805821c 52 ambr_027
af+ 0x8058250 52 ambr_028
af+ 0x8058284 122 ambr_029
af+ 0x8058300 50 ambr_030
af+ 0x8058334 56 ambr_031
af+ 0x805836c 80 ambr_032
af+ 0x80583bc 522 ambr_033
af+ 0x80585c8 3752 ambr_034
af+ 0x8059470 30 ambr_035
af+ 0x8059490 176 ambr_036
af+ 0x8059540 170 ambr_037
af+ 0x80595ec 220 ambr_038
af+ 0x80596c8 80 ambr_039
af+ 0x8059718 2630 ambr_040
af+ 0x805a160 74 ambr_041
af+ 0x805a1ac 602 ambr_042
af+ 0x805a408 612 ambr_043
af+ 0x805a66c 1380 ambr_044
af+ 0x805abd0 1338 ambr_045
af+ 0x805b10c 120 ambr_046
af+ 0x805b184 64 ambr_047
af+ 0x805b1c4 230 ambr_048
af+ 0x805b2ac 106 ambr_049
af+ 0x805b318 122 ambr_050
af+ 0x805b394 60 ambr_051
af+ 0x805b3d0 1812 ambr_052
af+ 0x805bae8 74 ambr_053
af+ 0x805bb34 126 ambr_054
af+ 0x805bbb4 502 ambr_055
af+ 0x805bdac 296 ambr_056
af+ 0x805bed4 594 ambr_057
af+ 0x805c128 4 ambr_058
af+ 0x805c12c 568 ambr_059
af+ 0x805c368 276 ambr_060
af+ 0x805c47c 404 ambr_061
af+ 0x805c610 20 ambr_062
af+ 0x805c624 182 ambr_063
af+ 0x805c6dc 156 ambr_064
af+ 0x805c778 108 ambr_065
af+ 0x805c7e4 18 ambr_066
af+ 0x805c7f8 1094 ambr_067
af+ 0x805cc40 86 ambr_068
af+ 0x805cc98 1084 ambr_069
af+ 0x805d0d4 22 ambr_070
af+ 0x805d0ec 44 ambr_071
af+ 0x805d11c 612 ambr_072
af+ 0x805d380 104 ambr_073
af+ 0x805d3e8 150 ambr_074
af+ 0x805d480 406 ambr_075
af+ 0x805d618 236 ambr_076
af+ 0x805d708 76 ambr_077
af+ 0x805d754 130 ambr_078
af+ 0x805d7d8 126 ambr_079
af+ 0x805d85c 66 ambr_080
af+ 0x805d8a4 1634 ambr_081
af+ 0x805df08 260 ambr_082
af+ 0x805e010 390 ambr_083
af+ 0x805e19c 440 ambr_084
af+ 0x805e354 560 ambr_085
af+ 0x805e584 1692 ambr_086
af+ 0x805ec20 240 ambr_087
af+ 0x805ed14 1114 ambr_088
af+ 0x805f180 32 ambr_089
af+ 0x805f1a0 98 ambr_090
af+ 0x805f202 64 ambr_091
af+ 0x805f242 42 ambr_092
af+ 0x805f26c 218 ambr_093
af+ 0x805f346 182 ambr_094
af+ 0x805f3fc 24 ambr_095
af+ 0x805f414 26 ambr_096
af+ 0x805f42e 70 ambr_097
af+ 0x805f474 62 ambr_098
af+ 0x805f4bc 54 ambr_099
af+ 0x805f4f2 10 ambr_100
af+ 0x805f4fc 20 ambr_101
af+ 0x805f510 18 ambr_102
af+ 0x805f522 16 ambr_103
af+ 0x805f532 368 ambr_104
af+ 0x805f6a2 88 ambr_105
af+ 0x805f6fa 44 ambr_106
af+ 0x805f726 74 ambr_107
af+ 0x805f770 114 ambr_108
af+ 0x805f7e2 358 ambr_109
af+ 0x805f948 294 ambr_110
af+ 0x805fa6e 62 ambr_111
af+ 0x805faac 62 ambr_112
af+ 0x805faea 14 ambr_113
af+ 0x805faf8 20 ambr_114
af+ 0x805fb0c 14 ambr_115
af+ 0x805fb1a 80 ambr_116
af+ 0x805fb6a 40 ambr_117
af+ 0x805fbc0 282 ambr_118
af+ 0x805fd00 178 ambr_119
af+ 0x805fdb8 182 ambr_120
af+ 0x805fe74 98 ambr_121
af+ 0x805fed8 294 ambr_122
af+ 0x8060000 268 ambr_123
af+ 0x806010c 238 ambr_124
af+ 0x8060200 162 ambr_125
af+ 0x80602a8 298 ambr_126
af+ 0x80603d8 68 ambr_127
af+ 0x806041c 72 ambr_128
af+ 0x8060464 86 ambr_129
af+ 0x80604ba 122 ambr_130
af+ 0x8060534 82 ambr_131
af+ 0x8060586 42 ambr_132
af+ 0x80605b0 42 ambr_133
af+ 0x80605da 326 ambr_134
af+ 0x8060720 214 ambr_135
af+ 0x8060920 770 ambr_136
af+ 0x8060c24 220 ambr_137
af+ 0x8060df0 72 ambr_138
af+ 0x8060e38 268 ambr_139
af+ 0x8060f4c 78 ambr_140
af+ 0x8060f9c 68 ambr_141
af+ 0x8060fe0 146 ambr_142
af+ 0x8061074 566 ambr_143
af+ 0x80612ac 138 ambr_144
af+ 0x8061340 252 ambr_145
af+ 0x8061440 50 ambr_146
af+ 0x8061480 14 ambr_147
af+ 0x80614a4 14 ambr_148
af+ 0x808cb84 30 F_4091
af+ 0x808cba4 38 F_4003
af+ 0x808cc00 190 F_4002
af+ 0x808ccbe 1444 usb_dnld_handle
CCa 0x808d082 0x2001d1a8 Time
CCa 0x808d088 0x2001d1a8 Time
CCa 0x808d092 0x2001d1a8 Time
CCa 0x808d09c 0x2001d1a8 Time
CCa 0x808d0a2 Call RTC_SetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct)
CCa 0x808d0a6 0x2001d1ac Date
CCa 0x808d0b0 0x2001d1ac Date
CCa 0x808d0ba 0x2001d1ac Date
CCa 0x808d0c4 0x2001d1ac Date
CCa 0x808d0c8 Call RTC_SetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct)
af+ 0x808d266 132 F_4055
af+ 0x808d2ea 134 F_4056
af+ 0x808d3d8 3036 usb_upld_handle
CCa 0x808d42c DFU_Command_Parser
CCa 0x808d5f0 0x2001d1ac Date
CCa 0x808d5f6 Call RTC_GetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct)
CCa 0x808d5fe 0x2001d1ac Date
CCa 0x808d60a 0x2001d1ac Date
CCa 0x808d616 0x2001d1ac Date
CCa 0x808d61e 0x2001d1a8 Time
CCa 0x808d624 Call RTC_GetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct)
CCa 0x808d62c 0x2001d1a8 Time
CCa 0x808d638 0x2001d1a8 Time
CCa 0x808d644 0x2001d1a8 Time
CCa 0x808d6a2 SystemConfig
CCa 0x808d6a6 ConfigData + x usb_upld_handle
CCa 0x808d96e SystemConfig
CCa 0x808d972 ConfigData + 0x4  (long) MyDMRID usb_upld_handle
CCa 0x808d976 SystemConfig
CCa 0x808d97a ConfigData + 0x4  (long) MyDMRID usb_upld_handle
CCa 0x808d984 SystemConfig
CCa 0x808d988 ConfigData + 0x4  (long) MyDMRID usb_upld_handle
CCa 0x808d9c8 SystemConfig
CCa 0x808d9cc ConfigData + x usb_upld_handle
af+ 0x808dfd0 520 F_4005
af+ 0x808e1dc 100 F_4057
af+ 0x808e244 12 F_4058
af+ 0x808e254 98 F_4059
af+ 0x808e2bc 96 F_4092
af+ 0x808e3a0 42 F_4060
af+ 0x808e3ca 42 F_4006
af+ 0x808e3f4 76 F_4007
af+ 0x808e440 80 F_4008
af+ 0x808e490 54 F_4009
af+ 0x808e4c6 96 F_4010
af+ 0x808e526 44 F_4011
af+ 0x8091fb8 106 F_4012
af+ 0x8092022 54 F_4013
af+ 0x8092058 342 F_4014
af+ 0x80921ae 284 F_4015
af+ 0x80922ca 416 F_4016
af+ 0x809246a 128 F_4017
af+ 0x80924ea 104 F_4018
af+ 0x8092552 416 F_4019
af+ 0x80926f2 148 F_4020
af+ 0x8092786 10 F_4021
af+ 0x8092790 194 F_4022
af+ 0x8092852 72 F_4023
af+ 0x809289a 90 F_4024
af+ 0x809290a 72 F_4061
af+ 0x8092954 242 F_4062
af+ 0x8092a4c 1916 F_4025
af+ 0x80931c8 210 F_4026
af+ 0x809329a 72 F_4027
af+ 0x80932e2 136 F_4028
af+ 0x809336a 244 F_4029
af+ 0x809345e 202 F_4030
af+ 0x8093528 28 F_4031
af+ 0x8093544 158 F_4032
af+ 0x80935e2 136 F_4033
af+ 0x809366a 186 F_4034
af+ 0x8093724 74 F_4035
af+ 0x809376e 28 F_4036
af+ 0x809378a 28 F_4037
af+ 0x80937b0 32 F_4063
af+ 0x80937f0 8 F_5047_uncertain
af+ 0x809381a 2 F_5048_uncertain
af+ 0x809381c 34 F_5003
CCa 0x809381e Call OS_ENTER_CRITICAL()
CCa 0x8093830 Call OS_EXIT_CRITICAL()
af+ 0x809383e 50 F_5004
CCa 0x809383e InterruptFunc Internal Perf
af+ 0x8093870 20 F_5005
af+ 0x8093884 20 F_5006
af+ 0x8093898 134 F_5007
CCa 0x8093914 0x10 PC4 Set RF_APC_SW RF Amplifier Switch.High Avtive
af+ 0x809391e 28 F_5008
af+ 0x809393a 58 F_5009
af+ 0x8093974 20 F_5010
CCa 0x8093974 Interrupt Func()
af+ 0x8093ae4 64 F_5029
CCa 0x8093b24 0x0800c0ee Interrupt Func
af+ 0x8093b24 16 F_5036
CCa 0x8093b2a Update interrupt flag (Clear) UIF TIMx_SR
af+ 0x8093b34 56 F_5011_wtf
CCa 0x8093b36 Call OS_ENTER_CRITICAL()
CCa 0x8093b44 Call OS_EXIT_CRITICAL()
af+ 0x8093c54 82 F_4219
af+ 0x8093ca6 202 F_4282
af+ 0x809558c 206 F_5002
af+ 0x80956a0 52 F_5001
af+ 0x80956d4 98 F_5000
af+ 0x809573c 366 LED_Process
CCa 0x8095740 SystemConfig
CCa 0x8095742 ConfigData + 0x1  (byte) ubfx r0, r0, 2, 1 .. lsls r0, r0, 0x1f LED_Process
CCa 0x80957ac SystemConfig
CCa 0x80957ae ConfigData + 0x0  (byte) LED_Process
af+ 0x80969de 16 F_4001
af+ 0x80969ee 34 F_4539
af+ 0x8096a10 34 F_4540
af+ 0x8096a32 34 F_4541
af+ 0x8096c0c 12 F_4545
af+ 0x8096c18 76 F_4542
af+ 0x8096c64 50 F_4543
af+ 0x8096c96 56 F_4544
af+ 0x80f96b0 6 F_5074
f ConfigData 0x50 @ 0x2001c658
f WelcomeBMP 1 @ 0x80f9ca8 # size is not correct
f WelcomeLine1 4 @ 0x2001cecc
fC WelcomeLine1 *Welcome_Line1
f WelcomeLine2 4 @ 0x2001cee0

