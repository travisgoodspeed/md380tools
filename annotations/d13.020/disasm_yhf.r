# Radare2 script to disassemble the "D13.020" firmware.
#   Based on the md380tool's "flash.r" + "cpu.r", but modfied
#   by DL4YHF to produce a listing with only the "interesting" parts.
#    
# 'f' name length @ address .  The '@' seems to be optional, e.g. radare2book.pdf page 129 of 216 
f VectorTable 0x188 @ 0x0800C000

f DummyForUnusedIRQs 32 @ 0x08093ed0 # called as subroutine from BusFault_Handler and others
af+ (DummyForUnusedIRQs) 32 DummyForUnusedIRQs # required to see the SYMBOL as operand for 'bl'
   # Because we're not analysing an EXE or ELF, Radare2 didn't 
   # automatically find out the size of functions (in bytes).
   # So look at the raw disassembly (in r2's "Visual" mode) to determine the sizes.
   # Making the size (in "af+") too large made r2 run wild.

f HandlerForDMA2_Stream3 98 @ 0x8094270
f NextAfterHandlers 1 @ 0x080942d2
af+ (HandlerForDMA2_Stream3) 98 HandlerForDMA2_Stream3 # required to see the SYMBOL as operand for 'bl'
# The stuff beginning at NextAfterHandlers looks like 'non-code' so declare it as data .
#  (the question was HOW. "Add Metadata" (C?), turn this into a section (S?), ..)
# > The C command allows to change type for a byte range. Three basic types are:
# > code (disassembly is done using asm.arch), data (a byte array) or string.
s NextAfterHandlers # wind back to the address of WHAT WB THOUGHT were 'data' (const)
CC const data in the interrupt handler section
Cd 98 # show these N bytes (at the current address) as hexdump, when listing them with 'pd' (!) later

s VectorTable # wind back to the VT and annotate it for the listing..
CC STM32F405 exception- and interrupt vector table
Cd 0x188 # RM0090 page 372 shows last vector of STM32F405 at VT-offset 0x0184, thus 0x0188 bytes 


# Read the addresses of certain interrupt handlers 
#      directly from the vector table, to annotate them below.
# From radare2-explorations.pdf : 
#  > One nifty way to seek to that address is to use the output of the previous command.
# Below, '(s VectorTable+4)' winds to the address of the RESET VECTOR in the VT .
# 'pxw 4' would then dump the address followed by the value (result: 0x0800c004  0x080f9245). 
# The tilde is a kind of grep to process the previous output,
# and the output can be accessed like an array (whitespace delimited). 
#  > If we then surround the expression with backticks (`), then it
#  > will be expanded to its value when executed, similar to bash.
s (VectorTable+0x04) # seek_addr = VectorTable+4 = address of the RESET HANDLER
s `pxw 4~[1]`      # seek_addr = *(DWORD*)(seek_addr);
# ex: f Reset_Handler # Reset_Handler = seek_addr;
   # if all went well, and you found out how to type a backtick on your keyboard,
   # the result (value of Reset_Handler) will now be 0x080F9245 (in the "D13.020" firmware) .
   # Radare2 doesn't know that the LSBit in 0x080F9245 indicates 'Thumb' code,
   # but the real interrupt service handler begins at an EVEN ADDRESS. Help him out:
s- 1 # seek_addr -= 1;  should be the 'real' (EVEN) handler address now,
     # example (Reset_Handler): $$ = 0x080f9244 . Let Radare2 treat this like a function
   # "af+ addr size name [type] [diff]" :  hand craft a function (***requires afb+ ***)
   #      ... but wtf is "afb+", and why does "af+" REQUIRE it ? 
   # "afb+ fa a sz [j] [f] ([t]( [d]))" :  add bb to function @ fcnaddr   
   #   (finding out what the parameters mean is a nightmare. Solution may be hidden in radare2book.)
f Reset_Handler 8 @ $$ # Reset_Handler = seek_addr (as a "flag")
af+ $$ 8 Reset_Handler # put the SIZE IN BYTES after the '$$' if you know it
   # The reset-handler in Tytera's firmware looks very much the same
   #     as the startup used for an LPC1788 in Keil's "MDK-ARM",
   #     where Reset_Handler for almost any Cortex-M look like this: 
   # Reset_Handler PROC         ; stolen from the startup for an LPC1788
   #               IMPORT  SystemInit
   #               IMPORT __main
   #   LDR     R0, =SystemInit  ; good place for an 'early' breakpoint
   #   BLX     R0
   #   LDR     R0, =__main      ; this is NOT the "real main" but Keil's scatterload-thingy 
   #   BX      R0               ; size of this minimalistic 'Reset_Handler' = 8 bytes
   #           |__  "looks like a call" (places the next address in LR for returning)
   #                but since __main (not main()!!) never returns, 
   #                this marks THE END of the 'Reset_Handler' !
# Address offsets of these handlers from RM0090 Rev 7 pages 369 to 372,
# but names (when implemented at all) are compatible with startup_stm32f4xx.s !
s (VectorTable+0x08) # seek_addr = VectorTable + offs(NMI_Handler)
s `pxw 4~[1]`
s- 1
f NMI_Handler
#af+ $$ 4 NMI_Handler

s (VectorTable+0x0C) # HardFault..
s `pxw 4~[1]`
s- 1
f HardFault_Handler
#af+ $$ 4 HardFault_Handler

s (VectorTable+0x10) # MemManage..
s `pxw 4~[1]`
s- 1
f MemManage
#af+ $$ 4 MemManage

s (VectorTable+0x14) # BusFault..
s `pxw 4~[1]`
s- 1
f BusFault_Handler
#af+ $$ 4 BusFault_Handler

s (VectorTable+0x18) # UsageFault..
s `pxw 4~[1]`
s- 1
f UsageFault_Handler
#af+ $$ 8 UsageFault_Handler

s (VectorTable+0x2C) # SVC_Handler.. (syscall via 'SWI' = Soft Ware Interrupt)
s `pxw 4~[1]`
s- 1
f SVC_Handler
#af+ $$ 8 SVC_Handler

s (VectorTable+0x30) # DebugMon_Handler..
s `pxw 4~[1]`
s- 1
f DebugMon_Handler
#af+ $$ 8 DebugMon_Handler

s (VectorTable+0x38) # PendSV_Handler..
s `pxw 4~[1]`
s- 1
f PendSV_Handler
#af+ $$ 8 PendVC_Handler

s (VectorTable+0x3C) # seek_addr = VectorTable+4*15 = address of SysTick_Handler
s `pxw 4~[1]`      # seek_addr = *(DWORD*)(seek_addr);
s- 1               # seek_addr -= 1;  should be the 'real' handler address now
f SysTick_Handler  # SysTick_Handler = seek_addr (as a "flag")
#af+ $$ 0x20 SysTick_Handler

s (VectorTable+0x40) # WWDG_Handler.. (gefensterter Wachhund)
s `pxw 4~[1]`
s- 1
f WWDG_IRQHandler
#af+ $$ 8 WWDG_IRQHandler

s (VectorTable+0x44) # PVD_Handler..
s `pxw 4~[1]`
s- 1
f PVD_IRQHandler
#af+ $$ 8 PVD_IRQHandler

s (VectorTable+0x48) # TAMP_STAMP_Handler.. (tampern und stampfen, sehr schön) 
s `pxw 4~[1]`
s- 1
f TAMP_STAMP_IRQHandler
#af+ $$ 8 TAMP_STAMP_IRQHandler

s (VectorTable+0x4C) # RTC_WKUP_Handler..
s `pxw 4~[1]`
s- 1
f RTC_WKUP_IRQHandler
af+ $$ 56 RTC_WKUP_IRQHandler # <- to see the SYMBOL as operand instead of a hex value

s (VectorTable+0x50) # FLASH_IRQHandler..
s `pxw 4~[1]`
s- 1
f FLASH_IRQHandler
#af+ $$ 8 FLASH_IRQHandler

s (VectorTable+0x54) # RCC_IRQHandler..
s `pxw 4~[1]`
s- 1
f RCC_IRQHandler
#af+ $$ 8 RCC_IRQHandler

s (VectorTable+0x58) # EXTI0_IRQHandler..
s `pxw 4~[1]`
s- 1
f EXTI0_IRQHandler
#af+ $$ 8 EXTI0_IRQHandler

s (VectorTable+0x5C) # EXTI1_IRQHandler..
s `pxw 4~[1]`
s- 1
f EXTI1_IRQHandler
#af+ $$ 8 EXTI1_IRQHandler

s (VectorTable+0x60) # EXTI2_IRQHandler..
s `pxw 4~[1]`
s- 1
f EXTI2_IRQHandler
#af+ $$ 8 EXTI2_IRQHandler

s (VectorTable+0x64) # EXTI3_IRQHandler..
s `pxw 4~[1]`
s- 1
f EXTI3_IRQHandler
#af+ $$ 8 EXTI3_IRQHandler

s (VectorTable+0x68) # EXTI4_IRQHandler..
s `pxw 4~[1]`
s- 1
f EXTI4_IRQHandler
#af+ $$ 8 EXTI4_IRQHandler

s (VectorTable+0x6C) # DMA1_Stream0_IRQHandler..
s `pxw 4~[1]`
s- 1
f DMA1_Stream0_IRQHandler
#af+ $$ 8 DMA1_Stream0_IRQHandler

s (VectorTable+0x70) # DMA1_Stream1_IRQHandler..
s `pxw 4~[1]`
s- 1
f DMA1_Stream1_IRQHandler
#af+ $$ 8 DMA1_Stream1_IRQHandler

s (VectorTable+0x74) # DMA1_Stream2_IRQHandler..
s `pxw 4~[1]`
s- 1
f DMA1_Stream2_IRQHandler
#af+ $$ 8 DMA1_Stream2_IRQHandler

s (VectorTable+0x78) # DMA1_Stream3_IRQHandler..
s `pxw 4~[1]`
s- 1
f DMA1_Stream3_IRQHandler
#af+ $$ 8 DMA1_Stream3_IRQHandler

s (VectorTable+0x7C) # DMA1_Stream4_IRQHandler..
s `pxw 4~[1]`
s- 1
f DMA1_Stream4_IRQHandler
#af+ $$ 8 DMA1_Stream4_IRQHandler

s (VectorTable+0x80) # DMA1_Stream5_IRQHandler..
s `pxw 4~[1]`
s- 1
f DMA1_Stream5_IRQHandler
#af+ $$ 8 DMA1_Stream5_IRQHandler

s (VectorTable+0x84) # DMA1_Stream6_IRQHandler..
s `pxw 4~[1]`
s- 1
f DMA1_Stream6_IRQHandler
#af+ $$ 8 DMA1_Stream6_IRQHandler

s (VectorTable+0x88) # ADC_IRQHandler..
s `pxw 4~[1]`
s- 1
f ADC_IRQHandler
#af+ $$ 8 ADC_IRQHandler

s (VectorTable+0x8C) # CAN1_TX_IRQHandler..
s `pxw 4~[1]`
s- 1
f CAN1_TX_IRQHandler
#af+ $$ 4 CAN1_TX_IRQHandler

s (VectorTable+0x90) # CAN1_RX0_IRQHandler..
s `pxw 4~[1]`
s- 1
f CAN1_RX0_IRQHandler
#af+ $$ 4 CAN1_RX0_IRQHandler

s (VectorTable+0x94) # CAN1_RX1_IRQHandler..
s `pxw 4~[1]`
s- 1
f CAN1_RX1_IRQHandler
#af+ $$ 4 CAN1_RX1_IRQHandler

s (VectorTable+0x98) # CAN1_SCE_IRQHandler..
s `pxw 4~[1]`
s- 1
f CAN1_SCE_IRQHandler
#af+ $$ 4 CAN1_SCE_IRQHandler

s (VectorTable+0x9C) # EXTI9_5_IRQHandler..
s `pxw 4~[1]`
s- 1
f EXTI9_5_IRQHandler
#af+ $$ 8 EXTI9_5_IRQHandler

s (VectorTable+0xA0) # TIM1_BRK_TIM9_IRQHandler..
s `pxw 4~[1]`
s- 1
f TIM1_BRK_TIM9_IRQHandler
#af+ $$ 8 TIM1_BRK_TIM9_IRQHandler

s (VectorTable+0xA4) # TIM1_UP_TIM10_IRQHandler..
s `pxw 4~[1]`
s- 1
f TIM1_UP_TIM10_IRQHandler
#af+ $$ 8 TIM1_UP_TIM10_IRQHandler

s (VectorTable+0xA8) # TIM1_TRG_COM_TIM11_IRQHandler..
s `pxw 4~[1]`
s- 1
f TIM1_TRG_COM_TIM11_IRQHandler
#af+ $$ 8 TIM1_TRG_COM_TIM11_IRQHandler

s (VectorTable+0xAC) # TIM1_CC_IRQHandler..
s `pxw 4~[1]`
s- 1
f TIM1_CC_IRQHandler
#af+ $$ 8 TIM1_CC_IRQHandler

s (VectorTable+0xB0) # TIM2_IRQHandler..
s `pxw 4~[1]`
s- 1
f TIM2_IRQHandler

af+ $$ 8 TIM3_IRQHandler
s (VectorTable+0xB4) # TIM3_IRQHandler..
s `pxw 4~[1]`
s- 1
f TIM3_IRQHandler
#af+ $$ 8 TIM3_IRQHandler

