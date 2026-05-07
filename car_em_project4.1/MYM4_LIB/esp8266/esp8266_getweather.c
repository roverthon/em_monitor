//#include "esp8266.h"
//#include "esp8266_getweather.h"
//#include "cjson.h"
//#include "delay.h"
//#include "uart.h"
//#include <stdio.h>
//#include <string.h>


//// 定义CJCON数据解析后存放数据的结构体
////Results results[7] = {0};
//Results results[1] = {0};
///**
//  * @brief  使用CJSON解析反馈的天气信息
//  * @note  
//  *			1、cJSON数据解析示例代码，如果解析数据不确定是否格式正确可以利用以下网站对需解析数据提前验证：
//  *				https://lzltool.cn/string-escape
//  *
//  *			2、利用cJSON对数据进行解析时，需要设置工程的 Heap_Size、Stack_Size 大小，即打开项目工程中的startup_stm32f40_41xxx.s启动文件，把Heap_Size、Heap_Size、Stack_Size修改为合适值，如：0x00002000
//  *         3、心知天气天气现象代码：https://docs.seniverse.com/api/start/code.html
//  * @param  sbuf：   cjson协议包
//  *         results：指向天气结构体的指针
//  * @retval 成功：返回0
//  *         失败：返回非0
//  */
//int8_t weather_cjson_parse(char *sbuf, Results *results)
//{
//	cJSON *Json=NULL,*Results=NULL,*Array=NULL,*Location=NULL,*Now=NULL,*Last_update=NULL,*Item=NULL;
//	
//	char *p_data = sbuf;
//	
//	//解析Json数据包
//	Json = cJSON_Parse(p_data);
//	if(Json == NULL)												//检测Json数据包是否存在语法上的错误，返回NULL表示数据包无效
//	{
//		//printf("Error before: [%s] \r\n", cJSON_GetErrorPtr());		//打印数据包语法错误的位置
//		return -1;
//	}
//	else
//	{
//		if((Results = cJSON_GetObjectItem(Json,"results")) != NULL)	//匹配字符串"results",获取数组内容
//		{
//			//获取数组中对象个数
//			//printf("cJSON_GetArraySize: size = %d\r\n", cJSON_GetArraySize(Results)); 
//			
//			if((Array = cJSON_GetArrayItem(Results,0)) != NULL)//获取父对象内容
//			{
//				//匹配子对象 location 
//				if((Location = cJSON_GetObjectItem(Array,"location")) != NULL)
//				{

//					if((Item = cJSON_GetObjectItem(Location,"name")) != NULL) //匹配子对象1中的成员 "name"
//					{
//						memcpy(results[0].location.name,Item->valuestring,strlen(Item->valuestring));
//						printf("%s: %s \r\n",Item->string,results[0].location.name);
//					}

//				}
//				//匹配子对象 now 
//				if((Now = cJSON_GetObjectItem(Array,"now")) != NULL)
//				{
//					printf("=================================Now=================================\r\n");
//					if((Item = cJSON_GetObjectItem(Now,"text")) != NULL)//匹配子对象2中的成员 "text"
//					{
//						memcpy(results[0].now.text,Item->valuestring,strlen(Item->valuestring));
//						printf("%s: %s \r\n",Item->string,results[0].now.text);
//					}
//					if((Item = cJSON_GetObjectItem(Now,"code")) != NULL)//匹配子对象2中的成员 "code"
//					{
//						memcpy(results[0].now.code,Item->valuestring,strlen(Item->valuestring));
//						printf("%s: %s \r\n",Item->string,results[0].now.code);
//					}

//				}

//			} 
//		}
//	}

//	//释放cJSON_Parse()分配出来的内存空间
//	cJSON_Delete(Json); 
//	Json = NULL;
//	return 0;
//}


///**
//  * @brief  通过ESP8266模块获取天气信息
//  * @note   天气API来自于心知天气(https://www.seniverse.com/insights)
//  * @param  xztq_key：心知天气的密钥(自行申请)
//  *         city_buf：需要查询天气的城市
//  * @retval 成功：返回0
//  *         失败：返回小于0的数
//  */
//int32_t ESP8266_GetWeatherFunc(char *xztq_key, char *city_buf)
//{
//	
//	int8_t ret    = 0;
//    char weather_buf[512]      = {0};
//    int32_t i                  = 0;
//    int16_t flag               = 0;

//    
//    // 天气情况
//    char *weathre_str[] = {"Sunny", "Clear", "Fair", "Fair", "Cloudy", "Partly Cloudy", "Partly Cloudy", "Mostly Cloudy", "Mostly Cloudy", "Overcast", 
//                           "Shower", "Thundershower", "Thundershower with Hail", "Light Rain", "Moderate Rain","Heavy Rain", "Storm", "Heavy Storm", "Severe Storm", "Ice Rain", 
//                           "Sleet", "Snow Flurry", "Light Snow", "Moderate Snow", "Heavy Snow", "Snowstorm", "Dust", "Sand", "Duststorm", "Sandstorm"};
//    
//		char *weathre_num[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", 
//                           "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", 
//                           "20", "21", "22", "23", "24", "25", "26", "27", "28", "29"};
//    				   
//	// 1、与网站建立连接
//	ret =  ESP8266_ConnectServer("TCP", "116.62.81.138", 80);
//	if(ret < 0)
//	{
//		printf("与网站建立连接失败\r\n");
//		return -4;
//	}
//	printf("01：与网站建立连接成功!\r\n");
//	delay_ms(1000);		

