#ifndef _PROJECT_H
#define _PROJECT_H

#include "stm32f4xx.h"  /* STM32F4寄存器定义和库函数 */
#include "KEY.h"


/* ==================== 风扇调速与RGB状态宏定义 ==================== */

/* --- 档位范围 --- */
#define FAN_GEAR_MAX         10      /* 最大档位(0~10, 对应占空比0%~100%) */
#define FAN_GEAR_OFF          0      /* 停止档位 */
#define FAN_DUTY_STEP        10      /* 每档占空比步进(%), gear*10 = 占空比% */

/* --- RGB状态标志 --- */
#define FAN_RGB_OFF           0      /* RGB熄灭(风扇停止) */
#define FAN_RGB_GREEN         1      /* 绿灯(低档: 占空比≤50%) */
#define FAN_RGB_BLUE          2      /* 蓝灯(中档: 51%~80%) */
#define FAN_RGB_RED           3      /* 红灯(高档: 81%~100%) */

/* --- RGB切换阈值(占空比%) --- */
#define FAN_DUTY_GREEN_MAX    50     /* ≤50% 绿灯区间 */
#define FAN_DUTY_BLUE_MAX     80     /* 51%~80% 蓝灯区间, >80% 红灯区间 */

/* --- 按键参数 --- */
#define FAN_DEBOUNCE_MS       30     /* 按键消抖时间(ms) */

/* ==================== 全局变量声明(在project.c中定义) ==================== */
extern u8 fan_gear;                  /* 当前风扇档位(0~10), 核心控制变量 */
extern u8 fan_rgb_state;             /* 当前RGB状态(FAN_RGB_OFF/GREEN/BLUE/RED) */


/* ==================== 函数声明 ==================== */
void Fan_KeyControl(void);           /* 按键调速(KEY2增速/KEY3减速/单击触发)+RGB联动 */
void Fan_DisplayStatus(void);        /* LCD显示风扇转速进度条+RGB颜色状态 */

void Sensor_Display_All(void);       /* 读取并显示火焰+光敏传感器(LCD标签+串口打印) */


#endif
