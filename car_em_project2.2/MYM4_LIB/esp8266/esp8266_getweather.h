/**
  ******************************************************************************
  * @file    esp8266_getweather.h
  * @author  FZetc飞贼
  * @version V0.0.1
  * @date    2026.01.15
  * @brief   1、通过esp8266模块获取天气
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
  *	
  */
  
#ifndef __ESP8266_GETWEATHER_H
#define __ESP8266_GETWEATHER_H


// 一、其它头文件
#include "stm32f4xx.h"                
#include "my_config.h"
#include "my_lib.h"

// 二、自定义的数据类型(结构体、联合体、枚举等)
//子对象1结构体 Location
typedef struct
{
	char id[32];
	char name[32];
	char country[32];
	char path[64];
	char timezone[32];
	char timezone_offset[32];
}Location;
 
//子对象2结构体 Now`
typedef struct
{
	char text[32];
	char code[32];
	char temperature[32];
}Now;

typedef struct
{
	Location location;		//子对象1
	Now now;				//子对象2
	char last_update[64];	//子对象3
}Results;

// 三、宏定义(函数、变量、常量)
#define XZTQ_KEY "PRSiSkr9eZH-ZFSUS"

// 四、变量声明

// 五、函数声明
extern int8_t  weather_cjson_parse(char *sbuf, Results *results);
extern int32_t ESP8266_GetWeatherFunc(char *xztq_key, char *city_buf);


#endif /* __ESP8266_GETWEATHER_H */
