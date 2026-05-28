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
//int8_t ESP8266_OnLineInit(void)
//{
//	int8_t ret = 0;
//    // 1. 复位模块，确保干净状态
//    if(ESP8266_Rst() != 0) {
//        printf("ESP8266 复位失败\r\n");
//        return -1;
//    }
//    delay_ms(1000); // 给模块足够时间启动

//	// 1、退出透传模式，才能输入AT指令给ESP8266模块(非必要，但要确保)
//	ret = ESP8266_ExitTransparentTransmission();
//	if(ret < 0)
//	{
//		printf("*****进入非透传模式失败!*****\r\n");
//		return -1;
//	}
//	printf("01：进入非透传模式成功!\r\n");
//	delay_ms(1000);
//	
//	// 2、关闭回显(非必要)
//	ret =  ESP8266_EnEcho(0);
//	if(ret < 0)
//	{
//		printf("*****关闭回显失败!*****\r\n");
//		return -1;
//	}
//	printf("02：关闭回显成功!\r\n");
//    // 4. 设置为 Station 模式
//    ESP8266_SendATCmd("AT+CWMODE=1\r\n");
//    if(ESP8266_CheckStrInRxbuf("OK", 3000) == -1) {
//        printf("设置 Station 模式失败\r\n");
//        //return -1;
//    }
//    delay_ms(500);

//	// 新增：开启多连接模式（非常重要！）
//    int8_t retry = 3; // 重试3次
//    while(retry--)
//    {
//        ESP8266_SendATCmd("AT+CIPMUX=1\r\n");
//        if(ESP8266_CheckStrInRxbuf("OK", 2000) == 0) {
//            printf("开启多连接成功！\r\n");
//            break;
//        }
//        delay_ms(500);
//    }
//    if(retry < 0) {
//        printf("开启多连接失败！\r\n");
//        return -1;
//    }
//    delay_ms(500);
//	// 3、连接热点
//	ret =  ESP8266_ConnectAp(SSID, PSWD);
//	if(ret < 0)
//	{
//		printf("*****连接热点失败!*****\r\n");
//		return -1;
//	}
//	printf("03：连接热点成功!\r\n");
//	delay_ms(1000);
//   esp8266_transparent_transmission_sta = 0;
//	ESP8266_SendATCmd("AT+CIPMUX=1\r\n");
//delay_ms(300);
//	return 0;
//}
int8_t ESP8266_OnLineInit(void)
{
    int8_t ret = 0;
    
    printf("[Init] ================== ESP8266 初始化开始 ==================\r\n");
    
    // 1. 复位模块
    if(ESP8266_Rst() != 0) {
        printf("[Init] 第一次复位失败，尝试第二次...\r\n");
        //delay_ms(1000);
				vTaskDelay(pdMS_TO_TICKS(1000));
        if(ESP8266_Rst() != 0) {
            printf("[Init] *** 连续两次复位失败！请检查硬件连接 ***\r\n");
            return -1;
        }
    }
    
    //delay_ms(2000);   // 给模块充足启动时间
		vTaskDelay(pdMS_TO_TICKS(1500));
    // 2. 退出透传 + 设置基本参数
    ESP8266_ExitTransparentTransmission();
    //delay_ms(500);
		vTaskDelay(pdMS_TO_TICKS(500));
    // 3. 关闭回显
    ret = ESP8266_EnEcho(0);
    if(ret < 0) printf("[Init] 关闭回显失败\r\n");

    // 4. 设置 Station 模式 + 多连接
    ESP8266_SendATCmd("AT+CWMODE=1\r\n");
    ESP8266_CheckStrInRxbuf("OK", 3000);
   // delay_ms(500);
		vTaskDelay(pdMS_TO_TICKS(500));
    ESP8266_SendATCmd("AT+CIPMUX=1\r\n");
    ESP8266_CheckStrInRxbuf("OK", 2000);
    //delay_ms(500);
		vTaskDelay(pdMS_TO_TICKS(500));
    // 5. 连接WiFi
    ret = ESP8266_ConnectAp(SSID, PSWD);
    if(ret < 0) {
        printf("[Init] 连接WiFi失败！\r\n");
        return -1;
    }

    printf("[Init] ================== ESP8266 初始化成功！ ==================\r\n");
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

				    if (xSemaphoreTake(esp_u3_recvbuf_sem_mutex_handle, pdMS_TO_TICKS(6000)) == pdTRUE)
						{
						// 发送前强制清理
						MY_LIB_ClearArray((int8_t*)u3_recvbuf, RX3_BUF_SIZE);
						 xSemaphoreGive(esp_u3_recvbuf_sem_mutex_handle);

						}

					u3_count = 0;
					u3_len   = 0;
					u3_flag  = 0;

					// 2、串口3发送字符串数据
					UART3_SendStr(cmd_str);
}


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
		
	    if (xSemaphoreTake(esp_u3_recvbuf_sem_mutex_handle, pdMS_TO_TICKS(6000)) == pdTRUE)
    {
    // 发送前强制清理
    MY_LIB_ClearArray((int8_t*)u3_recvbuf, RX3_BUF_SIZE);
		 xSemaphoreGive(esp_u3_recvbuf_sem_mutex_handle);

    }
    u3_count = u3_len = u3_flag = 0;

    sprintf(cmd, "AT+CIPSEND=%d,%d\r\n", link_id, len);
    UART3_SendStr(cmd);

    vTaskDelay(pdMS_TO_TICKS(120));   // 等待 >

    if(ESP8266_CheckStrInRxbuf(">", 1500) == 0)
    {
        UART3_SendStr(msg_str);
        printf("[SendMsg] LinkID=%d 发送完成\r\n", link_id);
        
        vTaskDelay(pdMS_TO_TICKS(80));   // 给模块处理时间
    }
    else
    {
        printf("[CIPSEND] 超时未收到 > !!!\r\n");
    }

 	    if (xSemaphoreTake(esp_u3_recvbuf_sem_mutex_handle, pdMS_TO_TICKS(6000)) == pdTRUE)
    {
    // 发送前强制清理
    MY_LIB_ClearArray((int8_t*)u3_recvbuf, RX3_BUF_SIZE);
		 xSemaphoreGive(esp_u3_recvbuf_sem_mutex_handle);

    }
    u3_count = u3_len = u3_flag = 0;
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
		if (xSemaphoreTake(esp_u3_recvbuf_sem_mutex_handle, pdMS_TO_TICKS(6000)) == pdTRUE)
    {
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
		 xSemaphoreGive(esp_u3_recvbuf_sem_mutex_handle);

    }

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
    printf("[RST] 开始复位ESP8266...\r\n");
    
    ESP8266_SendATCmd("AT+RST\r\n");
    
    // 增加更长的等待时间 + 更宽松的判断
    if(ESP8266_CheckStrInRxbuf("OK", 8000) == 0 || 
       ESP8266_CheckStrInRxbuf("ready", 8000) == 0)
    {
        printf("[RST] 复位成功！\r\n");
        //delay_ms(1500);        // 模块重启需要较长时间
				vTaskDelay(pdMS_TO_TICKS(1500));
        return 0;
    }
    
    printf("[RST] *** 复位失败 *** 当前接收缓冲区内容：\r\n");
    printf("%s\r\n", (char*)u3_recvbuf);
    return -1;
}


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
	//delay_ms(300);
	vTaskDelay(pdMS_TO_TICKS(300));
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
	//delay_ms(300);
	vTaskDelay(pdMS_TO_TICKS(300));
	// 1、进入透传模式
	ESP8266_SendATCmd("AT+CIPMODE=1\r\n");
	if(ESP8266_CheckStrInRxbuf("OK", 3000) == -1)
		return -1;
	//delay_ms(300);
	vTaskDelay(pdMS_TO_TICKS(300));

	// 2、开启发送
	ESP8266_SendATCmd("AT+CIPSEND\r\n");
	if(ESP8266_CheckStrInRxbuf(">", 3000) == -1)
		return -3;
	//delay_ms(300);
	vTaskDelay(pdMS_TO_TICKS(300));
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
	//delay_ms(100);
	vTaskDelay(pdMS_TO_TICKS(100));
	return 0 ;	 
}





