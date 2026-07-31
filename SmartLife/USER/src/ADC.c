#include "main.h"

/**
  * @brief  ADC1初始化
  * @param  void
  * @retval void
  * @note   光敏电阻(ADC_LDR) --- PC0 --- 模拟模式 --- ADC1_IN10
  *         12bit分辨率，单次转换，轮询模式(不用DMA)
  */
void ADC1_Config(void)
{
	/* 打开GPIOC时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	/* IO管脚初始化: PC0 = ADC1_IN10, 模拟模式, 无上拉下拉 */
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);

	/* 打开ADC1时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	/* ADC1初始化: 单次转换, 右对齐, 1次转换, 12bit, 关闭扫描 */
	ADC_InitTypeDef ADC_InitStruct = {0};
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;   /* 单次转换模式 */
	ADC_InitStruct.ADC_DataAlign          = ADC_DataAlign_Right;  /* 数据右对齐 */
	ADC_InitStruct.ADC_NbrOfConversion    = 1;          /* 转换次数为1 */
	ADC_InitStruct.ADC_Resolution         = ADC_Resolution_12b;   /* 12bit分辨率 */
	ADC_InitStruct.ADC_ScanConvMode       = DISABLE;    /* 关闭扫描模式 */
	ADC_Init(ADC1, &ADC_InitStruct);

	/* 配置通道: ADC1_IN10(PC0), 采样周期480Cycles */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_480Cycles);

	/* 使能ADC1 */
	ADC_Cmd(ADC1, ENABLE);
}

/**
  * @brief  ADC1获取光敏电阻数据
  * @param  void
  * @retval void
  * @note   轮询方式读取ADC转换值，转换为百分比后存到全局变量light_value
  *         light_value = 100 - (raw * 100.0 / 4096.0)
  *         数值越大表示光越暗，数值越小表示光越亮
  */
float light_value = 0.0f;           /* 光敏电阻值(百分比) */

void ADC1_GetValue(void)
{
	u16 raw_value = 0;              /* 原始ADC转换值(0~4095) */

	/* 配置通道: ADC1_IN10(PC0), 采样周期480Cycles */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_480Cycles);
	
	/* 软件触发ADC1转换 */
	ADC_SoftwareStartConv(ADC1);

	/* 等待转换完成(EOC标志位) */
	while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));

	/* 读取ADC转换结果 */
	raw_value = ADC_GetConversionValue(ADC1);

	/* 将原始值转换为百分比: 数值越大表示光越暗，数值越小表示光越亮 */
	light_value = 100.0f - (raw_value * 100.0f / 4096.0f);

	/* 通过串口打印光敏电阻数据 */
	printf("光照强度: %u  ==>  %.1f%%\r\n", raw_value, light_value);
}