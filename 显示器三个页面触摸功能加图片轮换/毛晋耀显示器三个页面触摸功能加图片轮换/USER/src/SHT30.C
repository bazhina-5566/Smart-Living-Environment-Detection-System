#include "main.h"


/**
  * @brief SHT30初始化
  * @param  void
  * @retval void
  * @note  调用IIC1_Config()初始化I2C引脚
  */

void Sht30_Config(void)
{
    
    IIC1_Config();                 /* 初始化I2C通信引脚(PB6=SCL, PB7=SDA) */
    
}


/**
  * @brief SHT30获取温度湿度数据
  * @param  void
  * @retval void
  * @note
    SHT30通信流程:
    1. 发送写命令(0x2C, 0x06)启动测量
    2. 等待15ms转换完成
    3. 读取6字节数据(温度高8位, 温度低8位, CRC, 湿度高8位, 湿度低8位, CRC)
    4. 计算实际温湿度值
  */

float te_value;                    /* 温度值(摄氏度) */
float hu_value;                    /* 湿度值(百分比) */

void Sht30_GetValue(void)
{
    u8 ret = 0;                    /* 接收应答返回值 */
    u8 buff[6] = {0};              /* 数据缓冲区(6字节) */
    u16 te = 0;                    /* 温度原始16位数据 */
    u16 hu = 0;                    /* 湿度原始16位数据 */
    
    /* === 第1步: 发送测量命令 === */
    IIC1_Start();                  /* 产生I2C起始信号 */
    
    IIC1_SendData(SHT30_WRITE_ADDR);  /* 发送SHT30写地址(0x88) */
    IIC1_RecAck();                 /* 接收从机应答 */
    
    IIC1_SendData(0x2C);           /* 发送命令高8bit(开启单次测量) */
    IIC1_RecAck();                 /* 接收从机应答 */
    
    IIC1_SendData(0x06);           /* 发送命令低8bit(高重复性) */
    IIC1_RecAck();                 /* 接收从机应答 */
    
A:  /* 重试标签: 如果读取时NACK则重试 */
    IIC1_Stop();                   /* 产生I2C停止信号 */
    
    delay_ms(15);                 /* 延时约15ms(等待SHT30测量完成) */
    
    /* === 第2步: 读取温湿度数据 === */
    IIC1_Start();                  /* 产生I2C起始信号 */
    
    IIC1_SendData(SHT30_READ_ADDR);   /* 发送SHT30读地址(0x89) */
    ret = IIC1_RecAck();           /* 接收从机应答 */
    if(ret == 1)                   /* 如果收到NACK(非应答) */
        goto A;                    /* 重试(从停止信号开始) */
    
    buff[0] = IIC1_RecData();      /* 温度高8bit */
    IIC1_SendAck(0);               /* 发送应答(ACK) */
    buff[1] = IIC1_RecData();      /* 温度低8bit */
    IIC1_SendAck(0);               /* 发送应答(ACK) */
    buff[2] = IIC1_RecData();      /* CRC校验码(本驱动不校验) */
    IIC1_SendAck(0);               /* 发送应答(ACK) */
    buff[3] = IIC1_RecData();      /* 湿度高8bit */
    IIC1_SendAck(0);               /* 发送应答(ACK) */
    buff[4] = IIC1_RecData();      /* 湿度低8bit */
    IIC1_SendAck(0);               /* 发送应答(ACK) */
    buff[5] = IIC1_RecData();      /* CRC校验码(本驱动不校验) */
    IIC1_SendAck(1);               /* 最后字节发送非应答(NACK) */
    
    IIC1_Stop();                   /* 产生I2C停止信号 */
    
    /* === 第3步: 数据处理与转换 === */
    te = buff[0] << 8 | buff[1];   /* 温度16bit数据 */
    hu = buff[3] << 8 | buff[4];   /* 湿度16bit数据 */
    
    /* 根据SHT30数据手册公式转换实际值 */
    te_value = -45 + 175 * (te / 65535.0f);   /* 温度 = -45 + 175 * (te/65535) */
    hu_value = 100 * (hu / 65535.0f);          /* 湿度 = 100 * (hu/65535) */
    
}