s (VectorTable+0xB8) # TIM4_IRQHandler..
s `pxw 4~[1]`
s- 1
f TIM4_IRQHandler
#af+ $$ 8 TIM4_IRQHandler

s (VectorTable+0xBC) # I2C1_EV_IRQHandler..
s `pxw 4~[1]`
s- 1
f I2C1_EV_IRQHandler
#af+ $$ 8 I2C1_EV_IRQHandler

s (VectorTable+0xC0) # I2C1_ER_IRQHandler..
s `pxw 4~[1]`
s- 1
f I2C1_ER_IRQHandler
#af+ $$ 8 I2C1_ER_IRQHandler

s (VectorTable+0xC4) # I2C2_EV_IRQHandler..
s `pxw 4~[1]`
s- 1
f I2C2_EV_IRQHandler
#af+ $$ 8 I2C2_EV_IRQHandler

s (VectorTable+0xC8) # I2C2_ER_IRQHandler..
s `pxw 4~[1]`
s- 1
f I2C2_ER_IRQHandler
#af+ $$ 8 I2C2_ER_IRQHandler

s (VectorTable+0xCC) # SPI1_IRQHandler..
s `pxw 4~[1]`
s- 1
f SPI1_IRQHandler
#af+ $$ 8 SPI1_IRQHandler

s (VectorTable+0xD0) # SPI2_IRQHandler..
s `pxw 4~[1]`
s- 1
f SPI2_IRQHandler
#af+ $$ 8 SPI2_IRQHandler

s (VectorTable+0xD4) # USART1_IRQHandler..
s `pxw 4~[1]`
s- 1
f USART1_IRQHandler
#af+ $$ 8 USART1_IRQHandler

s (VectorTable+0xD8) # USART2_IRQHandler..
s `pxw 4~[1]`
s- 1
f USART2_IRQHandler
#af+ $$ 8 USART2_IRQHandler

s (VectorTable+0xDC) # USART3_IRQHandler..
s `pxw 4~[1]`
s- 1
f USART3_IRQHandler
#af+ $$ 8 USART3_IRQHandler

s (VectorTable+0xE0) # EXTI15_10_IRQHandler..
s `pxw 4~[1]`
s- 1
f EXTI15_10_IRQHandler
#af+ $$ 8 EXTI15_10_IRQHandler

s (VectorTable+0xE4) # RTC_Alarm_IRQHandler..
s `pxw 4~[1]`
s- 1
f RTC_Alarm_IRQHandler
#af+ $$ 8 RTC_Alarm_IRQHandler

s (VectorTable+0xE8) # OTG_FS_WKUP_IRQHandler..
s `pxw 4~[1]`
s- 1
f OTG_FS_WKUP_IRQHandler
#af+ $$ 8 OTG_FS_WKUP_IRQHandler

s (VectorTable+0xEC) # TIM8_BRK_TIM12_IRQHandler..
s `pxw 4~[1]`
s- 1
f TIM8_BRK_TIM12_IRQHandler
#af+ $$ 8 TIM8_BRK_TIM12_IRQHandler

s (VectorTable+0xF0) # TIM8_UP_TIM13_IRQHandler..
s `pxw 4~[1]`
s- 1
f TIM8_UP_TIM13_IRQHandler
#af+ $$ 8 TIM8_UP_TIM13_IRQHandler

s (VectorTable+0xF4) # TIM8_TRG_COM_TIM14_IRQHandler..
s `pxw 4~[1]`
s- 1
f TIM8_TRG_COM_TIM14_IRQHandler
#af+ $$ 8 TIM8_TRG_COM_TIM14_IRQHandler

s (VectorTable+0xF8) # TIM8_CC_IRQHandler..
s `pxw 4~[1]`
s- 1
f TIM8_CC_IRQHandler
#af+ $$ 8 TIM8_CC_IRQHandler

s (VectorTable+0xFC) # DMA1_Stream7_IRQHandler..
s `pxw 4~[1]`
s- 1
f DMA1_Stream7_IRQHandler
#af+ $$ 8 DMA1_Stream7_IRQHandler

s (VectorTable+0x0100) # FSMC_IRQHandler..
s `pxw 4~[1]`
s- 1
f FSMC_IRQHandler
#af+ $$ 8 FSMC_IRQHandler

s (VectorTable+0x104) # SDIO_IRQHandler..
s `pxw 4~[1]`
s- 1
f SDIO_IRQHandler
#af+ $$ 8 SDIO_IRQHandler

s (VectorTable+0x108) # TIM5_IRQHandler..
s `pxw 4~[1]`
s- 1
f TIM5_IRQHandler
#af+ $$ 8 TIM5_IRQHandler

s (VectorTable+0x10C) # SPI3_IRQHandler..
s `pxw 4~[1]`
s- 1
f SPI3_IRQHandler
#af+ $$ 8 SPI3_IRQHandler

s (VectorTable+0x110) # USART4_IRQHandler..
s `pxw 4~[1]`
s- 1
f USART4_IRQHandler
#af+ $$ 8 USART4_IRQHandler

s (VectorTable+0x114) # USART5_IRQHandler..
s `pxw 4~[1]`
s- 1
f USART5_IRQHandler
#af+ $$ 8 USART5_IRQHandler

s (VectorTable+0x118) # TIM6_DAC_IRQHandler..
s `pxw 4~[1]`
s- 1
f TIM6_DAC_IRQHandler
#af+ $$ 8 TIM6_DAC_IRQHandler

s (VectorTable+0x11C) # TIM7_DAC_IRQHandler..
s `pxw 4~[1]`
s- 1
f TIM7_DAC_IRQHandler
#af+ $$ 8 TIM7_DAC_IRQHandler

#omitted : DMA2, RTH, CAN,

s (VectorTable+0x14C) # OTG_FS_IRQHandler..
s `pxw 4~[1]`
s- 1
f OTG_FS_IRQHandler
#af+ $$ 8 OTG_FS_IRQHandler

s (VectorTable+0x15C) # USART6_IRQHandler.. not used by Tytera, abused for backlight-PWM by DL4YHF
s `pxw 4~[1]`
s- 1
f USART6_IRQHandler
af+ $$ 8 USART6_IRQHandler

s (VectorTable+0x160) # I2C3_EV_IRQHandler..
s `pxw 4~[1]`
s- 1
f I2C3_EV_IRQHandler
#af+ $$ 8 I2C3_EV_IRQHandler

s (VectorTable+0x164) # I2C3_ER_IRQHandler..
s `pxw 4~[1]`
s- 1
f I2C3_ER_IRQHandler
#af+ $$ 8 I2C3_ER_IRQHandler

s (VectorTable+0x168) # OTG_HS_EP1_OUT_IRQHandler..
s `pxw 4~[1]`
s- 1
f OTG_HS_EP1_OUT_IRQHandler
#af+ $$ 8 OTG_HS_EP1_OUT_IRQHandler

s (VectorTable+0x16C) # OTG_HS_EP1_IN_IRQHandler..
s `pxw 4~[1]`
s- 1
f OTG_HS_EP1_IN_IRQHandler
#af+ $$ 8 OTG_HS_EP1_IN_IRQHandler

s (VectorTable+0x170) # OTG_HS_WKUP_IRQHandler..
s `pxw 4~[1]`
s- 1
f OTG_HS_WKUP_IRQHandler
#af+ $$ 8 OTG_HS_WKUP_IRQHandler

s (VectorTable+0x174) # OTG_HS_IRQHandler..
s `pxw 4~[1]`
s- 1
f OTG_HS_IRQHandler
#af+ $$ 8 OTG_HS_IRQHandler

s (VectorTable+0x178) # DCMI_IRQHandler..
s `pxw 4~[1]`
s- 1
f DCMI_IRQHandler
#af+ $$ 8 DCMI_IRQHandler

s (VectorTable+0x17C) # CRYP_IRQHandler..
s `pxw 4~[1]`
s- 1
f CRYP_IRQHandler
#af+ $$ 8 CRYP_IRQHandler

s (VectorTable+0x180) # HASH_RNG_IRQHandler..
s `pxw 4~[1]`
s- 1
f HASH_RNG_IRQHandler
#af+ $$ 8 HASH_RNG_IRQHandler

s (VectorTable+0x184) # FPU_IRQHandler..
s `pxw 4~[1]`
s- 1
f FPU_IRQHandler
#af+ $$ 8 FPU_IRQHandler
# Phew. And those weren't even ALL interrupts yet !



# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# BELOW: symbols converted by 'make', from md380tools/applet/src/symbols_d13.020 .
#        Editing the following section BY HAND is useless ! 
#        Instead, copy them from annotations/d13.020/flash_new.tmp (after 'make) .
s 0x080417e0
af+ $$ 4 dmr_CSBK_handler # dl4yhf this was ONE of the culprits for an error from R2 (already def'd)
f dmr_CSBK_handler 4 @ $$

s 0x0800d69c
af+ $$ 4 disp_something # dl4yhf this was the 2nd culprit for an error from R2 (already def'd)
f disp_something 4 @ $$

   
f md380_menu_id @ 0x2001e915
f md380_menu_mem_base @ 0x2001b274
f md380_menu_memory @ 0x2001d5cc
f mn_editbuffer_poi @ 0x200049fc
f md380_menu_edit_buf @ 0x2001cb9a
f md380_menu_0x2001d3f0 @ 0x2001e946
f md380_menu_depth @ 0x20004acc
f md380_menu_0x2001d3ef @ 0x2001e945
f md380_menu_0x2001d3f1 @ 0x2001e947

f md380_menu_0x2001d3ee @ 0x2001e944
f md380_menu_0x2001d3f4 @ 0x2001e94a
f md380_menu_0x2001d3ed @ 0x2001e943



# 'af+ addr size name [type] [diff]'  hand craft a function (requires afb+)
#      (but where is the 'afb+' ?  )
af+ 0x800c188 1446 md380_create_main_menu_entry
af+ 0x800c72e 86 md380_create_menu_entry
af+ 0x800fc84 18 md380_menu_entry_back

f menu_draw_something @ 0x0802872c
af+ 0x0802872c 4 menu_draw_something

f menu_draw_something2 @ 0x08028be8
af+ 0x08028be8 4 menu_draw_something2

f menu_draw_something3 @ 0x08027fcc
af+ 0x08027fcc 36 menu_draw_something3

f menu_draw_something4 @ 0x0802802a
af+ 0x0802802a 84 menu_draw_something4

f menu_draw_something5 @ 0x080280d2
af+ 0x080280d2 62 menu_draw_something5


f menu_set_something @ 0x08027f90
af+ 0x08027f90 54 menu_set_something

f menu_flag_something @ 0x2001e87f




af+ 0x80134a0 408 Create_Menu_Utilies
CCa 0x801351e R.a.d.i.o...S.e
# CCa 0x801351e R.a.d.i.o...S.e
CCa 0x801354e R.a.d.i.o...I.n.
CCa 0x801359a P.r.o.g.r.a.m..
CCa 0x80135ce G.P.S./.B.e.i.D
CCa 0x8013602 R.X...G.P.S.I.n.
af+ 0x80136c0 930 md380_menu_entry_programradio
CCa 0x8013710 E.d.i.t...C.h.a.
CCa 0x8013762 R.x...F.r.e.q.u
CCa 0x80137ba C.h.a.n.n.e.l.
CCa 0x80137e8 T.i.m.e...O.u.t
CCa 0x8013824 C.T.C./.D.C.S
CCa 0x8013890 C.o.l.o.r...C.o
CCa 0x80138be R.e.p.e.a.t.e.r.
CCa 0x80138ec T.x.C.o.n.t.a.
CCa 0x801391a G.r.o.u.p.L.i.s
CCa 0x801394a C.o.l.o.r...C.o
CCa 0x8013978 R.e.p.e.a.t.e.r


CCa 0x8013a1e E.n.t.e.r...P.
af+ 0x80156a4 104 Create_Menu_Entry_RX_QRG_shown
af+ 0x8015720 220 Create_Menu_Entry_RX_QRG_1


CCa 0x80157e2 Create_Menu_Entry_RX_QRG_2
CCa 0x80157e6 Create_Menu_Entry_RX_QRG_3

# dl4yhf "Cannot add function (duplicsted)" caused by the line below ?
af+ 0x80157fc 126 Create_Menu_Entry_RX_QRG_2


af+ 0x801587a 102 Create_Menu_Entry_RX_QRG_3
CCa 0x80158c4 Create_Menu_Entry_RX_QRG_4
af+ 0x8015900 628 Create_Menu_Entry_RX_QRG_4
f Create_Menu_Entry_RX_QRG_4 @ 0x8015900


af+ 0x8017cbc 1404 Create_Menu_Entry_RadioSettings

CCa 0x8017f48 I.n.t.r.o...S.c.
CCa 0x8017f94 K.e.y.p.a.d...L.
CCa 0x8017fcc L.a.n.g.u.a.g.e
CCa 0x8018018 L.E.D...I.n.d.i.
af+ 0x8018b28 236 md380_itow


af+ 0x801ad56 246 Create_Menu_Entry_LEDIndicator


af+ 0x801b042 26 md380_menu_numerical_input

