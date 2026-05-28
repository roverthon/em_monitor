/**
  ******************************************************************************
  * @file    uart.h
  * @author  RoverTyphon
  * @version V0.0.2
  * @date    2026.03.27
  * @brief   说明：
  *				1、uart外设初始化
  *				2、uart中断接收数据函数
  *				3、uart发送数据函数
  *
   ******************************************************************************
  * @attention
  *
  * 		本文档只供学习使用，不得商用，违者必究
  *
  *     有疑问或者建议：RoverTyphon@outlook.com 
	*
  ******************************************************************************
  */
 
 
#ifndef __UART_H		// 定义以防止递归包含
#define __UART_H


// 一、其它头文件
#include "stm32f4xx.h"                
#include "my_config.h"
#include "my_lib.h"
#include <stdio.h>

// 二、宏定义(函数、变量、常量)
#define UART2_RUN   0						// UART2接收的数据，回发给UART2上
#define UART2_DEBUG 1						// UART2接收的数据，转发给UART1上
#define UART3_ESP8266_RECV_MSG_DEBUG 0		// UART3接收的数据，转发给UART1上


#define RX1_BUF_SIZE 512
#define TX1_BUF_SIZE 512

#define RX2_BUF_SIZE 512
#define TX2_BUF_SIZE 512

#define RX3_BUF_SIZE 512
#define TX3_BUF_SIZE 512

// 三、自定义的数据类型(结构体、联合体、枚举等)

// 四、全局变量声明
extern volatile uint8_t u1_recvbuf[RX1_BUF_SIZE];
extern volatile uint8_t u1_count;
extern volatile uint8_t u1_flag;

// UART2
extern volatile uint8_t u2_recvbuf[RX2_BUF_SIZE];
extern volatile uint8_t u2_count;
extern volatile uint8_t u2_flag;

// UART3
extern volatile uint8_t  u3_recvbuf[RX3_BUF_SIZE];
extern volatile uint16_t u3_count;
extern volatile uint16_t u3_len;
extern volatile uint8_t  u3_flag;


// 五、函数声明
extern void UART1_Init(uint32_t baudrate);
extern void UART1_SendStr(const char* str);
extern void USART1_IRQHandler(void);

extern void UART2_Init(uint32_t baudrate);
extern void UART2_SendStr(const char* str);
extern void USART2_IRQHandler(void);

extern void UART3_Init(uint32_t baudrate);
extern void UART3_SendStr(const char* str);
extern void USART3_IRQHandler(void);

// 六、静态变量、静态函数定义

#endif 	/* __UART_H */
