#ifndef _SHT30_H
#define _SHT30_H

#include "main.h"

/* ===== SHT30 I2C地址 ===== */
#define SHT30_WRITE_ADDR  0x88    /* SHT30写地址(0x44<<1) */
#define SHT30_READ_ADDR   0x89    /* SHT30读地址(0x44<<1|1) */

/* ===== 全局变量声明 ===== */
extern float te_value;            /* 温度值(摄氏度) */
extern float hu_value;            /* 湿度值(百分比) */
extern u8 sht30_flag;             /* SHT30采集标志(1=需要采集) */

/* ===== 函数声明 ===== */
void Sht30_Config(void);          /* SHT30初始化函数声明 */
void Sht30_GetValue(void);         /* SHT30获取温湿度数据函数声明 */

#endif
