/**
  ******************************************************************************
  * @file    delay.h
  * @author  FZetc飞贼
  * @version V0.1.0
  * @date    2025.12.26
  * @brief   1、自己写的粗延时
  *          2、系统定时器的精准延时
  *
  ******************************************************************************
  * @attention
  *
  * 本文档只供学习使用，不得商用，违者必究
  *
  * 微信公众号：    FZetc飞贼
  * 全视频平台：    FZetc飞贼
  * CSDN博客：      https://blog.csdn.net/qq_58629108?type=blog
  * 有疑问或者建议： FZetcSnitch@163.com
  *
  ******************************************************************************
  */

#ifndef __DELAY_H	
#define __DELAY_H

// 一、其它头文件
#include "stm32f4xx.h" 
#include "my_config.h"
#include "my_lib.h"


// 二、宏定义(函数、变量、常量)
 
// 三、自定义的数据类型(结构体、联合体、枚举等)

// 四、全局变量声明

// 五、函数声明
extern void   DELAY_MyNms(uint16_t nms);
extern void   DELAY_SysTickInit(uint8_t sysclk);
extern void   delay_us(uint32_t nus);
extern void   delay_ms(u32 nms);

// 六、静态变量、函数定义

#endif /* __DELAY_H */

