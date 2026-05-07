/**
  ******************************************************************************
  * @file    main.h
  * @author  rovertyphon
  * @version V0.2.0
  * @date    2026.04.28
  * @brief   说明：
  *				LVGL配置与裁剪
  *          操作1：
  *            	1、保证stm32f4xx.h文件123行的HSE_VALUE时钟源值为8000000，保证system_stm32f4xx.c文件的316行的PLL_M系数为8
  *			   			2、板子上丝印层UART1跳线帽处，短接1-3和2-4，A-A口线要连接电脑和单片机USB口，保证调试通信和5V供电
  *            	3、串口助手的端口要打开连接开发板的那个串口端口，串口助手的波特率要和单片机串口程序保持一致
  *            	4、保证keil5软件和串口助手都是一个编码格式
	*								MQ2:
	*										AIN->DCMI_HREF(CA5)->PA4
	*										DIN->DCMI_D1(CA9)->PC7
	*								mq7:
	*										ao->DCMI_PCLK(CA16)->PA6
	*										DO->DCMI_PWDN(CA18)->PG9
	*								mq135:
	*										do->DCMI_XCLK(ca17)->PA8
	*								PIR:
	*										SIG->DCMI_D6(CA14)->PE5
  *			 注意：
  * 			(1)、lv_conf.h文件设置
					1、使用DMA时注意16位色高低字节交换，设置宏 LV_COLOR_16_SWAP 1
  *					2、颜色深度为16色，设置宏 LV_COLOR_DEPTH 16
  *					3、LVGL内存大小48Kbytes，设置宏LV_MEM_SIZE (48 * 1024U)
  *					4、帧率，设置 LV_DISP_DEF_REFR_PERIOD 30
  *					5、触摸灵敏度，设置 LV_INDEV_DEF_READ_PERIOD
  *					6、实时显示帧率，设置 LV_USE_PERF_MONITOR 1
  *					7、内存占用， 设置 LV_USE_MEM_MONITOR 1
  *					8、默认字体，设置 LV_FONT_DEFAULT &lv_font_montserrat_14
  *	     			   字体小点，代码体积随之减小	
  *
  *				(2)、你写中文的程序文件(比如main.c)、和生成的字库文件(比如lvgl_font.c)，必须是utf-8格式,否则会乱码(因为你的字库文件来源一般是utf-8格式)
  *				(3)、使用lvgl的示例工程时，需要注意在lv_conf.h文件上打开，比如lv_demo_widgets()示例，需打开#define LV_USE_DEMO_WIDGETS 1
  *				(4)、一些不用的组件可以在lv_conf.h文件屏蔽，减少程序体积
  *				(5)、优化等级可以设置为O2，设置成O3反而可能会造成程序体积增大
  *
	*				任务1：启动和创建任务
	*				任务2：lvgl检查
	*				任务3：app_task3_lvgl 		界面任务
	*				任务4：app_task4_rtc		 	时间任务
	*				任务5：app_task5_adc 		光照任务
	*				任务6：app_task6_dht11 	温湿度任务
	* 			任务8：app_task8_asrpro	语音任务
	*				任务9：app_task9_mq2			烟雾报警器
	*				任务10：app_task10_mq7		co报警器
	*				任务11:	app_task11_mq135 	空气质量
	*				任务12：app_task12_esp8266 wifi管理+时间+天气
	*				任务13: app_task13_weather	天气
	*				事件标志组：
	*								event1_group_handle（esp8266->weather(BIT_0))
	*				互斥锁：
	*								esptime_weather_sem_mutex_handle;（esp8266time<->weather)
	*				消息队列：
	*								msg_queue_handle (rtc->lvgl;adc->lvgl，dht11->lvgl,mq2->lvgl,mq7->lvgl,mq135->lvgl)
	*								time_queue_handle(esp8266time->rtc)
	*								
  *			 如何优化内存：
  *				https://share.note.youdao.com/s/I59zDn9f(第7点)
  *
  ******************************************************************************
  * @attention
  *
  * 本文档只供学习使用，不得商用，违者必究
  *
  * 有疑问或者建议：RoverTyphon@outlook.com
  *
  ******************************************************************************
  */

#include "main.h"

#include "lcd_screen.h"
#include "lcd_touch.h"
#include "lcd_font.h"
#include "lcd_image.h"
//#include "sensirion_arch_config.h"
//#include "sensirion_common.h"
//#include "sensirion_i2c.h"
//#include "iic.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
//#include "lv_demo_widgets.h"
#include "string.h"
#include "adc.h"
#include "FreeRTOS.h"		// 选编译配置文件，用于汇总所有源文件的编译选择控制
#include "task.h"			// 任务相关函数头文件
#include "semphr.h"			// 信号量头文件
#include "event_groups.h"	// 事件标志组头文件
#include "queue.h"			// 消息队列头文件
#include "rtc.h"
#include "lvgl.h"
#include "dht11.h"
//#include "sgp30.h"
#include "asrpro.h"
#include <stdlib.h>
#include "mq2.h"
#include "mq7.h"
#include "mq135.h"
#include "esp8266.h"
#include "esp8266_gettime.h"
#include "esp8266_getweather.h"
#include "pir.h"
// 位的设置
#define BIT_0 (1<<0)
#define BIT_1 (1<<1)
#define BIT_2 (1<<2)
#define BIT_3 (1<<3)
#define BIT_4 (1<<4)
#define BIT_5 (1<<5)
#define BIT_6 (1<<6)
#define BIT_7 (1<<7)
//全局变量
extern Results results[];

