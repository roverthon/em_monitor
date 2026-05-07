#include "esp8266_gettime.h"
#include "esp8266.h"
#include "uart.h"
#include "delay.h"
#include <stdio.h>
#include <string.h>

/**
  * @brief  通过ESP266模块获取网络时间
  * @note   时间API来自于(https://quan.suning.com/getSysTime.do)
  * @param  p：指向一个时间结构体变量的指针
  * @retval 成功：返回0
  *         失败：返回非0
  */
//int8_t ESP8266_GetTimeFunc(time_p p)
//{
//	int8_t ret    = 0;
//	int16_t i     = 0;
//	char *str     = NULL;
//	char buf[128] = {0};
//	
////	// 1、与网站建立连接
////	ret =  ESP8266_ConnectServer("TCP", "quan.suning.com", 80);
//	// === 修改点1：使用 LinkID 1 ===
//    ret = ESP8266_ConnectServer_LinkID(1, "TCP", "quan.suning.com", 80);
//	if(ret < 0)
//	{
//		printf("与网站建立连接失败\r\n");
//		return -1;
//	}
//	printf("01：与网站建立连接成功!\r\n");
//	delay_ms(500);
//	
//	
//	// 2、开启透传模式
//	ret =  ESP8266_EnterTransparentTransmission();
//	if(ret < 0)
//	{
//		printf("开启透传模式失败\r\n");
//		return -2;
//	}
//	printf("02：开启透传模式成功!\r\n");
//	delay_ms(500);

//	
//	// 3、向网站提出请求
//	ESP8266_SendATCmd("GET \"https://quan.suning.com/getSysTime.do\"");
//	delay_ms(500);

//	
//	// 4、暴力解析抓取到的时间
//	// a、截取到date:的片段
//	// printf("u3_recvbuf == %s\r\n", u3_recvbuf);
//	
//	str = strstr((const char*)u3_recvbuf, (const char*)"Date:");
//	str = str+6;
//	// printf("str == %s\r\n", str);
//	
//	// b、将date: 后面到GMT之前的字符串放置到buf数组中
//	while(*str++ != '\r')
//	{
//		buf[i++] = *(str-1);
//	}
//	buf[i] = '\0';
//	// printf("buf == %s\r\n", buf);
//	
//	// c、将buf中有','和':'去除，改成' '，为了方便sscanf函数的处理
//	i = 0;
//	while(buf[i++] != '\0')
//	{
//		if( (buf[i] == ',') || (buf[i] == ':'))
//		{
//			buf[i] = ' ';
//		}
//	}
//	// printf("buf == %s\r\n", buf);
//	
//	// d、使用sscanf函数将各个参数放置到时间结构体里面
//	sscanf(buf, "%s %s %s %s %s %s %s %s", 
//								p->weekday,
//								p->date,
//								p->month,
//								p->year,
//								p->hour,
//								p->min,
//								p->second,
//								p->GMT
//	       );
//	
//	
//	// 清空相关数据
//	MY_LIB_ClearArray((int8_t*)u3_recvbuf, RX3_BUF_SIZE);
//	u3_count = 0;
//	u3_len   = 0;
//	u3_flag  = 0;
//	delay_ms(500);
//		
//	// 5、退出透传模式(断开连接)
//	ret = ESP8266_ExitTransparentTransmission();
//	if(ret < 0)
//	{
//		printf("退出透传模式(断开连接)失败\r\n");
//		return -1;
//	}
//	printf("03：退出透传模式(断开连接)成功!\r\n");
//	delay_ms(500);

//	
//	return 0;
//}


/**
  * @brief  通过ESP266模块获取网络时间（多连接修复版）
  * @note   时间API来自于(https://quan.suning.com/getSysTime.do)
  * @param  p：指向一个时间结构体变量的指针
  * @retval 成功：返回0
  *         失败：返回非0
  */
int8_t ESP8266_GetTimeFunc(time_p p)
{
	int8_t ret    = 0;
	int16_t i     = 0;
	char *str     = NULL;
	char buf[128] = {0};
	
	// 1、与网站建立连接（使用多连接 LinkID 3，不冲突）
	ret = ESP8266_ConnectServer_LinkID(3, "TCP", "quan.suning.com", 80);
	if(ret < 0)
	{
		printf("与网站建立连接失败\r\n");
		return -1;
	}
	printf("01：与网站建立连接成功!\r\n");
	delay_ms(800);
	
	// ===================== 修复1：多连接 禁止透传！=====================
	// 直接删除所有透传代码！！！
	
	// 2、向网站提出请求（多连接必须用 LinkID 发送）
	ESP8266_SendMsg_LinkID(3, "GET /getSysTime.do HTTP/1.1\r\nHost: quan.suning.com\r\nConnection: close\r\n\r\n");
	delay_ms(1000);

	// 4、暴力解析抓取到的时间
	str = strstr((const char*)u3_recvbuf, (const char*)"Date:");
	if(str != NULL)
	{
		str = str+6;
		i = 0;
		while(*str++ != '\r' && i < 127)
		{
			buf[i++] = *(str-1);
		}
		buf[i] = '\0';
		
		// 格式化字符串
		i = 0;
		while(buf[i] != '\0')
		{
			if( (buf[i] == ',') || (buf[i] == ':') )
			{
				buf[i] = ' ';
			}
			i++;
		}
		
		// 解析时间
		sscanf(buf, "%s %s %s %s %s %s %s %s", 
					p->weekday,
					p->date,
					p->month,
					p->year,
					p->hour,
					p->min,
					p->second,
					p->GMT
		);
	}

	// 清空缓存
	MY_LIB_ClearArray((int8_t*)u3_recvbuf, RX3_BUF_SIZE);
	u3_count = 0;
	u3_len   = 0;
	u3_flag  = 0;
	delay_ms(500);
		
	// ===================== 修复2：关闭当前 LinkID 连接 =====================
	ESP8266_SendATCmd("AT+CIPCLOSE=3\r\n");
	delay_ms(800);

	return 0;
}
