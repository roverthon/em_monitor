/**
  ******************************************************************************
  * @file    esp8266_bemfayun_tcp.h
  * @author  RoverTyphon
  * @version V0.0.1
  * @date    2026.01.16
  * @brief   1、通过esp8266模块和巴法云相互通信
  *          
  ******************************************************************************
  * @attention
  *
  * 本文档只供学习使用，不得商用，违者必究
  *
  *
  ******************************************************************************
  *	
  */
  
#ifndef __ESP8266_BEMFAYUN_TCP_H
#define __ESP8266_BEMFAYUN_TCP_H


// 一、其它头文件
#include "stm32f4xx.h"   		// Device header
#include "my_config.h"
#include "my_lib.h"
#include "my_reg.h"

#define BEMFAYUN_LINK_ID  0

// 二、自定义的数据类型(结构体、联合体、枚举等)
//外部变量
extern  uint32_t bemfa_last_heartbeat;

// 三、宏定义(函数、变量、常量)
#define BEMFAYUN_SIYAO "b6351e37efec490cb774e8dd05914b13"

// 四、变量声明

// 五、函数声明
extern int32_t ESP8266_BemFaYun_Tcp_Connect(void);
extern int32_t ESP8266_BemFaYun_Tcp_Subscribe(const char *key, const char *topic);
extern int32_t ESP8266_BemFaYun_Tcp_Publish(const char *key, const char *topic, const char *msg);
extern void BemFaYun_SendHeartbeat(void);
#endif /* __ESP8266_BEMFAYUN_TCP_H */

