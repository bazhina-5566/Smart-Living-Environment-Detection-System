#include "fan.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"
#include "rgb.h"
#include "LCD.h"
#include "systick.h"
#include <stdio.h>

/*--------修改已有的函数-----------
Flame_GetValue()  删除了内部printf，串口打印统一由Sensor_Display_All管理
ADC1_GetValue()   删除了内部printf，同上
---------------------------------*/

/*--- 外部变量(在main.c中定义) ---
extern uint8_t ui_flag;
---------------------------------*/

/* ==================== 全局变量定义 ==================== */
u8 fan_gear      = FAN_GEAR_OFF;    /* 当前风扇档位(0~10), 核心控制变量 */
u8 fan_rgb_state = FAN_RGB_OFF;     /* 当前RGB状态(0=灭/1=绿/2=蓝/3=红) */

/* ================================================================== */
/*                  风扇按键调速 + RGB联动控制函数                      */
/* ================================================================== */

/**
 * @brief  按键控制风扇调速(KEY2增速 | KEY3减速 | 单击触发) + RGB联动
 * @param  void
 * @retval void
 * @note   需在主循环中每次迭代都调用(约5~10ms间隔)
 *
 *         核心逻辑:
 *           按键直接操作全局变量 fan_gear(0~10)
 *           fan_gear → 换算 speed(0~999) → Fan_SetSpeed()
 *           fan_gear → 换算 duty_pct%  → 判定 fan_rgb_state
 *           fan_rgb_state → 驱动 WS2812B 四颗灯珠
 *
 *         档位-转速-RGB 对照表:
 *           gear=0  duty=0%    speed=0   → RGB 灭
 *           gear=1  duty=10%   speed=100 → RGB 绿
 *           ...
 *           gear=5  duty=50%   speed=500 → RGB 绿
 *           gear=6  duty=60%   speed=600 → RGB 蓝
 *           ...
 *           gear=8  duty=80%   speed=800 → RGB 蓝
 *           gear=9  duty=90%   speed=900 → RGB 红
 *           gear=10 duty=100%  speed=999 → RGB 红
 */
void Fan_KeyControl(void)
{
    /* ---- 静态变量: 记录按键历史状态, 用于边沿检测与消抖 ---- */
    static u8   key2_last   = 1;         /* 1=释放, 0=按下(上拉, 按下为低) */
    static u8   key3_last   = 1;
    static u32  db2_tm      = 0;         /* 消抖计时 */
    static u32  db3_tm      = 0;
    static u8   key2_stable = 1;         /* 消抖后的稳定电平 */
    static u8   key3_stable = 1;

    u8            changed   = 0;         /* 本次是否有档位变更 */
    u8            duty_pct  = 0;
    u16           speed     = 0;
    u8            i;

    extern volatile uint32_t sys_tick;   /* systick.c 中的系统毫秒计数器 */

    /* ================================================================ */
    /*  1. 硬件消抖: 读原始电平 → 连续FAN_DEBOUNCE_MS稳定后采纳          */
    /* ================================================================ */
    u8 k2_raw = (GPIO_ReadInputDataBit(KEY2_PORT, KEY2_PIN) == 0); /* 0=按下 */
    u8 k3_raw = (GPIO_ReadInputDataBit(KEY3_PORT, KEY3_PIN) == 0);

    /* KEY2 消抖 */
    if (k2_raw != key2_stable)
    {
        db2_tm = sys_tick;
        key2_stable = k2_raw;
    }
    /* KEY3 消抖 */
    if (k3_raw != key3_stable)
    {
        db3_tm = sys_tick;
        key3_stable = k3_raw;
    }

    /* 消抖时间不足则保持上次电平 */
    u8 k2 = ((sys_tick - db2_tm) >= FAN_DEBOUNCE_MS) ? key2_stable : key2_last;
    u8 k3 = ((sys_tick - db3_tm) >= FAN_DEBOUNCE_MS) ? key3_stable : key3_last;

    /* ================================================================ */
    /*  2. KEY2 增速: 下降沿触发(单击), 每按一次增一档                   */
    /* ================================================================ */
    if (k2 == 0 && key2_last == 1)            /* 下降沿: 刚按下 */
    {
        if (fan_gear < FAN_GEAR_MAX)
        {
            fan_gear++;
            changed = 1;
        }
    }
    key2_last = k2;

    /* ================================================================ */
    /*  3. KEY3 减速: 下降沿触发(单击), 每按一次减一档                   */
    /* ================================================================ */
    if (k3 == 0 && key3_last == 1)            /* 下降沿: 刚按下 */
    {
        if (fan_gear > 0)
        {
            fan_gear--;
            changed = 1;
        }
    }
    key3_last = k3;

    /* ================================================================ */
    /*  4. 档位有变化时才换算并更新风扇 + RGB(避免重复操作)              */
    /* ================================================================ */
    if (!changed) return;

    /* --- 4a. 档位 → 转速 speed 值(0~999) --- */
    speed = (u16)fan_gear * 100;        /* gear×100, gear=10时为1000 */
    if (speed > 999) speed = 999;       /* 上限截断 */
    Fan_SetSpeed(speed);

    /* --- 4b. 档位 → 占空比% → RGB状态 --- */
    duty_pct = fan_gear * FAN_DUTY_STEP;    /* 0/10/20/.../100 */

    if (duty_pct == 0)
        fan_rgb_state = FAN_RGB_OFF;
    else if (duty_pct <= FAN_DUTY_GREEN_MAX)
        fan_rgb_state = FAN_RGB_GREEN;
    else if (duty_pct <= FAN_DUTY_BLUE_MAX)
        fan_rgb_state = FAN_RGB_BLUE;
    else
        fan_rgb_state = FAN_RGB_RED;

    /* --- 4c. RGB状态 → WS2812B 四颗灯珠 --- */
    Rgb_Reset();
    for (i = 0; i < 4; i++)
    {
        switch (fan_rgb_state)
        {
            case FAN_RGB_OFF:   Rgb_Control(0,   0,   0  ); break;  /* 灭 */
            case FAN_RGB_GREEN: Rgb_Control(0,   255, 0  ); break;  /* 绿 */
            case FAN_RGB_BLUE:  Rgb_Control(0,   0,   255); break;  /* 蓝 */
            case FAN_RGB_RED:   Rgb_Control(255, 0,   0  ); break;  /* 红 */
        }
    }
    Rgb_Reset();

    /* --- 4d. 串口调试输出 --- */
    printf("[FAN] gear=%d  duty=%d%%  speed=%d  RGB=%s\r\n",
           fan_gear, duty_pct, speed,
           fan_rgb_state == FAN_RGB_OFF   ? "OFF" :
           fan_rgb_state == FAN_RGB_GREEN ? "GREEN" :
           fan_rgb_state == FAN_RGB_BLUE  ? "BLUE"  : "RED");
}

