#include "mq135.h"

/*
  功能：初始化 MQ135 模块（仅使用 DO 数字输出）
  - PA8 作为普通 GPIO 输入（读取 DO）
*/
void MQ135_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 1. 使能 GPIOA 时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    // 2. 配置 PA8 为浮空输入（读取 DO）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; // 浮空输入，外部模块已带上下拉
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*
  功能：读取 MQ135 模块的数字输出 DO（PA8）
  返回：0 或 1
  （注意：模块 DO 在浓度高于阈值时输出低电平 0，低于阈值时输出高电平 1，
   具体取决于模块上的阈值电位器设定）
*/
uint8_t Get_MQ135_DO_Level(void)
{
    return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8);
}