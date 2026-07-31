#ifndef _UI_H                                        /* 防止头文件重复包含 */
#define _UI_H

#include "main.h"                                   /* 包含主头文件,使用其中的类型定义 */

/* 全局变量声明: 页面UI刷新标志,在 UI.c 中定义 */
/* ui_flag=0: 需要刷新静态UI; ui_flag=1: 静态UI已刷新,只执行动态代码 */
extern u8 ui_flag;
extern u8 touch_lock;    // 触摸防抖锁
extern uint8_t led_mode;
extern const char* led_mode_str[];
/* 页面函数声明 */
void Page_Home(void);                                /* 界面一: 首页 (帅照/签名) */
void Page_One(void);                                 /* 界面二: 4区域外设控制页 */
void Page_Two(void);                                 /* 界面三: 多图/GIF展示页 */
void Page_Sensor(void);
#endif                                               /* _UI_H */
