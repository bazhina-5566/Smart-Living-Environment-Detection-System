#include "main.h"                        

u8 sht30_flag = 1;
volatile uint32_t sys_tick = 0;

/* 外部声明 beep_timer（定义在 main.c） */
extern volatile uint32_t beep_timer;        // 新增

void SysTick_Handler(void)
{
    sys_tick++;

    /* --- 蜂鸣器定时关闭（新增） --- */
    if (beep_timer > 0)
    {
        beep_timer--;
        if (beep_timer == 0)
        {
            Buzzer_Off();                   // 时间到，关闭蜂鸣器
        }
    }
    /* --- 蜂鸣器处理结束 --- */

    static u16 time_cnt = 0;
    time_cnt++;
    if (time_cnt >= 1000)
    {
        time_cnt = 0;
        sht30_flag = 1;
    }
}