#include "main.h"

/* 全局变量定义 */
LED_Mode_t current_mode = LED_MODE_OFF;
float servo_current_angle = 90.0f;
uint8_t work_mode = 0;
uint8_t key = 0;
uint8_t ui_flag =0;
volatile uint32_t beep_timer = 0; 
extern uint8_t ui_flag;

/* 当前页面索引: 0=首页, 1=控制页, 2=图片页(暂预留) */
uint8_t current_page = 0; 

int main(void)
{
    /* ===== 1. 外设基础初始化 ===== */
    LED_Init();
    KEY_Init();
    Relay_Init();
    Buzzer_Init();
    Timer_Delay_Init();
    Fan_Config();
    Servo_Init();
    Rgb_Config();
    Servo_SetAngle(servo_current_angle);
    Update_Device_Links(servo_current_angle);
    Sht30_Config();
    MLX90614_Config();
    LCD_Init();
    Usart1_Config(256000);
    W25Q64_Config();
    CST816S_Config();
		ADC1_Config();   // 先配置 ADC1 的时钟、GPIO、并使能 ADC1
		Flame_Config();  // 再配置火焰传感器用到的 PC1
    SysTick_Config(SystemCoreClock / 1000);
    printf("系统初始化成功!!!!\r\n");

    /* ===== 2. UI与触摸状态初始化 ===== */
    ui_flag = 0;       // 确保首页首次进入强制重绘
    touch_lock = 0;    // 触摸防抖锁初始解锁
    cst.ges_sta = 0x00;
    cst.x = 0;
    cst.y = 0;

    while(1)
{
    /* ① 优先刷新LED，保证PWM平滑（仅调整顺序，不新增内容） */
    LED_Mode_Update();

    /* ② 扫描触摸屏 */
    CST_Scan(); 
	
	/*按键扫描*/
	Fan_KeyControl();

	if (cst.ges_sta == 0x04) 
	{
		current_page++;
		Buzzer_On();
		beep_timer = 100;
		if (current_page > 3) 
		{
			current_page = 0;
		}
		ui_flag = 0;
		touch_lock = 0;
		cst.ges_sta = 0x00;
		cst.x = 0;
		cst.y = 0;
		printf("检测到右滑，切换至界面: %d\r\n", current_page);
	}

	if(sht30_flag == 1)
    {
       sht30_flag = 0;                          /* 清除采集标志 */    /* ③ 右滑切页处理 */
		/* ④ 页面渲染 */
		switch (current_page)
		{
			case 0: Page_Home(); break;
			case 1: Page_One(); break;
			case 2: Page_Two(); break;
			case 3: Page_Sensor(); break;
			default: current_page = 0; break;
		}

	}
	
	}
}

