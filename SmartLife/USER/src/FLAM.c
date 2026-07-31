#include "main.h"

/**
  * @brief  火焰传感器初始化
  * @param  void
  * @retval void
  * @note   火焰传感器(ADC_IR) --- PC1 --- 模拟模式 --- ADC1_IN11
  *         12bit分辨率，单次转换，轮询模式(不用DMA)
  */
void Flame_Config(void)
{
	/* 打开GPIOC时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	/* IO管脚初始化: PC1 = ADC1_IN11, 模拟模式, 无上拉下拉 */
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_1;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/**
  * @brief  火焰传感器获取数据
  * @param  void
  * @retval void
  * @note   轮询方式读取ADC转换值，转换为百分比后存到全局变量flame_value
  *         flame_value = 100 - (raw * 100.0 / 4096.0)
  *         数值越大表示火焰越近(越大)，数值越小表示火焰越远(越小)
  */
float flame_value = 0.0f;           /* 火焰传感器值(百分比) */

void Flame_GetValue(void)
{
	u16 adc_raw = 0;                /* 原始ADC转换值(0~4095) */

	/* 配置通道: ADC1_IN10(PC0), 采样周期480Cycles */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 1, ADC_SampleTime_480Cycles);
	
	/* 软件触发ADC1转换 */
	ADC_SoftwareStartConv(ADC1);

	/* 等待转换完成(EOC标志位) */
	while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));

	/* 读取ADC转换结果 */
	adc_raw = ADC_GetConversionValue(ADC1);

	/* 将原始值转换为百分比: 越近值越大, 越远值越小 */
	flame_value = 100.0f - (adc_raw * (100.0f / 4096.0f));

	/* 通过串口打印火焰传感器数据 */
	printf("Flame raw:%d  flame:%.1f%%\r\n", adc_raw, flame_value);
}