#ifndef __IIC_H             /* 防止重复包含 */
#define __IIC_H

#include "main.h"           /* STM32F4系列头文件 */

/* ===== I2C引脚宏定义(SHT30: SDA=PB7, SCL=PB6) ===== */
#define IIC1_SDA_H      (GPIO_SetBits(GPIOB, GPIO_Pin_7))       /* SDA置高 */
#define IIC1_SDA_L      (GPIO_ResetBits(GPIOB, GPIO_Pin_7))     /* SDA置低 */
#define IIC1_SCL_H      (GPIO_SetBits(GPIOB, GPIO_Pin_6))       /* SCL置高 */
#define IIC1_SCL_L      (GPIO_ResetBits(GPIOB, GPIO_Pin_6))     /* SCL置低 */
#define IIC1_SDA_IN     (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7))  /* 读取SDA电平 */

#define IIC1_DELAY_TIME 6       /* I2C延时时间参数 */

/* ===== 函数声明 ===== */
void IIC1_Config(void);             /* I2C引脚初始化函数声明 */
void IIC1_Start(void);              /* I2C起始信号函数声明 */
void IIC1_Stop(void);               /* I2C停止信号函数声明 */
void IIC1_SendAck(u8 ack);          /* I2C发送应答函数声明(0=应答,1=非应答) */
u8 IIC1_RecAck(void);               /* I2C接收应答函数声明(返回0=应答,1=非应答) */
void IIC1_SendData(u8 data);        /* I2C发送8bit数据函数声明 */
u8 IIC1_RecData(void);              /* I2C接收8bit数据函数声明 */

#endif