#CCa : adds a comment for a given address (this is NOT the reason for "Cannot add function")
CCa 0x801b8e8 md380_menu_0x2001d3f1


af+ 0x801fe5c 5454 f_4225
af+ 0x802256a 324 aes_startup_check
af+ 0x80226c0 18 Get_Welcome_Line1_from_spi_flash
af+ 0x80226d2 18 Get_Welcome_Line2_from_spi_flash




af+ 0x8023ee4 394 Edit_Message_Menu_Entry


af+ 0x8025ae4 888 F_4315


af+ 0x802b3f6 80 md380_RTC_GetTime
af+ 0x802b50c 76 md380_RTC_GetDate

#also defined further below:
af+ 0x802dfbc 1908 md380_f_4137


af+ 0x802f9dc 4056 Beep_Process

CCa 0x802fa36 re issue 227
CCa 0x802fc1e beep 9
CCa 0x802fa54 no dmr sync tone
CCa 0x802fa66 dmr sync
CCa 0x802fad8 roger beep
CCa 0x802fbe4 beginn roger beep
CCa 0x802fd54 beginn dmr sync

f bp_2001e8a7 @ 0x2001e8a7
f bp_freq2 @ 0x2001e6c4
f bp_freq @ 0x2001e6c0


f bp_set_freq @ 0x08030ad8
af+ 0x8030ad8 16 bp_set_freq
af+ 0x8030b58 72 bp_tone_off
f bp_tone_on @ 0x08030b08
af+ 0x8030b08 80 bp_tone_on
f bp_glisando @ 0x8030ae8
af+ 0x8030ae8 32 bp_glisando
f bp_sempost @ 0x0802f994
af+ 0x802f994 36 bp_sempost
f bp_sempost2 @ 0x0802f9b8
af+ 0x802f9b8 24 bp_sempost2

af+ 0x80309b8 236 Sub2CalledFromTIM8ISR # dl4yhf 2017-01-03, ends at 8030aa2 (?), called from TIM8 handler. Tone-generator ?
af+ 0x8030aa4 52 F_293


f md380_OSMboxPend @ 0x8031084
af+ 0x8031084 258 md380_OSMboxPend
af+ 0x803119c 86 md380_OSMboxPost

af+ 0x8031276 52 md380_spiflash_sektor_erase4k
af+ 0x80312aa 52 md380_spiflash_block_erase64k
af+ 0x80312de 76 spiflash_program_page
af+ 0x803132a 332 F_1069_spiflash_multiple_spiflash_program_page
af+ 0x8031476 70 md380_spiflash_read
af+ 0x80314bc 58 md380_spi_sendrecv
af+ 0x80314f6 18 spiflash_write_enable
af+ 0x8031508 34 md380_spiflash_wait
af+ 0x803152a 28 md380_spiflash_enable
af+ 0x8031546 24 md380_spiflash_disable
af+ 0x803155e 704 md380_spiflash_write
af+ 0x8031830 52 spiflash_Erase_Security_Registers_44h
af+ 0x8031864 76 spiflash_Program_Security_Registers_42h
af+ 0x80318b0 78 md380_spiflash_security_registers_read
af+ 0x8033eb4 104 OSTimeDly

af+ 0x8033ca6 (0xB4-0xA6) gfx_call_via_ptr__GfxInfoPlus0x18
af+ 0x8033cba (0xC4-0xBA) gfx_set_something_in_GfxInfoPlus0x24
af+ 0x8033cc4 (0xCC-0xC4) gfx_store_something_in_GfxInfoPlus0x22
af+ 0x8033cd0 (0xCE8-0xCD0) func_3cd0
af+ 0x8033ce8 (0xD42-0xCE8) func_3ce8
af+ 0x8033d42 (0xD76-0xD42) func_3d42
af+ 0x8033d78 (0xDAC-0xD78) SomeBitbangingOnGPIOD

af+ 0x803b39a 144 main_menu
af+ 0x803f708 76 OSSemCreate
af+ 0x803f754 218 OSSemPend
af+ 0x803f844 92 OSSemPost

f F_414 @ 0x8040670
af+ 0x8040670 880 F_414
af+ 0x8040a02 740 dmr_call_start
af+ 0x8040ce6 92 dmr_before_squelch
af+ 0x8040d44 94 F_858


af+ 0x8040de0 1540 dmr_sms_arrive
af+ 0x8041430 864 dmr_call_end

af+ 0x8043de4 8 OS_ENTER_CRITICAL
CCa 0x8043de4 Not 'invalid' but 'mrs r0, PRIMASK' !
af+ 0x8043dec 6 OS_EXIT_CRITICAL
CCa 0x8043dec Not 'invalid' but 'msr PRIMASK, r0' !
af+ 0x80462bc 314 Start
af+ 0x8046520 684 Start_multiple_tasks
af+ 0x8049e14 798 Start_2_more_tasks__init_vocoder_tasks__Q
CCa 0x804c74c md380_menu_edit_buf
#dl4yhf md380_menu_edit_buf defined twice, but "CCa" doesn't seem to care
CCa 0x804c76e md380_menu_edit_buf
af+ 0x804dd70 430 dmr_handle_data
af+ 0x804e580 204 OSTaskCreateExt
af+ 0x804e64c 90 OSTaskNameSet
af+ 0x804eb64 152 md380_f_4098
af+ 0x804ec66 298 md380_f_4102
CCa 0x804ee2c md380_menu_0x200011e4
af+ 0x80531d8 124 ambe_encode_thing__size_not_correct
af+ 0x8053680 140 ambe_decode_wav
af+ 0x8055100 68 usb_setcallbacks
af+ 0x8059b02 40 usb_send_packet
af+ 0x808eb30 190 usb_do_setup
af+ 0x808ebee 1444 usb_dnld_handle
af+ 0x808f308 3036 usb_upld_handle
af+ 0x8090370 80 usb_dfu_write
af+ 0x80903c0 54 usb_dfu_read
af+ 0x809662e 34 usb_serialnumber

f menu_entry_back_1 @ 0x800fc85

f md380_radio_config @ 0x2001dadc

f rc_write_radio_config_to_flash @ 0x080226f6
af+ 0x080226f6 18 rc_write_radio_config_to_flash

Cd 4 @ 0x0800c784
Cd 4 @ 0x0800c788
Cd 4 @ 0x0800c7ac

f menugreen.Contacts.800fcbc 0 0x800fcbc
f menugreen.Scan.8037e34 0 0x8037e34
f menugreen.Zone.80131ac 0 0x80131ac
f menugreen.Messages.8023858 0 0x8023858
f menugreen.Call_Log.8034274 0 0x8034274
f menugreen.Utilities.80134a0 0 0x80134a0
f menugreen.Radio_Set.8017cbc 0 0x8017cbc
f menugreen.Radio_Inf.80165b8 0 0x80165b8
f menugreen.MD380Tool.809c3b0 0 0x809c3b0
f menugreen.GPS_BeiDo.8016688 0 0x8016688
f menugreen.RX_GPSInf.8013638 0 0x8013638
f menugreen.Talkaroun.8019170 0 0x8019170
f menugreen.Tone_Ale.8019798 0 0x8019798
f menugreen.Power.8019c7c 0 0x8019c7c
f menugreen.Squelch.801a1e0 0 0x801a1e0
f menugreen.Intro_Scr.801a4c6 0 0x801a4c6
f menugreen.Keypad_Lo.801a6c8 0 0x801a6c8
f menugreen.Language.801ab84 0 0x801ab84
f menugreen.LED_Indic.801ad56 0 0x801ad56
f menugreen.VOX.80185d8 0 0x80185d8
f menugreen.SiteRoam1.801ae5c 0 0x801ae5c
f menugreen.Passwd_Lo.801af7c 0 0x801af7c
f menugreen.SiteRoam2.800fc84 0 0x800fc84
f menugreen.Record.8018f04 0 0x8018f04
f menugreen.Clock.8018868 0 0x8018868
f menugreen.GPS.8018248 0 0x8018248

f menugreen.prog.80136c0 0 0x80136c0
f menugreen.prog.rxf.80156a4 0 0x80156a4
f menugreen.prog.rxf.show.8015720 0 0x8015720
f menugreen.prog.rxf.edit.801587a 0 0x801587a

f menugreen.prog.txf.8015b74 0 0x8015b74
f menugreen.prog.txf.show.8015be8 0 0x8015be8
f menugreen.prog.txf.edit.8015d58 0 0x8015d58

f menugreen.prog.Channel_N.8016024 0 0x8016024
f menugreen.prog.Time_Out_.8016320 0 0x8016320
f md380_menu_entry_back 0 0x800fc84
f menugreen.prog.Color_Cod.8015540 0 0x8015540
f menugreen.prog.Repeater_.80153dc 0 0x80153dc
f menugreen.prog.TxContact.8013e78 0 0x8013e78
f menugreen.prog.GroupList.8013fbc 0 0x8013fbc

f menugreen.Talk_Perm.8019998 0 0x8019998
f menugreen.Keypad_To.8019b40 0 0x8019b40

f menugreen.All_Tones.80198b0 0 0x80198b0
f menugreen.All_Tones.TurnOn.8019290 0 0x8019290
f menugreen.All_Tones.TurnOff.80194f4 0 0x80194f4

f menugreen.talkaround.TurnOn.8019290 0 0x8019290
f menugreen.talkaround.TurnOff.80194f4 0 0x80194f4

f menugreen.Mode.801840c 0 0x801840c
f menugreen.CH_Mode.80184e8 0 0x80184e8
f menugreen.MR_Mode.801855c 0 0x801855c

f menugreen.Backlight.8019eb0 0 0x8019eb0
f menugreen.backlight.Always.801a440 0 0x801a440
f menugreen.backlight.5S.801a03e 0 0x801a03e
f menugreen.backlight.10S.801a0c8 0 0x801a0c8
f menugreen.backlight.15S.801a154 0 0x801a154

f menugreen.Repeater_Slot.80153dc 0 0x80153dc
f menugreen.Repeater_Slot_1_2.80154a4 0 0x80154a4

f menugreen.msg.Inbox.80264fc 0 0x80264fc
f menugreen.msg.Write.8023cfe 0 0x8023cfe
f menugreen.msg.Quick_Tex.80239ec 0 0x80239ec
f menugreen.msg.Sent_Item.803a024 0 0x803a024
f menugreen.msg.Drafts.8024a80 0 0x8024a80
f menugreen.msg.unk.8023c34 0 0x8023c34
f menugreen.msg.quicktext.8023de2 0 0x8023de2
f menugreen.msg.Send.802409c 0 0x802409c
f menugreen.msg.Save.80249a8 0 0x80249a8
f menugreen.msg.Clear.8023e24 0 0x8023e24
f menugreen.msg.Contacts.802417e 0 0x802417e
f menugreen.msg.Manual_Di.8024220 0 0x8024220
f menugreen.msg.enter_id.802430c 0 0x802430c
f menugreen.msg.enter_id_ok.8024740 0 0x8024740

af+ 0x08024740 608 menugreen.msg.enter_id_ok

af+ 0x80154a4 152 menu_timeslot

f menucall.Contacts 0 0x800c278
f menucall.Scan 0 0x800c2f4
f menucall.Zone 0 0x800c326
f menucall.Messages 0 0x800c358
f menucall.Call_Log 0 0x800c38a
f menucall.Utilities 0 0x800c3bc

f menucall.Radio_Set 0 0x8013528
f menucall.Radio_Inf 0 0x801355c
f menucall.Program_R 0 0x080135a8
f menucall.Program_R_patched 0 0x809d818
f menucall.MD380Tool 0 0x809d834
f menucall.GPS_BeiDo 0 0x80135dc
f menucall.RX_GPSInf 0 0x8013610


f menu.dispatcher.unkn1 0 0x0800eb98
f menu.dispatch.greenkey 0 0x0800ed3a
f menu.dispatch.redkey_maybe 0 0x0800f2a0
af+ 0x0800eb98 1810 menu_dispatcher

f draw_datetime_row 0 0x0800df1a
af+ 0x0800df1a 250 draw_datetime_row
f f_4225 0 0x0801fe5c

f call_F_4225_1 0 0x0802db42
f call_F_4225_2 0 0x080468e6

f update_scr_16 0 0x0802015e
f update_scr_17 0 0x08020236
f update_scr_18 0 0x08020376
f update_scr_19 0 0x0802046c
f update_scr_20 0 0x08020052
f update_scr_21 0 0x0801fee0
f update_scr_22 0 0x0801ff3e

f update_scr_27 0 0x08020612
f update_scr_28 0 0x080206b2
f update_scr_29 0 0x080206ee
f update_scr_30 0 0x0801ff8e
f update_scr_31 0 0x08020018
f update_scr_32 0 0x080207b4
f update_scr_33 0 0x0802080e

f update_scr_35 0 0x080201bc
f update_scr_36 0 0x0802020e
f update_scr_other 0 0x0802082c

f after_update_scr 0 0x080213aa


f scr_1 0 0x0802daf8

f draw_botline_text 0 0x08046810
f draw_topline_text_maybe 0 0x080467fa

f mainloop_entry 0 0x080468ae
f jmp_to_mainloop 0 0x080468f6
f md380_f_4520 0 0x802c83c
af+ 0x802c83c 2370 md380_f_4520

af+ 0x0804fdf4 10 set_AAAA
af+ 0x0804fdfe 10 set_CCCC