int8_t ESP8266_ConnectAp(char *ssid, char *pswd)
{
	char cmd[64];
	
	// 1、设置STATION模式
	ESP8266_SendATCmd("AT+CWMODE=1\r\n");
	if(ESP8266_CheckStrInRxbuf("OK", 3000) == -1)
		return -1;
	//delay_ms(500);
	vTaskDelay(pdMS_TO_TICKS(500));
	// 2、一次性拼接指令，稳定不炸
	sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pswd);
	ESP8266_SendATCmd(cmd);
	
	// 3、等待获取IP
	if(ESP8266_CheckStrInRxbuf("WIFI GOT IP", 10000) == -1)
	{
		return -1;	
	}
	
	// 连接成功后延时，让网络稳定
	//delay_ms(1000);
	vTaskDelay(pdMS_TO_TICKS(500));
	return 0;	
}



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
    uint8_t retry = 3;        
    
    printf("[Connect] LinkID=%d → %s://%s:%d\r\n", link_id, mode, ip, port);

    // 1. 确保基础状态正确
    ESP8266_ExitTransparentTransmission();
    vTaskDelay(pdMS_TO_TICKS(100));

    ESP8266_SendATCmd("AT+CIPMODE=0\r\n");
    vTaskDelay(pdMS_TO_TICKS(100));

    ESP8266_SendATCmd("AT+CIPMUX=1\r\n");
    vTaskDelay(pdMS_TO_TICKS(150));

    // 2. 尝试连接
    for (uint8_t i = 0; i < retry; i++)
    {
		if (xSemaphoreTake(esp_u3_recvbuf_sem_mutex_handle, pdMS_TO_TICKS(6000)) == pdTRUE)
    {
    // 发送前强制清理
    MY_LIB_ClearArray((int8_t*)u3_recvbuf, RX3_BUF_SIZE);
		 xSemaphoreGive(esp_u3_recvbuf_sem_mutex_handle);

    }

        u3_count = u3_len = u3_flag = 0;

        sprintf(cmd, "AT+CIPSTART=%d,\"%s\",\"%s\",%d\r\n", link_id, mode, ip, port);
        ESP8266_SendATCmd(cmd);

        // 等待响应
        if (ESP8266_CheckStrInRxbuf("CONNECT", 2500) == 0 ||
            ESP8266_CheckStrInRxbuf("OK", 2500) == 0 ||
            ESP8266_CheckStrInRxbuf("ALREADY CONNECT", 2000) == 0)
        {
            printf("LinkID=%d 连接成功\r\n", link_id);
            vTaskDelay(pdMS_TO_TICKS(300));
            return 0;
        }

        printf("[Connect] LinkID=%d 第%d次尝试失败\r\n", link_id, i+1);
        vTaskDelay(pdMS_TO_TICKS(800));
    }

    printf("LinkID=%d 连接失败！\r\n", link_id);
    return -1;
}

