#ifndef _MLX90614_H
#define _MLX90614_H

#include "main.h"                 /* STM32F4系列头文件 */

/* ===== MLX90614 I2C地址 ===== */
#define MLX90614_WRITE_ADDR  0xB4  /* MLX90614写地址(0x5A<<1) */
#define MLX90614_READ_ADDR   0xB5  /* MLX90614读地址(0x5A<<1|1) */

/* ===== 全局变量声明 ===== */
extern float te_item;             /* MLX90614物体温度值(摄氏度) */

/* ===== 函数声明 ===== */
void MLX90614_Config(void);       /* MLX90614初始化函数声明 */
void Mlx_90614_GetItemTe(void);   /* MLX90614获取物体温度函数声明 */

#endif
