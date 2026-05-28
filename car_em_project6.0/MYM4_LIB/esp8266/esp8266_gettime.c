#include "esp8266_gettime.h"
#include "esp8266.h"
#include "uart.h"
#include "delay.h"
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"		// 选编译配置文件，用于汇总所有源文件的编译选择控制
#include "task.h"			// 任务相关函数头文件



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
//		printf("========== sscanf 解析结果 ==========\n");
//		printf("weekday: [%s]\n", p->weekday);
//		printf("date   : [%s]\n", p->date);
//		printf("month  : [%s]\n", p->month);
		printf("year   : [%s]\n", p->year);
		printf("hour   : [%s]\n", p->hour);
//		printf("min    : [%s]\n", p->min);
//		printf("second : [%s]\n", p->second);
//		printf("GMT    : [%s]\n", p->GMT);
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