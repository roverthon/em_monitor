#include "esp8266.h"
#include "delay.h"
#include "uart.h"
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"

// 全局变量定义
int esp8266_transparent_transmission_sta = 0;	// 退出透传模式的状态标志位

// ================================================一、ESP8266模块的功能使用================================================//

/**
  * @brief  通过ESP8266模块来上网
  * @note   None
  * @param  None
  * @retval 成功：返回0
  *         失败：返回非0
  */
int8_t ESP8266_OnLineInit(void)
{
	int8_t ret = 0;
    // 1. 复位模块，确保干净状态
    if(ESP8266_Rst() != 0) {
        printf("ESP8266 复位失败\r\n");
        return -1;
    }
    delay_ms(1000); // 给模块足够时间启动

	// 1、退出透传模式，才能输入AT指令给ESP8266模块(非必要，但要确保)
	ret = ESP8266_ExitTransparentTransmission();
	if(ret < 0)
	{
		printf("*****进入非透传模式失败!*****\r\n");
		return -1;
	}
	printf("01：进入非透传模式成功!\r\n");
	delay_ms(1000);
	
	// 2、关闭回显(非必要)
	ret =  ESP8266_EnEcho(0);
	if(ret < 0)
	{
		printf("*****关闭回显失败!*****\r\n");
		return -1;
	}
	printf("02：关闭回显成功!\r\n");
    // 4. 设置为 Station 模式
    ESP8266_SendATCmd("AT+CWMODE=1\r\n");
    if(ESP8266_CheckStrInRxbuf("OK", 3000) == -1) {
        printf("设置 Station 模式失败\r\n");
        //return -1;
    }
    delay_ms(500);

	// 新增：开启多连接模式（非常重要！）
    int8_t retry = 3; // 重试3次
    while(retry--)
    {
        ESP8266_SendATCmd("AT+CIPMUX=1\r\n");
        if(ESP8266_CheckStrInRxbuf("OK", 2000) == 0) {
            printf("开启多连接成功！\r\n");
            break;
        }
        delay_ms(500);
    }
    if(retry < 0) {
        printf("开启多连接失败！\r\n");
        return -1;
    }
    delay_ms(500);
	// 3、连接热点
	ret =  ESP8266_ConnectAp(SSID, PSWD);
	if(ret < 0)
	{
		printf("*****连接热点失败!*****\r\n");
		return -1;
	}
	printf("03：连接热点成功!\r\n");
	delay_ms(1000);
   esp8266_transparent_transmission_sta = 0;
	return 0;
}

// =========================================二、ESP8266模块的基本函数和基础指令函数=========================================//
/**
  * @brief  初始化ESP8266的wifi模块
  * @note   这个ESP8266模块现在连接在GEC-M4开发板的UART3
  *         GEC-M4开发板                                ESP8266模块
  *         ESP8266-》TXD3_WIFI-》USART3_TX-》PB10      RX
  *         ESP8266-》RXD3_WIFI-》USART3_RX-》PB11      TX
  * @param  None
  * @retval None
  */
void ESP8266_Init(uint32_t baudrate)
{
	UART3_Init(baudrate);
}

/**
  * @brief  发送AT指令给ESP8266模块
  * @note   None
  * @param  cmd_str：ESP8266模块的AT指令
  * @retval None
  */
void ESP8266_SendATCmd(const char* cmd_str)
{
	// 1、清空相关数据
	MY_LIB_ClearArray((int8_t*)u3_recvbuf, RX3_BUF_SIZE);
	u3_count = 0;
	u3_len   = 0;
	u3_flag  = 0;

	// 2、串口3发送字符串数据
	UART3_SendStr(cmd_str);
}

///**
//  * @brief  发送信息给服务器
//  * @note   None
//  * @param  msg_str：要发送的信息
//  * @retval None
//  */
//void ESP8266_SendMsg(const char* msg_str)
//{
//	// 1、清空相关数据
//	MY_LIB_ClearArray((int8_t*)u3_recvbuf, RX3_BUF_SIZE);
//	u3_count = 0;
//	u3_len   = 0;
//	u3_flag  = 0;

//	// 2、串口3发送字符串数据
//	UART3_SendStr(msg_str);
//}
/**
  * @brief  多连接模式下 按ID发送数据（你现在必须用这个）
  * @param  link_id: 0~4
  * @param  msg_str: 要发送的字符串
  * @retval None
  */
