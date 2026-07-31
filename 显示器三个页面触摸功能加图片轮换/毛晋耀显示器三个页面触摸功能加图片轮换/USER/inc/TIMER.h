#ifndef _TIMER_H
#define _TIMER_H

#include "stm32f4xx.h" /* Ìæ»»µôÔ­À´µÄ main.h */

void Timer_Delay_Init(void);
void timer_delay_us(uint32_t us);
void timer_delay_ms(uint32_t ms);

#endif