/**
 * @brief  在 LCD 上绘制风扇转速进度条与 RGB 颜色状态
 * @param  void
 * @retval void
 * @note   读取全局变量 fan_gear / fan_rgb_state 进行绘制
 *         布局(16px字号, 位于屏幕下部):
 *           y=178: 清除区域 (178~248)
 *           y=180: "风扇转速: XX%"
 *           y=203: [===========          ] (进度条 200px 宽)
 *           y=228: "RGB: 绿灯"
 *         进度条填充颜色与 fan_rgb_state 同步:
 *           灭→灰   绿→GREEN   蓝→BLUE   红→RED
 */
void Fan_DisplayStatus(void)
{
    u8  duty_pct = fan_gear * FAN_DUTY_STEP;  /* 占空比 0~100 */
    u8  disp[32];
    u16 bx, by, bw, bh;                       /* 进度条坐标和尺寸 */
    u16 fill_w;                               /* 填充宽度(px) */
    u16 fill_color;                           /* 填充颜色 */

    /* ================================================================ */
    /*  1. 清除显示区域(避免叠影) — 放在传感器区域下方                   */
    /* ================================================================ */
    Lcd_Clear(8, 178, 232, 248, WHITE);

    /* ================================================================ */
    /*  2. 标题行: "风扇转速: 60%"                                       */
    /* ================================================================ */
    Lcd_ZikuStr(10, 180, BLACK, WHITE, 16, (u8 *)"\xB7\xE7\xC9\xC8\xD7\xAA\xCB\xD9:");
    sprintf((char *)disp, "%d%%", duty_pct);
    Lcd_ZikuStr(100, 180, RED, WHITE, 16, disp);

    /* ================================================================ */
    /*  3. 进度条(水平柱状)                                              */
    /*     边框: BLACK  填充: 与RGB状态同色                             */
    /*     尺寸: 200x18 px     位置: (15, 203)                          */
    /* ================================================================ */
    bx = 15;   by = 203;                  /* 起点 */
    bw = 200;  bh = 18;                   /* 宽200 高18(含2px边框) */

    /* --- 3a. 画边框(空心矩形: 上下左右4条线) --- */
    Lcd_Clear(bx,         by,          bx + bw, by + 1,      BLACK);  /* 上边框 */
    Lcd_Clear(bx,         by + bh - 1, bx + bw, by + bh,     BLACK);  /* 下边框 */
    Lcd_Clear(bx,         by,          bx + 1,  by + bh,     BLACK);  /* 左边框 */
    Lcd_Clear(bx + bw - 1, by,         bx + bw, by + bh,     BLACK);  /* 右边框 */

    /* --- 3b. 选择填充颜色(与RGB状态同色) --- */
    switch (fan_rgb_state)
    {
        case FAN_RGB_GREEN: fill_color = GREEN;   break;
        case FAN_RGB_BLUE:  fill_color = BLUE;    break;
        case FAN_RGB_RED:   fill_color = RED;     break;
        default:            fill_color = 0x8410;  break;  /* OFF→灰色 */
    }

    /* --- 3c. 填充(内缩1px, 避免覆盖边框) --- */
    fill_w = (u16)((bw - 2) * duty_pct / 100);
    if (fill_w > 0)
    {
        Lcd_Clear(bx + 1, by + 1, bx + 1 + fill_w, by + bh - 1, fill_color);
    }
    /* 未填充部分保持 WHITE (已被步骤1清除) */

    /* ================================================================ */
    /*  4. RGB 状态行: "RGB: 绿灯"                                       */
    /* ================================================================ */
    Lcd_ZikuStr(10, 228, BLACK, WHITE, 16, (u8 *)"RGB:");

    switch (fan_rgb_state)
    {
        case FAN_RGB_OFF:
            Lcd_ZikuStr(60, 228, 0x8410, WHITE, 16, (u8 *)"\xCF\xA8\xC3\xF0");
            break;
        case FAN_RGB_GREEN:
            Lcd_ZikuStr(60, 228, GREEN, WHITE, 16, (u8 *)"\xC2\xCC\xB5\xC6");
            break;
        case FAN_RGB_BLUE:
            Lcd_ZikuStr(60, 228, BLUE,  WHITE, 16, (u8 *)"\xC0\xB6\xB5\xC6");
            break;
        case FAN_RGB_RED:
            Lcd_ZikuStr(60, 228, RED,   WHITE, 16, (u8 *)"\xBA\xEC\xB5\xC6");
            break;
    }

    /* --- 串口同步输出 --- */
    printf("[LCD] \xB7\xE7\xC9\xC8:%d%%  RGB:%s\r\n",
           duty_pct,
           fan_rgb_state == FAN_RGB_OFF   ? "OFF" :
           fan_rgb_state == FAN_RGB_GREEN ? "GREEN" :
           fan_rgb_state == FAN_RGB_BLUE  ? "BLUE"  : "RED");
}

