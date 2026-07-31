#include "main.h"


float te_item;                      /* 物体温度值(摄氏度) */

/**
  * @brief MLX90614初始化
  * @param 无
  * @retval 无
  * @note  初始化I2C通信引脚
  */
void MLX90614_Config(void)
{
    IIC1_Config();                  /* 初始化I2C通信引脚(PB6=SCL, PB7=SDA) */
}
/**

  * @brief MLX90614获取物体温度
  * @param 无
  * @retval 无
  * @note  SMBus协议: 写命令后发重复起始信号再读数据
  *        读取RAM地址0x07(Tobj1=物体温度)
  */
void Mlx_90614_GetItemTe(void)
{
    u8 buff[3] = {0};               /* 数据缓冲区(低8bit, 高8bit, PEC) */
    u16 te = 0;                     /* 温度原始16位数据 */

    /* === 第1步: 发送读取命令(0x07 = Tobj1寄存器) === */
    IIC1_Start();                   /* 产生I2C起始信号 */
    IIC1_SendData(MLX90614_WRITE_ADDR);  /* 发送MLX90614写地址(0xB4) */
    IIC1_RecAck();                  /* 接收从机应答 */
    IIC1_SendData(0x07);            /* 发送命令(读取RAM地址0x07=Tobj1) */
    IIC1_RecAck();                  /* 接收从机应答 */

    /* === 第2步: 重复起始信号, 读取温度数据(SMBus协议) === */
    IIC1_Start();                   /* 产生重复起始信号 */
    IIC1_SendData(MLX90614_READ_ADDR);   /* 发送MLX90614读地址(0xB5) */
    IIC1_RecAck();                  /* 接收从机应答 */

    buff[0] = IIC1_RecData();       /* 读取温度低8bit */
    IIC1_SendAck(0);                /* 发送应答(ACK) */
    buff[1] = IIC1_RecData();       /* 读取温度高8bit */
    IIC1_SendAck(0);                /* 发送应答(ACK) */
    buff[2] = IIC1_RecData();       /* 读取PEC校验码(本驱动不校验) */
    IIC1_SendAck(1);                /* 最后字节发送非应答(NACK) */

    IIC1_Stop();                    /* 产生I2C停止信号 */

    /* === 第3步: 数据处理与转换 === */
    te = buff[0] | (u16)buff[1] << 8;   /* 低8bit | 高8bit */

    /* 根据MLX90614数据手册公式: 温度 = 原始值 * 0.02 - 273.15 */
    te_item = te * 0.02f - 273.15f;
	
}
