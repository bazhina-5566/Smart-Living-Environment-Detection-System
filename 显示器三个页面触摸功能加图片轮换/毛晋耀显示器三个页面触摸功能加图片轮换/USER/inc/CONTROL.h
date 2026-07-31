#ifndef __CONTROL_H
#define __CONTROL_H
#include "main.h"  
// 声明全局控制变量
extern float servo_current_angle;
extern LED_Mode_t current_mode;

// 联动判断与控制函数
void Update_Device_Links(float angle);

// LED 模式状态机执行函数
void LED_Process_Mode(LED_Mode_t mode);

// 串口指令解析函数
void Parse_Command(char *cmd);

#endif /* __CONTROL_H */