void ESP8266_SendMsg_LinkID(uint8_t link_id, const char* msg_str)
{
    char cmd[64];
    uint16_t len = strlen(msg_str);

    // 清空缓冲区
    MY_LIB_ClearArray((int8_t*)u3_recvbuf, RX3_BUF_SIZE);
    u3_count = 0;
    u3_len   = 0;
    u3_flag  = 0;

    // 多连接必须先发这条指令！！！
    sprintf(cmd, "AT+CIPSEND=%d,%d\r\n", link_id, len);
    UART3_SendStr(cmd);

    // 等待模块返回 >
    delay_ms(100);
    if(ESP8266_CheckStrInRxbuf(">", 1000) == 0)
    {
        // 收到 > 才能发真正的数据
        UART3_SendStr(msg_str);
    }
}

/**
  * @brief  检查接收的数据包里面的字符串
  * @note   还有超时时间计算
  * @param  str：    接收字符串里面的特殊字符串标志(wifi模块返回的数据)
  *         timeout：超时时间
  * @retval 成功：返回0
  *         失败：返回非0	
  */
//int8_t ESP8266_CheckStrInRxbuf(char* str, int32_t timeout)     // "OK", 5000
//{
//	char *dest = str;
//	char *src  = (char*)u3_recvbuf; 						   // 回发：OK 0,CONNECT
//	
//	// 等待串口接收完毕或超时
//	while( (strstr(src, dest) == NULL) && (timeout>0) )
//	{
//		delay_ms(1);
//		timeout--;
//	}
//	
//	if(timeout>0)
//		return 0;

//	return -1;
//}
int8_t ESP8266_CheckStrInRxbuf(char* str, int32_t timeout)
{
    char *dest = str;

    // 等待串口接收完毕或超时
    while( (strstr( (char*)u3_recvbuf, dest ) == NULL) && (timeout>0) )
    {
        //delay_ms(1);
			delay_ms(1);
			//vTaskDelay(1);  
        timeout--;
    }

    if(timeout>0)
        return 0;

    return -1;
}

/**
  * @brief  模块复位
  * @note   None
  * @param  None
  * @retval 成功：返回0
  *         失败：返回非0
  */
int8_t ESP8266_Rst(void)
{
	ESP8266_SendATCmd("AT+RST\r\n");
	if(ESP8266_CheckStrInRxbuf("OK", 5000) == -1)
		return -1;
		delay_ms(1000);
	return 0 ;	
}


///**
//  * @brief  退出透传模式(断开连接)
//  * @note   None
//  * @param  None
//  * @retval 成功：返回0
//  */
//int8_t ESP8266_ExitTransparentTransmission(void)
//{
//	// 1、退出透传模式
//	ESP8266_SendATCmd("+++");
//	delay_ms(500);
//	
//	// 2、关闭透传模式设置
//    ESP8266_SendATCmd("AT+CIPMODE=0\r\n");
//    if(ESP8266_CheckStrInRxbuf("OK", 1000) == -1)
//        return -1;
//	delay_ms(500);
//	
//	// 3、复位
//	ESP8266_SendATCmd("AT+RST\r\n");
//	if(ESP8266_CheckStrInRxbuf("OK", 5000) == -1)
//		return -1;
//	delay_ms(500);

//	// 4、记录当前ESP8266模块工作在非透传模式下
//	esp8266_transparent_transmission_sta = 0;
//	
//	return 0;
//}


///**
//  * @brief  进入透传模式
//  * @note   None
//  * @param  None
//  * @retval 成功：返回0
//  *         失败：返回非0
//  */
//int8_t ESP8266_EnterTransparentTransmission(void)
//{
//	// 1、进入透传模式
//	ESP8266_SendATCmd("AT+CIPMODE=1\r\n");
//	if(ESP8266_CheckStrInRxbuf("OK", 5000) == -1)
//		return -1;
//	delay_ms(500);

//	
//	// 2、开启发送模式
//	ESP8266_SendATCmd("AT+CIPSEND\r\n");
//	if(ESP8266_CheckStrInRxbuf("OK", 5000) == -1)
//		return -3;
//	delay_ms(500);
//	
//	// 3、记录当前ESP8266模块工作在透传模式下
//	esp8266_transparent_transmission_sta = 1;

//	return 0;
//}
/**
  * @brief  退出透传模式（多连接模式下简化版，不复位）
  */
