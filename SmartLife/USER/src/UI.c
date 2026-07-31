#include "main.h"
#include "PIC.h"
#include <stdio.h>
uint8_t led_mode = 0;                     // 保留变量，Page_Two 仍会显示
const char* led_mode_str[] = {"常亮", "闪烁", "呼吸"};
// UI.c 第5行，保留不动
/* =================== 外设状态 =================== */
uint8_t led_status = 0;
uint8_t fan_status = 0;
uint8_t buzzer_status = 0;
uint8_t relay_status = 0;
/* =================== 图片页管理 =================== */
u8 img_index = 0;
u8 auto_play_en = 0;
u16 auto_play_cnt = 0;
#define AUTO_PLAY_CYCLE 200

/* =================== 非阻塞触摸防抖 =================== */
u8 touch_lock = 0;
uint32_t touch_lock_time = 0;
#define TOUCH_DEBOUNCE_MS  200

extern volatile uint32_t sys_tick;

__weak void Touch_ClearGestureFlag(void)
{
    /* CST816S 手势寄存器读后自动清零，无需额外操作 */
}

/* =================== 区域枚举（仅保留LED和风扇） =================== */
typedef enum {
    AREA_NONE = 0,
    AREA_LED,
    AREA_FAN
} TouchArea_t;

TouchArea_t GetTouchArea(uint16_t x, uint16_t y)
{
    if (x >= 10 && x <= 110 && y >= 40 && y <= 120)
        return AREA_LED;
    else if (x >= 130 && x <= 230 && y >= 40 && y <= 120)
        return AREA_FAN;
    else
        return AREA_NONE;
}

/* =================== 界面一：个人主页 =================== */
void Page_Home(void)
{
    if(ui_flag == 0)
    {
        ui_flag = 1;
        touch_lock = 0;
        touch_lock_time = 0;

        Lcd_DisplayPic(0, 0, (u8 *)gImage_11);
        Lcd_ZikuStr(80, 280, RED, WHITE, 16, (u8 *)"宾哥真帅");
    }
}

/* =================== 界面二：外设控制页（仅LED和风扇） =================== */
void Page_One(void)
{
    if(ui_flag == 0)
    {
        ui_flag = 1;
        touch_lock = 0;
        touch_lock_time = 0;
        Lcd_Clear(0, 0, 240, 320, WHITE);

        // LED 开关
        Lcd_Clear(10, 40, 110, 120, led_status ? RED : BLUE);
        Lcd_ZikuStr(20, 70, WHITE, led_status ? RED : BLUE, 16, 
                    led_status ? (u8 *)"LED ON " : (u8 *)"LED OFF");

        // 风扇开关
		if(fan_gear){fan_status=1;}
		else{fan_status=0;}
		
        Lcd_Clear(130, 40, 230, 120, fan_status ? YELLOW : GREEN);
        Lcd_ZikuStr(140, 70, BLACK, fan_status ? YELLOW : GREEN, 16, 
                    fan_status ? (u8 *)"FAN ON " : (u8 *)"FAN OFF");
    }

    /* -------- 非阻塞解锁 -------- */
    if(touch_lock == 1)
    {
        if(sys_tick - touch_lock_time >= TOUCH_DEBOUNCE_MS)
            touch_lock = 0;
    }

    /* -------- 触摸响应 -------- */
    if(touch_lock == 0 && cst.ges_sta != 0x00 && cst.x != 0 && cst.y != 0)
    {
        TouchArea_t area = GetTouchArea(cst.x, cst.y);
        if(area != AREA_NONE && cst.ges_sta == 0x05)
        {
            touch_lock = 1;
            touch_lock_time = sys_tick;
            Touch_ClearGestureFlag();

            switch(area)
            {
                case AREA_LED:
                    led_status = !led_status;
                    LED_SetAll(led_status);
                    Lcd_Clear(10, 40, 110, 120, led_status ? RED : BLUE);
                    Lcd_ZikuStr(20, 70, WHITE, led_status ? RED : BLUE, 16, 
                                led_status ? (u8 *)"LED ON " : (u8 *)"LED OFF");
                    printf("LED 状态切换:%s\r\n", led_status ? "开启" : "关闭");
                    break;

                case AREA_FAN:
                    fan_status = !fan_status;
                    Fan_SetSpeed(fan_status ? 800 : 0);
                    Lcd_Clear(130, 40, 230, 120, fan_status ? YELLOW : GREEN);
                    Lcd_ZikuStr(140, 70, BLACK, fan_status ? YELLOW : GREEN, 16, 
                                fan_status ? (u8 *)"FAN ON " : (u8 *)"FAN OFF");
                    printf("风扇 状态切换:%s\r\n", fan_status ? "开启" : "关闭");
                    break;

                default:
                    break;
            }
        }
        cst.ges_sta = 0x00;
    }
}

