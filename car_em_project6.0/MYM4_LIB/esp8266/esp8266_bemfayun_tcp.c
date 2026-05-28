#include "esp8266_bemfayun_tcp.h"
#include "esp8266.h"
#include "delay.h"
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "uart.h"
#include "task.h"

// ==================== 新增：心跳相关 ====================
static uint32_t bemfa_last_heartbeat = 0;

/**
  * @brief  巴法云心跳（cmd=0）
  */
void BemFaYun_SendHeartbeat(void)
{
    char buf[64] = {0};
    sprintf(buf, "cmd=0&uid=%s\r\n", BEMFAYUN_SIYAO);
    
    // 先尝试发送
    ESP8266_SendMsg_LinkID(BEMFAYUN_LINK_ID, buf);
    
    // 如果发送失败（缓冲区出现 ERROR），则重连
    if (strstr((char*)u3_recvbuf, "ERROR") != NULL || 
        strstr((char*)u3_recvbuf, "link is not valid") != NULL)
    {
        printf("[BemFa] 连接已断开，正在重连...\r\n");
        ESP8266_BemFaYun_Tcp_Connect();           // 重连
        // 重新订阅
        ESP8266_BemFaYun_Tcp_Subscribe(BEMFAYUN_SIYAO, "guangzhao111");
        ESP8266_BemFaYun_Tcp_Subscribe(BEMFAYUN_SIYAO, "zhiliang111");
        ESP8266_BemFaYun_Tcp_Subscribe(BEMFAYUN_SIYAO, "CO111");
        ESP8266_BemFaYun_Tcp_Subscribe(BEMFAYUN_SIYAO, "yanwu111");
        ESP8266_BemFaYun_Tcp_Subscribe(BEMFAYUN_SIYAO, "wenshidu111");
    }
    
    printf("[BemFa] 心跳处理完成\r\n");
}
/**
  * @brief  巴法云连接
  */
int32_t ESP8266_BemFaYun_Tcp_Connect(void)
{
    ESP8266_ExitTransparentTransmission();
    ESP8266_SendATCmd("AT+CIPMUX=1\r\n");
    delay_ms(200);

    if (ESP8266_ConnectServer_LinkID(BEMFAYUN_LINK_ID, "TCP", "bemfa.com", 8344) < 0)
    {
        printf("巴法云连接失败\r\n");
        return -1;
    }
    printf("巴法云连接成功 (LinkID=0)\r\n");
    delay_ms(800);
    
    bemfa_last_heartbeat = xTaskGetTickCount();   // 更新心跳时间戳
    return 0;
}

/* 以下两个函数保持你原来的即可 */
int32_t ESP8266_BemFaYun_Tcp_Subscribe(const char *key, const char *topic)
{
    char buf[256] = {0};
    sprintf(buf, "cmd=1&uid=%s&topic=%s\r\n", key, topic);
    ESP8266_SendMsg_LinkID(BEMFAYUN_LINK_ID, buf);
    delay_ms(300);
    return 0;
}

int32_t ESP8266_BemFaYun_Tcp_Publish(const char *key, const char *topic, const char *msg)
{
    char buf[512] = {0};
    sprintf(buf, "cmd=2&uid=%s&topic=%s&msg=%s\r\n", key, topic, msg);
    ESP8266_SendMsg_LinkID(BEMFAYUN_LINK_ID, buf);
    delay_ms(200);
    return 0;
}
