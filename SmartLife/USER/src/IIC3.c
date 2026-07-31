#include "main.h"                                   /* 包含i2c头文件 */

/* =================== I2C3管脚初始化 =================== */
void I2C3_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure={0};             /* 定义GPIO初始化结构体变量 */

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);   /* 开启GPIOA时钟(PA8用) */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);   /* 开启GPIOC时钟(PC9用) */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);   /* 开启GPIOD时钟(PD1,PD5用) */

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;              /* PD5: 复位引脚 */
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;           /* 输出模式 */
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;           /* 推挽输出 */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;       /* 速度100MHz */
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;        /* 无上下拉 */
    GPIO_Init(GPIOD, &GPIO_InitStructure);                   /* 初始化GPIOD */

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;              /* PA8: SCL时钟线 */
    GPIO_Init(GPIOA, &GPIO_InitStructure);                   /* 初始化GPIOA */

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;              /* PD1: TINT中断引脚 */
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;            /* 输入模式 */
    GPIO_Init(GPIOD, &GPIO_InitStructure);                   /* 初始化GPIOD */

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;           /* PC9: SDA数据线 */
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;           /* 开漏输出 */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStructure);                   /* 初始化GPIOC */

    TP_RST_H;                                                /* RST置高(不复位) */
    I2C3_SCL_H;                                              /* SCL空闲高电平 */
    I2C3_SDA_H;                                              /* SDA空闲高电平 */
}

void I2C3_Start(void)                                        /* I2C起始信号: SCL高时SDA从高变低 */
{
    I2C3_SDA_H;
    I2C3_SCL_H;
    delay_us(I2C3_DELAY_TIME);
    I2C3_SDA_L;                                              /* SDA从高变低->起始信号 */
    delay_us(I2C3_DELAY_TIME);
}

void I2C3_Stop(void)                                         /* I2C停止信号: SCL高时SDA从低变高 */
{
    I2C3_SDA_L;
    I2C3_SCL_H;
    delay_us(I2C3_DELAY_TIME);
    I2C3_SDA_H;                                              /* SDA从低变高->停止信号 */
    delay_us(I2C3_DELAY_TIME);
}

void I2C3_SendAck(u8 ack)                                    /* 发送应答位: ack=0应答, ack=1非应答 */
{
    I2C3_SCL_L;
    delay_us(I2C3_DELAY_TIME);
    if(ack)  I2C3_SDA_H;                                     /* 非应答: SDA拉高 */
    else     I2C3_SDA_L;                                     /* 应答:   SDA拉低 */
    delay_us(I2C3_DELAY_TIME);
    I2C3_SCL_H;
    delay_us(I2C3_DELAY_TIME);
    I2C3_SCL_L;
    delay_us(I2C3_DELAY_TIME);
}

u8 I2C3_RecAck(void)                                         /* 接收应答: 返回0=应答, 1=非应答 */
{
    u8 ack = 0;
    I2C3_SDA_H;                                              /* 释放SDA, 切换输入 */
    I2C3_SCL_L;
    delay_us(I2C3_DELAY_TIME);
    I2C3_SCL_H;                                              /* SCL上升沿采样 */
    delay_us(I2C3_DELAY_TIME);
    if(I2C3_SDA_IN)  
		ack = 1;                               				 /* SDA=高 -> 非应答 */
    delay_us(I2C3_DELAY_TIME);
    I2C3_SCL_L; 											/* 拉低时钟线, 保证IIC时序完整性 */
    delay_us(I2C3_DELAY_TIME);
    return ack;
}

void I2C3_SendData(u8 data)                                  /* 发送8bit数据(高位先出) */
{
    u8 i;
    for(i = 0; i < 8; i++)
    {
        I2C3_SCL_L;
        delay_us(I2C3_DELAY_TIME);
        if(data & (0x80 >> i))  
			I2C3_SDA_H;                  						/* 该bit=1 -> SDA高 */
        else                     
			I2C3_SDA_L;                							  /* 该bit=0 -> SDA低 */
        delay_us(I2C3_DELAY_TIME);
        I2C3_SCL_H;                                          /* SCL上升沿, 从机采样 */
        delay_us(I2C3_DELAY_TIME);
    }
}