f draw_statusline @ 0x08033dac
af+ 0x08033dac 244 draw_statusline

f draw_statusline_more @ 0x08021694
af+ 0x08021694 294 draw_statusline_more

af+ 0x08036fba 2 do_nothing_1
af+ 0x08036fbc 2 do_nothing_2


# gfx_

f gfx_drawbmp @ 0x80237fe
af+ 0x80237fe 86 gfx_drawbmp

f gfx_blockfill @ 0x801d88c
af+ 0x801d88c 30 gfx_blockfill

f gfx_linefill @ 0x0801d81a
af+ 0x0801d81a 104 gfx_linefill

f gfx_info @ 0x2001da1c

f gfx_rc @ 0x080249be

f gfx_newline @ 0x08033c04
af+ 0x08033c04 26 gfx_newline

f gfx_get_xpos @ 0x08021888
af+ 0x08021888 8 gfx_get_xpos

f gfx_get_ypos @ 0x08021890
af+ 0x08021890 8 gfx_get_ypos

f gfx_set_fg_color 0 0x801d370
af+ 0x801d370 8 gfx_set_fg_color

f gfx_set_fg_color2 0 0x080331e0
af+ 0x080331e0 24 gfx_set_fg_color2

f gfx_set_bg_color @ 0x801d368
af+ 0x801d368 8 gfx_set_bg_color

f gfx_set_bg_color2 @ 0x080331c8
af+ 0x080331c8 24 gfx_set_bg_color2

af+ 0x8021874 16 gfx_select_font
f gfx_select_font 0 0x8021874

f gfx_drawchar_pos @ 0x08021940
af+ 0x08021940 18 gfx_drawchar_pos

f gfx_drawchar @ 0x0802189c
af+ 0x0802189c 154 gfx_drawchar

f gfx_clear3 0 0x0801dcc0
af+ 0x0801dcc0 40 gfx_clear3

f gfx_drawtext @ 0x800def6
af+ 0x800def6 36 gfx_drawtext

f gfx_drawtext2 @ 0x801dd08
af+ 0x801dd08 18 gfx_drawtext2

f gfx_drawtext3 @ 0x0802b142
af+ 0x0802b142 148 gfx_drawtext3

f gfx_drawtext4 @ 0x0801dd1a
af+ 0x0801dd1a 18 gfx_drawtext4

f gfx_drawtext5 @ 0x0801dd2c
af+ 0x0801dd2c 16 gfx_drawtext5

f gfx_drawtext6 @ 0x08027728
af+ 0x08027728 154 gfx_drawtext6

f gfx_drawtext7 @ 0x080277c2
af+ 0x080277c2 16 gfx_drawtext7

f gfx_drawtext8 @ 0x08036fc0
af+ 0x08036fc0 378 gfx_drawtext8

f gfx_drawtext9 0 0x0802b0d4
af+ 0x0802b0d4 110 gfx_drawtext9

s 0x0801d960
f gfx_drawchar_unk @ $$
af+ $$ (0x988-0x960) gfx_drawchar_unk

CCa 0x08036ff4 check_for_0_term 
CCa 0x08037118 check_for_0_term_and_loop

CCa 0x08033c96 mult_off21_off23_font

f gfx_drawtext10 @ 0x0800ded8
af+ 0x0800ded8 30 gfx_drawtext10

f convert_freq_to_str @ 0x800e398

f draw_channel_label 0 0x0800e5a6

af+ 0x0800e538 98 draw_zone_channel
f draw_zone_channel 0 0x0800e538
f draw_zone_label 0 0x0800e682

af+ 0x0800e6e8 (0x6f6-0x6e8) func_e6e8
af+ 0x0800e6f6 (0x704-0x6f6) func_e6f6

f screen_unknown1 0 0x0800e728
f menu_6_15_1 0 0x0800e7a8
f menu_6_1_1 0 0x0800e7cc

f scr_mode_stable 0 0x08020830

f return_to_mode_1_from10 0 0x0800fc96
af+ 0x0800fc96 32 return_to_mode_1_from10

f F_4315 0 0x08025ae4

f promisc_audio_frame @ 0x08040cce 
f normal_audio_frame @ 0x08040cc4
f audio_for_me_or_not @ 0x08040c7a
f re_create_event_8 @ 0x08040c02
f event_36 @ 0x08041616
f event_4 @ 0x08041e44



af+ 0x0804edd0 2 dummy_0x0804edd0
af+ 0x0804f688 2 dummy_0x0804f688

f This_function_called_Read_Channel_Switch @ 0x0804fd04
af+ 0x0804fd04 136 This_function_called_Read_Channel_Switch


# keyborked
# struct keyboard_data
f kb_keypressed @ 0x2001e5f8  # ex: "keypressed_struct"

f keylocked_flags @ 0x2001e5f9

f kb_row_col_pressed @ 0x2001e7ba
f kb_row_col_pressed_last @ 0x2001e7bc
f kb_keydown_debounce @ 0x2001e889
f kb_keycode @ 0x2001e890

f keyup_keydown @ 0x2001e889
f keycode_old @ 0x2001e891
f keypressed_duringmenu @ 0x2001e5f3
f keypress_max_time @ 0x2001e7b8
f keypress_time_some_button @ 0x2001e7ac
f keypress_time_lower_button @ 0x2001e7b0
f keypress_time_upper_button @ 0x2001e7b2
f kb_key_press_time @ 0x2001e7be # ex: keypress_time_all

f kb_keypressed @ 0x2001e5f8 # ex: keypress_flag

f keypress_max_time_reached @ 0x0804face

f init_radioconfig_2_from_spi @ 0x08022714
f read_40_from_2100_spi @ 0x08022716
f menu_pointer_maybe_channel_data @ 0x2001def8

f base_for_longpress_struct @ 0x2001e5ec

f check_for_ptt_switch @ 0x0804ebfc
af+ 0x0804ebfc 106 check_for_ptt_switch

f kb_enter_alpha @ 0x0802e0b8

f get_keycode_from_table @ 0x804f8e4
f get_keycode_from_table_2 @ 0x804f8ea

f store_keycode @ 0x0804fb24
f kb_handler @ 0x0804f94c
af+ 0x0804f94c 384 kb_handler
CCa 0x0804fa32 definite keydown
CCa 0x0804fa1e jump if b0 not set, reset debounce
CCa 0x0804fa30 not debounced yet, jump
CCa 0x0804fa4e jump if long keypress count is reached

f radio_status_1 @ 0x2001e5f0

f dispatch_keyboard_2 @ 0x0802c83c

af+ 0x0801dd5c 2182 often_called_something_keycode_menu

f biglist_pollsubsys_maybe @ 0x0804eb64
af+ 0x0804eb64 152 biglist_pollsubsys_maybe
af+ 0x0804fc32 2 dummy_0x0804fc32
af+ 0x0804fc2e 2 dummy_0x0804fc2e
af+ 0x0804fc2e 2 dummy_0x0804fc2e
af+ 0x0804fc26 2 dummy_0x0804fc26
af+ 0x0804fc28 2 dummy_0x0804fc28
af+ 0x0804fc2a 2 dummy_0x0804fc2a
af+ 0x0804fc2c 2 dummy_0x0804fc2c

#####
# GUI

f gui_control @ 0x0802d1b2
af+ 0x0802d1b2 4 gui_control

f gui_opmode1_prev @ 0x2001e94c
f gui_opmode1 @ 0x2001e94d
f gui_opmode2 @ 0x2001e94b
f gui_opmode3 @ 0x2001e892

# GUI
#####

f some_init @ 0x0802d368
af+ 0x0802d368 4 some_init

af+ 0x801eb00 1436 handle_keycode_F_4171

f dispatch_event @ 0x0803c39c


# 0x20017468

########
# mbox

# 0x20017468
f event1_buffer @ 0x2001e8aa
f event1_mbox_poi_radio @ 0x2001e65c

f beep_code_send @ 0x2001e8a8
f event2_mbox_poi_beep @ 0x2001e67c

# 0x20017348
f event3_mbox_poi @ 0x2001e664

f event4_mbox_poi @ 0x2001e660

# 20017438
f event5_buffer @ 0x2001e8a9
f event5_mbox_poi @ 0x2001e658


# mbox
########

# event5 events
f ev5_1 @ 0x803b958
f ev5_2 @ 0x803b980
f ev5_3 @ 0x803b986
f ev5_4_8 @ 0x803b9a4
f ev5_12 @ 0x803ba14
f ev5_14 @ 0x803bf2a
f ev5_15 @ 0x803ba5e
f ev5_16 @ 0x803bad8
f ev5_17 @ 0x803baec
f ev5_18 @ 0x803bc86
f ev5_19 @ 0x803bd46
f ev5_20 @ 0x803be8e
f ev5_21 @ 0x803b9c2
f ev5_22 @ 0x803b9fa
f ev5_rest @ 0x803bf5a

# radioevents

f handle_inter_request_deny @ 0x08040690
f handle_inter_sendstart @ 0x080406a2
f handle_inter_sendstop @ 0x080406b4
f handle_inter_lateentry @ 0x080406c6
f handle_inter_recvdata @ 0x0804077a
f handle_inter_recvmessage @ 0x080409ac
f handle_inter_quit @ 0x080409be
f handle_inter_phy @ 0x080409d0

f jump_if_b7_0 @ 0x080406ce

f some_radio_state @ 0x2001e8b0 
f some_radio_state_prev @ 0x2001e8af

f re_test_for_04 @ 0x08041da8

f re_wait_for_event @ 0x0803c7fc

f re_handle_1 @ 0x0803c85c
f re_handle_2 @ 0x0803ca2c
f re_handle_3 @ 0x0803c8cc
f re_handle_4 @ 0x0803c956
f re_handle_5 @ 0x0803c984
f re_handle_7 @ 0x0803c9fc
f re_handle_8 @ 0x0803ca08
f re_handle_9 @ 0x0803ca14
f re_handle_a @ 0x0803ca20
f re_handle_e @ 0x0803ca3a

f re_last_radio_event @ 0x2001e8a1
    
f phone_ringing @ 0x0804df0e

# tasks

f create_Sys_Inter @ 0x08046548
f create_RTC_Timer @ 0x0804657c
f create_State_Change @ 0x080467b8
f create_ChAccess_Pr @ 0x0804678a

f task_state_change @ 0x0803c330
f task_rtc_timer @ 0x080467cc

# memory
f write_current_channel_info_to_spi @ 0x08022cac
af+ 0x08022cac 26 write_current_channel_info_to_spi
f write_current_channel_info_to_spi_long @ 0x080231a8
af+ 0x080231a8 26 write_current_channel_info_to_spi_long

f selected_channel @ 0x2001e850
#CCa 0x2001e850 selected_channel

f struct_channel_info2 @ 0x2001de78
f current_channel_info @ 0x2001deb8
f channel_info_read_spi_init @ 0x08022aa6
af+ 0x08022aa6 156 channel_info_read_spi_init

# c5000
f c5000_pll_init @ 0x0803f95c
f c5000_strange_init @ 0x0803f982
f c5000_iffreq_init @ 0x0803f9c6

f c5000_handle_0 @ 0x080408a4
f c5000_handle_1 @ 0x080408ba
f c5000_handle_2 @ 0x080408d0
f c5000_handle_3 @ 0x080408e6
f c5000_handle_4 @ 0x08040912
f c5000_handle_5 @ 0x08040914
f c5000_handle_6 @ 0x08040916
f c5000_handle_7 @ 0x0804092c
f c5000_handle_8 @ 0x08040944
f c5000_handle_9 @ 0x0804095c
f c5000_handle_A @ 0x0804098a
f c5000_handle_B_F @ 0x080409a2

f c5000_handle_Voice_LC @ 0x080408ba
f c5000_handle_Terminator_LC @ 0x080408d0
f c5000_handle_CSBK @ 0x080408e6
f c5000_handle_DataHeader @ 0x08040916
f c5000_handle_DataRate1_2 @ 0x0804092c
f c5000_handle_DataRate3_4 @ 0x08040944
f c5000_handle_DataRate1 @ 0x0804098a
f c5000_handle_PI @ 0x080408a4
f c5000_handle_idle @ 0x0804095c

f c5000_init_lowregs @ 0x0803fed4
f c5000_enable_audio @ 0x08040226

f c5000_wr_60_1 @ 0x0803ff16
f c5000_wr_60_2 @ 0x08040230
f c5000_wr_60_3 @ 0x08040382
f c5000_wr_60_4 @ 0x08046b32
f c5000_wr_60_5 @ 0x0805055a


af+ 0x0804dd36 24 some_func_pend
af+ 0x0804dd4e 20 some_func_post
af+ 0x0804dc84 (0xF6-0x84) some_bitband_io
af+ 0x0804dcf6 28 some_bitband_io_range

af+ 0x080402f8 218 c5000_some2
af+ 0x08040290 86 c5000_some3

af+ 0x0803ff84 38 c5000_spi0_writereg
af+ 0x0803ffd0 42 c5000_spi0_readreg
af+ 0x0803ffaa 38 c5000_spi0_writereg_1

f c5000_spi0_readreg_maybe @ 0x0803fffa
af+ 0x0803fffa 46 c5000_spi0_readreg_maybe

f c5000_maybe_read_packet @ 0x08040028
af+ 0x08040028 52 c5000_maybe_read_packet

