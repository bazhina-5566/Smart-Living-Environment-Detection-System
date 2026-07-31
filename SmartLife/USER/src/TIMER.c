#include "TIMER.h"

void Timer_Delay_Init(void)
{
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    TIM_TimeBaseInitTypeDef TIM_InitStruct = {0};
    TIM_InitStruct.TIM_Prescaler = 84 - 1;
    TIM_InitStruct.TIM_Period = 65536 - 1;  
    TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;   
    TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;  
    TIM_TimeBaseInit(TIM3, &TIM_InitStruct);   
    TIM_Cmd(TIM3, ENABLE);
}

// 硬件TIM3精准微秒延时
void timer_delay_us(uint32_t us)
{
    TIM_SetCounter(TIM3, 0);          // 计数器清零，从头计数
    while(TIM_GetCounter(TIM3) < us); // 循环等待计数达到指定微秒数
}

// 基于us封装毫秒延时
void timer_delay_ms(uint32_t ms)
{
    while(ms--)
    {
        timer_delay_us(1000);
    }
}