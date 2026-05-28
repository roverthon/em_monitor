/**
  ******************************************************************************
  * @file    esp8266_gettime.h
  * @author  RoverTyphon
  * @version V0.0.1
  * @date    2026.04.08
  * @brief   说明：
  *				1、通过esp8266模块联网+http协议获取网络时间
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
 
#ifndef __ESP8266_GETTIME_H		// 定义以防止递归包含
#define __ESP8266_GETTIME_H


// 一、其它头文件
#include "stm32f4xx.h"                
#include "my_config.h"
#include "my_lib.h"

// 二、宏定义(函数、变量、常量)

// 三、自定义的数据类型(结构体、联合体、枚举等)
typedef struct
{
	// 年月日、星期
	char year[16];
	char month[16];
	char date[16];
	char weekday[16];
	
	// 时分秒
	char hour[16];
	char min[16];
	char second[16];
	
	// 世界时间(格林威治时间：伦敦时间)
	char GMT[16];

}time_t, *time_p;


// 四、全局变量声明

// 五、函数声明
extern int8_t ESP8266_GetTimeFunc(time_p p);
	
			
		
// 六、静态变量、静态函数定义

#endif 	/* __ESP8266_GETTIME_H */
