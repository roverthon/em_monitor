/**
  ******************************************************************************
  * @file    my_lib.h
  * @author  RoverTyphon
  * @version V0.0.1
  * @date    2026.03.20
  * @brief   说明：
  *				lcd相关参数、变量、函数功能等
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
 
#ifndef __DHT11_H		// 定义以防止递归包含
#define __DHT11_H


// 一、其它头文件
#include "stm32f4xx.h"                
#include "my_config.h"
#include "my_lib.h"

// 二、宏定义(函数、变量、常量)
#define DHT11_OUT PGout(9)
#define DHT11_IN  PGin(9)

// 三、自定义的数据类型(结构体、联合体、枚举等)

// 四、全局变量声明
	
// 五、函数声明
extern void   DHT11_Init(void);
extern int8_t DHT11_SelectMode(GPIOMode_TypeDef GPIO_Mode);
extern int8_t DHT11_GetData(uint8_t pbuf[5]);

// 六、静态变量、静态函数定义

#endif 	/* dht11 */