// 静态变量只缓存 MQ2、MQ7、温度、湿度
static float last_mq2  = 0.0f;
static float last_mq7  = 0.0f;
static int   last_temp = 0;
static int   last_humi = 0;

// 定义判断“在 3.3±0.05 范围内”的宏
#define EQ_3_3(val)  ((val) > 3.25f && (val) < 3.35f)
//队列配置
#define QUEUE_LEN 32
//创建消息队列格式——枚举和结构体
//定义类型
typedef enum {
	msg_type_adc,
	msg_type_rtc,
	msg_type_dht11,
	msg_type_mq2,
	msg_type_mq7,
	msg_type_mq135,
	msg_type_esp8266_time,
	msg_type_weather
} MsgTypeDef;
typedef struct {
    MsgTypeDef type;          // 数据类型标记
    uint16_t adc_val;         // ADC
    RTC_DateTypeDef date;     // RTC日期
    RTC_TimeTypeDef time;     // RTC时间
		uint8_t wenshidu[16];						//温湿度
		float	mq2_num;				//烟雾
		float	mq7_num;				//co
		uint16_t	mq135_num;			//空气质量
		
		//Results weather;
} MsgStructDef;
//天气结构体
typedef struct {
	MsgTypeDef sendweather;
	char weather_text[24];
	char weather_name[24];
}weather_struct;

//时间结构体
typedef struct{
	MsgTypeDef sendtime;
	time_t esp_time;
	
}time_struct;

/* 任务句柄 */
TaskHandle_t app_task1_init_handle      = NULL;
TaskHandle_t app_task2_lvgl_task_handle = NULL;
TaskHandle_t app_task3_lvgl_handle	    = NULL;
TaskHandle_t app_task4_rtc_handle	    = NULL;
TaskHandle_t app_task5_adc_handle	    = NULL;
TaskHandle_t app_task6_dht11_handle	    = NULL;
TaskHandle_t app_task8_asrpro_handle	    = NULL;
TaskHandle_t app_task9_mq2_handle	    = NULL;
TaskHandle_t app_task10_mq7_handle	    = NULL;
TaskHandle_t app_task11_mq135_handle	    = NULL;
TaskHandle_t app_task12_esp8266_handle	    = NULL;
TaskHandle_t app_task13_weather_handle	    = NULL;
/* 任务函数*/
static void app_task1_init(void *pvParameters);
static void app_task2_lvgl_task(void *pvParameters);
static void app_task3_lvgl(void *pvParameters);
static void app_task4_rtc(void *pvParameters);
static void app_task5_adc(void *pvParameters);
static void app_task6_dht11(void *pvParameters);
static void app_task8_asrpro(void *pvParameters);
static void app_task9_mq2(void *pvParameters);
static void app_task10_mq7(void *pvParameters);
static void app_task11_mq135(void *pvParameters);
static void app_task12_esp8266(void *pvParameters);
static void app_task13_weather(void *pvParameters);
//// 事件标志组
static EventGroupHandle_t event1_group_handle;
// 互斥信号量
static SemaphoreHandle_t esptime_weather_sem_mutex_handle;
//消息队列
static QueueHandle_t msg_queue_handle = NULL;
static QueueHandle_t time_queue_handle = NULL;
static QueueHandle_t weather_queue_handle = NULL;

// 声明一个自定义的 TTF 字体
LV_FONT_DECLARE(my_font);


/**
  * @brief  主函数
  * @note   None
  * @param  None
  * @retval None
  */
int main(void)
{
	/* 设置系统中断优先级分组4 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	/* 系统定时器中断频率为configTICK_RATE_HZ */
    SysTick_Config(SystemCoreClock / configTICK_RATE_HZ);
	

//	// 三、创建事件标志组
	event1_group_handle    = xEventGroupCreate();
	//创建互斥信号量
	esptime_weather_sem_mutex_handle = xSemaphoreCreateMutex();
	
	 // 三、创建消息队列、  
   msg_queue_handle  = xQueueCreate(QUEUE_LEN, sizeof(MsgStructDef)); 
	time_queue_handle  = xQueueCreate(QUEUE_LEN, sizeof(time_struct)); 
	weather_queue_handle  = xQueueCreate(QUEUE_LEN, sizeof(weather_struct)); 
	
	/* 创建app_task_init任务 */
    xTaskCreate((TaskFunction_t)app_task1_init,          /* 任务入口函数 */
                (const char *)"app_task1_init",          /* 任务名字 */
                (uint16_t)256,                           /* 任务栈大小 */
                (void *)NULL,                             /* 任务入口函数参数 */
                (UBaseType_t)7,                          /* 任务的优先级 */	// (注意：优先级数字越小表示任务优先级越低(跟stm32相反)、优先级默认上限为(configMAX_PRIORITIES - 1))
                (TaskHandle_t *)&app_task1_init_handle); /* 任务控制块指针 */
				
    /* 开启任务调度 */
    vTaskStartScheduler();
}

