/**
  ******************************************************************************
  * @file    ASRPRO.c
  * @author  RoverTyphon
  * @version V0.0.1
  * @date    2026.02.05
  * @brief   
  *
  ******************************************************************************
  * @attention
  *
  * 本文档只供学习使用，不得商用，违者必究
  *
  *  * 有疑问或者建议：RoverTyphon@outlook.com 
	*
  ******************************************************************************
  */

#include "ASRPRO.h"
#include "uart.h"
// Platform
#include "stm32f4xx.h"

// Port
//#include "softuart.h"


static void ASRPRO_UART_Init(void)
{
	// 采用模拟串口, Tx为PB7, Rx为PA4
	//SoftUart_Init(2, GPIOB, GPIO_Pin_7, GPIOA, GPIO_Pin_4);
	
	// 开始监听串口数据
	//SoftUartEnableRx(2);
}

void ASRPRO_Init(void)
{
	//ASRPRO_UART_Init();
	UART2_Init(115200);
}
