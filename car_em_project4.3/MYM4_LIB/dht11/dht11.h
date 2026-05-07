/**
  ******************************************************************************
  * @file    dht11.h
  * @author  FZetc飞贼
  * @version V0.0.1
  * @date    2026.03.30
  * @brief   说明：
  *				1、初始化dht11温湿度模块
  *				2、获取ht11温湿度测到的数据
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
