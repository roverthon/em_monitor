/**
  ******************************************************************************
  * @file    lcd_font.h
  * @author  粤嵌温工+FZetc飞贼整理
  * @version V0.0.1
  * @date    2026.04.19
  * @brief   说明：lcd的字库
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
 
#ifndef __LCD_FONT_H		// 定义以防止递归包含
#define __LCD_FONT_H


// 一、其它头文件
#include "stm32f4xx.h"                
#include "my_config.h"
#include "my_lib.h"

// 二、宏定义(函数、变量、常量)
extern const unsigned char asc2_1206[95][12];
extern const unsigned char asc2_1608[95][16];
extern const unsigned char asc2_2412[95][36];
extern const char*         g_font_dot_matrix_32_index[28];
extern const char          g_font_dot_matrix_32[28][128];
	
// 三、自定义的数据类型(结构体、联合体、枚举等)
// 四、全局变量声明

// 五、函数声明
	
// 六、静态变量、静态函数定义

#endif 	/* __LCD_FONT_H */

