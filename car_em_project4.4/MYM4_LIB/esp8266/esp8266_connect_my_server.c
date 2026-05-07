#include "esp8266.h"
#include "esp8266_connect_my_server.h"
#include "delay.h"
#include <stdio.h>

/**
  * @brief  通过ESP8266模块来来连接自己ubuntu系统上部署的中转服务器
  * @note   None
  * @param  reg_msg：注册信息
  * @retval 成功：返回0
  *         失败：返回非0
  */
int8_t ESP8266_ConnectMyServer_Func(const char* reg_msg)
{
	char buf[32] = {0};
	int8_t ret = 0;
	
	// 1、与ubuntu系统上的服务器建立连接
	ret = ESP8266_ConnectServer(SOCKET_MODE, SOCKET_IP, SOCKET_PORT);
	if(ret < 0)
	{
		printf("*****连接ubuntu系统上的服务器失败!*****\r\n");
		return -1;
	}
	printf("01：连接ubuntu系统上的服务器成功!\r\n");
	delay_ms(500);
	
	// 2、开启透传模式
	ret = ESP8266_EnterTransparentTransmission();
	if(ret < 0)
	{
		printf("*****开启透传模式失败!*****\r\n");
		return -1;
	}
	printf("02：开启透传模式成功!\r\n");
	delay_ms(500);
	
	// 3、向服务器发出请求(注册)
	sprintf(buf, "#reg %s", reg_msg);
	ESP8266_SendMsg(buf);
	delay_ms(500);
	
	return 0;
		
}