//		
//	// 2、开启透传模式
//	ret =  ESP8266_EnterTransparentTransmission();
//	if(ret < 0)
//	{
//		printf("开启透传模式失败\r\n");
//		return -6;
//	}
//	printf("02：开启透传模式成功!\r\n");
//	delay_ms(1000);	
//	

//	// 3、向网站提出请求   
//    sprintf(weather_buf,"GET https://api.seniverse.com/v3/weather/now.json?key=%s&location=%s&language=zh-Hans&unit=c\r\n", city_buf, xztq_key);
//	ESP8266_SendATCmd(weather_buf);
//	delay_ms(2000);	

//    // 4、解析获取到的天气
//    // printf("u3_recvbuf = %s\r\n", u3_recvbuf);                      // 打印从网站返回的时间的数据(全部)
//	weather_cjson_parse((char *)u3_recvbuf, results);

//    // 获取天气情况
//    i = 0;
//    for(i=0; i<30; i++)
//    {
//        if(strcmp(results->now.code, weathre_num[i]) == 1)
//        {
//            flag = i;
//        }
//    }
//    printf("天气情况 = %s\r\n", weathre_str[flag+1]);
//	//printf("天气温度 = %s\r\n", results[0].now.temperature);
//	
//	
//	// 清空相关数据
//	MY_LIB_ClearArray((int8_t*)u3_recvbuf, RX3_BUF_SIZE);
//	u3_count = 0;
//	u3_len   = 0;
//	u3_flag  = 0;
//	delay_ms(500);
//	
//	
//	// 5、退出透传模式(断开连接)
//	ret = ESP8266_ExitTransparentTransmission();
//	if(ret < 0)
//	{
//		//printf("退出透传模式(断开连接)失败\r\n");
//		return -1;
//	}
//	//printf("03：退出透传模式(断开连接)成功!\r\n");
//	delay_ms(1000);	
//    return 0;
//}
#include "esp8266.h"
#include "esp8266_getweather.h"
#include "delay.h"
#include "uart.h"
#include <stdio.h>
#include <string.h>

// 全局解析结果结构体（与原代码一致）
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

    // 天气描述对照表
    char *weathre_str[] = {"Sunny", "Clear", "Fair", "Fair", "Cloudy", "Partly Cloudy", "Partly Cloudy", "Mostly Cloudy", "Mostly Cloudy", "Overcast", 
                           "Shower", "Thundershower", "Thundershower with Hail", "Light Rain", "Moderate Rain","Heavy Rain", "Storm", "Heavy Storm", "Severe Storm", "Ice Rain", 
                           "Sleet", "Snow Flurry", "Light Snow", "Moderate Snow", "Heavy Snow", "Snowstorm", "Dust", "Sand", "Duststorm", "Sandstorm"};
    
		char *weathre_num[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", 
                           "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", 
                           "20", "21", "22", "23", "24", "25", "26", "27", "28", "29"};

    // 1. 连接心知天气服务器
    ret = ESP8266_ConnectServer("TCP", "116.62.81.138", 80);
    if(ret < 0)
    {
        printf("服务器连接失败\r\n");
        return -4;
    }
    printf("服务器连接成功\r\n");
    delay_ms(1000);

    // 2. 开启透传
    ret = ESP8266_EnterTransparentTransmission();
    if(ret < 0)
    {
        printf("透传开启失败\r\n");
        return -6;
    }
    printf("透传开启成功\r\n");
    delay_ms(1000);

    // ===================== 修复：原代码KEY和城市参数颠倒！ =====================
    sprintf(weather_buf, "GET https://api.seniverse.com/v3/weather/now.json?key=%s&location=%s&language=en&unit=c\r\n\r\n",
            xztq_key, city_buf);
		//中文形式：GET https://api.seniverse.com/v3/weather/now.json?key=%s&location=%s&language=zh-Hans&unit=c\r\n\r\n
    ESP8266_SendATCmd(weather_buf);
    delay_ms(2000);

    // 3. 暴力解析天气数据（替换CJSON）
    weather_brute_parse((char *)u3_recvbuf, results);

    // 4. 匹配天气文字（修复：strcmp相等返回0）
    for(i=0; i<10; i++)
    {
        if(strcmp(results[0].now.code, weathre_num[i]) == 0)
        {
            flag = i;
            break;
        }
    }
    printf("天气情况：%s\r\n", weathre_str[flag]);
    printf("城市：%s\r\n", results[0].location.name);
    printf("天气：%s\r\n", results[0].now.text);

    // 清空接收缓存
    MY_LIB_ClearArray((int8_t*)u3_recvbuf, RX3_BUF_SIZE);
    u3_count = 0;
    u3_len   = 0;
    u3_flag  = 0;
    delay_ms(500);

    // 5. 退出透传
    ret = ESP8266_ExitTransparentTransmission();
    return ret;
}

