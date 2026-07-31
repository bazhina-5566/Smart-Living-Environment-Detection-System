#include "main.h"
#define LED_PINS (GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14)

extern volatile uint32_t sys_tick;
extern uint8_t led_status;
extern uint8_t led_mode;

void LED_Init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    GPIO_InitTypeDef GPIO_LED = {0};
    GPIO_LED.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_LED.GPIO_OType = GPIO_OType_PP;
    GPIO_LED.GPIO_Pin   = LED_PINS;
    GPIO_LED.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_LED.GPIO_Speed = GPIO_Speed_2MHz;

    GPIO_Init(GPIOD, &GPIO_LED);

    LED_SetAll(0); 
}

void LED_SetAll(uint8_t state)
{
    if (state) {
        GPIO_ResetBits(GPIOD, LED_PINS); 
    } else {
        GPIO_SetBits(GPIOD, LED_PINS);  
    }
}

void LED_Mode_On(void)
{
    LED_SetAll(1);
}

// 完全保留你原有的阻塞式闪烁函数，不做改动
void LED_Mode_Flash(void)
{
    LED_SetAll(1);
    timer_delay_ms(100);
    LED_SetAll(0);
    timer_delay_ms(100);
}

// 完全保留你原有的阻塞式呼吸函数，不做改动
void LED_Mode_Breath(void)
{
    int16_t bright; 
    
    for (bright = 0; bright <= 1000; bright += 7)
    {
        LED_SetAll(1);
        timer_delay_us(bright);
        LED_SetAll(0);
        timer_delay_us(1000 - bright);
    }

    for (bright = 1000; bright >= 0; bright -= 7)
    {
        LED_SetAll(1);
        timer_delay_us(bright);
        LED_SetAll(0);
        timer_delay_us(1000 - bright);
    }
}

/* 非阻塞控制变量 —— 完全沿用你原有的变量名，不新增 */
static uint32_t led_tick = 0;
static uint8_t  led_pwm_state = 0;
static uint16_t breath_on_us = 0;

/* 主循环调用函数 —— 仅优化内部逻辑，无新增函数 */
void LED_Mode_Update(void)
{
    // 模式切换自动重置状态（用函数内部静态变量实现，不额外创建函数）
    static uint8_t last_mode = 0xFF;
    if (led_mode != last_mode)
    {
        led_tick = sys_tick;
        led_pwm_state = 0;
        breath_on_us = 0;
        last_mode = led_mode;
    }

    if (!led_status) {
        GPIO_SetBits(GPIOD, LED_PINS);
        return;
    }

    switch (led_mode) {
        case 0: // 常亮
            GPIO_ResetBits(GPIOD, LED_PINS);
            break;

        case 1: // 闪烁（周期 1000ms）
            if (sys_tick - led_tick >= 500) {
                led_tick = sys_tick;
                if (led_pwm_state) {
                    GPIO_SetBits(GPIOD, LED_PINS);
                    led_pwm_state = 0;
                } else {
                    GPIO_ResetBits(GPIOD, LED_PINS);
                    led_pwm_state = 1;
                }
            }
            break;

       case 2: // 呼吸
            // 每1ms更新一次亮度三角波
            if (sys_tick - led_tick >= 1) {
                led_tick = sys_tick;
                static int8_t dir = 1;
                breath_on_us += 7 * dir;
                if (breath_on_us >= 1000) { 
                    dir = -1; 
                    breath_on_us = 1000; 
                }
                if (breath_on_us <= 0) { 
                    dir = 1;  
                    breath_on_us = 0; 
                }
            }

            // 每次调用都刷新PWM电平（修复原代码只在1ms时更新的频闪问题）
            uint32_t cycle = sys_tick % 20;
            uint32_t on_time = (breath_on_us * 20) / 1000;
            if (cycle < on_time)
                GPIO_ResetBits(GPIOD, LED_PINS);
            else
                GPIO_SetBits(GPIOD, LED_PINS);
            break;
    }
}