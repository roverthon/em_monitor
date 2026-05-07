#include "timer.h"
#include "../lvgl.h"

/**
  * @brief  初始化定时器3片内外设
  * @note   None
  * @param  None
  * @retval None
  */
void TIMER3_Init(void)
{
	// 0、外设信息结构体
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;		// 基本定时器功能信息结构体
	NVIC_InitTypeDef 		 NVIC_InitStructure;		// 嵌套向量中断控制器信息结构体
	
	// 1、使能定时器3的硬件时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	// 2、使能定时器3的全局中断(配置定时器3的中断优先级)
	NVIC_InitStructure.NVIC_IRQChannel 					 = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 		 = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd 				 = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	// 3、基本定时器配置
	TIM_TimeBaseStructure.TIM_Prescaler 	= 8400-1;				// 预分频值(一次分频)：168MHZ(AHB/1) /4(APB1/4) *2(APB的预分频值不是1，所以要*2) / 8400(为了更好的计算) == 输出频率：10000HZ(168000000/4*2/8400)
	TIM_TimeBaseStructure.TIM_Period 		= 10000/1000-1;			// 周期(计数值)(中断间隔)(加载值)：	使用10000HZ的时钟，去计数5000的计数值，花费的时间为：0.5s(500ms)
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;					// 时钟分频(二次分频)：STM32F407芯片没有二次分频，写0即可
	TIM_TimeBaseStructure.TIM_CounterMode 	= TIM_CounterMode_Up;	// 计数模式：从下往上计数
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);					// 使用该函数，将配置的信息写入到相应的寄存器中

	// 4、使能定时器3中断(更新中断)
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	// 5、使能定时器计数
	TIM_Cmd(TIM3, ENABLE);

}



/**
  * @brief  定时器3的中断服务函数
  * @note   None
  * @param  None
  * @retval None
  */
void TIM3_IRQHandler(void)
{
	static int8_t flag = 0;
	
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)		// 判断定时器3更新中断标志位是否被触发了
	{

		lv_tick_inc(1);
		
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);		// 清空定时器的中断挂起标志位
	}
}





