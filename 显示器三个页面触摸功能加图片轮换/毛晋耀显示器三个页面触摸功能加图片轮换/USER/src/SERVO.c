#include "servo.h"

/************************ 参数配置 ************************/
// 定时器参数：TIM5时钟84MHz → 1MHz计数频率，20ms周期(标准舵机50Hz)
#define SERVO_TIM_PSC    83     // 预分频系数
#define SERVO_TIM_ARR    19999  // 自动重装载值

/**
 * @brief  舵机PWM初始化：TIM5_CH2 -> PA1
 */
void Servo_Init(void)
{
    GPIO_InitTypeDef        GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef       TIM_OCInitStructure;

    /* 1. 开启外设时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

    /* 2. 配置PA1为复用推挽输出，复用功能为TIM5 */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 引脚复用映射：PA1 -> TIM5通道2
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM5);

    /* 3. 定时器时基配置 */
    TIM_TimeBaseStructure.TIM_Period        = SERVO_TIM_ARR;
    TIM_TimeBaseStructure.TIM_Prescaler     = SERVO_TIM_PSC;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

    /* 4. 配置通道2为PWM1模式 */
    TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse       = 1500;   // 初始默认90°，对应1.5ms高电平
    TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;
    TIM_OC2Init(TIM5, &TIM_OCInitStructure);

    /* 使能预装载，防止PWM突变 */
    TIM_OC2PreloadConfig(TIM5, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM5, ENABLE);

    /* 5. 启动定时器 */
    TIM_Cmd(TIM5, ENABLE);
}

/**
 * @brief  设置舵机转动角度，自带超范围限幅
 * @param  angle: 0 ~ 180 度
 */
void Servo_SetAngle(float angle)
{
    uint16_t ccr_value;

    // 角度限幅，避免舵机超行程损坏
    if(angle < 0.0f)
        angle = 0.0f;
    if(angle > 180.0f)
        angle = 180.0f;

    // 角度映射为CCR值：0°→0.5ms(500)，180°→2.5ms(2500)
    ccr_value = (uint16_t)(500 + (angle / 180.0f) * 2000);

    // 更新比较寄存器，修改PWM占空比
    TIM_SetCompare2(TIM5, ccr_value);
}