u8 I2C3_RecData(void)                                        /* 接收8bit数据(高位先出) */
{
    u8 data = 0;
    u8 i;
    I2C3_SDA_H;                                              /* 释放SDA, 切换输入 */
    for(i = 0; i < 8; i++)
    {
        I2C3_SCL_L;
        delay_us(I2C3_DELAY_TIME);
        I2C3_SCL_H;                                          /* SCL上升沿采样 */
        delay_us(I2C3_DELAY_TIME);
        data <<= 1;
        if(I2C3_SDA_IN)  data |= 1;                          /* SDA=高 -> 该bit=1 */
        delay_us(I2C3_DELAY_TIME);
    }
    return data;
}

void CST816S_WriteByte(u8 addr, u8 data)                     /* CST816S写寄存器 */
{
    I2C3_Start();
    I2C3_SendData(0x2A);                                     /* 设备地址(写): 0x15<<1|0 */
    I2C3_RecAck();
    I2C3_SendData(addr);                                     /* 寄存器地址 */
    I2C3_RecAck();
    I2C3_SendData(data);                                     /* 要写入的数据 */
    I2C3_RecAck();
    I2C3_Stop();
}

void CST816S_RecData(u8 addr, u8 len, u8 *data)              /* CST816S读多字节 */
{
    I2C3_Start();
    I2C3_SendData(0x2A);                                     /* 写设备地址, 指定寄存器 */
    I2C3_RecAck();
    I2C3_SendData(addr);
    I2C3_RecAck();

    I2C3_Start();                                            /* 重启总线 */
    I2C3_SendData(0x2B);                                     /* 读设备地址: 0x15<<1|1 */
    I2C3_RecAck();

    len -= 1;                                                /* 前len-1字节发ACK */
    while(len--)
    {
        *data = I2C3_RecData();
        I2C3_SendAck(0);                                     /* 发送ACK, 继续读 */
        data++;
    }
    *data = I2C3_RecData();                                  /* 最后1字节发NACK */
    I2C3_SendAck(1);
    I2C3_Stop();
}

void CST816S_Config(void)                                    /* CST816S初始化(读ID) */
{
    u8 id = 0;

    I2C3_Config();                                           /* I2C3管脚初始化 */

    TP_RST_L;   delay_ms(50);                                /* 复位触摸芯片 */
    TP_RST_H;   delay_ms(50);

    CST816S_WriteByte(0xEC, 0x01);                           /* 使能双击 */
    CST816S_WriteByte(0xFA, 0x11);                           /* 工作模式 */
    CST816S_WriteByte(0xFD, 0x00);                           /* 其他配置 */

    CST816S_RecData(0xA7, 1, &id);                           /* 读芯片ID(应为0xB5) */
    printf("ChipID:0x%02x\r\n", id);
}

CST cst;                                                     /* 全局触摸数据 */

void Get_XY(void)                                            /* 读取触摸坐标 */
{
    u8 buff[4] = {0};
    CST816S_RecData(0x03, 4, buff);                          /* 读4字节触摸数据 */
    cst.x = (buff[0] & 0x0f) << 8 | buff[1];                /* X坐标: 高4位+低8位 */
    cst.y = (buff[2] & 0x0f) << 8 | buff[3];                /* Y坐标: 高4位+低8位 */
    printf("x:%d   y:%d\r\n", cst.x, cst.y);
}

void CST_Scan(void)                                           /* 触摸扫描(检测手势) */
{
    u8 flag = 0xff;
    if(TP_TINT == 0)                                          /* PD1低电平=有触摸按下 */
    {
        CST816S_RecData(0x01, 1, &flag);                      /* 读手势寄存器 */
        printf("flag:0x%02x\r\n", flag);
        cst.ges_sta = flag;

        if(flag == 0x01)  { printf("Up!\r\n");     Get_XY(); } /* 上滑 */
        if(flag == 0x02)  { printf("Down!\r\n");   Get_XY(); } /* 下滑 */
        if(flag == 0x03)  { printf("Left!\r\n");   Get_XY(); } /* 左滑 */
        if(flag == 0x04)  { printf("Right!\r\n");  Get_XY(); } /* 右滑 */
        if(flag == 0x05)  { printf("Click!\r\n");  Get_XY(); } /* 单击 */
        if(flag == 0x0b)  { printf("DClick!\r\n"); Get_XY(); } /* 双击 */
        if(flag == 0x0c)  { printf("LongP!\r\n");  Get_XY(); } /* 长按 */
    }
}