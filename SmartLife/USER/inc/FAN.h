#ifndef _FAN_H
#define _FAN_H

#include "stm32f4xx.h"  // 提供u16、外设类型等基础定义
#include "KEY.h"
/* ================= 函数声明 ================= */
void Fan_GPIO_Config(void);     /* 风扇GPIO初始化(PC8复用为TIM8_CH3) */
void Fan_TIM_Config(void);      /* 风扇定时器时基初始化(TIM8, 1kHz频率) */
void Fan_PWM_Config(void);      /* 风扇PWM输出通道初始化(TIM8_CH3) */
void Fan_Config(void);          /* 风扇完整初始化总入口 */
void Fan_SetSpeed(u16 speed);   /* 风扇速度控制函数，参数范围0~999 */

#endif
