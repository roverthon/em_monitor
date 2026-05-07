#include "adc.h"


uint16_t ret_val = 0;
//1、ADC3_IN5光敏传感器初始化(PF7引脚)
void ADC3_IN5_LightInit(void)
{
	/* 0、外设信息配置结构体*/
	ADC_InitTypeDef       ADC_InitStructure;										//ADC外设基础信息配置结构体
	ADC_CommonInitTypeDef ADC_CommonInitStructure;									//ADC通用信息配置结构体
	GPIO_InitTypeDef      GPIO_InitStructure;										//GPIO基础信息配置结构体

	/* 1、使能ADC3和GPIOF组时钟 */ 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);							//84MHZ	

	/* 2、配置ADC3通道5作为模拟输入 */
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_7;									//引脚：			第7个引脚
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AN;									//模式：			模拟模式
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;								//是否上下拉：   不拉
	GPIO_Init(GPIOF, &GPIO_InitStructure);											//将配置的外设信息，写入到相对应的寄存器中

	/* 3、ADC通用配置 */
	ADC_CommonInitStructure.ADC_Mode 				= ADC_Mode_Independent;			//独立工作模式：1个ADC硬件独立工作
	ADC_CommonInitStructure.ADC_Prescaler 			= ADC_Prescaler_Div2;			//ADC的分频系数：84MHZ/2 = 42MHZ 	
	ADC_CommonInitStructure.ADC_DMAAccessMode 		= ADC_DMAAccessMode_Disabled;	//DMA模式：类似内存映射，不需要
	ADC_CommonInitStructure.ADC_TwoSamplingDelay 	= ADC_TwoSamplingDelay_5Cycles;	//两个采样点的时间间隔：5个时钟间隔
	ADC_CommonInit(&ADC_CommonInitStructure);										//将配置的外设信息，写入到相对应的寄存器中

	/* 4、ADC3外设信息配置 */
	ADC_InitStructure.ADC_Resolution 				= ADC_Resolution_12b;			//采样精度为12位
	ADC_InitStructure.ADC_ScanConvMode 				= DISABLE;						//是否多通道检测(单通道)			
	ADC_InitStructure.ADC_ContinuousConvMode 		= ENABLE;						//连续转换，单次转换
	ADC_InitStructure.ADC_ExternalTrigConvEdge 		= ADC_ExternalTrigConvEdge_None;//禁用外部脉冲来让ADC工作
	ADC_InitStructure.ADC_ExternalTrigConv 			= ADC_ExternalTrigConv_T1_CC1;	//不需要外部触发
	ADC_InitStructure.ADC_DataAlign 				= ADC_DataAlign_Right;			//默认右对齐方式
	ADC_InitStructure.ADC_NbrOfConversion 			= 1;							//转换的通道数
	ADC_Init(ADC3, &ADC_InitStructure);

	/* 5、ADC3通道5配置(为所选的ADC配置相应的常规通道()及其采用时间) */
	/*
		ADC3： 					哪个ADC外设
		ADC_Channel_5：			ADC通道
		1：						表示转换序号，如果ADC需要对多通道进行转换的话，指定转换通道的顺序，实现排队转换
		ADC_SampleTime_3Cycles;	表示转换该采样点的时间间隔，3个时钟周期(3/42MHZ)
	*/
	
	ADC_RegularChannelConfig(ADC3, ADC_Channel_5, 1, ADC_SampleTime_3Cycles);


	/* 6、清除ADC标志位 */
	ADC_ClearFlag(ADC3, ADC_FLAG_EOC);
	
	/* 7、使能ADC */
	ADC_Cmd(ADC3, ENABLE);
}

//2、获取ADC值
uint16_t ADC3_IN5_GetLightVal(void)
{
	
	
	/* 1、启动ADC软件转换 */ 
	ADC_SoftwareStartConv(ADC3);
	
	/* 2、等待转换结果(没有转换完毕，就让其一直堵塞于此) */
	while(ADC_GetFlagStatus(ADC3, ADC_FLAG_EOC) == RESET);
	
	/* 3、得到转换后的值(这个不是电压值) */
	ret_val = ADC_GetConversionValue(ADC3);

	return ret_val;
}




