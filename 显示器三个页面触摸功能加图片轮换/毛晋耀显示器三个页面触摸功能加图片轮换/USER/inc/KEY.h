#ifndef __KEY_H
#define __KEY_H
#include "stm32f4xx.h" /* Ìæ»»µôÔ­À´µÄ main.h */
#define KEY1_PORT    GPIOA
#define KEY1_PIN     GPIO_Pin_0  // PA0

#define KEY2_PORT    GPIOB
#define KEY2_PIN     GPIO_Pin_0  // PB0

#define KEY3_PORT    GPIOB
#define KEY3_PIN     GPIO_Pin_1  // PB1

void KEY_Init(void);
uint8_t KEY_Scan(void);
uint8_t KEY1_count(void); 
uint8_t KEY2_count(void);
uint8_t KEY3_count(void);
#endif
