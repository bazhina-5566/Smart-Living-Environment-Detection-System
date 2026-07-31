#include "main.h"

/**
  * @brief  依据当前舵机角度更新风扇档位与继电器开关状态
  * @param  angle 当前舵机角度 (0.0 ~ 180.0)
  * @retval 无
  */
void Update_Device_Links(float angle)
{
    // 1. 继电器联动逻辑 (修复 45 度死区)
    if (angle < 45.0f)
    {
        Relay_On();
        //printf("[Linkage] Relay: ON (Angle < 45 deg)\r\n");
    }
    else // >= 45.0f
    {
        Relay_Off();
       // printf("[Linkage] Relay: OFF (Angle >= 45 deg)\r\n");
    }

    // 2. 风扇 PWM 档位联动逻辑 (使用 <= 连续区间，消除 120/150/180 死区)
    if (angle > 90.0f && angle <= 120.0f)
    {
        Fan_SetSpeed(300);  // 一档：约 30% 占空比
       // printf("[Linkage] Fan: Level 1 (30%%)\r\n");
    }
    else if (angle > 120.0f && angle <= 150.0f)
    {
        Fan_SetSpeed(600);  // 二档：60% 占空比
        //printf("[Linkage] Fan: Level 2 (60%%)\r\n");
    }
    else if (angle > 150.0f && angle <= 180.0f)
    {
        Fan_SetSpeed(999);  // 三档：100% 占空比
       // printf("[Linkage] Fan: Level 3 (100%%)\r\n");
    }
    else
    {
        Fan_SetSpeed(0);    // 其他区间关闭风扇
       // printf("[Linkage] Fan: OFF\r\n");
    }
}

/**
  * @brief  根据当前系统 LED 模式轮询执行对应的动作
  * @param  mode 当前 LED 模式
  * @retval 无
  */
void LED_Process_Mode(LED_Mode_t mode)
{
    switch (mode)
    {
        case LED_MODE_OFF:
            LED_SetAll(0);
            break;

        case LED_MODE_ON:
            LED_Mode_On();
            break;

        case LED_MODE_FLASH:
            LED_Mode_Flash(); // 内部带 100ms 延时
            break;

        case LED_MODE_BREATH:
            LED_Mode_Breath(); // 内部带微秒级软 PWM 渐变
            break;

        default:
            LED_SetAll(0);
            break;
    }
}

/**
  * @brief  解析 USART1 接收到的指令字符串并执行对应操作
  * @param  cmd 接收到的字符串
  * @retval 无
  */
void Parse_Command(char *cmd)
{
    // 去除末尾的换行符 \r 或 \n
    cmd[strcspn(cmd, "\r\n")] = '\0';

    // ---------------- 1. LED 指令解析 ----------------
    if (strcasecmp(cmd, "led on") == 0)
    {
        current_mode = LED_MODE_ON;
        printf("[CMD] LED set to ON\r\n");
    }
    else if (strcasecmp(cmd, "led off") == 0)
    {
        current_mode = LED_MODE_OFF;
        printf("[CMD] LED set to OFF\r\n");
    }
    else if (strcasecmp(cmd, "led flash") == 0)
    {
        current_mode = LED_MODE_FLASH;
        printf("[CMD] LED set to FLASH\r\n");
    }
    else if (strcasecmp(cmd, "led breath") == 0)
    {
        current_mode = LED_MODE_BREATH;
        printf("[CMD] LED set to BREATH\r\n");
    }
    // ---------------- 2. 舵机指令解析（RESET完成后的位置是90°） ----------------
    else if (strncasecmp(cmd, "servo ", 6) == 0)
    {
        char *param = cmd + 6;

        if (strcmp(param, "+10") == 0)
        {
            servo_current_angle += 10.0f;
        }
        else if (strcmp(param, "-10") == 0)
        {
            servo_current_angle -= 10.0f;
        }
        else
        {
            // 解析绝对数值角度
            servo_current_angle = (float)atof(param);
        }

        // 角度安全限幅保护 (0 ~ 180度)
        if (servo_current_angle < 0.0f)   servo_current_angle = 0.0f;
        if (servo_current_angle > 180.0f) servo_current_angle = 180.0f;

        // 设置舵机物理角度
        Servo_SetAngle(servo_current_angle);
        printf("[CMD] Servo Angle: %.1f deg\r\n", servo_current_angle);

        // 触发风扇与继电器的联动更新
        Update_Device_Links(servo_current_angle);
    }
    else
    {
        printf("[CMD Error] Unknown Command: '%s'\r\n", cmd);
    }
}
