/**
  ******************************************************************************
  * @file    esp8266_connect_my_server.h
  * @author  FZetc飞贼
  * @version V0.0.1
  * @date    2026.01.16
  * @brief   1、连接自己的服务器(ubuntu系统上)
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

#ifndef __ESP8266_CONNECT_MY_SERVER_H	
#define __ESP8266_CONNECT_MY_SERVER_H

// 一、其它头文件
#include "stm32f4xx.h" 
#include "my_config.h"
#include "my_lib.h"
#include "my_reg.h"

// 二、宏定义(函数、变量、常量)
// 你的ubuntu系统服务器的协议模式、ip地址、端口号
#define SOCKET_MODE "TCP"
#define SOCKET_IP	"112.124.109.25"
#define SOCKET_PORT  50000

// 三、自定义的数据类型(结构体、联合体、枚举等)

// 四、全局变量声明

// 五、函数声明
extern int8_t ESP8266_ConnectMyServer_Func(const char* reg_msg);

// 六、静态变量、函数定义

#endif /* __ESP8266_CONNECT_MY_SERVER_H */