int8_t ESP8266_ExitTransparentTransmission(void)
{
	// 多连接模式下不能发 +++，不能复位！
	// 只需要关闭连接 + 关闭透传标志
    
//	// 1. 关闭所有网络连接（防止干扰指令）
//	ESP8266_SendATCmd("AT+CIPCLOSE\r\n");
//	ESP8266_CheckStrInRxbuf("OK", 2000);
//	delay_ms(300);

	// 2. 强制关闭透传模式
	ESP8266_SendATCmd("AT+CIPMODE=0\r\n");
	ESP8266_CheckStrInRxbuf("OK", 1000);
	delay_ms(300);

	// 3. 标记非透传
	esp8266_transparent_transmission_sta = 0;
	
	return 0;
}

/**
  * @brief  进入透传模式（多连接模式下自动关闭多连接，保证兼容）
  */
int8_t ESP8266_EnterTransparentTransmission(void)
{
	// 重点：透传必须关闭多连接！
	ESP8266_SendATCmd("AT+CIPMUX=0\r\n");
	delay_ms(300);

	// 1、进入透传模式
	ESP8266_SendATCmd("AT+CIPMODE=1\r\n");
	if(ESP8266_CheckStrInRxbuf("OK", 3000) == -1)
		return -1;
	delay_ms(300);

	// 2、开启发送
	ESP8266_SendATCmd("AT+CIPSEND\r\n");
	if(ESP8266_CheckStrInRxbuf(">", 3000) == -1)
		return -3;
	delay_ms(300);
	
	// 3、标记透传状态
	esp8266_transparent_transmission_sta = 1;

	return 0;
}

/**
  * @brief  回显打开或关闭
  * @note   None
  * @param  flag：开关模块的回显信息
  * @retval 成功：返回0
  *         失败：返回非0
  */
int8_t ESP8266_EnEcho(int8_t flag)
{
	if(flag == 1)
		ESP8266_SendATCmd("ATE1\r\n");			// 开启回显
	else
		ESP8266_SendATCmd("ATE0\r\n");			// 关闭回显
	
	if(ESP8266_CheckStrInRxbuf("OK", 5000) == -1)
		return -1;
	delay_ms(100);
	return 0 ;	 
}




/**
  * @brief  连接热点
  * @note   None
  * @param  ssid：热点的名字
  *         pswd：热点的密码
  * @retval 成功：返回0
  *         失败：返回非0
  */
//int8_t ESP8266_ConnectAp(char *ssid, char *pswd)
//{
//	// 1、设置STATION模式(这个模式可以上网，功耗又不会太高)
//	ESP8266_SendATCmd("AT+CWMODE=1\r\n");
//	if(ESP8266_CheckStrInRxbuf("OK", 5000) == -1)
//		return -1;	
//	
//	// 2、复位
//	ESP8266_SendATCmd("AT+RST\r\n");
//	if(ESP8266_CheckStrInRxbuf("OK", 5000) == -1)
//		return -2;
//	
//	
//	// 3、连接目标热点
//	ESP8266_SendATCmd("AT+CWJAP=");
//	ESP8266_SendATCmd("\""); ESP8266_SendATCmd(ssid); 	ESP8266_SendATCmd("\"");	// ssid
//	ESP8266_SendATCmd(","); 
//	ESP8266_SendATCmd("\""); ESP8266_SendATCmd(pswd); 	ESP8266_SendATCmd("\"");	// pswd
//	ESP8266_SendATCmd("\r\n");
//	
//	
//	if(ESP8266_CheckStrInRxbuf("WIFI GOT IP", 5000) == -1)
//	{
//		return -1;	
//	}
//	
//	return 0;	
//}
int8_t ESP8266_ConnectAp(char *ssid, char *pswd)
{
	char cmd[64];
	
	// 1、设置STATION模式
	ESP8266_SendATCmd("AT+CWMODE=1\r\n");
	if(ESP8266_CheckStrInRxbuf("OK", 3000) == -1)
		return -1;
	delay_ms(500);

	// 2、一次性拼接指令，稳定不炸
	sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pswd);
	ESP8266_SendATCmd(cmd);
	
	// 3、等待获取IP
	if(ESP8266_CheckStrInRxbuf("WIFI GOT IP", 10000) == -1)
	{
		return -1;	
	}
	
	// 连接成功后延时，让网络稳定
	delay_ms(1000);
	return 0;	
}
/**
  * @brief  连接服务器
  * @note   可以指定协议进行连接(TCP/UDP)
  * @param  mode：协议类型(TCP/UDP)
  *         ip：  目标服务器的IP地址
  *         port：目标服务器的端口号   // 推荐, 平常做实验可用的端口号范围(49151-65535)
  * @retval 成功：返回0
  *         失败：返回非0
  */
