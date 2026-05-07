#include "pir.h"

// PE5 初始化为普通浮空输入
void PIR_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_AHB1PeriphClockCmd(PIR_RCC_CLK, ENABLE);

    GPIO_InitStruct.GPIO_Pin   = PIR_PIN;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL; // 浮空输入
    GPIO_Init(PIR_PORT, &GPIO_InitStruct);
}

// 返回值：1有人  0没人
uint8_t PIR_Scan(void)
{
    return GPIO_ReadInputDataBit(PIR_PORT, PIR_PIN);
}