#include "fan.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"

/**
 * @brief  风扇GPIO初始化(PC8复用为TIM8_CH3)
 * @param  无
 * @retval 无
 */
void Fan_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* 开启GPIOC时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    /* 配置PC8为复用推挽输出 */
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_8;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* PC8复用映射到TIM8 */
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM8);
}

/**
 * @brief  风扇定时器时基初始化(TIM8, 1kHz PWM频率)
 * @param  无
 * @retval 无
 */
void Fan_TIM_Config(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct = {0};

    /* 开启TIM8时钟(APB2总线，主频168MHz) */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);

    /* 时基配置：计数频率1MHz，自动重装载值999，输出PWM频率1kHz */
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period      = 1000 - 1;   // ARR = 999
    TIM_TimeBaseInitStruct.TIM_Prescaler   = 168 - 1;    // PSC = 167
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM8, &TIM_TimeBaseInitStruct);

    /* 使能TIM8计数器 */
    TIM_Cmd(TIM8, ENABLE);
}

/**
 * @brief  风扇PWM输出通道初始化(TIM8_CH3)
 * @param  无
 * @retval 无
 */
void Fan_PWM_Config(void)
{
    TIM_OCInitTypeDef TIM_OCInitStruct = {0};

    /* 配置PWM模式1，高电平有效 */
    TIM_OCInitStruct.TIM_OCMode      = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OCPolarity  = TIM_OCPolarity_High;
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_Pulse       = 0;    // 初始占空比0，风扇停止
    TIM_OC3Init(TIM8, &TIM_OCInitStruct);

    /* 使能通道3预装载寄存器 */
    TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);

    /* 高级定时器专属：开启主输出PWM */
    TIM_CtrlPWMOutputs(TIM8, ENABLE);
}

/**
 * @brief  风扇完整初始化总入口
 * @param  无
 * @retval 无
 */
void Fan_Config(void)
{
    Fan_GPIO_Config();
    Fan_TIM_Config();
    Fan_PWM_Config();
}

/**
 * @brief  设置风扇转速（占空比）
 * @param  speed: 0~999，对应0%~100%占空比
 * @retval 无
 */
void Fan_SetSpeed(u16 speed)
{
    /* 限制参数范围，避免超出ARR值 */
    if(speed > 999)
    {
        speed = 999;
    }
    TIM_SetCompare3(TIM8, speed);
}

