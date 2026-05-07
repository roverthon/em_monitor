#include "key.h"

/**
  * @brief  初始化按键模块
  * @note   GEC-M4板子
  *				S1(丝印层)->KEY0(网络标号)->PA0(芯片引脚标号)
  *				按键采用上拉输入模式，无按键按下时为高电平，按下时为低电平
  *
  * @param  None
  * @retval None
  */
void KEY_Init(void)
{
    // 定义GPIO初始化结构体
    GPIO_InitTypeDef GPIO_InitStruct;
    
    // 1. 使能GPIOA时钟（AHB1总线）
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    
    // 2. 配置PA0引脚参数
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_0;          				// 指定初始化PA0引脚
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN;       				// 引脚模式：输入模式
    GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;       				// 上拉电阻使能（无按键按下时为高电平）
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;  				// 引脚速度（输入模式下不影响，仅为标准配置）
	GPIO_Init(GPIOA, &GPIO_InitStruct);
    
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;  // 指定初始化PE2、3、4引脚
	GPIO_Init(GPIOE, &GPIO_InitStruct);

}
