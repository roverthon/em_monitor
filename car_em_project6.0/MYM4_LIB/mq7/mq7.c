#include "mq7.h"

/*
  功能：初始化 MQ7 一氧化碳传感器
  - PA6 作为 ADC1_IN6（模拟输入，读取AO）
  - PG9 作为普通 GPIO 输入（读取 DO）
*/
void MQ7_Init(void)
{
    GPIO_InitTypeDef        GPIO_InitStructure;
    ADC_CommonInitTypeDef   ADC_CommonInitStructure;
    ADC_InitTypeDef         ADC_InitStructure;

    // 1. 使能时钟：GPIOA(PA6) + GPIOG(PG9) + ADC1
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOG, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    // 2. 配置 PA6 为模拟输入（ADC）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 3. 配置 PG9 为浮空输入（读取 DO）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOG, &GPIO_InitStructure);

    // 4. ADC 公共配置（和原MQ2完全一致）
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInit(&ADC_CommonInitStructure);

    // 5. ADC 常规配置（和原MQ2完全一致）
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    // 6. 配置 ADC 通道：PA6 对应 ADC1 通道 6
    ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1, ADC_SampleTime_56Cycles);

    // 7. 使能 ADC1
    ADC_Cmd(ADC1, ENABLE);
}

/*
  功能：获取 PA6 上的MQ7模拟电压值
  返回：电压值（0~3.3V）
*/
float Get_MQ7_AO_Voltage(void)
{
    uint16_t adc_value;

    // 配置ADC通道6
    ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1, ADC_SampleTime_56Cycles);

    // 软件启动转换
    ADC_SoftwareStartConv(ADC1);

    // 等待转换完成
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);

    // 读取结果
    adc_value = ADC_GetConversionValue(ADC1);

    // 转换为电压
    return adc_value * 3.3f / 4096.0f;
}

/*
  功能：读取 MQ7 模块的数字输出 DO（PG9）
  返回：0 或 1
  浓度超标时输出低电平，正常时高电平（可调节模块电位器）
*/
uint8_t Get_MQ7_DO_Level(void)
{
    return GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_9);
}