/* =================== 界面三：系统状态页 =================== */
void Page_Two(void)
{
    if(ui_flag == 0)
    {
        ui_flag = 1;
        touch_lock = 0;
        touch_lock_time = 0;

        Lcd_Clear(0, 0, 240, 320, BLACK);
        Lcd_ZikuStr(50, 30, WHITE, BLACK, 16, (u8 *)"System Status");
        Lcd_ZikuStr(10, 60,  WHITE, BLACK, 16, (u8 *)"LED:");
        Lcd_ZikuStr(10, 100,  WHITE, BLACK, 16, (u8 *)"Fan:");
    }

    /* 动态刷新 */
    Lcd_Clear(80, 60, 230, 70, BLACK);
    Lcd_Clear(80, 100, 230, 110, BLACK);
    
	if(fan_gear){fan_status=1;}
		else{fan_status=0;}
		
    if(led_status)
    {
        Lcd_ZikuStr(80, 60, GREEN, BLACK, 16, (u8 *)"ON");
    }
    else
        Lcd_ZikuStr(80, 60, RED, BLACK, 16, (u8 *)"OFF");

    if(fan_status)
        Lcd_ZikuStr(80, 100, GREEN, BLACK, 16, (u8 *)"ON");
    else
        Lcd_ZikuStr(80, 100, RED, BLACK, 16, (u8 *)"OFF");
	
	/*LCD进度条显示*/
	Fan_DisplayStatus();
	
    /* 保留防抖解锁（避免锁带到下页），不处理任何触摸手势 */
    if(touch_lock == 1)
    {
        if(sys_tick - touch_lock_time >= TOUCH_DEBOUNCE_MS)
            touch_lock = 0;
    }
    // 不调用 cst.ges_sta 清除，完全忽略触摸
}

/* =================== 界面四：传感器显示页（非阻塞，不影响滑动） =================== */
/* =================== 界面四：传感器显示页（实时刷新，非阻塞） =================== */
void Page_Sensor(void)
{
	Lcd_Clear (0,0,240,320,WHITE);
	Sensor_Display_All();
}

//void Page_Sensor(void)
//{
//    if(ui_flag == 0)
//    {
//        ui_flag = 1;
//        touch_lock = 0;
//        touch_lock_time = 0;
//        Lcd_Clear(0, 0, 240, 320, WHITE);
//        Lcd_ZikuStr(50, 100, RED, WHITE, 16, (u8 *)"Page 3 OK");
//    }
//    // 保留解锁逻辑
//    if(touch_lock == 1)
//    {
//        if(sys_tick - touch_lock_time >= TOUCH_DEBOUNCE_MS)
//            touch_lock = 0;
//    }
//}
///* =================== 界面四：传感器显示页（已移除所有触摸处理） =================== */
//void Page_Sensor(void)
//{
//    /* -------- 静态UI -------- */
//    if(ui_flag == 0)
//    {
//        ui_flag = 1;
//        touch_lock = 0;
//        touch_lock_time = 0;

//        Lcd_Clear(0, 0, 240, 320, WHITE);
//        Lcd_ZikuStr(50, 10, RED, WHITE, 16, (u8 *)"Sensor Data");
//        Lcd_ZikuStr(10, 65,  RED, WHITE, 16, (u8 *)"火焰传感器:");
//        Lcd_ZikuStr(10, 115, RED, WHITE, 16, (u8 *)"光敏电阻:");
//    }

//    /* -------- 仅保留全局防抖解锁，不处理任何触摸事件 -------- */
//    if(touch_lock == 1)
//    {
//        if(sys_tick - touch_lock_time >= TOUCH_DEBOUNCE_MS)
//            touch_lock = 0;
//    }
//    // 移除了 cst.ges_sta 的清除，本页不干预触摸状态

//    /* -------- 动态刷新传感器数值 -------- */
//    u8 disp_buff[32] = {0};
//    Flame_GetValue();
//    ADC1_GetValue();

//    Lcd_Clear(10, 85, 150, 105, WHITE);
//    Lcd_Clear(10, 135, 150, 155, WHITE);

//    sprintf((char *)disp_buff, "%.1f%%", flame_value);
//    Lcd_ZikuStr(10, 85, RED, WHITE, 16, disp_buff);

//    sprintf((char *)disp_buff, "%.1f%%", light_value);
//    Lcd_ZikuStr(10, 135, RED, WHITE, 16, disp_buff);
//}