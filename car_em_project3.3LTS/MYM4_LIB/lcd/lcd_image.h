/**
  ******************************************************************************
  * @file    lcd_image.h
  * @author  粤嵌温工+FZetc飞贼整理
  * @version V0.0.1
  * @date    2026.04.19
  * @brief   说明：lcd的图库
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
 
#ifndef __LCD_IMAGE_H		// 定义以防止递归包含
#define __LCD_IMAGE_H


// 一、其它头文件
#include "stm32f4xx.h"                
#include "my_config.h"
#include "my_lib.h"

// 二、宏定义(函数、变量、常量)
	
// 三、自定义的数据类型(结构体、联合体、枚举等)
// 图像信息结构体
typedef struct 
{
    const char* name;              // 图像名字，方便索引指定图像数据
    const unsigned char* address;  // 图像数组入口地址
    unsigned int width;            // 图像宽度
    unsigned int height;           // 图像高度
    unsigned int size;             // 图像数组大小
} image_info_t;


// 四、全局变量声明
extern const unsigned char g_image_hezhao_320x240[153600];

// 五、函数声明
	
// 六、静态变量、静态函数定义
static const image_info_t g_image_tbl[3] = {
    {"hezhao", g_image_hezhao_320x240, 320, 240, 153600 },
};

#endif 	/* __LCD_IMAGE_H */

