#ifndef _LCD_H                   /* 防止重复包含 */
#define _LCD_H

#include "main.h"           /* STM32F4系列头文件 */

/* ===== LCD引脚宏定义 ===== */
#define LCD_SDA_H   (GPIO_SetBits(GPIOB, GPIO_Pin_5))     /* LCD数据线置高(PB5) */
#define LCD_SDA_L   (GPIO_ResetBits(GPIOB, GPIO_Pin_5))   /* LCD数据线置低(PB5) */
#define LCD_SCK_H   (GPIO_SetBits(GPIOB, GPIO_Pin_3))     /* LCD时钟线置高(PB3) */
#define LCD_SCK_L   (GPIO_ResetBits(GPIOB, GPIO_Pin_3))   /* LCD时钟线置低(PB3) */
#define LCD_CS_H    (GPIO_SetBits(GPIOB, GPIO_Pin_4))     /* LCD片选置高(PB4) */
#define LCD_CS_L    (GPIO_ResetBits(GPIOB, GPIO_Pin_4))   /* LCD片选置低(PB4) */
#define LCD_DATA    (GPIO_SetBits(GPIOD, GPIO_Pin_3))     /* LCD数据/命令选择为数据(PD3=高) */
#define LCD_CMD     (GPIO_ResetBits(GPIOD, GPIO_Pin_3))   /* LCD数据/命令选择为命令(PD3=低) */
#define LCD_BLK_ON  (GPIO_SetBits(GPIOD, GPIO_Pin_4))     /* LCD背光开启(PD4=高) */
#define LCD_BLK_OFF (GPIO_ResetBits(GPIOD, GPIO_Pin_4))   /* LCD背光关闭(PD4=低) */

/* ===== 颜色定义 ===== */
#define WHITE   0xFFFF
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x7FFF
#define YELLOW  0xFFE0
#define MAGENTA 0xF81F




/* ===== 函数声明 ===== */

////宾哥
extern const unsigned char gImage_11[153608];
////源
//extern const unsigned char gImage_Y11[151376];
//extern const unsigned char gImage_Y12[151376];
//extern const unsigned char gImage_Y13[151376];
//extern const unsigned char gImage_Y14[151376];
//extern const unsigned char gImage_Y15[151376];
extern u8 eng1[];
extern u8 eng2[];
extern u8 eng3[];
extern u8 eng4[];
extern u8 eng6[];
extern u8 chi1[];
extern u8 chi2[];
extern u8 chi3[];
extern u8 chi4[];
extern u8 chi5[];
extern u8 chi6[];
extern u8 chi7[];
extern u8 eng111[];
extern u8 eng112[];
extern u8 eng113[];
extern u8 *my_string[];
void Lcd_Port_Config(void);        /* LCD引脚初始化函数声明 */
void Lcd_TransferData(u8 data);    /* LCD发送8bit数据(SPI方式)函数声明 */
void Lcd_Send_Data(u8 data);       /* LCD发送8bit数据/参数函数声明 */
void Lcd_Send_Cmd(u8 cmd);         /* LCD发送8bit命令函数声明 */
void LCD_Init(void);               /* LCD初始化函数声明(含参数配置+清屏) */
void Lcd_Send_Data16b(u16 data);   /* LCD发送16bit数据函数声明 */
void Lcd_SetPostion(u16 xs, u16 ys, u16 xe, u16 ye); /* LCD设置显示范围函数声明 */
void Lcd_Clear(u16 xs, u16 ys, u16 xe, u16 ye, u16 color); /* LCD清屏函数声明 */
void Lcd_DrawPoint(u16 x, u16 y, u16 color);                    /* LCD画点函数声明 */
void Lcd_DisplayChar(u16 x, u16 y, u16 font_color, u16 bg_color, u8 size, u8 *eng); /* LCD显示单个英文字符函数声明 */
void Lcd_DisplayString(u16 x, u16 y, u16 font_color, u16 bg_color, u8 size, u8 *str); /* LCD显示字符串函数声明 */
void Lcd_DisplayChi(u16 x, u16 y, u16 font_color, u16 bg_color, u8 size, u8 *chi); /* LCD显示单个汉字函数声明 */
void Lcd_DisplayPic(u16 x, u16 y, u8 *pic);    /* LCD显示图片函数声明 */
void Lcd_ZikuChi(u16 x,u16 y,u16 font_color,u16 bg_color,u8 size,u8 *chi);
void Lcd_ZikuEng(u16 x, u16 y, u16 font_color, u16 bg_color, u8 size, u8 code);
void Lcd_ZikuStr(u16 x, u16 y, u16 font_color, u16 bg_color, u8 size, u8 *str);
#endif
