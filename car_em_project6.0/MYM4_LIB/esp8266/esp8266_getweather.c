
#include "esp8266.h"
#include "esp8266_getweather.h"
#include "delay.h"
#include "uart.h"
#include <stdio.h>
#include <string.h>

// 全局解析结果结构体
Results results[1] = {0};

/**
  * @brief  暴力字符串解析天气JSON（无CJSON）
  * @param  sbuf: 天气返回的原始字符串
  * @param  results: 存储解析结果
  * @retval 0成功
  */
int8_t weather_brute_parse(char *sbuf, Results *results)
{
    char *p = NULL;
    // 清空结构体
    memset(results, 0, sizeof(Results));

    // ===================== 1. 提取城市名称 name =====================
    p = strstr(sbuf, "\"name\":\"");
    if(p != NULL)
    {
        p += strlen("\"name\":\"");        // 跳过关键字
        char *end = strchr(p, '\"');       // 找到结束的双引号
        if(end != NULL)
        {
            strncpy(results->location.name, p, end - p); // 安全拷贝
        }
    }

    // ===================== 2. 提取天气描述 text =====================
    p = strstr(sbuf, "\"text\":\"");
    if(p != NULL)
    {
        p += strlen("\"text\":\"");
        char *end = strchr(p, '\"');
        if(end != NULL)
        {
            strncpy(results->now.text, p, end - p);
        }
    }

    // ===================== 3. 提取天气代码 code =====================
    p = strstr(sbuf, "\"code\":\"");
    if(p != NULL)
    {
        p += strlen("\"code\":\"");
        char *end = strchr(p, '\"');
        if(end != NULL)
        {
            strncpy(results->now.code, p, end - p);
        }
    }

    return 0;
}

/**
  * @brief  ESP8266获取天气（修复参数bug，暴力解析）
  * @param  xztq_key: 心知天气密钥
  * @param  city_buf: 城市名
  * @retval 0成功
  */

int32_t ESP8266_GetWeatherFunc(char *xztq_key, char *city_buf)
{
    int8_t ret    = 0;
    char weather_buf[512] = {0};
    int32_t i      = 0;
    int16_t flag  = 0;

    // 天气描述对照表（你原版完整表，保留不动）
    char *weathre_str[] = {"Sunny", "Clear", "Fair", "Fair", "Cloudy", "Partly Cloudy", "Partly Cloudy", "Mostly Cloudy", "Mostly Cloudy", "Overcast", 
                           "Shower", "Thundershower", "Thundershower with Hail", "Light Rain", "Moderate Rain","Heavy Rain", "Storm", "Heavy Storm", "Severe Storm", "Ice Rain", 
                           "Sleet", "Snow Flurry", "Light Snow", "Moderate Snow", "Heavy Snow", "Snowstorm", "Dust", "Sand", "Duststorm", "Sandstorm"};
    
    char *weathre_num[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", 
                           "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", 
                           "20", "21", "22", "23", "24", "25", "26", "27", "28", "29"};

    ret = ESP8266_ConnectServer_LinkID(1, "TCP", "116.62.81.138", 80);
    if(ret < 0) {
        printf("服务器连接失败\r\n");
        return -4;
    }
    printf("服务器连接成功\r\n");
    delay_ms(800);

    sprintf(weather_buf, "GET /v3/weather/now.json?key=%s&location=%s&language=en&unit=c HTTP/1.1\r\nHost: api.seniverse.com\r\nConnection: close\r\n\r\n",
            xztq_key, city_buf);

    ESP8266_SendMsg_LinkID(1, weather_buf);
    delay_ms(2500);

    // 3. 暴力解析天气数据
    weather_brute_parse((char *)u3_recvbuf, results);

    // 4. 匹配天气文字
    for(i=0; i<30; i++)  
    {
        if(strcmp(results[0].now.code, weathre_num[i]) == 0)
        {
            flag = i;
            break;
        }
    }
    printf("天气情况：%s\r\n", weathre_str[flag]);
    printf("城市：%s\r\n", results[0].location.name);
   // printf("天气：%s\r\n", results[0].now.text);

    // 清空接收缓存
    MY_LIB_ClearArray((int8_t*)u3_recvbuf, RX3_BUF_SIZE);
    u3_count = 0;
    u3_len   = 0;
    u3_flag  = 0;
    delay_ms(500);

    // 关闭连接
    ESP8266_SendATCmd("AT+CIPCLOSE=1\r\n");
    delay_ms(800);

    return 0;
}


