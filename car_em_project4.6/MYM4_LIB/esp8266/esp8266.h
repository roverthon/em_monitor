/**
  ******************************************************************************
  * @file    esp8266.h
  * @author  FZetc飞贼
  * @version V0.0.1
  * @date    2026.04.08
  * @brief   说明：
  *				1、初始化esp8266的wifi模块
  *				2、单片机和ESP8266模块通信
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
 
#ifndef __ESP8266_H		// 定义以防止递归包含
#define __ESP8266_H


// 一、其它头文件
#include "stm32f4xx.h"                
#include "my_config.h"
#include "my_lib.h"


// 二、宏定义(函数、变量、常量)
#define SSID "1234561"
#define PSWD "one23456one"

// 三、自定义的数据类型(结构体、联合体、枚举等)

// 四、全局变量声明

// 五、函数声明
extern int8_t ESP8266_OnLineInit(void);
	
extern void   ESP8266_Init(uint32_t baudrate);
extern void   ESP8266_SendATCmd(const char* cmd);
extern int8_t ESP8266_CheckStrInRxbuf(char* str, int32_t timeout);	
extern int8_t ESP8266_Rst(void);
extern int8_t ESP8266_ExitTransparentTransmission(void);
extern int8_t ESP8266_EnterTransparentTransmission(void);	
extern int8_t ESP8266_EnEcho(int8_t flag);
extern int8_t ESP8266_ConnectAp(char *ssid, char *pswd);
//extern int8_t ESP8266_ConnectServer(char *mode, char *ip, uint16_t port);			
//extern int8_t ESP8266_ConnectServer_serve(char *mode, char *ip, uint16_t port);
extern int8_t ESP8266_CheckStrInRxbuf(char* str, int32_t timeout);
//extern void ESP8266_SendMsg(const char* msg_str);
//extern int8_t ESP8266_ConnectServer_LinkID(uint8_t link_id, char *mode, char *ip, uint16_t port);
// 多连接模式 唯一保留的服务器连接函数
extern int8_t ESP8266_ConnectServer_LinkID(uint8_t link_id, char *mode, char *ip, uint16_t port);

// 多连接模式 必须使用的带ID发送函数
extern void ESP8266_SendMsg_LinkID(uint8_t link_id, const char* msg_str);
// 六、静态变量、静态函数定义

#endif 	/* __ESP8266_H */