/**
  * @brief  滴答钩子（tick hook）函数
  * @note   滴答中断的主要作用是推进任务的定时器，检查是否有过期的定时器，以及触发任务的切换
  * @param  None
  * @retval None
  */
void vApplicationTickHook(void)
{
    lv_tick_inc(1); // 告诉 LVGL 已经过去了 1 毫秒
}

/**
  * @brief  初始化硬件任务函数
  * @note   一些硬件的初始化，可以将其放在此处进行，该任务可以包含创建任务的功能
  * @param  None
  * @retval None
  */
static void app_task1_init(void *pvParameters)
{
	// 一、外设相关变量区域
	BaseType_t xReturned;	
	
	PIR_Init(); 														//PIR初始化
	UART1_Init(115200);											// 串口1初始化为115200
	UART3_Init(115200);											// 串口1初始化为115200
//	IIC_Init();
	LED_Init();													// led、蜂鸣器、按键初始化
	BUZZER_Init();
	KEY_Init();
	RTC_ConfigInit();									// RTC时钟初始化并设置日期时间		
	DHT11_Init();
	lv_init();													// 初始化lvgl
	lv_port_disp_init();										// 初始化lvgl显示设备
	lv_port_indev_init();										// 初始化lvgl输入设备
	ADC3_IN5_LightInit();								// ADC3通道5的初始化(光敏传感器)
	ASRPRO_Init();
	//sensirion_i2c_init();
	MQ2_Init();												//烟雾传感器初始化
	MQ7_Init();												//CO传感器
	MQ135_Init();												//空气质量传感器
	ESP8266_Init(115200);	
	// 三、外设具现化区域(任务创建的地方)
	// 1、创建app_task2_lvgl_task任务
	xReturned = xTaskCreate(app_task2_lvgl_task,"app_task2_lvgl_task", 512, NULL,  7, &app_task2_lvgl_task_handle);     
	if( xReturned == pdPASS )
    {
		printf("app_task2_lvgl_task create success!\r\n");
    }
	
	// 2、创建app_task3_lvgl界面任务
	xReturned = xTaskCreate(app_task3_lvgl,"app_task3_lvgl", 512, NULL,  7, &app_task3_lvgl_handle);     
	if( xReturned == pdPASS )
    {
		printf("app_task3_lvgl create success!\r\n");
    }
	
	// 3、创建app_task4_rtc时间任务
	xReturned = xTaskCreate(app_task4_rtc,"app_task4_rtc", 256, NULL,  6, &app_task4_rtc_handle);     
	if( xReturned == pdPASS )
    {
		printf("app_task4_rtc create success!\r\n");
    }
	// 4、创建app_task5_adc光照任务                                    
	xReturned = xTaskCreate(app_task5_adc,"app_task5_adc", 128, NULL,  6, &app_task5_adc_handle);     
	if( xReturned == pdPASS )
    {
		printf("app_task5_adc create success!\r\n");
    }
		// 4、创建app_task6_dht11温湿度任务
	xReturned = xTaskCreate(app_task6_dht11,"app_task6_dht11", 128, NULL,  7, &app_task6_dht11_handle);     
	if( xReturned == pdPASS )
    {
		printf("app_task6_dht11 create success!\r\n");
    }
	
		// 4、创建app_task8_asrpro co2voc任务
	xReturned = xTaskCreate(app_task8_asrpro,"app_task8_asrpro", 128, NULL,  5, &app_task8_asrpro_handle);     
	if( xReturned == pdPASS )
    {
		printf("app_task8_asrpro create success!\r\n");
    }
		// 4、创建app_task9_mq2 烟雾采集器任务
	xReturned = xTaskCreate(app_task9_mq2,"app_task9_mq2", 128, NULL,  5, &app_task9_mq2_handle);     
	if( xReturned == pdPASS )
    {
		printf("app_task9_mq2 create success!\r\n");
    }
		// 4、创建app_task10_mq7 CO采集器任务
	xReturned = xTaskCreate(app_task10_mq7,"app_task10_mq7", 128, NULL,  5, &app_task10_mq7_handle);     
	if( xReturned == pdPASS )
    {
		printf("app_task10_mq7 create success!\r\n");
    }
		// 4、创建app_task11_mq135 空气质量任务
	xReturned = xTaskCreate(app_task11_mq135,"app_task11_mq135", 128, NULL,  5, &app_task11_mq135_handle);     
	if( xReturned == pdPASS )
    {
		printf("app_task11_mq135 create success!\r\n");
    }
		// 4、创建app_task12_esp8266 wifi加时间
	xReturned = xTaskCreate(app_task12_esp8266,"app_task12_esp8266", 256, NULL,  6, &app_task12_esp8266_handle);     
	if( xReturned == pdPASS )
    {
		printf("app_task12_esp8266 create success!\r\n");
    }
	// 4、创建app_task13_weather 加天气
	xReturned = xTaskCreate(app_task13_weather,"app_task13_weather", 256, NULL,  5, &app_task13_weather_handle);     
	if( xReturned == pdPASS )
    {
		printf("app_task13_weather create success!\r\n");
    }
		

		vTaskDelete(NULL);											// 初始化硬件、并创建其它任务后，自己删除自己(不浪费资源)
	
		
}


