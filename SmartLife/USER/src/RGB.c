#include "rgb.h"

/*******************************************************************************
 * 函数名: Rgb_Config
 * 功能:   WS2812B 硬件GPIO初始化 + 初始全灭
 * 输入:   无
 * 输出:   无
 *******************************************************************************/
void Rgb_Config(void)
{
    /* 使能GPIOB的AHB1时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    /* 定义GPIO初始化结构体并清零 */
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;    /* 推挽输出模式 */
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;    /* 推挽输出类型 */
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_15;      /* PB15引脚 */
    GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL; /* 无上下拉 */
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;  /* 输出速度 */

    GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* 上电复位 + 初始全灭 */
    Rgb_Reset();
    Rgb_Control(0, 0, 0);
    Rgb_Control(0, 0, 0);
    Rgb_Control(0, 0, 0);
    Rgb_Control(0, 0, 0);
    Rgb_Reset();
}

/*******************************************************************************
 * 函数名: Rgb_Reset
 * 功能:   WS2812B复位 - 拉低数据线超过50us，锁存一帧数据
 * 输入:   无
 * 输出:   无
 *******************************************************************************/
void Rgb_Reset(void)
{
    RGB_DATA_L;         /* 拉低数据线 */
    delay_us(280);      /* 延时280us，满足>50us复位要求 */
}

/*******************************************************************************
 * 函数名: Rgb_SendData
 * 功能:   发送24bit颜色数据到WS2812B
 * 输入:   color - 24bit颜色数据(GRB格式)
 * 输出:   无
 * 说明:   从高位到低位逐位发送
 *         '1'码: 高电平0.7us + 低电平0.6us
 *         '0'码: 高电平0.35us + 低电平0.8us
 *******************************************************************************/
void Rgb_SendData(u32 color)
{
    u8 i;  /* 位循环变量 */
    u8 j;  /* 空循环延时变量 */

    /* 循环发送24bit数据(从高位到低位) */
    for(i = 0; i < 24; i++)
    {
        if(color & (0x800000 >> i))
        {
            /* 发送'1'码 */
            RGB_DATA_H;
            delay_us(1);
            RGB_DATA_L;
            for(j = 0; j < 11; j++);
        }
        else
        {
            /* 发送'0'码 */
            RGB_DATA_H;
            for(j = 0; j < 11; j++);
            RGB_DATA_L;
            delay_us(1);
        }
    }
}

/*******************************************************************************
 * 函数名: Rgb_Control
 * 功能:   设置单颗RGB彩灯的颜色
 * 输入:   r - 红色亮度值(0~255)
 *         g - 绿色亮度值(0~255)
 *         b - 蓝色亮度值(0~255)
 * 输出:   无
 * 说明:   WS2812B为GRB顺序，自动完成RGB→GRB转换
 *         级联场景下，连续调用N次对应第1~N颗灯
 *******************************************************************************/
void Rgb_Control(u8 r, u8 g, u8 b)
{
    u32 color = 0;

    color |= (u32)g << 16;  /* Green 放高位[23:16] */
    color |= (u32)r << 8;   /* Red 放中间位[15:8] */
    color |= (u32)b;        /* Blue 放低位[7:0] */

    Rgb_SendData(color);
}
