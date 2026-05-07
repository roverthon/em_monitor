/**
  ******************************************************************************
  * @file    uart.h
  * @author  FZetc飞贼
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
  * 本文档只供学习使用，不得商用，违者必究
  *
  * 微信公众号：    FZetc飞贼
  * 全视频平台：    FZetc飞贼
  * CSDN博客：      https://blog.csdn.net/qq_58629108?type=blog
  * 有疑问或者建议：FZetcSnitch@163.com
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

// 三、自定义的数据类型(结构体、联合体、枚举等)

// 四、全局变量声明

// 五、函数声明
extern void UART1_Init(uint32_t baudrate);
extern void UART1_SendStr(const char* str);
extern void USART1_IRQHandler(void);

// 六、静态变量、静态函数定义

#endif 	/* __UART_H */
