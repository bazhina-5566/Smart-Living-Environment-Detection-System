#ifndef _FLAME_H
#define _FLAME_H

#include "main.h"

extern float flame_value;          /* 火焰传感器值(百分比) */

void Flame_Config(void);           /* 火焰传感器初始化函数声明 */
void Flame_GetValue(void);         /* 火焰传感器获取数据函数声明 */

#endif