//int8_t ESP8266_ConnectServer(char *mode, char *ip, uint16_t port)
//{
//	char port_buf[16] = {0};
//	// 连接服务器
//	ESP8266_SendATCmd("AT+CIPSTART=");

//	ESP8266_SendATCmd("\""); ESP8266_SendATCmd(mode); 	ESP8266_SendATCmd("\"");	// mode
//	ESP8266_SendATCmd(","); 
//	ESP8266_SendATCmd("\""); ESP8266_SendATCmd(ip); 	ESP8266_SendATCmd("\"");	// ip
//	ESP8266_SendATCmd(",");
//	sprintf(port_buf,"%d", port);													// port
//	ESP8266_SendATCmd(port_buf);
//	ESP8266_SendATCmd("\r\n");
//	
//	if(ESP8266_CheckStrInRxbuf("CONNECT", 5000) == -1)
//	{
//		if(ESP8266_CheckStrInRxbuf("OK", 5000) == -1)
//		{
//			return -1;	
//		}
//	}
//	
//	return 0;		
//}

///**
//  * @brief  连接服务器
//  * @note   可以指定协议进行连接(TCP/UDP)
//  * @param  mode：协议类型(TCP/UDP)
//  *         ip：  目标服务器的IP地址
//  *         port：目标服务器的端口号   // 推荐, 平常做实验可用的端口号范围(49151-65535)
//  * @retval 成功：返回0
//  *         失败：返回非0
//  */
//int8_t ESP8266_ConnectServer_serve(char *mode, char *ip, uint16_t port)
//{
//	char port_buf[16] = {0};
//	// 连接服务器
//	ESP8266_SendATCmd("AT+CIPSTART=");

//	ESP8266_SendATCmd("\""); ESP8266_SendATCmd(mode); 	ESP8266_SendATCmd("\"");	// mode
//	ESP8266_SendATCmd(","); 
//	ESP8266_SendATCmd("\""); ESP8266_SendATCmd(ip); 	ESP8266_SendATCmd("\"");	// ip
//	ESP8266_SendATCmd(",");
//	sprintf(port_buf,"%d", port);													// port
//	ESP8266_SendATCmd(port_buf);
//	ESP8266_SendATCmd("\r\n");
//	
//	if(ESP8266_CheckStrInRxbuf("CONNECT", 5000) == -1)
//	{
//		if(ESP8266_CheckStrInRxbuf("OK", 5000) == -1)
//		{
//			return -1;	
//		}
//	}
//	
//	return 0;		
//}


/**
  * @brief  连接服务器（支持多连接，指定Link ID）
  * @param  link_id：0~4
  * @param  mode：TCP/UDP
  * @param  ip
  * @param  port
  */
int8_t ESP8266_ConnectServer_LinkID(uint8_t link_id, char *mode, char *ip, uint16_t port)
{
	char cmd[128] = {0};
    int8_t ret = -1;
    uint8_t retry = 2;

    // 确保非透传 + 多连接已开启
    ESP8266_ExitTransparentTransmission();
    ESP8266_SendATCmd("AT+CIPMUX=1\r\n");
    delay_ms(300);

//    // 关闭旧连接，防止冲突
//    ESP8266_SendATCmd("AT+CIPCLOSE\r\n");
//    delay_ms(500);
	    // 确保非透传
    ESP8266_SendATCmd("AT+CIPMODE=0\r\n");
    delay_ms(100);
		    // 确保多连接开启
    ESP8266_SendATCmd("AT+CIPMUX=1\r\n");
    delay_ms(100);
    while(retry--)
    {
        sprintf(cmd, "AT+CIPSTART=%d,\"%s\",\"%s\",%d\r\n", 
                link_id, mode, ip, port);
        
        ESP8266_SendATCmd(cmd);

        if(ESP8266_CheckStrInRxbuf("CONNECT", 3000) == 0 ||
           ESP8266_CheckStrInRxbuf("OK", 2500) == 0 ||
           ESP8266_CheckStrInRxbuf("ALREADY", 2000) == 0)
        {
            printf("LinkID=%d 连接成功\r\n", link_id);
            return 0;
        }

        delay_ms(800);
    }

    printf("LinkID=%d 连接失败\r\n", link_id);
    return -1;
}

