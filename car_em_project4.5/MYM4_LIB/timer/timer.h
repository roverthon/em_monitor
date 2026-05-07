/**
  ******************************************************************************
  * @file    timer.h
  * @author  FZetc飞贼
  * @version V0.0.1
  * @date    2026.03.27
  * @brief   说明：
  *				1、外设定时器初始化
  *				2、外设定时器中断服务函数
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
 
#ifndef __TIMER_H		// 定义以防止递归包含
#define __TIMER_H


// 一、其它头文件
#include "stm32f4xx.h"                
#include "my_config.h"
#include "my_lib.h"

// 二、宏定义(函数、变量、常量)

// 三、自定义的数据类型(结构体、联合体、枚举等)

// 四、全局变量声明

// 五、函数声明
extern void TIMER3_Init(void);
extern void TIMER14_Ch1_Init(void);
extern void TIMER13_Ch1_Init(void);
extern void TIMER13_SetFreq(uint16_t freq);
extern void TIMER13_SetDuty(uint16_t duty);

extern void TIM3_IRQHandler(void);



// 六、静态变量、静态函数定义

#endif 	/* __TIMER_H */
