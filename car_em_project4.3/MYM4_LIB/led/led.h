/**
  ******************************************************************************
  * @file    led.h
  * @author  FZetc飞贼
  * @version V0.0.1
  * @date    2026.03.18
  * @brief   说明：
  *				1、初始化led模块
  *				2、指定led模块的亮和灭
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
 
#ifndef __LED_H		// 定义以防止递归包含
#define __LED_H


// 一、其它头文件
#include "stm32f4xx.h"                
#include "my_config.h"
#include "my_lib.h"

// 二、宏定义(函数、变量、常量)
#define LED1(X) (X)? (PFout(9)=0):  (PFout(9) =1)											
#define LED2(X) (X)? (PFout(10)=0): (PFout(10)=1)	
#define LED3(X) (X)? (PEout(13)=0): (PEout(13)=1)	
#define LED4(X) (X)? (PEout(14)=0): (PEout(14)=1)	

// 三、自定义的数据类型(结构体、联合体、枚举等)

// 四、全局变量声明

// 五、函数声明
extern void LED_Init(void);
extern void LED_ON(uint8_t led_num);
extern void LED_OFF(uint8_t led_num);
extern void LED_WaterRunning(uint16_t speed, uint8_t direction);
extern void LED_HorseRunning(uint16_t speed, uint8_t direction);
extern void LED_ShowBin(int8_t num);

	
// 六、静态变量、静态函数定义

#endif 	/* __LED_H */
