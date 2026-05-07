#include "mq2.h"

/*
  功能：初始化 MQ2 模块
  - PA4 作为 ADC1_IN4（模拟输入）
  - PC7 作为普通 GPIO 输入（读取 DO）
*/
void MQ2_Init(void)
{
    GPIO_InitTypeDef        GPIO_InitStructure;
    ADC_CommonInitTypeDef   ADC_CommonInitStructure;
    ADC_InitTypeDef         ADC_InitStructure;

    // 1. 使能时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    // 2. 配置 PA4 为模拟输入（ADC）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; // 模拟模式不需要上下拉
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 3. 配置 PC7 为浮空输入（读取 DO）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; // 浮空输入，外部模块已带上下拉
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // 4. ADC 公共配置
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;   // 84MHz/2=42MHz
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInit(&ADC_CommonInitStructure);

    // 5. ADC 常规配置
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;   // 单次转换
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; // 软件触发
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    // 6. 配置 ADC 通道：PA4 对应 ADC1 通道 4，采样时间可设长一点（如56周期）
    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_56Cycles);

    // 7. 使能 ADC1
    ADC_Cmd(ADC1, ENABLE);
}

/*
  功能：获取 PA4 上的模拟电压值
  返回：电压值（0~3.3V）
*/
float Get_MQ2_AO_Voltage(void)
{
    uint16_t adc_value;

    // 再次确保通道配置（可省略，但保留以便切换采样时间）
    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_56Cycles);

    // 软件启动转换
    ADC_SoftwareStartConv(ADC1);

    // 等待转换完成
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);

    // 读取转换结果
    adc_value = ADC_GetConversionValue(ADC1);

    // 转换为电压：3.3V * ADC值 / 4096
    return adc_value * 3.3f / 4096.0f;
}

/*
  功能：读取 MQ2 模块的数字输出 DO（PC7）
  返回：0 或 1
  （注意：模块 DO 通常为 TTL 电平，有烟雾时输出低电平 0，无烟雾时高电平 1，
   具体取决于模块上的阈值电位器设定）
*/
uint8_t Get_MQ2_DO_Level(void)
{
    return GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7);
}