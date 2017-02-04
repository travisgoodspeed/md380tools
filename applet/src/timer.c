/*
 * timer.c


Contains all of our timer functions, including interrupts
 *
 *  Created on: Jan 7, 2017
 *      Author: clay
 */

#include "timer.h"

#include "stm32f4xx.h"
#include "misc.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"

#define PERIOD_ON 1000

uint8_t TIM12_enabled = 0;

void enable_TIM12_timer_interrupt(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	/* Enable the TIM5 global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM8_BRK_TIM12_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* TIM12 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = PERIOD_ON; // 1 MHz down to 1 KHz (1 ms)
	TIM_TimeBaseStructure.TIM_Prescaler = 84 - 1; // 24 MHz Clock down to 1 MHz (adjust per your clock)
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM12, &TIM_TimeBaseStructure);
	/* TIM IT enable */
	TIM_ITConfig(TIM12, TIM_IT_Update, ENABLE);

	//This function actually starts the timer.
	TIM_Cmd(TIM12, ENABLE);

	TIM12_enabled = 1;
}

//TIM12 Interrupt is currently being used to dim the backlight from the factory brightness.
void New_TIM12_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM12, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM12, TIM_IT_Update);

		//TIM12->ARR = PERIOD_OFF;              //Sets the value you will count to next before it interrupts
	}
}