f c5000_read_inter @ 0x08040680
f c5000_read_dll_cc @ 0x080406ec
f c5000_read_dll_datatype @ 0x080407a8

f c5000_dispatch_dll_datatype @ 0x0804087e
f c5000_check_for_lcss_continue @ 0x08040858
f c5000_jump_if_vod @ 0x0804087a

f dmr_pi_dummy @ 0x8040a00

f is_this_the_check_for_group_rx_list @ 0x08040b94

f msg_send_maybe @ 0x08024fb0
af+ 0x08024fb0 212 msg_send_maybe

f msg_send_maybe2 @ 0x08024dbc
af+ 0x08024dbc 166 msg_send_maybe2

f msg_create_menu_item_something @ 0x08024448
af+ 0x08024448 2 msg_create_menu_item_something

f msg_convert @ 0x08027656
af+ 0x08027656 112 msg_convert

af+ 0x08024658 190 msg_sms_post_showack 
f msg_sms_post_showack @ 0x08024658

f msg_sms_hdr @ 0x2001e1d0
f msg_sms_bdy @ 0x2001cefc

f msg_sms_hdr_prep @ 0x2001cb54
f msg_sms_bdy_prep @ 0x2001ccbc

f msg_bdy_prep_to_editbuf @ 0x08024402
f msg_editbuf_to_hdr_prep @ 0x080243d6 
f msg_stack_to_hdr_prep_options @ 0x080243b0

f msg_flash_write @ 0x08022fe8
f msg_process_sms2 @ 0x0803dd8c
f msg_process_sms @ 0x0803dd0c
af+ 0x0803dd0c 260 msg_process_sms

# ((( mag

f dispatch_event5_mbox @ 0x0803b8f4

f msg_last_event @ 0x2001e8f4
f msg_0x2001e8f3 @ 0x2001e8f3
f msg_status_flag1 @ 0x2001e8f5
f msg_timer_500 @ 0x2001e5d0
f msg_sms_flags_shifted @ 0x2001e895
f msg_dest_addr @ 0x2001e5dc

f msg_handle_types @ 0x0804c858
af+ 0x0804c858 2 msg_handle_types
f msg_handle_type_11 @ 0x804ca3c
f msg_handle_type_21 @ 0x804c888
f msg_handle_type_31 @ 0x804ca3c
f msg_handle_type_other @ 0x804cb00

f msg_handle_types @ 0x0804c858


# msg )))


f msg_f1 @ 0x08024ec4
af+ 0x08024ec4 162 msg_f1
f msg_f2 @ 0x08024f66
af+ 0x08024f66 64 msg_f2
f msg_f4_spi_rd @ 0x08022ebc
af+ 0x08022ebc 18 msg_f3_spi_rd

f spiflash_read_3ff30_288 @ 0x08022e88
af+ 0x08022e88 26 spiflash_read_3ff30_288
f spiflash_write_3ff30_288 @ 0x08022ea2
af+ 0x08022ea2 26 spiflash_write_3ff30_288


f msg_buffer @ 0x2001db2c
f msg_buff_complete @ 0x20018490

f c5000_master_handler @ 0x08041cfc

f sema1_poi @ 0x2001e650
f sema2_poi @ 0x2001e670 

#
f simplex_or_repeater_flagword @ 0x2001e898


#

f some_state_var @ 0x2001e8b8
f smeter_rssi @ 0x2001e534

#

f c5000_set_local_addr @ 0x0803cae0
af+ 0x0803cae0 84 c5000_set_local_addr

f c5000_set_local_addr2 @ 0x0803cb34
af+ 0x0803cb34 96 c5000_set_local_addr2

f flash_write_50_at_40000 @ 0x08022e76
af+ 0x08022e76 18 flash_write_50_at_40000

f flash_read_50_at_40000 @ 0x08022e64
af+ 0x08022e64 18 flash_read_50_at_40000

f menu_add_number_of_menuentries_counts @ 0x0800fc54
af+ 0x0800fc54 48 menu_add_number_of_menuentries_counts

f menu_memory_poi @ 0x2001e700

#
f zone_name @ 0x2001cddc
f contact @ 0x2001e1ac

f load_contact_call @ 0x08022bda
f load_contact @ 0x08022992
af+ 0x08022992 36 load_contact
f load_contact_spiflash @ 0x0802297c

f unprogrammed_str @ 0x80cfb78

f display_unprog_screen @ 0x0802d57c
af+ 0x0802d57c 608 display_unprog_screen

f display_idle_screen @ 0x802d7f8
af+ 0x802d7f8 740 display_idle_screen

# from link file >>>
f backlight_timer @ 0x2001e7f8
f md380_menu_0x2001d3c1 @ 0x2001e914
#f OSTaskCreateExt @ 0x0804e580
f ambe_outbuffer0 @ 0x20013f28
#f main_menu @ 0x0803b39a
#f Start_multiple_tasks @ 0x08046520
f botlinetext @ 0x2001e410
f md380_dfu_state @ 0x2001e962
f ambe_decode_wav @ 0x08053680
f ambe_encode_thing @ 0x080531d8
f md380_RTC_GetDate @ 0x0802b50c
f md380_blockadr @ 0x2001e754
f md380_f_4137 @ 0x0802dfbc
af+ 0x08036cc0 4 aes_loadkey
f aes_loadkey @ 0x08036cc0
f bp_tone_off @ 0x08030b58
f OS_EXIT_CRITICAL @ 0x08043dec
f usb_dnld_handle @ 0x0808ebee
f dmr_before_squelch @ 0x08040ce6
f md380_dfutargetadr @ 0x20004a14 # ex: md380_dfu_target_adr
f usb_do_setup @ 0x0808eb30
f aes_startup_check @ 0x0802256a
f print_buffer @ 0x2001e0d0
af+ 0x080f8510 4 welcomebmp
f welcomebmp @ 0x080f8510
f Start_2_more_tasks__init_vocoder_tasks @ 0x08049e14
f ambe_outbuffer1 @ 0x20013fc8
f OSTaskNameSet @ 0x0804e64c
f usb_send_packet @ 0x08059b02
f OSSemPend @ 0x0803f754
f md380_f_4102 @ 0x0804ec66
f OS_ENTER_CRITICAL @ 0x08043de4
f usb_serialnumber @ 0x0809662e
f OSSemCreate @ 0x0803f708
af+ 0x080cff30 4 md380_wt_programradio
f md380_wt_programradio @ 0x080cff30
f dmr_call_end @ 0x08041430
f dmr_sms_arrive @ 0x08040de0
f Get_Welcome_Line1_from_spi_flash @ 0x080226c0
af+ 0x08036c38 4 aes_cipher
f aes_cipher @ 0x08036c38
f c5000_spi0_readreg @ 0x0803ffd0
f md380_packet @ 0x2001ae74
af+ 0x080cf780 4 gfx_font_norm
f gfx_font_norm @ 0x080cf780
f Start @ 0x080462bc
f OSTimeDly @ 0x08033eb4
f usb_upld_handle @ 0x0808f308
f dmr_call_start @ 0x08040a02
f md380_spiflash_read @ 0x08031476
f ambe_inbuffer @ 0x2001410e
f md380_packetlen @ 0x2001e758
f md380_spiflash_disable @ 0x08031546
f md380_RTC_GetTime @ 0x0802b3f6
af+ 0x0804b234 4 ambe_unpack
f ambe_unpack @ 0x0804b234
f md380_spiflash_block_erase64k @ 0x080312aa
f md380_program_radio_unprohibited @ 0x2001e574
f c5000_spi0_writereg @ 0x0803ff84
f usb_dfu_write @ 0x08090370
f Beep_Process @ 0x0802f9dc
f md380_thingy2 @ 0x2001e963
f md380_spiflash_enable @ 0x0803152a
#f Edit_Message_Menu_Entry @ 0x08023ee4
f md380_spi_sendrecv @ 0x080314bc
f md380_menu_entry_selected @ 0x2001e903
f usb_setcallbacks @ 0x08055100
af+ 0x0809a4c0 4 gfx_font_small
f gfx_font_small @ 0x0809a4c0
f Get_Welcome_Line2_from_spi_flash @ 0x080226d2
f md380_spiflash_wait @ 0x08031508
f md380_usbstring @ 0x2001d504
f md380_spiflash_sektor_erase4k @ 0x08031276
f OSSemPost @ 0x0803f844
f md380_OSMboxPost @ 0x0803119c
f usb_dfu_read @ 0x080903c0
f dmr_handle_data @ 0x0804dd70
f ambe_mystery @ 0x20013594
f md380_spiflash_write @ 0x0803155e
f md380_spiflash_security_registers_read @ 0x080318b0
f toplinetext @ 0x2001e3fc
# f md380_menu_0x2001d3f0 @ 0x2001b246         # already set above, removed (dl4yhf 2016-12)
# f md380_create_main_menu_entry @ 0x0800c188  # already defined as a FUNCTION, do we also need it as a FLAG ?
# f md380_create_menu_entry @ 0x0800c72e       # will be set below, with more details (af
f Create_Menu_Entry_RX_QRG_2 @ 0x080157fc
f md380_itow @ 0x08018b28
f md380_menu_numerical_input @ 0x0801b042
# from link file <<<

# from link file >>>
f channel_name @ 0x2001e1f4
f channel_num @ 0x2001e8c1
# from link file <<<

f q_status_4 @ 0x2001e604

f q_struct_1 @ 0x2001e600

# ABOVE: stuff formerly in flash.r (converted by 'make', from md380tools/applet/src/symbols_d13.020 ) .
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# BELOW: more application-specific stuff added by DL4YHF, since 2017-01-03
#  Begin with the two functions called from Reset_Handler.
#  Note the odd addresses in the caller, but the callee (THUMB) is at an even address.
s 0x8094358  # bet this is "SystemInit()", first function called from Reset_Handler
af+ $$ (0x80943aa-$$) SystemInit # af+ <current_address> <size in bytes> <name>
# ex: f SystemInit # with this, the disassembly didn't show SystemInit being called from Reset_Handler !
f SystemInit 2 @ ($$+1) # addr+1, because the CALLER'S dest-addr has the LSBit set for Thumb mode
                        # (ignore the tilde in the disassembled function..)
s 0x80943aa # subroutine called from SystemInit().. 
af+ $$ 202 RCC_Init
f RCC_Init @ ($$)
CCa 0x080943b8 Set RCC_CR bit 16 = HSEON
CCa 0x080943c4 Isolate RCC_CR bit 17 = HSE clock ready ?
CCa 0x080943f8 Read RCC_APB1ENR
CCa 0x080943fa Set bit 24 = PWREN for APB1  
CCa 0x08094400 What a waste of code memory. Could have set R1 before.
CCa 0x08094406 PWR_CR bit 14 = voltage regulator control
CCa 0x08094412 Copy RCC clock config register to itself ?
CCa 0x08094434 RCC_CR bit 24 = 'PLL ON'
CCa 0x08094440 Check 'PLL Ready'-bit
CCa 0x0809444a Set FLASH_ACR (waitstates, etc)
CCa 0x08094452 clear RCC_CFGR bits 31+30 to select SYSCLK for MCO2
CCa 0x0809445c select PLL as system clock (?)
                        
# 2nd subroutine, called from Reset_Handler, and never returns:
s 0x80FAFDC  # if 'they' used Keil's ecosystem, this is __main which calls the scatterload-thingy
af+ $$ 12 __main
f __main 12 @ ($$+1) # similar as above, to see the called function name in disassembly

s 0x80D0010 # 1st subroutine called from __main (above)
af+ $$ 26 FPU_Init # called from __main() [not main()] 
f FPU_Init 26 @ $$

s 0x80F7ECC # 2nd subroutine called from __main() 
af+ $$ 26 _main2
f _main2 26 @ $$

s 0x80F7EE6 # 1st subroutine called from _main2() . Formerly Keil's _main_scatterload-thingy ?
af+ $$ 26 _main2_init_sub1
f _main2_init_sub1 26 @ $$

s 0x8099A3C # 2nd subroutine called from _main2() 
af+ $$ 32 _main2_init_sub2
f _main2_init_sub2 32 @ $$

s 0x8099F08 # 3rd subroutine called from _main2() 
af+ $$ 34 _main2_init_sub3
f _main2_init_sub3 34 @ $$

s 0x80F7EE6 # 4th subroutine called from _main2() 
af+ $$ (0xEF6-0xEE6) _main3
f _main3 6 @ $$

s 0x8051d7e # called from _main2_init_sub1 (1st)
af+ $$ (0x8A-0x7E) func_1d7e

s 0x804e2e0 # called from _main2_init_sub1 (2nd)
af+ $$ (0xFC-0xE0) func_e2e0

s 0x8043ed0 # called from _main2_init_sub1 (3rd)
af+ $$ (0xFC-0xD0) InitGlobalsAndStartRealTimeKernel

