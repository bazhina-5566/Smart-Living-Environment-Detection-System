#ifndef _USART1_H             /* 防止重复包含 */
#define _USART1_H

///* =================== USART结构体定义 =================== */
//typedef struct usart                  /* USART数据结构体 */
//{
//    u8 usart_buff[50];                /* 串口接收缓冲区(最多50字节) */
//    u8 len;                           /* 当前帧已接收的字节数(由中断累加) */
//    u8 usart_flag;                    /* 接收完成标志: 1=收到一帧完整数据(由空闲中断置1) */
//} USART_t;                            /* 结构体类型重定义 */

//extern USART_t usart1;                /* 全局USART结构体变量(中断和主循环共享) */
/*******************结构体定义***********************/
typedef struct{

     u8 buff[256];  
     u16 len;


}U1;

/* =================== 函数声明 =================== */
void Usart1_Config(u32 baud);              /* USART1初始化函数(参数baud=波特率) */
void usart1_send_byte(u8 data);            /* USART1发送单字节函数 */
void usart1_send_str(u8 *str);             /* USART1发送字符串函数 */

#endif
