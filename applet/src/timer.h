/*
 * timer.h
 *
 *  Created on: Jan 7, 2017
 *      Author: clay
 */

#ifndef TIMER_H_
#define TIMER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void New_TIM12_IRQHandler(void);
void enable_TIM12_timer_interrupt(void);

extern uint8_t TIM12_enabled;

#endif /* TIMER_H_ */