s 0x8046280 # called from _main2_init_sub1 (4th)
af+ $$ (0xBC-0x80) Create_Start_Task_AndSetItsName
# Details, and EVEN THE SOURCECODE of OSTaskCreateEx()
# is in the uC/OS-II User's Manual, page 156 .
# For the ARM-ABI's calling convention,
#  registers R0 - R3 are used for the first four arguments 
#   ("NCRN" begins with the RIGHTMOST argument, u16Options, in R0),
#  other arguments are pushed to the stack by the caller ("NSAA" begins with *SP), thus:
CCa 0x8046282 reserve stack space for arguments #5 to #9
CCa 0x8046284 3 = OS_TASK_OPT_STK_CHK + OS_TASK_OPT_STK_CLR
CCa 0x8046286 SP[4] = u16Options, 9th and last arg for OSTaskCreateExt
CCa 0x804628a SP[3] = pvExt = user supplied data, 8th arg for OSTaskCreateExt
CCa 0x804628c task stack size (static array)
CCa 0x8046290 SP[2] = u32StackSize, 7th arg for OSTaskCreateExt
CCa 0x8046296 SP[1] = pBotOfStack, 6th arg for OSTaskCreateExt
CCa 0x804629a SP[0] = u16ID, 5th arg for OSTaskCreateExt
CCa 0x804629c R3 = u8Prio, 4th arg for OSTaskCreateExt
CCa 0x804629e R2 once was pTopOfStack, but what is this ?
CCa 0x80462a2 R1 = pvData, 2nd arg for OSTaskCreateExt
CCa 0x80462a4 R0 = pTaskFunc = PC+0x15 = Start(), ca 0x80462bc 
CCa 0x80462a8 OSTaskCreateExt(pTaskFunc,pvData,pTOS,u8Prio,u16ID,pBOS,u32StkSize,pvExt,u16Options)
CCa 0x80462b4 OSTaskNameSet(R0=INT8U prio, R1=char *pname, R2=INT8U *err)
CCa 0x80462b8 clear local vars (arguments) from stack

s 0x20014ab4 # looks like another TASK stack, passed to OSTaskCreateExt
f Stack_for_Start_Task 512 @ $$

# functions called from task Start(), annotated since 2017-01, first OUTSIDE loops .
# 'af+' to know the're FUNCTIONS, 'f' to have them listed along with other symbols .

s 0x8044cb4
af+ $$ 4 func_4cb4
f func_4cb4 4 @ $$

s 0x80451ce
af+ $$ 4 func_51ce
f func_51ce 4 @ $$

s 0x8045414
af+ $$ 4 func_5414
f func_5414 4 @ $$

s 0x80458f8 # called via 'bl', caused strange 'LEA'-decodes in comments
af+ $$ (0x5922-0x58F8) func_58f8
#f func_58f8 (0x5922-0x58F8) @ $$ # removed to prevent unhelpful 'LEA'-decodes

s 0x8045d18
af+ $$ 4 func_5d18
f func_5d18 4 @ $$

#s 0x80460a8
#af+ $$ 4 func_60a8
#f func_60a8 4 @ $$

s 0x804ce2c
af+ $$ 4 func_ce2c
f func_ce2c 4 @ $$

s 0x8030fde
af+ $$ 4 func_0fde
f func_0fde 4 @ $$

# above: functions called from task Start(), once (kind of "late init"-things)
# below: functions called from task Start(), in a loop.

s 0x8045954
af+ $$ (0xC96-0x954) FuncWithAwfulLongSwitch
#f FuncWithAwfulLongSwitch (0xC96-0x954) @ $$ # removed to prevent unhelpful 'LEA'-decodes

s 0x8044e00
af+ $$ (0x50C8-0x4E00) LongSwitchWithRadioStatus1
f LongSwitchWithRadioStatus1 (0x50C8-0x4E00) @ $$

s 0x8045484
af+ $$ (0x82A-0x484) SomethingWithChannelsRadioConfigAndBeeps
#f SomethingWithChannelsRadioConfigAndBeeps (0x82A-0x484) @ $$

s 0x8045d94
af+ $$ (0x604A-0x5D94) SomethingWithLongpressSettingRadioStatus1
f SomethingWithLongpressSettingRadioStatus1 (0x604A-0x5D94) @ $$

s 0x8045C9C 
af+ $$ (0xD0A-0xC9C) CalledFromLongpressThing
#f CalledFromLongpressThing (0xD0A-0xC9C) @ $$ # removed to prevent unhelpful 'LEA'-decodes

s 0x804520c
af+ $$ (0x5398-0x520C) func_520c
f func_520c (0x5398-0x520C) @ $$

s 0x80450c8
af+ $$ (0x156-0x0C8) SomethingWithGuiOpmode2
f SomethingWithGuiOpmode2 (0x156-0x0C8) @ $$

s 0x80460f0
af+ $$ (0x0F8-0x0F0) Calls_6050
# f Calls_6050 (0x0F8-0x0F0) @ $$

s 0x8046050
af+ $$ (0x0F8-0x0F0) func_6050
#f func_6050 (0x0F8-0x0F0) @ $$

s 0x80460f8
af+ $$ (0x204-0x0F8) SomethingWithGPIOA_and_RadioStatus1
f SomethingWithGPIOA_and_RadioStatus1 (0x204-0x0F8) @ $$
CCa 0x804612C return but not end of function

s 0x804d688
af+ $$ (0x92-0x88) SetBit30_ptrR0plus8
f SetBit30_ptrR0plus8 (0x92-0x88) @ $$

s 0x804d692
af+ $$ (0x98-0x92) ExtendU16toU32_ptrR0plus4C
f ExtendU16toU32_ptrR0plus4C (0x98-0x92) @ $$

s 0x803d5e4
af+ $$ (0x76E-0x5E4) SomethingWithGPIOC_and_Backlight_Timer
f SomethingWithGPIOC_and_Backlight_Timer (0x76E-0x5E4) @ $$

s 0x804d1b8
af+ $$ (0x49E-0x1B8) SomethingWithRadioStatus1
f SomethingWithRadioStatus1 (0x49E-0x1B8) @ $$

s 0x803e372
af+ $$ (0x3F6-0x372) func_e372
f func_e372 (0x3F6-0x372) @ $$

s 0x803da68
af+ $$ (0xBDE-0xA68) func_da68
f func_da68 (0xBDE-0xA68) @ $$

s 0x803dbf0
af+ $$ (0xC82-0xBF0) func_dbf0
f func_dbf0 (0xC82-0xBF0) @ $$

s 0x803dc90
af+ $$ (0xCF2-0xC90) func_dc90
f func_dc90 (0xCF2-0xC90) @ $$
#------ end of functions directly, repeatedly called from task Start() -----
CCa 0x080463f4 end task 'Start()', never returns

s 0x8044472 # 
af+ $$ (0x5B6-0x472) func_4472
CCa 0x8044472 pData=pData; unused argument in R0

s 0x8044618 #
af+ $$ (0x658-0x618) func_4618

s 0x8044658 #
af+ $$ (0x758-0x658) SomethingWith_RCC_and_PLL_I2C

s 0x804e132 # called from InitGlobalsAndStartRealTimeKernel
af+ $$ (0x13A-0x132) ClearSomeHalfWordInRAM 
s 0x80442ce # called from InitGlobalsAndStartRealTimeKernel
af+ $$ (0x2FA-0x2CE) ClearSomeVariablesInRAM
s 0x80442fa # called from InitGlobalsAndStartRealTimeKernel
af+ $$ (0x32E-0x2FA) ClearSomeBlocksInRAM 
s 0x8044368 # called from InitGlobalsAndStartRealTimeKernel
af+ $$ (0x3C6-0x368) Func4_of_10 
s 0x804426e # called from InitGlobalsAndStartRealTimeKernel
af+ $$ (0x2CE-0x26E) Func5_of_10 
s 0x80482c4 # called from InitGlobalsAndStartRealTimeKernel
af+ $$ (0x312-0x2C4) Func6_of_10 

s 0x804432e # called from InitGlobalsAndStartRealTimeKernel
af+ $$ (0x368-0x32E) Create_uCOS_Idle_Task 
CCa 0x804433A Task stack size (static array)
CCa 0x8044350 R0 = pTaskFunc = PC+0x101 = &OS_IdleTask, ca 0x08044452
CCa 0x8044354 OSTaskCreateExt(pTaskFunc,pvData,pTopOfStack,u8Prio,u16ID,pBotOfStack,u32StackSize,pvExt,u16Options)

s 0x8044452 # task function, passed to OsTaskCreateExt somewhere. Names inspired by uC/OSII-demo ..
af+ $$ (0x472-0x452) OS_IdleTask # name inspired by uCOS2/EX1.C
CCa 0x8044452 pData=pData; unused argument in R0
CCa 0x8044456 endless 'Idle' task loop
CCa 0x8044460 OSIdleTaskCtr++; # inspired by Micrium, for uC_OS3
CCa 0x8044470 must never return
f OSIdleTaskCtr 4 @ 0x2001E710

af+ 0x804E144 (0x14C-0x144) OSIdleTaskHook
af+ 0x80323E6 (0x416-0x3E6) WaitForInterruptInIdle
CCa 0x8032412 does the CPU save power here ?

s 0x20018E70 # something in RAM, passed to OSTaskCreateExt
f Stack_for_Idle_Task 256 @ $$
# some strings in Flash, passed to OSTaskCreateExt etc
f s_uCOS2_Start_Task 4 @ 0x80fbda8
f s_uCOS2_Idle_Task 13 @ 0x80f8f54
f s_uCOS2_Tmr_Task 12 @ 0x80f8f64
f s_Call_Process 12 @ 0x80F8F74
f s_FMTx_Process 12 @ 0x80F8F84
f s_Beep_Process 12 @ 0x80F8F94
f s_TimeSlot_Inter 14 @ 0x80F8F84
f s_State_Change 12 @ 0x80F8FB4
f s_DFU_in_HS_mode 14 @ 0x80F8FD4
f s_000000000010B 12 @ 0x80F8FE4
f s_000000000010C 12 @ 0x80F8FF4
f s_DFU_Interface 13 @ 0x80F9004



s 0x804b728 # called from InitGlobalsAndStartRealTimeKernel
af+ $$ (0x7D6-0x728) CreateTwoSemaphores 
s 0x804e13a # called from InitGlobalsAndStartRealTimeKernel
af+ $$ (0x13C-0x13A) DoNothing_only_BX_LR 
s 0x804e304 # called from InitGlobalsAndStartRealTimeKernel
af+ $$ (0x3E8-0x304) ManyStrangeSimpleMoves
CCa 0x0804e308 maybe just a dummy to suppress linker warnings

s 0x8095810 # endlessly called from 0x80f7ef0 ? 
af+ $$ 16 CalledForever
CCa $$ Possibly for 'unexpected return from main()'
f CalledForever 16 @ $$

s 0x8044024 
af+ $$ (0x66-0x24) func_4024

s 0x8044434 # called from func_4024
af+ $$ (0x52-0x34) func_4434

s 0x8043df2 # called from func_4024
af+ $$ ($E1E-0xDF2) func_3df2
CCa 0x80F30988 Really an invalid opcode here ? 

s 0x8044066 # .. 411a called from SysTick_Handler (after leaving a critical section)
af+ $$ 182 SysTick_Sub1
f SysTick_Sub1 182 @ $$

s 0x804e2a0 # .. e2c2 called from SysTick_Sub1
af+ $$ 36 SysTick_Sub2
f SysTick_Sub2 36 @ $$

s 0x804e302 # .. e2c2 called from SysTick_Sub2
af+ $$ 2 nop_BX_LR # 'returns without doing anything'. Dummy, or nop() ?
f nop_BX_LR 2 @ $$

s 0x804411c # .. 4194
af+ $$ 122 func_411c
f func_411c 122 @ $$

s 0x8043efc # also called from SysTick_Handler (after leaving a critical section)
af+ $$ 148 SysTick_Sub2
f SysTick_Sub2 148 @ $$

s 0x8043f90 
af+ $$ 58 func_3f90
f func_3f90 58 @ $$

s 0x8043fca 
af+ $$ 90 func_3fca
f func_3fca 90 @ $$




s 0x8061aea # called from the wakeup-IRQ-handler, *after* SystemInit()
af+ $$ 40 WakeUp_Sub1
f WakeUp_Sub1 40 @ $$

s 0x8051e66 # also called from the wakeup-IRQ-handler, *after* SystemInit()
af+ $$ 6 ClearEXTIPendingBits_R0
f ClearEXTIPendingBits_R0 6 @ $$

s 0x80938c8 # called from USB (OTG) IRQ handler
af+ $$ 210 CalledFromUSB_IRQ
f CalledFromUSB_IRQ 210 @ $$
  # expecting usb_upld_handle(), usb_dnld_handle() to be called from here ?
  # No... but see also: applet/src/symbols_d13.020 ..
s 0x809399a # right after the USB subroutine
af+ $$ 72 func_809399a
f func_809399a 72 @ $$

s 0x80939e2
af+ $$ 136 func_80939e2
f func_80939e2 136 @ $$

s 0x8051e42 # called from EXTI3 + EXTI2 (pin-change) interrupt handler
af+ $$ 36 CalledFromPinChangeIRQ
f CalledFromPinChangeIRQ 36 @ $$

s 0x8043aa0 # called from EXTI2 (pin-change) interrupt handler
af+ $$ 4 StoreR1_in_R0plus0x24 # give this a better name when you know the purpose !
f StoreR1_in_R0plus0x24 4 @ $$
s 0x8043aa4
af+ $$ 4 StoreR1_in_R0plus0x2C # give this a better name when you know ..
f StoreR1_in_R0plus0x2C 4 @ $$
s 0x8043aa8
af+ $$ 4 LoadR0_from_R0plus0x24 # give this a better name when you .. etc
f LoadR0_from_R0plus0x24 4 @ $$

