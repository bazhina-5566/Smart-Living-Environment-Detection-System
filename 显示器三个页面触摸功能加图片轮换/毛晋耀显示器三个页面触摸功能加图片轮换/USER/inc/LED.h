#ifndef __LED_H
#define __LED_H
// 假设 LED1 对应 GPIO_Pin_12
#define LED1_TURN    GPIO_ToggleBits(GPIOD, GPIO_Pin_12)
// 如果还想定义 LED2、LED3:
#define LED2_TURN    GPIO_ToggleBits(GPIOD, GPIO_Pin_13)
#define LED3_TURN    GPIO_ToggleBits(GPIOD, GPIO_Pin_14)
#include "stm32f4xx.h" 

/* 1. 先定义类型，再包含其他子头文件 */
typedef enum {
    LED_MODE_OFF = 0, 
    LED_MODE_ON,      
    LED_MODE_FLASH,   
    LED_MODE_BREATH   
} LED_Mode_t;

#include "TIMER.h"

/* 函数声明 */
void LED_Init(void);
void LED_SetAll(uint8_t state);
void LED_Mode_On(void);
void LED_Mode_Flash(void);
void LED_Mode_Breath(void);
void LED_Mode_Update(void);
#endif /* 记得在末尾按一下 Enter 键回车换行 */
