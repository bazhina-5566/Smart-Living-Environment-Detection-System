#ifndef _ADC_H
#define _ADC_H

#include "main.h"

extern float light_value;          /* 光敏电阻值(百分比) */

void ADC1_Config(void);            /* ADC1初始化函数声明 */
void ADC1_GetValue(void);          /* ADC1获取光敏电阻数据函数声明 */

#endif