# subroutines called from various timer IRQ handlers (access SFRs via base address + register offsets)
af+ 0x803e45e (0x47E-0x45E) TimerIRQ_Sub11
af+ 0x803f2c8 (0x314-0x2C8) TimerIRQ_Sub9
af+ 0x803f314 (0x32E-0x314) func_f314
af+ 0x803f32e (0x34E-0x32E) func_f32e
af+ 0x803f34e (0x4A0-0x34E) SomethingWithRadioStatus1 
af+ 0x803f4a0 (0x4FA-0x4A0) SomeBitFiddling
af+ 0x803f4fa (0x55E-0x4FA) SomethingWithGPIOC_TIM8_TIM7
af+ 0x803f55e (0x5A8-0x55E) SomethingWithTIM7_RadioStatus1
af+ 0x803f5a8 (0x5CC-0x5A8) SomethingWithTIM7_ChannelInfo2
af+ 0x803f5cc (0x5E0-0x5CC) func_f5cc
af+ 0x803f5e0 (0x626-0x5E0) Something2_TIM7_RadioStatus1
af+ 0x803f626 (0x682-0x626) SomethingWithGPIOC_TIM7_Status
af+ 0x803f682 (0x6D6-0x682) SomethingWithGFX_Info
af+ 0x803f6d6 (0x708-0x6D6) SomethingBeforeOsSemCreate   
af+ 0x8043ac8 (0xAE4-0xAC8) TimerIRQ_Sub2
af+ 0x8043ae4 (0xB62-0xAE4) SomethingWithTIM1_TIM8
af+ 0x8043e62 (0xBEE-0xE62) 
af+ 0x8043bee (0xC78-0xBEE) SomethingTestingTIM1_TIM8
af+ 0x8043c78 (0xCE0-0xC78) SomethingElseWithTIM1_TIM8
Cd (0xD20-0xCE0) @0x8043ce0 # looks like data, not executable. End of a C module ?
af+ 0x8043d62 (0xD84-0xD62) func_3d62
af+ 0x8043d84 (0xD9C-0xD84) TimerIRQ_Sub3
af+ 0x8043d9c (0xDCA-0xD9C) TimerIRQ_Sub5
af+ 0x8043dca (0xDD2-0xDCA) CobbleUpR1_and_StoreInR0plus16 # uxth mvns strh - holy sh..
af+ 0x8043dd2 (0xDE4-0xDD2) func_3dd2 # followed by OS_ENTER_CRITICAL with a strange opcode @ 0x8043de4 !

af+ 0x8047bac (0xC06-0xBAC) TimerIRQ_Sub1
af+ 0x8047c06 (0xC2E-0xC06) TimerIRQ_Sub4
af+ 0x8047c2e (0xC56-0xC2E) TimerIRQ_Sub8
af+ 0x8047c56 (0xC8E-0xC56) WaitAndDoSomethingWithGPIOC
af+ 0x8047c8e (0xCEC-0xC8E) SomethingWithChannelInfo2
af+ 0x8047cec (0xD00-0xCEC) func_7cec

af+ 0x804811a (0x23A-0x11A) TimerIRQ_Sub6
af+ 0x804823a (0x2C4-0x23A) func_823a

af+ 0x80522f4 (0x310-0x2F4) TimerIRQ_Sub7_writes_DAC # accesses the DAC-registers
af+ 0x8052310 (0x32C-0x310) SomethingElseWritingDAC


# subroutines called from the 'RTC wakeup' IRQ handler (but also some timer event handlers)
af+ 0x802b6c0 (0x6DE-0x6C0) RTCWakeupIRQ_Sub1
af+ 0x804811a (0x23A-0x11A) RTCWakeupIRQ_Sub2

# subroutines called from by DMA handlers
af+ 0x804e768 4 DMAHandler_Sub1
af+ 0x804e7ba 4 DMAHandler_Sub2


# various functions easily recognizeable by push with lr / pop with pc ...
#  .. rename when their purpose is known
af+ 0x8095824 10 func_5824
af+ 0x809582e 10 func_582e
af+ 0x8095838 20 func_5838
af+ 0x0802b80a 4 StoreHalfR1_in_R0plus0x18
af+ 0x0802b80e 4 StoreHalfR1_in_R0plus0x1A
af+ 0x0802b812 (0x20-0x12) StoreR1R2_in_R0plus0x18_or_1A
af+ 0x0802b820 (0x72-0x20) func_b820
af+ 0x0804dd12 (0x36-0x12) CalledFromSPI0_ReadReg
af+ 0x0802B7EE (0x80a-0x7ee) CalledFromSomeBitbangIO

# more stuff seen when "panning around" in r2's Visual mode...
s 0x08094800 # Look at this whatever-it-is in 'V'isual mode / pXA .. 
             # with 64 bytes per line, it looks like a bitmap graphic
CC Font or pixel graphic with 64 bytes per line
Cd (0x80952D0-0x8094800) # data, no executable code. Exact length unknown.

s 0x080f81b4 # 'wide' strings, 16 bit where 8 bits per character would be sufficient
f Wide_Strings @ $$
CC Wide strings, 'USB Mode' etc
Cs (0x80f8510-$$) # guesstimated length by address difference

# next : welcomebmp @ 80f8510, already annotated

s 0x080f86a0 # 8-bit strings ..
f ASCII_Strings @ $$
CC ASCII strings, 'uC/OS-II TmrLock' etc
Cs (0x80f86da-$$) # guesstimated length by address difference

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# BELOW: None-application-specific stuff from cpu.r 
#       (modified by DL4YHF to see a few more ON-CHIP PERIPHERAL REGISTERS)
#    Note: We use the hardware register names as in "RM0090" (STM32F40x Reference Manual) where applicable,
#          not the bulky junk from ST's "STM32cube-MX" or whatever they call it now.
#          'f name 12 @ 33' sets flag 'name' with length 12 at offset 33
#          'f' lists them, possibly sorted by order of definition (not address).
#    Here, flags are also used for special function registers ("on-chip peripherals").

#f io_0x40000400 @ 0x40000400
f io_TIM3 @ 0x40000400

#f io_0x40000800 @ 0x40000800
f io_TIM4 @ 0x40000800

#f io_0x40000c00 @ 0x40000c00
f io_TIM5 @ 0x40000c00

f io_TIM6 @ 0x40001000
f io_TIM7 @ 0x40001400

#f io_0x40002800 @ 0x40002800
f io_RTC @ 0x40002800 # offset 0 : "RTC_TR" (RM0090 page 817)

f io_RTC_DR @ 0x40002804
f io_RTC_CR @ 0x40002808
f io_RTC_ISR @ 0x4000280c
f io_RTC_PRER @ 0x40002810
f io_RTC_WUTR @ 0x40002814
f io_RTC_CALIBR @ 0x40002818
f io_RTC_ALARMAR @ 0x4000281C
f io_RTC_ALARMBR @ 0x40002820
f io_RTC_WPR @ 0x40002824
f io_RTC_TAFCR @ 0x40002840
f io_RTC_BKP0R @ 0x40002850

f io_IWDG @ 0x40003000
f io_IWDG_PR @ 0x40003004
f io_IWDG_RLR @ 0x40003008
f io_IWDG_SR @ 0x40003008

f io_SPI2 @ 0x40003800
f io_SPI2_CR2 @ 0x40003804
f io_SPI2_SR @ 0x40003808
f io_SPI2_DR @ 0x4000380c
f io_SPI2_CRCPR @ 0x40003810
f io_SPI2_RXRCR @ 0x40003814
f io_SPI2_TXRCR @ 0x40003818
f io_SPI2_I2SCFGR @ 0x4000381c
f io_SPI2_I2SPR @ 0x40003820

f io_SPI3 @ 0x40003c00
f io_SPI3_CR2 @ 0x40003c04
f io_SPI3_SR @ 0x40003c08
f io_SPI3_DR @ 0x40003c0c
f io_SPI3_CRCPR @ 0x40003c10
f io_SPI3_RXRCR @ 0x40003c14
f io_SPI3_TXRCR @ 0x40003c18
f io_SPI3_I2SCFGR @ 0x40003c1c
f io_SPI3_I2SPR @ 0x40003c20

f io_I2S3ext @ 0x40004000 # I2S is another SPI port, usable as I2S (digital audio). RM0090 Rev7 page 884.
f io_I2S3_CR2 @ 0x40004004
f io_I2S3_SR @ 0x40004008
f io_I2S3_DR @ 0x4000400c
f io_I2S3_CRCPR @ 0x40004010
f io_I2S3_RXRCR @ 0x40004014
f io_I2S3_TXRCR @ 0x40004018
f io_I2S3_I2SCFGR @ 0x4000401c
f io_I2S3_I2SPR @ 0x40004020


f io_USART3 @ 0x40004800

f io_I2C1 @ 0x40005400

f io_PWR @ 0x40007000 # RM0090 Rev7 page 146.
f io_PWR_CSR @ 0x40007004

f io_DAC @ 0x40007400 # RM0090 Rev7 page 450.
f io_DAC_SWTRIG @ 0x40007404
f io_DAC_DHR12R1 @ 0x40007408
f io_DAC_DHR12L1 @ 0x4000740c
f io_DAC_DHR8R1 @ 0x40007410
f io_DAC_DHR8R2 @ 0x40007414
f io_DAC_DHR12L2 @ 0x40007418
f io_DAC_DHR8R2 @ 0x4000741c
f io_DAC_DHR12RD @ 0x40007420
f io_DAC_DHR12LD @ 0x40007424
f io_DAC_DHR8RD @ 0x40007428
f io_DAC_DOR1 @ 0x4000742c
f io_DAC_DOR2 @ 0x40007430
f io_DAC_SR @ 0x40007434


f io_0x4000f88d @ 0x4000f88d # wtf.. ?

f io_TIM1 @ 0x40010000

f io_TIM8 @ 0x40010400

f io_USART1 @ 0x40011000

f io_USART6 @ 0x40011400

f io_ADC123 @ 0x40012000
f io_ADC1_DATA @ 0x4001204c # RM0090 Rev7 page 427
f io_ADC2 @ 0x40012100
f io_ADC2_DATA @ 0x4001214c
f io_ADC3 @ 0x40012200
f io_ADC_COMMON_SR @ 0x40012300
f io_ADC_COMMON_CR @ 0x40012304
f io_ADC_COMMON_DR @ 0x40012308


f io_SPI1 @ 0x40013000

f io_SYSCFG @ 0x40013800 # RM0090 Rev7 page 291
f io_SYSCFG_PMC @ 0x40013804
f io_SYSCFG_EXTICR1 @ 0x40013808
f io_SYSCFG_EXTICR2 @ 0x4001380C
f io_SYSCFG_EXTICR3 @ 0x40013810
f io_SYSCFG_EXTICR4 @ 0x40013814
f io_SYSCFG_CMPCR @ 0x40013820

f io_EXTI @ 0x40013c00 # RM0090 Rev7 page 384
f io_EXTI_EMR @ 0x40013c04
f io_EXTI_RTSR @ 0x40013c08
f io_EXTI_FTSR @ 0x40013c0c
f io_EXTI_SWIER @ 0x40013c10
f io_EXTI_PR @ 0x40013c14

f io_GPIOA @ 0x40020000

f io_GPIOB @ 0x40020400

f io_GPIOC @ 0x40020800

f io_GPIOD @ 0x40020c00

f io_GPIOE @ 0x40021000
f io_CRC @ 0x40023000 # RM0090 Rev7 page 114 
f io_CRC_CTRL @ 0x40023008

f io_RCC @ 0x40023800 # RM0090 Rev7 page 263
f io_RCC_PLLCFG @ 0x40023804
f io_RCC_CFGR @ 0x40023808
f io_RCC_CIR @ 0x4002380c
f io_RCC_AHB1 @ 0x40023830 # dl4yhf took some liberty here..
f io_RCC_AHB2 @ 0x40023834
f io_RCC_AHB3 @ 0x40023838
f io_RCC_APB1 @ 0x40023840 # used but not found when referenced @ 0x080943F6 ?!
f io_RCC_APB2 @ 0x40023844
f io_RCC_APB1 @ 0x40023870
f io_RCC_CSR @ 0x40023874
f io_RCC_PLL_I2S @ 0x40023884

f io_FLASH @ 0x40023c00
f io_FLASH_KEY @ 0x40023c04
f io_FLASH_OPT_KEY @ 0x40023c08
f io_FLASH_STATUS @ 0x40023c0c # RM0090 Rev7 page 100
f io_FLASH_CTRL @ 0x40023c10 # RM0090 Rev7 page 102

