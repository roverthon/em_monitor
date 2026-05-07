#include "exti.h"
#include "key.h"
#include "delay.h"
#include "buzzer.h"
#include "led.h"
#include <stdio.h>


/**
  * @brief  初始化按键触发的外部中断
  * @note   GEC-M4板子
  *				S1(丝印层)->KEY0(网络标号)->PA0(芯片引脚标号)->EXTI0线
  *				S2(丝印层)->KEY1(网络标号)->PE2(芯片引脚标号)->EXTI2线
  *				S3(丝印层)->KEY2(网络标号)->PE3(芯片引脚标号)->EXTI3线
  *				S4(丝印层)->KEY3(网络标号)->PE4(芯片引脚标号)->EXTI4线	
  *
  * @param  None
  * @retval None
  */
void EXTI0234_Init(void)
{

	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	
	// 1、配置引脚为输入上拉模式
	KEY_Init();

	// 2、将GPIO引脚连接到相应的EXTI中
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);									// 使能SYSCFG硬件时钟
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);							// 连接PA0引脚至EXTI0线
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2);							// 连接PE2引脚至EXTI2线
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3);							// 连接PE3引脚至EXTI3线
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4);							// 连接PE4引脚至EXTI4线
	
	// 3、配置外部中断			                                                
	EXTI_InitStructure.EXTI_Line 		= 	EXTI_Line0|EXTI_Line2|EXTI_Line3|EXTI_Line4;	// 外部中断：0、2、3、4线
	EXTI_InitStructure.EXTI_Mode		=	EXTI_Mode_Interrupt;							// 模式：中断模式
	EXTI_InitStructure.EXTI_Trigger		=	EXTI_Trigger_Falling;							// 触发方式：下降沿触发
	EXTI_InitStructure.EXTI_LineCmd		= 	ENABLE;											// 使能外部中断
	EXTI_Init(&EXTI_InitStructure);															// 使用该函数，将配置的信息写入到相应的寄存器中
				                                                                            
	// 4、配置事件的优先等级，并设置中断请求通道	
	// a、EXTI0线
	NVIC_InitStructure.NVIC_IRQChannel						= 	EXTI0_IRQn;					// 中断请求通道：外部中断0通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority    =   0x02;						// 抢占式优先级：0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority           =   0x02;						// 响应式优先级(次优先级)：0
	NVIC_InitStructure.NVIC_IRQChannelCmd					= 	ENABLE;						// 使能NVIC相应的中断请求通道
	NVIC_Init(&NVIC_InitStructure);
	
	// b、EXTI2线
	NVIC_InitStructure.NVIC_IRQChannel						= 	EXTI2_IRQn;					
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority    =   0x02;						
	NVIC_InitStructure.NVIC_IRQChannelSubPriority           =   0x02;						
	NVIC_InitStructure.NVIC_IRQChannelCmd					= 	ENABLE;						
	NVIC_Init(&NVIC_InitStructure);

	// c、EXTI4线
	NVIC_InitStructure.NVIC_IRQChannel						= 	EXTI4_IRQn;					
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority    =   0x00;						
	NVIC_InitStructure.NVIC_IRQChannelSubPriority           =   0x00;						
	NVIC_InitStructure.NVIC_IRQChannelCmd					= 	ENABLE;						
	NVIC_Init(&NVIC_InitStructure);
}



/**
  * @brief  外部中断5到9的中断服务函数
  * @note   S1(丝印层)->KEY0(网络标号)->PA0(芯片引脚标号)->EXTI0线
  * @param  None
  * @retval None
  */

void EXTI0_IRQHandler(void)								
{
	
	if(EXTI_GetITStatus(EXTI_Line0) == SET)		// 判断是否触发了外部中断0线
	{
		DELAY_MyNms(10000);
		LED1(ON);
		EXTI_ClearITPendingBit(EXTI_Line0);		// 清除外部中断0线的中断挂起标志位
	}
}


/**
  * @brief  外部中断2的中断服务函数
  * @note   S2(丝印层)->KEY1(网络标号)->PE2(芯片引脚标号)->EXTI2线
  * @param  None
  * @retval None
  */

void EXTI2_IRQHandler(void)								
{
	
	if(EXTI_GetITStatus(EXTI_Line2) == SET)		// 判断是否触发了外部中断2线
	{
		DELAY_MyNms(20000);
		LED2(ON);
		EXTI_ClearITPendingBit(EXTI_Line2);		// 清除外部中断2线的中断挂起标志位
	}
}



/**
  * @brief  外部中断4的中断服务函数
  * @note   S4(丝印层)->KEY3(网络标号)->PE4(芯片引脚标号)->EXTI4线	
  * @param  None
  * @retval None
  */

void EXTI4_IRQHandler(void)								
{
	
	if(EXTI_GetITStatus(EXTI_Line4) == SET)		// 判断是否触发了外部中断4线
	{
		DELAY_MyNms(20000);
		LED4(ON);
		EXTI_ClearITPendingBit(EXTI_Line4);		// 清除外部中断4线的中断挂起标志位
	}
}


