#ifndef __SERVO_H
#define __SERVO_H

#include "stm32f4xx.h"

/**
 * @brief  舵机PWM初始化，使用TIM5通道2输出50Hz信号，对应引脚PA1
 */
void Servo_Init(void);

/**
 * @brief  设置舵机转动角度
 * @param  angle: 目标角度，范围 0 ~ 180°
 */
void Servo_SetAngle(float angle);

#endif
