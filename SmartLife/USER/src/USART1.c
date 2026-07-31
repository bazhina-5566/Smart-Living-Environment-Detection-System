#include "main.h"             /* 包含main.h头文件 */

///* =================== 全局变量定义 =================== */
//USART_t usart1 = {0};                /* USART结构体变量(所有串口相关数据集中管理) */

/**
  * @brief USART1初始化(含RXNE中断 + 空闲中断使能)
  * @param baud  波特率(如115200)
  * @retval 无
  * @note   PA9=TX, PA10=RX, 使能接收中断和空闲中断
  */
void Usart1_Config(u32 baud)         /* 定义USART1初始化函数，参数baud为波特率 */
{
    /* ===== 1. 开启GPIOA和USART1时钟 ===== */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);        /* 开启GPIOA时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);       /* 开启USART1时钟(APB2=84MHz) */

    /* ===== 2. 配置PA9(TX)和PA10(RX)为复用功能 ===== */
    GPIO_InitTypeDef GPIO_InitStruct={0};                         /* 定义GPIO结构体变量并清零 */
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;                     /* 设置为复用功能模式 */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;          /* 选择Pin9(TX)和Pin10(RX) */
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;                /* 设置速度为50MHz */
    GPIO_Init(GPIOA, &GPIO_InitStruct);                           /* 初始化GPIOA */

    /* ===== 3. PA9/PA10映射到USART1 ===== */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);    /* PA9复用为USART1_TX */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);   /* PA10复用为USART1_RX */

    /* ===== 4. 配置USART1参数 ===== */
    USART_InitTypeDef USART_InitStruct={0};                       /* 定义USART结构体变量并清零 */
    USART_InitStruct.USART_BaudRate = baud;                       /* 设置波特率(如115200) */
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; /* 无硬件流控 */
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;  /* 使能发送和接收 */
    USART_InitStruct.USART_Parity = USART_Parity_No;              /* 无校验位 */
    USART_InitStruct.USART_StopBits = USART_StopBits_1;           /* 1位停止位 */
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;      /* 8位数据长度 */
    USART_Init(USART1, &USART_InitStruct);                        /* 初始化USART1 */

    /* ===== 5. 使能RXNE接收中断 + 空闲中断 ===== */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);                /* 使能USART1接收中断(每收到1字节触发) */
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);                /* 使能USART1空闲中断(一帧数据接收完毕触发) */

    /* ===== 6. 配置NVIC中断优先级 ===== */
	NVIC_InitTypeDef NVIC_InitStruct = {0};                    /* 定义NVIC结构体变量并清零 */
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;             /* 中断通道选择USART1 */
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;     /* 抢占优先级设为2 */
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;            /* 响应优先级设为2 */
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;               /* 使能USART1中断通道 */
	NVIC_Init(&NVIC_InitStruct);  
    /* ===== 7. 使能USART1 ===== */
    USART_Cmd(USART1, ENABLE);                                     /* 开启USART1 */
}

/**
  * @brief USART1发送单字节函数
  * @param data  要发送的字节数据
  * @retval 无
  */
void usart1_send_byte(u8 data)       /* 定义发送单字节函数 */
{
    while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));          /* 等待发送数据寄存器空 */
    USART_SendData(USART1, data);                                 /* 发送一个字节数据 */
}

/**
  * @brief USART1发送字符串函数
  * @param str  要发送的字符串指针
  * @retval 无
  */
void usart1_send_str(u8 *str)        /* 定义发送字符串函数 */
{
    while(*str != '\0')              /* 遍历字符串直到结束符 */
    {
        usart1_send_byte(*str);      /* 逐个字节发送 */
        str++;                       /* 指针移动到下一个字符 */
    }
}

///**
//  * @brief USART1中断服务函数(RXNE接收中断 + 空闲中断)
//  * @param 无
//  * @retval 无
//  * @note   RXNE中断: 逐字节接收存入缓冲区
//  *         IDLE中断: 一帧数据接收完毕,置位完成标志(不清零len,由主循环处理完后清零)
//  */
//void USART1_IRQHandler(void)
//{
//	if(USART_GetITStatus(USART1, USART_IT_RXNE))
//	{
//		usart1.usart_buff[usart1.len++] = USART_ReceiveData(USART1);
//	}
//	if(USART_GetITStatus(USART1, USART_IT_IDLE))
//	{
//		usart1.usart_buff[usart1.len] = '\0';
//		usart1.len = 0;
//		usart1.usart_flag = 1;
//		USART_ReceiveData(USART1);
//	}
//}
/**
  * @brief USART1中断服务函数
  * @param  void
  * @retval void
  * @author He
  *  
  */
U1 u1;
void USART1_IRQHandler(void)
{
    u8 data;
    static u32 addr = 0;
    if(USART_GetITStatus(USART1,USART_IT_RXNE))
    {
        USART_ClearITPendingBit(USART1,USART_IT_RXNE);
        //user handle
        file_flag = 1;
        
        data = USART_ReceiveData(USART1);
        
        W25Q64_CrossPageWrite(addr++,1,&data);
        
    }
    if(USART_GetITStatus(USART1,USART_IT_IDLE))
    {
        USART1->SR;
        USART1->DR;
        //user handle
       
        if(addr >= 0x001D3374)
        {
            file_flag = 0;
            transfer_flag = 0;
            
        }
     
    }   
}
//void USART1_IRQHandler(void)                    /* 串口1中断服务函数 */
//{
//    u8 data;                                     /* 暂存接收到的1字节数据 */
//    static u32 addr = 0;                         /* 静态变量：记录当前写入W25Q64的地址 */
//    if(USART_GetITStatus(USART1,USART_IT_RXNE))  /* 判断是否为RXNE中断(收到1字节) */
//    {
//        USART_ClearITPendingBit(USART1,USART_IT_RXNE);  /* 清除RXNE中断标志位 */
//        /* user handle */
//        file_flag = 1;                           /* 置位文件传输标志，通知主循环正在传输 */
//        
//        data = USART_ReceiveData(USART1);        /* 从串口接收寄存器读取数据 */
//        
//        W25Q64_CrossPageWrite(addr++,1,&data);   /* 将数据写入W25Q64，地址自增1 */
//        
//    }
//    if(USART_GetITStatus(USART1,USART_IT_IDLE))  /* 判断是否为IDLE中断(串口空闲) */
//    {
//        USART1->SR;                              /* 读SR寄存器以清除IDLE标志位(标准做法) */
//        USART1->DR;                              /* 读DR寄存器以清除IDLE标志位(标准做法) */
//        /* user handle */
//       
//        if(addr >= 0x001D3374)                   /* 判断是否达到目标文件大小(约1.9MB) */
//        {
//            file_flag = 0;                       /* 清除文件传输标志 */
//            transfer_flag = 0;                   /* 清除传输进行中标志，表示传输完成 */
//            
//        }
//     
//    }
//}
/**
  * @brief printf重定向(fputc)
  * @param c      要输出的字符
  * @param stream 文件流指针(未使用)
  * @retval 返回输出的字符
  * @note   将printf输出重定向到USART1
  */
int fputc(int c, FILE * stream)      /* printf重定向函数 */
{
    usart1_send_byte(c);             /* 通过USART1发送字符 */
    return c;                        /* 返回发送的字符 */
}
