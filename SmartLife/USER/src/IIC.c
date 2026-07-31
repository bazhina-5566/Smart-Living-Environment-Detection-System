#include "main.h"


/**
  * @brief IIC1管脚初始化
  * @param void
  * @retval void
  * @note
    I2C1_SDA    ---   PB7   ---  通用开漏输出
    I2C1_SCL    ---   PB6   ---  通用推挽输出
  */

void IIC1_Config(void)
{
    
    /* 打开GPIOB时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    
    /* IO管脚初始化 */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* 配置PB7为开漏输出(SDA数据线) */
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;  /* 开漏输出 */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    /* 配置PB6为推挽输出(SCL时钟线) */
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;  /* 推挽输出 */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    /* 设置初始电平: SDA和SCL均为高电平(空闲状态) */
    IIC1_SDA_H;
    IIC1_SCL_H;
    
}


/**
  * @brief IIC1起始信号
  * @param void
  * @retval void
  * @note
    I2C起始信号: SCL高电平时, SDA由高变低
  */

void IIC1_Start(void)
{
    
    /* 注意: 起始信号中一定要先操作数据线 */
    IIC1_SDA_H;                    /* SDA置高 */
    IIC1_SCL_H;                    /* SCL置高 */
    timer_delay_us(IIC1_DELAY_TIME);     /* 延时等待稳定 */
    IIC1_SDA_L;                    /* SDA拉低(产生起始信号) */
    timer_delay_us(IIC1_DELAY_TIME);     /* 延时保持 */
    
}



/**
  * @brief IIC1停止信号
  * @param void
  * @retval void
  * @note
    I2C停止信号: SCL高电平时, SDA由低变高
  */

void IIC1_Stop(void)
{
    
    IIC1_SDA_L;                    /* SDA置低 */
    IIC1_SCL_H;                    /* SCL置高 */
    timer_delay_us(IIC1_DELAY_TIME);     /* 延时等待稳定 */
    IIC1_SDA_H;                    /* SDA拉高(产生停止信号) */
    timer_delay_us(IIC1_DELAY_TIME);     /* 延时保持 */
    
}


/**
  * @brief IIC1发送1bit应答位
  * @param u8 ack    0:发送应答     1:发送非应答
  * @retval void
  */

void IIC1_SendAck(u8 ack)
{
    
    IIC1_SCL_L;                    /* 拉低时钟, 主机发送数据到从机 */
    timer_delay_us(IIC1_DELAY_TIME);     /* 等待时钟线完全变为低电平 */
    if(ack)
        IIC1_SDA_H;                /* 拉高数据线, 发送'1'非应答 */
    else
        IIC1_SDA_L;                /* 拉低数据线, 发送'0'应答 */
    timer_delay_us(IIC1_DELAY_TIME);     /* 时钟线保持时间 */
    IIC1_SCL_H;                    /* 拉高时钟, 让从机接收主机发送的1bit应答位 */
    timer_delay_us(IIC1_DELAY_TIME);     /* 时钟线保持时间 */
    
    IIC1_SCL_L;                    /* 拉低时钟线, 保证IIC时序完整性 */
    timer_delay_us(IIC1_DELAY_TIME);
    
}


/**
  * @brief IIC1接收1bit应答位
  * @param  void
  * @retval u8    0: 从机回复应答    1: 从机回复非应答
  */

u8 IIC1_RecAck(void)
{
    u8 ack = 0;
    IIC1_SDA_H;                    /* 切换为输入状态(释放SDA) */
    
    IIC1_SCL_L;                    /* 从机发送1bit应答位到主机 */
    timer_delay_us(IIC1_DELAY_TIME);
    IIC1_SCL_H;                    /* 主机开始接收从机bit应答位 */
    timer_delay_us(IIC1_DELAY_TIME);
    if(IIC1_SDA_IN)
        ack = 1;                   /* 接收到非应答(NACK) */
    
    IIC1_SCL_L;                    /* 拉低时钟线, 保证IIC时序完整性 */
    timer_delay_us(IIC1_DELAY_TIME);
    
    return ack;
 
}

/**
  * @brief IIC1发送8bit数据
  * @param  u8 data
  * @retval void
  */

void IIC1_SendData(u8 data)
{
    
    for(u8 i = 0; i < 8; i++)
    {
        IIC1_SCL_L;                /* 主机发送数据 */
        timer_delay_us(IIC1_DELAY_TIME);
        if(data & (0x80 >> i))     /* 从高位到低位逐位发送 */
            IIC1_SDA_H;            /* 发送bit '1' */
        else
            IIC1_SDA_L;            /* 发送bit '0' */
        timer_delay_us(IIC1_DELAY_TIME);
        IIC1_SCL_H;                /* 从机开始接收数据 */
        timer_delay_us(IIC1_DELAY_TIME);  
    }
}


/**
  * @brief IIC1接收8bit数据
  * @param  void
  * @retval u8   接收到的8bit数据
  */

u8 IIC1_RecData(void)
{
    u8 data = 0;
    
    IIC1_SDA_H;                    /* 切换输入状态(释放SDA) */
    
    for(u8 i = 0; i < 8; i++)
    {
        IIC1_SCL_L;                /* 从机发送数据 */
        timer_delay_us(IIC1_DELAY_TIME);
        IIC1_SCL_H;                /* 主机接收数据 */
        timer_delay_us(IIC1_DELAY_TIME);
        data <<= 1;                /* 左移1位 */
        if(IIC1_SDA_IN)
            data |= 1;             /* 读取到高电平, 此位置1 */
        timer_delay_us(IIC1_DELAY_TIME);
    }
    return data;
}

