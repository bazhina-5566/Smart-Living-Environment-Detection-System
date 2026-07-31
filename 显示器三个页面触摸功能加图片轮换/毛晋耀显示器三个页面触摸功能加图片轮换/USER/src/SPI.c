#include "spi.h"
/**
  * @brief SPI1控制器初始化
  * @param  void
  * @retval void
  * @author He
  *  
    SPI1_MOSI     ---   SPI1       ---  PA7  --- 复用模式  
    SPI1_MISO     ---   SPI1       ---  PA6  --- 复用模式
    SPI1_SCK      ---   SPI1       ---  PA5  --- 复用模式
  */

void Spi1_Config(void)
{
    
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
    
   GPIO_InitTypeDef GPIO_InitStruct={0};
   GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
   GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
   GPIO_InitStruct.GPIO_Speed= GPIO_Speed_50MHz;
   GPIO_Init(GPIOA,&GPIO_InitStruct);
    
   //IO与片上外设的映射
   GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1);
   GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_SPI1); 
   GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_SPI1); 
   
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
   
   SPI_InitTypeDef SPI_InitStruct={0};
   SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
   SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
   SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
   SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
   SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
   SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
   SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
   SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;//软件管理片选
   SPI_Init(SPI1,&SPI_InitStruct);
   
   SPI_Cmd(SPI1,ENABLE);
    
}


/**
  * @brief SPI1数据交换函数
  * @param  u8 data   发送数据
  * @retval u8        收的数据
  * @author He
  *  
  */

u8 Spi1_TransferData(u8 data)
{
    
    //等待发送标志位为1
    while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE));
    //发送1byte数据
    SPI_I2S_SendData(SPI1,data);
    //等待接收标志位为1
    while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE));
    //接收数据
    return SPI_I2S_ReceiveData(SPI1);
    
}






