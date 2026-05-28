/**
  ******************************************************************************
  * @file    esp8266_getweather.h
  * @author  RoverTyphon
  * @version V0.0.1
  * @date    2026.01.15
  * @brief   1、通过esp8266模块获取天气
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
 

//#endif /* __ESP8266_GETWEATHER_H */
#ifndef __ESP8266_GETWEATHER_H
#define __ESP8266_GETWEATHER_H

// 其它头文件
#include "stm32f4xx.h"                
#include "my_config.h"
#include "my_lib.h"

//  Location结构体（城市信息）
typedef struct
{
	char name[24];
}Location;
 
// Now结构体（天气信息）
typedef struct
{
	char text[24];
	char code[4];
}Now;

// 总结果结构体
typedef struct
{
	Location location;
	Now now;
}Results;

// 心知天气KEY
#define XZTQ_KEY "SfFn8hMTTl6140omb"

// 函数声明（暴力解析替换CJSON解析）
extern int8_t  weather_brute_parse(char *sbuf, Results *results);
extern int32_t ESP8266_GetWeatherFunc(char *xztq_key, char *city_buf);

#endif