/**
  * @brief  定期检查与执行lvgl所有已注册的周期性任务
  * @note   诸如界面元素动画更新、定时数据刷新、系统状态监测等操作
  * @param  None
  * @retval None
  */
static void app_task2_lvgl_task(void *pvParameters)
{
    for (;;)
    {
        lv_task_handler();
        vTaskDelay(5);
    }
}

/** 
  * @brief  LVGL界面显示任务
  * @note   None
  * @param  None
  * @retval None
  */
static void app_task3_lvgl(void *pvParameters)
{
	MsgStructDef recv_msg;
	weather_struct weather_msg;
	char time_buf[32];
	char adc_buf[32];
	char dht11_wendu[16];
	char dht11_shidu[16];
	static uint16_t pir_timer = 0;  // 静态变量，初始化=0，仅第一次执行时生效
	#define PIR_OFF_TIME  10       // 10秒无人自动熄屏（可随意修改数字）
	LV_IMG_DECLARE(image_chezaibeijing);
	//创建背景beijing
	  lv_obj_t *beijing= lv_img_create(lv_scr_act());                           	// 在当前活动屏幕上创建图像控件
    lv_img_set_src(beijing, &image_chezaibeijing);                                    	// 设置图像源
    lv_obj_align(beijing, LV_ALIGN_CENTER, 0, 0);                              	// 将图像控件居中对齐到屏幕
    lv_obj_set_size(beijing, image_chezaibeijing.header.w, image_chezaibeijing.header.h);    	// 设置图像控件的大小

	

		// 屏幕分辨率：320*240，所有对齐均基于屏幕中心(160,120)偏移

		// 1. 顶部左：时间标签 shijian
		lv_obj_t *shijian = lv_label_create(lv_scr_act());  
		lv_obj_align(shijian, LV_ALIGN_CENTER, -80, -90);  // 左半顶部区域中心(80,30)
		lv_obj_set_width(shijian, 120);
		lv_obj_set_style_text_color(shijian, lv_color_golden(), 0);


		// 2. 顶部右：天气标签 weather_display
		lv_obj_t *weather_display = lv_label_create(lv_scr_act());
		lv_obj_align(weather_display, LV_ALIGN_CENTER, 80, -90);  // 右半顶部区域中心(240,30)
		lv_obj_set_width(weather_display, 120);
		lv_obj_set_style_text_color(weather_display, lv_color_golden(), 0);


		// ------------------- 第二行（上中部，Y=60~120）-------------------
		// 左1：汉字标签 烟雾
		lv_obj_t *yanwu = lv_label_create(lv_scr_act());
		lv_obj_set_style_text_font(yanwu, &my_font, 0);
		lv_label_set_text(yanwu, "烟雾");
		lv_obj_set_width(yanwu, 80);
		lv_obj_align(yanwu, LV_ALIGN_CENTER, -120, -30);  // 区域中心(40,90)
		lv_obj_set_style_text_color(yanwu, lv_color_green(), 0);

		// 左2：mq2_data 数据标签
		lv_obj_t *mq2_data = lv_label_create(lv_scr_act());
		lv_obj_align(mq2_data, LV_ALIGN_CENTER, -10, -30);  // 区域中心(120,90)
		lv_obj_set_width(mq2_data, 120);
		lv_obj_set_style_text_color(mq2_data, lv_color_golden(), 0);

		// 右1：汉字标签 CO浓度
		lv_obj_t *COnongdu = lv_label_create(lv_scr_act());
		lv_obj_set_style_text_font(COnongdu, &my_font, 0);
		lv_label_set_text(COnongdu, "CO浓度");
		lv_obj_set_width(COnongdu, 80);
		lv_obj_align(COnongdu, LV_ALIGN_CENTER, 40, -30);  // 区域中心(200,90)
		lv_obj_set_style_text_color(COnongdu, lv_color_green(), 0);

		// 右2：mq7_data 数据标签
		lv_obj_t *mq7_data = lv_label_create(lv_scr_act());
		lv_obj_align(mq7_data, LV_ALIGN_CENTER, 150, -30);  // 区域中心(280,90)
		lv_obj_set_width(mq7_data, 120);
		lv_obj_set_style_text_color(mq7_data, lv_color_golden(), 0);


		// ------------------- 第三行（中中部，Y=120~180）-------------------
		// 左1：汉字标签 亮度
		lv_obj_t *liangdu = lv_label_create(lv_scr_act());
		lv_obj_set_style_text_font(liangdu, &my_font, 0);
		lv_label_set_text(liangdu, "光照亮度");
		lv_obj_set_width(liangdu, 80);
		lv_obj_align(liangdu, LV_ALIGN_CENTER, -120, 30);  // 区域中心(40,150)
		lv_obj_set_style_text_color(liangdu, lv_color_green(), 0);

		// 左2：guangzhao 数据标签
		lv_obj_t *guangzhao = lv_label_create(lv_scr_act());  
		lv_obj_align(guangzhao, LV_ALIGN_CENTER, -10, 30);  // 区域中心(120,150)
		lv_obj_set_width(guangzhao, 120);
		lv_obj_set_style_text_color(guangzhao, lv_color_golden(), 0);

		// 右1：汉字标签 空气质量
		lv_obj_t *kongqizhiliang = lv_label_create(lv_scr_act());
		lv_obj_set_style_text_font(kongqizhiliang, &my_font, 0);
		lv_label_set_text(kongqizhiliang, "空气质量");
		lv_obj_set_width(kongqizhiliang, 80);
		lv_obj_align(kongqizhiliang, LV_ALIGN_CENTER, 40, 30);  // 区域中心(200,150)
		lv_obj_set_style_text_color(kongqizhiliang, lv_color_green(), 0);

		// 右2：mq135data 数据标签
		lv_obj_t *mq135_data = lv_label_create(lv_scr_act());
		lv_obj_align(mq135_data, LV_ALIGN_CENTER, 150, 30);  // 区域中心(280,150)
		lv_obj_set_width(mq135_data, 120);
		lv_obj_set_style_text_color(mq135_data, lv_color_golden(), 0);


		// ------------------- 第四行（下中部，Y=180~240）-------------------
		// 左1：汉字标签 湿度
		lv_obj_t *xianzaishidu = lv_label_create(lv_scr_act());
		lv_obj_set_style_text_font(xianzaishidu, &my_font, 0);
		lv_label_set_text(xianzaishidu, "湿度");
		lv_obj_set_width(xianzaishidu, 80);
		lv_obj_align(xianzaishidu, LV_ALIGN_CENTER, -120, 90);  // 区域中心(40,210)
		lv_obj_set_style_text_color(xianzaishidu, lv_color_green(), 0);

		// 左2：shidu 数据标签
		lv_obj_t *shidu = lv_label_create(lv_scr_act());  
		lv_obj_align(shidu, LV_ALIGN_CENTER, -10, 90);  // 区域中心(120,210)
		lv_obj_set_width(shidu, 120);
		lv_obj_set_style_text_color(shidu, lv_color_golden(), 0);

		// 右1：汉字标签 温度
		lv_obj_t *xianzaiwendu = lv_label_create(lv_scr_act());
		lv_obj_set_style_text_font(xianzaiwendu, &my_font, 0);
		lv_label_set_text(xianzaiwendu, "温度");
		lv_obj_set_width(xianzaiwendu, 80);
		lv_obj_align(xianzaiwendu, LV_ALIGN_CENTER, 40, 90);  // 区域中心(200,210)
		lv_obj_set_style_text_color(xianzaiwendu, lv_color_green(), 0);

		// 右2：wendu 数据标签
		lv_obj_t *wendu = lv_label_create(lv_scr_act());  
		lv_obj_align(wendu, LV_ALIGN_CENTER, 150, 90);  // 区域中心(280,210)
		lv_obj_set_width(wendu, 120);
		lv_obj_set_style_text_color(wendu, lv_color_golden(), 0);
    for (;;)
    {
		if(PIR_Scan() == 1)	// 检测到有人
		{
			pir_timer = 0;		// 清空计时
			LCD_LED = 1;		// 打开背光（亮屏）
		}
		else	// 没人
		{
			pir_timer++;
			if(pir_timer >= PIR_OFF_TIME * 2)	// 任务延时50ms，*2=1秒
			{
				LCD_LED = 0;	// 关闭背光（熄屏）
			}
		}

			// 等待位0置为1，第一个pdTRUE表示执行本函数后会清除标志位，第二个pdFALSE表示逻辑或触发的（仅修改变量名）
		//	if(xEventGroupWaitBits(event1_group_handle, BIT_0|BIT_1, pdTRUE, pdFALSE, portMAX_DELAY)==pdPASS)
		//	{
			if(xQueueReceive(msg_queue_handle,&recv_msg,0)==pdPASS)
			{

				if(recv_msg.type==msg_type_rtc)
				{
					sprintf(time_buf, "20%02d-%02d-%02d\n%02d\n%02d:%02d:%02d",
					recv_msg.date.RTC_Year,
					recv_msg.date.RTC_Month,
					recv_msg.date.RTC_Date,
					recv_msg.date.RTC_WeekDay,
					recv_msg.time.RTC_Hours,
					recv_msg.time.RTC_Minutes,
					recv_msg.time.RTC_Seconds
						);
					lv_label_set_text(shijian, time_buf);
					}
					
					if(recv_msg.type==msg_type_adc)
					{
					sprintf(adc_buf, "%02d\r\n", recv_msg.adc_val);
					lv_label_set_text(guangzhao, adc_buf);
						}
					if(recv_msg.type==msg_type_dht11)
					{

					sprintf(dht11_wendu, "%dC", recv_msg.wenshidu[2]);
				  lv_label_set_text(wendu, dht11_wendu);
					last_temp = recv_msg.wenshidu[2];   // 缓存温度
					
					sprintf(dht11_shidu, "%d%%RH", recv_msg.wenshidu[0]);
					lv_label_set_text(shidu, dht11_shidu);
						last_humi = recv_msg.wenshidu[0];   // 缓存湿度
					}
					// MQ2 烟雾传感器（float 类型，显示两位小数）
					if (recv_msg.type == msg_type_mq2)
					{
							char mq2_buf[8];
							sprintf(mq2_buf, "%.2f", recv_msg.mq2_num);
							lv_label_set_text(mq2_data, mq2_buf);
						last_mq2 = recv_msg.mq2_num;
					}

					// MQ7 CO传感器（float 类型，显示两位小数）
					if (recv_msg.type == msg_type_mq7)
					{
							char mq7_buf[8];
							sprintf(mq7_buf, "%.2f", recv_msg.mq7_num);
							lv_label_set_text(mq7_data, mq7_buf);
						last_mq7 = recv_msg.mq7_num;
					}

					// MQ135 空气质量传感器（uint16_t 类型，显示整数）
					if (recv_msg.type == msg_type_mq135)
					{
							char mq135_buf[8];
							sprintf(mq135_buf, "%d", recv_msg.mq135_num);
							lv_label_set_text(mq135_data, mq135_buf);
					}
										if (EQ_3_3(last_mq2) || EQ_3_3(last_mq7) || last_temp > 36 || last_humi > 90)
					{
							BUZZER(OFF);
						
					}
					else
					{
							BUZZER(OFF);
					}		
			}
			//天气信息
			if(xQueueReceive(weather_queue_handle, &weather_msg, 0) == pdPASS)
			{
					if(weather_msg.sendweather == msg_type_weather)   // 注意使用 sendweather
					{
							char weather_buf[64];
							sprintf(weather_buf, "%s\r\n%s", weather_msg.weather_name, weather_msg.weather_text);
							lv_label_set_text(weather_display, weather_buf);
					}
			}
       vTaskDelay(50);
    }
}
// 月份英文缩写转数字 (Jan=1, Feb=2, ..., Dec=12)
static uint8_t month_str_to_num(const char *month_str)
{
    const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    for (int i = 0; i < 12; i++) {
        if (strcmp(month_str, months[i]) == 0)
            return i + 1;
    }
    return 0; // 无效返回0
}