f io_DMA1 @ 0x40026000 # RM0090 Rev7 page 332 (offset0 = "LISR", whatever that means)
f io_DMA1_HISR @ 0x40026004
f io_DMA1_LIFCR @ 0x40026008
f io_DMA1_HIFCR @ 0x4002600C
f io_DMA1_S0CR @ 0x40026010
f io_DMA1_S0NDTR @ 0x40026014
f io_DMA1_S0PAR @ 0x40026018
f io_DMA1_S0M0AR @ 0x4002601C
f io_DMA1_S0M1AR @ 0x40026020
f io_DMA1_S0FCR @ 0x40026024
f io_DMA1_S1CR @ 0x40026028
f io_DMA1_S1NDTR @ 0x4002602c
f io_DMA1_S1PAR @ 0x40026030
f io_DMA1_S1M0AR @ 0x40026034
f io_DMA1_S1M1AR @ 0x40026038
f io_DMA1_S1FCR @ 0x4002603c
f io_DMA1_S2CR @ 0x40026040
f io_DMA1_S2NDTR @ 0x40026044
f io_DMA1_S2PAR @ 0x40026048
f io_DMA1_S2M0AR @ 0x4002604c
f io_DMA1_S2M1AR @ 0x40026050
f io_DMA1_S2FCR @ 0x40026054
f io_DMA1_S5CR @ 0x40026088
f io_DMA1_S5NDTR @ 0x4002608c
f io_DMA1_S5PAR @ 0x40026090
f io_DMA1_S5M0AR @ 0x40026094
f io_DMA1_S5M1AR @ 0x40026098
f io_DMA1_S5FCR @ 0x4002609c


f io_DMA2 @ 0x40026400
f io_DMA2_HISR @ 0x40026404
f io_DMA2_LIFCR @ 0x40026408
f io_DMA2_HIFCR @ 0x4002640C
f io_DMA2_S0CR @ 0x40026410
f io_DMA2_S0NDTR @ 0x40026414
f io_DMA2_S0PAR @ 0x40026418
f io_DMA2_S0M0AR @ 0x4002641C
f io_DMA2_S0M1AR @ 0x40026420
f io_DMA2_S0FCR @ 0x40026424
f io_DMA2_S1CR @ 0x40026428
f io_DMA2_S1NDTR @ 0x4002642c
f io_DMA2_S1PAR @ 0x40026430
f io_DMA2_S1M0AR @ 0x40026434
f io_DMA2_S1M1AR @ 0x40026438
f io_DMA2_S1FCR @ 0x4002643c
f io_DMA2_S2CR @ 0x40026440
f io_DMA2_S2NDTR @ 0x40026444
f io_DMA2_S2PAR @ 0x40026448
f io_DMA2_S2M0AR @ 0x4002644c
f io_DMA2_S2M1AR @ 0x40026450
f io_DMA2_S2FCR @ 0x40026454
f io_DMA2_S3CR @ 0x40026458
f io_DMA2_S3NDTR @ 0x4002645c
f io_DMA2_S3PAR @ 0x40026460
f io_DMA2_S3M0AR @ 0x40026464
f io_DMA2_S3M1AR @ 0x40026468
f io_DMA2_S3FCR @ 0x4002646c
f io_DMA2_S5CR @ 0x40026488
f io_DMA2_S5NDTR @ 0x4002648c
f io_DMA2_S5PAR @ 0x40026490
f io_DMA2_S5M0AR @ 0x40026494
f io_DMA2_S5M1AR @ 0x40026498
f io_DMA2_S5FCR @ 0x4002649c

f io_USB_OTG_HS @ 0x40040000 # RM0090 Rev7 page 1450 (offset0 = "GOTGCTL")
f io_USB_OTG_HS_GOTGINT @ 0x40040004 # omitted a bunch of these 'intuitive' register names..
 
f io_0x400a6666 @ 0x400a6666 # don't think this is a valid "I/O"-address


# ABOVE: stuff formerly in cpu.r, last edited by DL4YHF 2016-01-03
#
# BELOW: Radare2 instructions to "produce" a nicer disassembly listing, with hex-dumps where applicable .
#    Output redirected to a plain old text file . 
#    A '> listing.txt' after a command creates a new file and writes the output to it.
#    A '>> listing.txt' after the command APPENDS the output to a file. 
#    But due to some super-stupid bug somewhere (radare2.exe ? cmd.exe ?), 
#    lines in files created this way were terminated with 0x0A 0x0D instead of 0x0D 0x0A !
#    This stupidity can be 'undone' with a good text editor (like Notepad++) or Python :
#    Replacing the bizarre "\n\r" with "\r\n" (or, if you prefer "\n"). Everything else is insane.
#
#    Some of the "listable elements" where checked interactively, using commands like these:
#      'pxw' = "print hex word" (a word has 32 bits here).
#      'pdf @FuncName' = "print disassemble function" (only for functions with an already known length)
# 
# "Your orders please" .. Outcomment all for testing, or those parts of the listing that you don't need :


# Show current working directory in the first line, so we know the original firmware version (e.g. d13.020) later
pwd > listing.txt # only ONE '>' here to create a fresh (empty) output file

# Vector Table as a 32-bit hex dump:
pxw 0x188 @VectorTable >> listing.txt

# Reset_Handler, SystemInit() and __main() next, because  
# this is the first code executed after quitting the bootloader .
# List these functions by order of execution, not by memory address : 
pdf @Reset_Handler >> listing.txt
pdf @SystemInit >> listing.txt
pdf @RCC_Init >> listing.txt # called from SystemInit() so list it here
pdf @__main >> listing.txt # called after SystemInit() so list it here
pdf @FPU_Init >> listing.txt # contains an esoteric "vmsr fpscr, r0" (FPU-related, PM0214 page 169)
pdf @_main2 >> listing.txt # 2nd function called from __main() [not main()]
pdf @_main2_init_sub1 >> listing.txt
pdf @_main2_init_sub2 >> listing.txt # almost as bizarre as Keil's "scatterload" !
pdf @_main2_init_sub3 >> listing.txt # still none of the already annotated 'upper level' functions in sight !
pdf @_main3 >> listing.txt
pdf @CalledForever >> listing.txt
pdf @func_1d7e >> listing.txt
pdf @func_e2e0 >> listing.txt
pdf @InitGlobalsAndStartRealTimeKernel >> listing.txt
pdf @ClearSomeHalfWordInRAM >> listing.txt
pdf @ClearSomeVariablesInRAM >> listing.txt
pdf @ClearSomeBlocksInRAM >> listing.txt
pdf @Func4_of_10 >> listing.txt
pdf @Func5_of_10 >> listing.txt
pdf @Func6_of_10 >> listing.txt
pdf @Create_uCOS_Idle_Task >> listing.txt
pdf @OS_IdleTask >> listing.txt
pdf @OSIdleTaskHook >> listing.txt
pdf @WaitForInterruptInIdle >> listing.txt
pdf @CreateTwoSemaphores >> listing.txt
pdf @DoNothing_only_BX_LR >> listing.txt
pdf @ManyStrangeSimpleMoves >> listing.txt
pdf @Create_Start_Task_AndSetItsName >> listing.txt

# endless 'Start' task and functions called from there...
pdf @Start >> listing.txt
pdf @SomethingWithGPIOA_and_RadioStatus1 >> listing.txt
pdf @SomethingWithGPIOC_and_Backlight_Timer >> listing.txt
pdf @FuncWithAwfulLongSwitch >> listing.txt
pdf @LongSwitchWithRadioStatus1 >> listing.txt
pdf @SomethingWithChannelsRadioConfigAndBeeps >> listing.txt
pdf @SomethingWithLongpressSettingRadioStatus1 >> listing.txt
pdf @SomethingWithGuiOpmode2 >> listing.txt
pdf @SomethingWithGPIOA_and_RadioStatus1 >> listing.txt
pdf @SomethingWithGPIOC_and_Backlight_Timer >> listing.txt
pdf @SomethingWithRadioStatus1 >> listing.txt
pdf @func_3df2 >> listing.txt
pdf @SetBit30_ptrR0plus8 >> listing.txt
pdf @ExtendU16toU32_ptrR0plus4C >> listing.txt

# EXCEPTION- and INTERRUPT vectors (can be told from each other by the _IRQ in the names)
s DummyForUnusedIRQs  # ex: s NMI_Handler
pD (NextAfterHandlers-DummyForUnusedIRQs) >> listing.txt # this disassembles MOST (but not all) 
  # interrupt- and exception handlers in a contiguous block.
  # "pd N" disassembles N opcodes, "pD" diassembes N bytes. 
  # Both are aware of constant DATA blocks, and shows 'data' in hex.

# Functions called from various INTERRUPT SERVICE HANDLERS :
pdf @CobbleUpR1_and_StoreInR0plus16 >> listing.txt # may be a tone generator (?) 
pdf @Sub2CalledFromTIM8ISR >> listing.txt
pdf @SysTick_Sub1 >> listing.txt
pdf @SysTick_Sub2 >> listing.txt
pdf @nop_BX_LR >> listing.txt
pdf @func_3f90 >> listing.txt
pdf @func_3fca >> listing.txt
pdf @func_4024 >> listing.txt
pdf @func_411c >> listing.txt
pdf @WakeUp_Sub1 >> listing.txt
pdf @RTCWakeupIRQ_Sub1 >> listing.txt
pdf @RTCWakeupIRQ_Sub2 >> listing.txt
pdf @ClearEXTIPendingBits_R0 >> listing.txt
pdf @CalledFromUSB_IRQ >> listing.txt
pdf @CalledFromPinChangeIRQ >> listing.txt
pdf @func_5824 >> listing.txt
pdf @func_582e >> listing.txt
pdf @func_5838 >> listing.txt
    
# Disassemble individual functions, most of these annotated by Travis and others:
pdf @md380_create_main_menu_entry >> listing.txt
pdf @md380_create_menu_entry >> listing.txt
pdf @md380_menu_entry_back >> listing.txt
pdf @menu_draw_something >> listing.txt
pdf @menu_draw_something2 >> listing.txt
pdf @menu_draw_something3 >> listing.txt
pdf @menu_draw_something4 >> listing.txt
pdf @menu_draw_something5 >> listing.txt
pdf @kb_handler >> listing.txt

    # Graphic functions
pdf @gfx_drawbmp >> listing.txt
pdf @gfx_blockfill >> listing.txt
pdf @gfx_linefill >> listing.txt
pdf @gfx_newline >> listing.txt
pdf @gfx_get_xpos >> listing.txt
pdf @gfx_get_ypos >> listing.txt
pdf @gfx_set_fg_color >> listing.txt
pdf @gfx_set_fg_color2 >> listing.txt
pdf @gfx_set_bg_color >> listing.txt
pdf @gfx_set_bg_color2 >> listing.txt
pdf @gfx_select_font >> listing.txt
pdf @gfx_drawchar_pos >> listing.txt
pdf @gfx_drawchar >> listing.txt
pdf @gfx_clear3 >> listing.txt
pdf @gfx_drawtext >> listing.txt
pdf @gfx_drawtext2 >> listing.txt
pdf @gfx_drawtext3 >> listing.txt
pdf @gfx_drawtext4 >> listing.txt
pdf @gfx_drawtext5 >> listing.txt
pdf @gfx_drawtext6 >> listing.txt
pdf @gfx_drawtext7 >> listing.txt
pdf @gfx_drawtext8 >> listing.txt
pdf @gfx_drawtext9 >> listing.txt
pdf @gfx_drawtext10 >> listing.txt
pdf @gfx_drawchar_unk >> listing.txt
pdf @draw_zone_channel >> listing.txt

    # RTOS kernel
pdf @OSSemCreate >> listing.txt
pdf @OSSemPend >> listing.txt
pdf @OSSemPost >> listing.txt
pdf @OSTaskCreateExt >> listing.txt
pdf @OSTaskNameSet >> listing.txt
pdf @OSTimeDly >> listing.txt
pdf @md380_OSMboxPost >> listing.txt
pdf @md380_OSMboxPend >> listing.txt
pdf @OS_ENTER_CRITICAL >> listing.txt
pdf @OS_EXIT_CRITICAL >> listing.txt

    # USB ("API" layer)
pdf @usb_setcallbacks >> listing.txt
pdf @usb_send_packet >> listing.txt
pdf @usb_do_setup >> listing.txt
pdf @usb_dnld_handle >> listing.txt
pdf @usb_upld_handle >> listing.txt
pdf @usb_dfu_write >> listing.txt
pdf @usb_dfu_read >> listing.txt
pdf @usb_serialnumber >> listing.txt    

    # C5000 'baseband chip'
pdf @c5000_spi0_writereg >> listing.txt
pdf @c5000_spi0_readreg >> listing.txt
pdf @c5000_spi0_writereg_1 >> listing.txt
pdf @some_func_pend >> listing.txt # called from c5000_spi0_write/readreg
pdf @some_bitband_io_range >> listing.txt # called from c5000_spi0_write/readreg
pdf @some_bitband_io >> listing.txt # called from the above..
pdf @StoreHalfR1_in_R0plus0x18 >> listing.txt # first called from the bitbang / bitband - thing ..
pdf @StoreHalfR1_in_R0plus0x1A >> listing.txt
pdf @StoreR1R2_in_R0plus0x18_or_1A >> listing.txt
pdf @some_func_post >> listing.txt # called from c5000_spi0_write/readreg
pdf @dmr_call_start >> listing.txt
pdf @dmr_before_squelch >> listing.txt
pdf @dmr_sms_arrive >> listing.txt
pdf @dmr_call_end >> listing.txt
pdf @dmr_CSBK_handler >> listing.txt

# Finally append a complete list of all 'known' symbols (outcomment if not needed) :
f >> listing.txt # 'f' (without args) lists all "flags". Kind of our symbol table... but too long
