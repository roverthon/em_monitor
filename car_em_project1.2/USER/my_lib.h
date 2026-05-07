/**
  ******************************************************************************
  * @file    my_lib.h
  * @author  FZetc飞贼
  * @version V0.0.1
  * @date    2026.03.20
  * @brief   说明：
  *				1、比较两个数组的元素是否一致
  *				2、清空数组
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
 
#ifndef __MY_LIB_H		// 定义以防止递归包含
#define __MY_LIB_H


// 一、其它头文件
#include "stm32f4xx.h"                

// 二、宏定义(函数、变量、常量)


// 三、自定义的数据类型(结构体、联合体、枚举等)

// 四、全局变量声明

// 五、函数声明
extern int8_t MY_LIB_CmpArray(int8_t* p1_buf, int8_t* p2_buf, uint16_t len);
extern void   MY_LIB_ClearArray(int8_t* p_buf, uint16_t len);

// 六、静态变量、静态函数定义

#endif 	/* __MY_LIB_H */
