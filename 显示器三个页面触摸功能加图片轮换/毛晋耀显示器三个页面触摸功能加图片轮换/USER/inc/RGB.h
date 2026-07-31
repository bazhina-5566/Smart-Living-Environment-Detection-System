#ifndef _RGB_H
#define _RGB_H

#include "main.h"

/* ===== WS2812B 数据线宏定义(PB15) ===== */
#define RGB_DATA_H    (GPIO_SetBits(GPIOB, GPIO_Pin_15))   /* 数据线置高电平 */
#define RGB_DATA_L    (GPIO_ResetBits(GPIOB, GPIO_Pin_15)) /* 数据线置低电平 */

#define RGB_NUM  4  /* WS2812B彩灯数量(4颗级联：U7→U8→U9→U10) */

/* ===== 函数声明 ===== */
void Rgb_Config(void);      /* RGB彩灯初始化 */
void Rgb_Reset(void);       /* WS2812B复位锁存 */
void Rgb_SendData(u32 color); /* 发送24bit颜色数据 */
void Rgb_Control(u8 r, u8 g, u8 b); /* 设置单颗灯的RGB颜色 */

#endif