// 星期英文缩写转数字 (Mon=1, Tue=2, ..., Sun=7)  注：根据STM32 RTC惯例，周一=1，周日=7
static uint8_t weekday_str_to_num(const char *weekday_str)
{
    const char *weekdays[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
    for (int i = 0; i < 7; i++) {
        if (strcmp(weekday_str, weekdays[i]) == 0)
            return i + 1;
    }
    return 0; // 无效返回0
}


/**
  * @brief  RTC任务
  * @note   None
  * @param  None
  * @retval None
  */

static void app_task4_rtc(void *pvParameters)
{
    time_struct recv_msg;
    MsgStructDef msg;

    // 初始设置一个默认时间（可根据需求修改）
    RTC_SetDateTime(0, 0, 0, 0, 0, 0, 0);  // 年,月,日,星期,时,分,秒

    for (;;)
    {
       // memset(&msg, 0, sizeof(MsgStructDef));

			//2、获取当前时间(指得是你设置的日期)
				RTC_GetTime(RTC_Format_BIN, &msg.time);	
				RTC_GetDate(RTC_Format_BIN, &msg.date);
		
        msg.type = msg_type_rtc;
        // 非阻塞接收队列消息（处理网络时间同步）
        if (xQueueReceive(time_queue_handle, &recv_msg, 0) == pdPASS)
        {
            if (recv_msg.sendtime == msg_type_esp8266_time)
            {
                // 确保字符串以'\0'结尾（假设发送端已正确填充，此处再保证一下）
                recv_msg.esp_time.year[15] = '\0';
                recv_msg.esp_time.month[15] = '\0';
                recv_msg.esp_time.date[15] = '\0';
                recv_msg.esp_time.weekday[15] = '\0';
                recv_msg.esp_time.hour[15] = '\0';
                recv_msg.esp_time.min[15] = '\0';
                recv_msg.esp_time.second[15] = '\0';

                // 转换年份（取后两位，例如2026 -> 26）
                int year_full = atoi(recv_msg.esp_time.year);
                msg.date.RTC_Year = year_full % 100;

                // 转换月份（英文 -> 数字）
                msg.date.RTC_Month = month_str_to_num(recv_msg.esp_time.month);

                // 转换日期
                msg.date.RTC_Date = atoi(recv_msg.esp_time.date);

                // 转换星期（英文 -> 数字）
                msg.date.RTC_WeekDay = weekday_str_to_num(recv_msg.esp_time.weekday);

                // 转换时分秒
                msg.time.RTC_Hours   = atoi(recv_msg.esp_time.hour)+8;
                msg.time.RTC_Minutes = atoi(recv_msg.esp_time.min);
                msg.time.RTC_Seconds = atoi(recv_msg.esp_time.second);
                // 验证转换结果有效性
                if (msg.date.RTC_Year >= 0 && msg.date.RTC_Year <= 99 &&
                    msg.date.RTC_Month >= 1 && msg.date.RTC_Month <= 12 &&
                    msg.date.RTC_Date >= 1 && msg.date.RTC_Date <= 31 &&
                    msg.date.RTC_WeekDay >= 1 && msg.date.RTC_WeekDay <= 7 &&
                    msg.time.RTC_Hours <= 23 && msg.time.RTC_Minutes <= 59 && msg.time.RTC_Seconds <= 59)
                {
									// 设置RTC硬件时间
								RTC_SetDateTime(msg.date.RTC_Year, msg.date.RTC_Month, msg.date.RTC_Date, msg.time.RTC_Hours,
																msg.time.RTC_Minutes, msg.time.RTC_Seconds,
																msg.date.RTC_WeekDay);
			
							//	msg.type=msg_type_rtc;
										
                }
//                else
//                {
//                    printf("【警告】网络时间解析失败(无效数据): year=%d, month=%d, date=%d, wday=%d, time=%02d:%02d:%02d\r\n",
//                           msg.date.RTC_Year, msg.date.RTC_Month, msg.date.RTC_Date,
//                           msg.date.RTC_WeekDay, msg.time.RTC_Hours,
//                           msg.time.RTC_Minutes, msg.time.RTC_Seconds);
//                }
								
            }
        }

				// 将消息发送到队列供其他任务使用
        xQueueSend(msg_queue_handle, &msg, 0);

        // 其他任务逻辑... (例如定期读取RTC并更新UI等)
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/**
* @brief 任务5：ADC光照采集任务
  * @note   None
  * @param  None
  * @retval None
  */
static void app_task5_adc(void *pvParameters)
{
			// adc外设相关变量区域
	//	uint16_t adc_light_val   = 0;
		vTaskDelay(2000);
		MsgStructDef msg;
	
	
    for (;;)
    {
			// 获取锁
		//	xSemaphoreTake(adc_rtc_sem_mutex_handle, portMAX_DELAY);   
			memset(&msg, 0, sizeof(MsgStructDef));
			//1、ADC转换值
			msg.adc_val = ADC3_IN5_GetLightVal();
		
			//2、电压值
	//		msg.adc_val = adc_light_val*3300/0xFFF;;
			msg.type=msg_type_adc;
		//	msg.adc_val=adc_light_val;
			xQueueSend(msg_queue_handle,&msg,0);
//			printf("ADC光照采集值：%d\r\n", msg.adc_val);
			//xEventGroupSetBits(event1_group_handle,  BIT_1);
			
			// 释放锁
			//xSemaphoreGive(adc_rtc_sem_mutex_handle);

       vTaskDelay(500);
    }
}

/**
* @brief 任务6：dht11温湿度采集任务
  * @note   None
  * @param  None
  * @retval None
  */
static void app_task6_dht11(void *pvParameters)
{


		MsgStructDef msg;
		uint8_t dht11_ret=0;
		//memset(msg.wenshidu,0,sizeof(msg.wenshidu));
	
    for (;;)
    {
			memset(&msg, 0, sizeof(MsgStructDef));
			if(dht11_ret==DHT11_GetData(msg.wenshidu))
			{
			
			msg.type=msg_type_dht11;
			printf("【DHT11温湿度】温度：%d℃  湿度：%d%%RH\r\n", 
                   msg.wenshidu[0],  // 温度整数部分
                   msg.wenshidu[1]); // 湿度整数部分
				
			xQueueSend(msg_queue_handle,&msg,0);
			}

			

	

       vTaskDelay(6000);
    }
}




/**
	* @brief 任务8 asrpro  语音任务
  * @note   None
  * @param  None
  * @retval None
  */
static void app_task8_asrpro(void *pvParameters)
{
    

    /* 3. 主循环：每秒读取一次并打印 */
    for (;;) {
       
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}


/**
	* @brief 任务9 ,mq2  烟雾任务
  * @note   None
  * @param  None
  * @retval None
  */
static void app_task9_mq2(void *pvParameters)
{
    MsgStructDef msg;
		 //float voltage;
		//uint8_t alarm;
	
    /* 3. 主循环：每秒读取一次并打印 */
    for (;;) {
			memset(&msg, 0, sizeof(MsgStructDef));
			msg.type=msg_type_mq2;
			msg.mq2_num	= Get_MQ2_AO_Voltage();   // 读取模拟电压
      //  alarm = Get_MQ2_DO_Level();       // 读取数字报警信号
	//		printf("task9: %.2f V\r\n", msg.mq2_num);
			xQueueSend(msg_queue_handle,&msg,0);
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
/**
 * @brief 任务10 ,mq7  CO任务
 * @note   采用MQ2任务格式，通过消息队列发送模拟电压值
 * @param  None
 * @retval None
 */
static void app_task10_mq7(void *pvParameters)
{
    MsgStructDef msg;
    MsgStructDef msg135;
    for (;;) {
        memset(&msg, 0, sizeof(MsgStructDef));
        msg.type = msg_type_mq7;                    // 假设已定义 msg_type_mq7
        msg.mq7_num = Get_MQ7_AO_Voltage();         // 读取模拟电压，存入对应字段
   //     printf("task10: %.2f V\r\n", msg.mq7_num);
        xQueueSend(msg_queue_handle, &msg, 0);
        vTaskDelay(pdMS_TO_TICKS(1000));
				memset(&msg135, 0, sizeof(MsgStructDef));
        msg135.type = msg_type_mq135;                  // 假设已定义 msg_type_mq135
        msg135.mq135_num = Get_MQ135_DO_Level();     // 读取报警信号（0或1），存入对应字段
       // printf("task11: %d\r\n", msg.mq135_num);
        xQueueSend(msg_queue_handle, &msg135,0);
        vTaskDelay(pdMS_TO_TICKS(1000));

    }
}
/**
 * @brief 任务11 ,mq135  空气质量任务
 * @note   采用MQ2任务格式，通过消息队列发送数字报警状态
 * @param  None
 * @retval None
 */
static void app_task11_mq135(void *pvParameters)
{
  //  MsgStructDef msg135;
    for (;;) {
//				memset(&msg135, 0, sizeof(MsgStructDef));
//        msg135.type = msg_type_mq135;                  // 假设已定义 msg_type_mq135
//        msg135.mq135_num = Get_MQ135_DO_Level();     // 读取报警信号（0或1），存入对应字段
//       // printf("task11: %d\r\n", msg.mq135_num);
//        xQueueSend(msg_queue_handle, &msg135,0);
//        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/**
 * @brief 任务12 esp8266  wifi+时间加天气
 * @note   采用MQ2任务格式，通过消息队列发送时间网络完成
 * @param  None
 * @retval None
 */
static void app_task12_esp8266(void *pvParameters)
{
    time_struct msg;
			
			//time_t time = {0};
//printf(">>> app_task12_esp8266 ENTERED <<<\r\n");   // <--- 加这一
		// vTaskDelay(pdMS_TO_TICKS(1000));
	//	delay_ms(500);
	printf("This is ESP8266模块(实现连接热点(实现上网)) test\r\n");			
	// 1、使得你的ESP8266模块连接热点，能够上网
	//printf("##### ESP8266模块连接网络中 #####\r\n");
	while(ESP8266_OnLineInit() != 0)
	{
		//printf("ESP8266模块连接网络中.......\r\n\r\n");
		 vTaskDelay(pdMS_TO_TICKS(1000));
	}
	printf("##### ESP8266模块连接网络成功! #####\r\n");
	
	

    for (;;) {
					// 获取锁
				xSemaphoreTake(esptime_weather_sem_mutex_handle, portMAX_DELAY);  
        memset(&msg, 0, sizeof(time_struct));
				msg.sendtime=msg_type_esp8266_time;
				
       	// 2、获取苏宁易购前端网页服务器的时间
				//printf("\r\n");
			//	printf("##### 获取苏宁易购网络时间中 #####\r\n");
				ESP8266_GetTimeFunc(&msg.esp_time);
				//printf("获取苏宁易购网络时间为 == %d: %s: %s\r\n", (atoi(msg.esp_time.hour)+8)%24, msg.esp_time.min, msg.esp_time.second);
				//printf("##### 获取苏宁易购网络时间成功 #####\r\n");
				
				//printf("苏宁易购时间: year=%s month=%s date=%s hour=%s min=%s sec=%s weekday=%s\r\n",
//				msg.esp_time.year, msg.esp_time.month, msg.esp_time.date,
//				msg.esp_time.hour, msg.esp_time.min, msg.esp_time.second,
//				msg.esp_time.weekday);
				vTaskDelay(pdMS_TO_TICKS(1000));

				xQueueSend(time_queue_handle, &msg, 5);
				xEventGroupSetBits(event1_group_handle,  BIT_0);
							// 释放锁
				xSemaphoreGive(esptime_weather_sem_mutex_handle);	
        vTaskDelay(pdMS_TO_TICKS(1800000));
    }
}

/**
 * @brief 任务13 天气
 * @note   通过消息队列发送天气网络完成
 * @param  None
 * @retval None
 */
static void app_task13_weather(void *pvParameters)
{
				//sgStructDef msg;
				 weather_struct msg;
    for (;;) {
				xEventGroupWaitBits(event1_group_handle, BIT_0, pdFALSE, pdFALSE, portMAX_DELAY);
					// 获取锁
				xSemaphoreTake(esptime_weather_sem_mutex_handle, portMAX_DELAY);  
				memset(&msg, 0, sizeof(weather_struct));
			  //msg.type=msg_type_weather;
				msg.sendweather=msg_type_weather;
				printf("##### 获取天气中 #####\r\n");
				ESP8266_GetWeatherFunc(XZTQ_KEY, "guangzhou");;
			
				printf("##### 获取天气中成功 #####\r\n");
				strcpy(msg.weather_name,results[0].location.name);
				strcpy(msg.weather_text,results[0].now.text);
				// 释放锁
				printf("城市：%s\n", 	msg.weather_name);
				printf("天气：%s\n", msg.weather_text);
				xQueueSend(weather_queue_handle, &msg, 5);				
				xSemaphoreGive(esptime_weather_sem_mutex_handle);	
			
				vTaskDelay(pdMS_TO_TICKS(1000));
    }
}