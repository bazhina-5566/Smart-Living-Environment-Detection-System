#ifndef _I2C_H                                    /* 防止头文件重复包含 */
#define _I2C_H

#include "main.h"                                 /* 包含主头文件(芯片寄存器定义) */

/* =================== I2C3引脚宏定义(CST816S触摸芯片) =================== */

/* I2C3数据线SDA: PC9, 置高电平 */
#define I2C3_SDA_H   (GPIO_SetBits(GPIOC, GPIO_Pin_9))

/* I2C3数据线SDA: PC9, 置低电平 */
#define I2C3_SDA_L   (GPIO_ResetBits(GPIOC, GPIO_Pin_9))

/* I2C3时钟线SCL: PA8, 置高电平 */
#define I2C3_SCL_H   (GPIO_SetBits(GPIOA, GPIO_Pin_8))

/* I2C3时钟线SCL: PA8, 置低电平 */
#define I2C3_SCL_L   (GPIO_ResetBits(GPIOA, GPIO_Pin_8))

/* 读取SDA引脚电平(用于接收应答和数据) */
#define I2C3_SDA_IN  (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_9))

/* 触摸中断引脚: PD1, 低电平表示有触摸按下 */
#define TP_TINT      (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_1))

/* 触摸芯片复位引脚: PD5, 置高 */
#define TP_RST_H     (GPIO_SetBits(GPIOD, GPIO_Pin_5))

/* 触摸芯片复位引脚: PD5, 置低 */
#define TP_RST_L     (GPIO_ResetBits(GPIOD, GPIO_Pin_5))

/* I2C时序延时参数(数值越大速度越慢, 可根据实际调整) */
#define I2C3_DELAY_TIME  6

/* =================== CST816S触摸数据结构体 =================== */

/* 触摸数据变量类型定义 */
typedef struct {
    u16 x;                                         /* X轴坐标(范围0~239, 对应LCD宽度) */
    u16 y;                                         /* Y轴坐标(范围0~319, 对应LCD高度) */
    u8 ges_sta;                                    /* 手势状态(0x05=单击, 0x0B=双击, 0x04=左滑) */
} CST;

/* 全局触摸数据变量(在i2c.c中定义, 其他文件通过此声明使用) */
extern CST cst;

/* =================== 函数声明 =================== */

/* I2C3总线管脚初始化(配置GPIO模式和初始电平) */
void I2C3_Config(void);

/* I2C起始信号: SCL高电平时, SDA从高切换到低 */
void I2C3_Start(void);

/* I2C停止信号: SCL高电平时, SDA从低切换到高 */
void I2C3_Stop(void);

/* I2C发送应答位: ack=0表示应答(ACK), ack=1表示非应答(NACK) */
void I2C3_SendAck(u8 ack);

/* I2C接收应答位: 返回值0=收到应答, 1=收到非应答 */
u8 I2C3_RecAck(void);

/* I2C发送8bit数据(高位先出, MSB First) */
void I2C3_SendData(u8 data);

/* I2C接收8bit数据(高位先出, MSB First) */
u8 I2C3_RecData(void);

/* 向CST816S的指定寄存器写入1字节数据 */
void CST816S_WriteByte(u8 addr, u8 data);

/* 从CST816S的指定寄存器地址开始, 连续读取len字节数据 */
void CST816S_RecData(u8 addr, u8 len, u8 *data);

/* CST816S触摸芯片初始化(配置寄存器 + 读取芯片ID) */
void CST816S_Config(void);

/* 读取触摸点XY坐标(从寄存器0x03读取4字节数据) */
void Get_XY(void);

/* 触摸扫描函数(判断中断引脚 -> 读手势 -> 读坐标) */
void CST_Scan(void);

#endif                                             /* _I2C_H */