/* ================================================================== */
/*                    火焰 + 光敏传感器显示函数                        */
/* ================================================================== */

/**
 * @brief  读取火焰+光敏传感器, LCD显示(带名称标签) + 串口打印
 * @param  void
 * @retval void
 * @note   整合两个传感器的读取、LCD显示和串口打印于一个函数
 *         LCD布局:
 *           y=65:  "火焰传感器:"
 *           y=85:  火焰数值(如"75.5%")
 *           y=115: "光敏电阻:"
 *           y=135: 光敏数值(如"60.3%")
 */
void Sensor_Display_All(void)
{
    u8 disp_buff[32] = {0};

    /* 清除LCD显示区域 */
    Lcd_Clear(10, 65, 230, 150, WHITE);

    /* 读取火焰传感器值 */
    Flame_GetValue();
    /* 读取光敏传感器值 */
    ADC1_GetValue();

    /* ========== 串口打印 ========== */
    printf("\xBB\xF0\xD1\xE6\xB4\xAB\xB8\xD0\xC6\xF7: %.1f%%\r\n", flame_value);
    printf("\xB9\xE2\xC3\xF4\xB5\xE7\xD7\xE8:   %.1f%%\r\n", light_value);

    /* ========== LCD显示火焰传感器 ========== */
    Lcd_ZikuStr(10, 65, RED, WHITE, 16, (u8 *)"\xBB\xF0\xD1\xE6\xB4\xAB\xB8\xD0\xC6\xF7:");
    sprintf((char *)disp_buff, "%.1f%%", flame_value);
    Lcd_ZikuStr(10, 85, RED, WHITE, 16, disp_buff);

    /* ========== LCD显示光敏电阻 ========== */
    Lcd_ZikuStr(10, 115, RED, WHITE, 16, (u8 *)"\xB9\xE2\xC3\xF4\xB5\xE7\xD7\xE8:");
    sprintf((char *)disp_buff, "%.1f%%", light_value);
    Lcd_ZikuStr(10, 135, RED, WHITE, 16, disp_buff);
}
