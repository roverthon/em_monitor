/**
  ******************************************************************************
  * @file    main.h
  * @author  rovertyphon
  * @version V0.0.1
  * @date    2026.04.19
  * @brief   说明：
  *				LVGL配置与裁剪
  *          操作1：
  *            	1、保证stm32f4xx.h文件123行的HSE_VALUE时钟源值为8000000，保证system_stm32f4xx.c文件的316行的PLL_M系数为8
  *			   	2、GEC-M4板子上丝印层UART1跳线帽处，需要短接1-3和2-4，A-A口线要连接电脑和单片机USB口，保证调试通信和5V供电
  *            	3、串口助手的端口要打开连接开发板的那个串口端口，串口助手的波特率要和单片机串口程序保持一致
  *            	4、保证keil5软件和串口助手都是一个编码格式
  *							5.SGP30:
	*										SGP30(SDA)->DCMI_SCL->PD6
	*										SGP30(SCL)->DCMI__SDA->PD7
	*								MQ2:
	*										AIN->DCMI_HREF(CA5)->PA4
	*										DIN->DCMI_D1(CA9)->PC7
	*								mq7:
	*										ao->DCMI_PLCK(CA16)->PA6
	*										DO->DCMI_PWDN(CA18)->PG9
	*								mq135:
	*										do->DCMI_XCLK(ca17)->PA8
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
	* 			任务7：app_task7_sgp30  co2voc任务
	* 			任务8：app_task8_asrpro	语音任务
	*				任务9：app_task9_mq2			烟雾报警器
	*				任务10：app_task10_mq7		co报警器
	*				任务11:	app_task11_mq135 空气质量
	*				事件标志组：
	*								event1_group_handle（rtc->lvgl(BIT_0))(废)
	*				互斥锁：
	*								adc_rtc_sem_mutex_handle;（adc<->rtc)(废)
	*				消息队列：
	*								msg_queue_handle (rtc->lvgl;adc->lvgl，dht11->lvgl,)
  *			 如何优化内存：
  *				https://share.note.youdao.com/s/I59zDn9f(第7点)
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

#include "main.h"

#include "lcd_screen.h"
#include "lcd_touch.h"
#include "lcd_font.h"
#include "lcd_image.h"
#include "sensirion_arch_config.h"
#include "sensirion_common.h"
#include "sensirion_i2c.h"
#include "iic.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lv_demo_widgets.h"
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
#include "sgp30.h"
#include "asrpro.h"
#include "mq2.h"
#include "mq7.h"
#include "mq135.h"
// 位的设置
#define BIT_0 (1<<0)
#define BIT_1 (1<<1)
#define BIT_2 (1<<2)
#define BIT_3 (1<<3)
#define BIT_4 (1<<4)
#define BIT_5 (1<<5)
#define BIT_6 (1<<6)
#define BIT_7 (1<<7)

//队列配置
#define QUEUE_LEN 32
//创建消息队列格式——枚举和结构体
//定义类型
typedef enum {
	msg_type_adc,
	msg_type_rtc,
	msg_type_dht11,
	msg_type_sgp30,
	msg_type_mq2
} MsgTypeDef;
typedef struct {
    MsgTypeDef type;          // 数据类型标记
    uint16_t adc_val;         // ADC
    RTC_DateTypeDef date;     // RTC日期
    RTC_TimeTypeDef time;     // RTC时间
		uint8_t wenshidu[16];						//温湿度
		uint16_t	co2_num;				//co2
		uint16_t  voc_num;				//voc
		
} MsgStructDef;

/* 任务句柄 */
TaskHandle_t app_task1_init_handle      = NULL;
TaskHandle_t app_task2_lvgl_task_handle = NULL;
TaskHandle_t app_task3_lvgl_handle	    = NULL;
TaskHandle_t app_task4_rtc_handle	    = NULL;
TaskHandle_t app_task5_adc_handle	    = NULL;
TaskHandle_t app_task6_dht11_handle	    = NULL;
TaskHandle_t app_task7_sgp30_handle	    = NULL;
TaskHandle_t app_task8_asrpro_handle	    = NULL;
TaskHandle_t app_task9_mq2_handle	    = NULL;
TaskHandle_t app_task10_mq7_handle	    = NULL;
TaskHandle_t app_task11_mq135_handle	    = NULL;
/* 任务函数*/
static void app_task1_init(void *pvParameters);
static void app_task2_lvgl_task(void *pvParameters);
static void app_task3_lvgl(void *pvParameters);
static void app_task4_rtc(void *pvParameters);
static void app_task5_adc(void *pvParameters);
static void app_task6_dht11(void *pvParameters);
static void app_task7_sgp30(void *pvParameters);
static void app_task8_asrpro(void *pvParameters);
static void app_task9_mq2(void *pvParameters);
static void app_task10_mq7(void *pvParameters);
static void app_task11_mq135(void *pvParameters);
//// 事件标志组
//static EventGroupHandle_t event1_group_handle;
// 互斥信号量
//static SemaphoreHandle_t adc_rtc_sem_mutex_handle;
//消息队列
static QueueHandle_t msg_queue_handle = NULL;
	
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
//	event1_group_handle    = xEventGroupCreate();
	//创建互斥信号量
	//adc_rtc_sem_mutex_handle = xSemaphoreCreateMutex();
	
	 // 三、创建消息队列、  
   msg_queue_handle  = xQueueCreate(QUEUE_LEN, sizeof(MsgStructDef)); 
	
	
	/* 创建app_task_init任务 */
    xTaskCreate((TaskFunction_t)app_task1_init,          /* 任务入口函数 */
                (const char *)"app_task1_init",          /* 任务名字 */
                (uint16_t)1024,                           /* 任务栈大小 */
                (void *)NULL,                            /* 任务入口函数参数 */
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
	

	UART1_Init(115200);											// 串口1初始化为115200
	IIC_Init();
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
	sensirion_i2c_init();
	 MQ2_Init();												//烟雾传感器初始化
	 MQ7_Init();												//CO传感器
		 MQ135_Init();												//空气质量传感器
	// 三、外设具现化区域(任务创建的地方)
	// 1、创建app_task2_lvgl_task任务
	xReturned = xTaskCreate(app_task2_lvgl_task,"app_task2_lvgl_task", 512, NULL,  7, &app_task2_lvgl_task_handle);     
	if( xReturned == pdPASS )
    {
		printf("app_task2_lvgl_task create success!\r\n");
    }
	
	// 2、创建app_task3_lvgl界面任务
	xReturned = xTaskCreate(app_task3_lvgl,"app_task3_lvgl", 1024, NULL,  6, &app_task3_lvgl_handle);     
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
	xReturned = xTaskCreate(app_task5_adc,"app_task5_adc", 256, NULL,  6, &app_task5_adc_handle);     
	if( xReturned == pdPASS )
    {
		printf("app_task5_adc create success!\r\n");
    }
		// 4、创建app_task6_dht11温湿度任务
	xReturned = xTaskCreate(app_task6_dht11,"app_task6_dht11", 256, NULL,  6, &app_task6_dht11_handle);     
	if( xReturned == pdPASS )
    {
		printf("app_task6_dht11 create success!\r\n");
    }
		// 4、创建app_task7_sgp30 co2voc任务
	xReturned = xTaskCreate(app_task7_sgp30,"app_task7_sgp30", 512, NULL,  5, &app_task7_sgp30_handle);     
	if( xReturned == pdPASS )
    {
		printf("app_task7_sgp30 create success!\r\n");
    }
		// 4、创建app_task8_sgp30 co2voc任务
	xReturned = xTaskCreate(app_task8_asrpro,"app_task8_asrpro", 512, NULL,  5, &app_task8_asrpro_handle);     
	if( xReturned == pdPASS )
    {
		printf("app_task8_asrpro create success!\r\n");
    }
		// 4、创建app_task9_mq2 烟雾采集器任务
	xReturned = xTaskCreate(app_task9_mq2,"app_task9_mq2", 512, NULL,  5, &app_task9_mq2_handle);     
	if( xReturned == pdPASS )
    {
		printf("app_task9_mq2 create success!\r\n");
    }
		// 4、创建app_task10_mq7 CO采集器任务
	xReturned = xTaskCreate(app_task10_mq7,"app_task10_mq7", 512, NULL,  5, &app_task10_mq7_handle);     
	if( xReturned == pdPASS )
    {
		printf("app_task10_mq7 create success!\r\n");
    }
		// 4、创建app_task11_mq135 空气质量任务
	xReturned = xTaskCreate(app_task11_mq135,"app_task11_mq135", 512, NULL,  7, &app_task11_mq135_handle);     
	if( xReturned == pdPASS )
    {
		printf("app_task11_mq135 create success!\r\n");
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
	char time_buf[32];
	char adc_buf[32];
	char dht11_wendu[16];
	char dht11_shidu[16];
	LV_IMG_DECLARE(image_chezaibeijing);
	//创建背景beijing
	  lv_obj_t *beijing= lv_img_create(lv_scr_act());                           	// 在当前活动屏幕上创建图像控件
    lv_img_set_src(beijing, &image_chezaibeijing);                                    	// 设置图像源
    lv_obj_align(beijing, LV_ALIGN_CENTER, 0, 0);                              	// 将图像控件居中对齐到屏幕
    lv_obj_set_size(beijing, image_chezaibeijing.header.w, image_chezaibeijing.header.h);    	// 设置图像控件的大小

	
	// 1. 创建中心时间标签 shijian
	lv_obj_t *shijian = lv_label_create(lv_scr_act());  // 创建标签控件
	lv_obj_align(shijian, LV_ALIGN_CENTER, 0, 0);       // 屏幕设置
	lv_obj_set_width(shijian, 200);
	lv_obj_set_style_text_color(shijian, lv_color_golden(), 0);     // 设置文字颜色为黑色
	
//		// 1. 创建温度标签 wendu
	lv_obj_t *wendu = lv_label_create(lv_scr_act());  // 创建标签控件
	lv_obj_align(wendu, LV_ALIGN_CENTER, 0, 60);       // 屏幕设置
	lv_obj_set_width(wendu, 200);
	lv_obj_set_style_text_color(wendu, lv_color_golden(), 0);     // 设置文字颜色为黑色
	
		// 1. 创建湿度标签 shidu
	lv_obj_t *shidu = lv_label_create(lv_scr_act());  // 创建标签控件
	lv_obj_align(shidu, LV_ALIGN_CENTER, 0, 70);       // 屏幕设置
	lv_obj_set_width(shidu, 200);
	lv_obj_set_style_text_color(shidu, lv_color_golden(), 0);     // 设置文字颜色为黑色
	
	
	
		// 1. 创建标签光照guangzhao
	lv_obj_t *guangzhao = lv_label_create(lv_scr_act());  // 创建标签控件
	lv_obj_align(guangzhao, LV_ALIGN_CENTER, 0, 40);      		 // 屏幕设置
	lv_obj_set_width(guangzhao, 200);
	lv_obj_set_style_text_color(guangzhao, lv_color_golden(), 0);     // 设置文字颜色为黑色
 
    for (;;)
    {
			// 等待位0置为1，第一个pdTRUE表示执行本函数后会清除标志位，第二个pdFALSE表示逻辑或触发的（仅修改变量名）
		//	if(xEventGroupWaitBits(event1_group_handle, BIT_0|BIT_1, pdTRUE, pdFALSE, portMAX_DELAY)==pdPASS)
		//	{
			if(xQueueReceive(msg_queue_handle,&recv_msg,0)==pdPASS)
			{
//				switch(recv_msg.type)
//			{
//				case msg_type_rtc:
//					sprintf(time_buf, "20%02d-%02d-%02d\r\n%d\r\n%02d:%02d:%02d",
//					recv_msg.date.RTC_Year, recv_msg.date.RTC_Month, recv_msg.date.RTC_Date,
//					recv_msg.date.RTC_WeekDay,
//					recv_msg.time.RTC_Hours, recv_msg.time.RTC_Minutes, recv_msg.time.RTC_Seconds);
//					lv_label_set_text(shijian, time_buf);
//					break;
//				
//				case msg_type_adc:
//					sprintf(adc_buf, "%d", recv_msg.adc_val);
//					lv_label_set_text(guangzhao, adc_buf);
//					break;
//				
//				case msg_type_dht11:
//					sprintf(dht11_wendu, "%dC", recv_msg.wenshidu[2]);
//					lv_label_set_text(wendu, dht11_wendu);
//					
//					sprintf(dht11_shidu, "%d%%RH", recv_msg.wenshidu[0]);
//					lv_label_set_text(shidu, dht11_shidu);
//					break;
//				
//				default: break;
//			}
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
//						printf(adc_buf, "%02d\r\n", recv_msg.adc_val);
//						lv_label_set_text(guangzhao, adc_buf);
					sprintf(dht11_wendu, "%dC", recv_msg.wenshidu[2]);
				  lv_label_set_text(wendu, dht11_wendu);
					
					sprintf(dht11_shidu, "%d%%RH", recv_msg.wenshidu[0]);
					lv_label_set_text(shidu, dht11_shidu);
					}
			}
       vTaskDelay(50);
    }
}

/**
  * @brief RTC时间业务任务
  * @note   None
  * @param  None
  * @retval None
  */
static void app_task4_rtc(void *pvParameters)
{
		
		RTC_SetDateTime(26,04,20, 10, 15, 00, 1);
		MsgStructDef msg;

    for (;;)
    {
		memset(&msg, 0, sizeof(MsgStructDef));
		// 获取锁
		//xSemaphoreTake(adc_rtc_sem_mutex_handle, portMAX_DELAY);   
		//1、获取当前日期(指得是你设置的日期)
		RTC_GetDate(RTC_Format_BIN, &msg.date);
		
		//2、获取当前时间(指得是你设置的日期)
		RTC_GetTime(RTC_Format_BIN, &msg.time);					
		
		msg.type=msg_type_rtc;
			printf("RTC时间：20%02d-%02d-%02d 星期%d %02d:%02d:%02d\r\n",
			   msg.date.RTC_Year,
			   msg.date.RTC_Month,
			   msg.date.RTC_Date,
			   msg.date.RTC_WeekDay,
			   msg.time.RTC_Hours,
			   msg.time.RTC_Minutes,
			   msg.time.RTC_Seconds);
		xQueueSend(msg_queue_handle,&msg,0);
						
			// 设置RTC事件标志组位0为1（仅修改变量名）
		//xEventGroupSetBits(event1_group_handle,  BIT_0);	


				// 释放锁
			//xSemaphoreGive(adc_rtc_sem_mutex_handle);
       vTaskDelay(900);
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

       vTaskDelay(50);
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
	* @brief 任务7 sgp30  co2voc采集任务
  * @note   None
  * @param  None
  * @retval None
  */
static void app_task7_sgp30(void *pvParameters)
{
    int16_t err;
    uint16_t tvoc_ppb, co2_eq_ppm;

    /* 1. 探测 SGP30（反复直到成功） */
    while (sgp_probe() != STATUS_OK) {
        printf("SGP30 probe failed, retrying...\r\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    printf("SGP30 probe success\r\n");

    /* 2. 初始化 IAQ 基线（重置为默认值） */
    if (sgp_iaq_init() != STATUS_OK) {
        printf("IAQ init failed\r\n");
    } else {
        printf("IAQ init done\r\n");
    }

    /* 3. 主循环：每秒读取一次并打印 */
    for (;;) {
        err = sgp_measure_iaq_blocking_read(&tvoc_ppb, &co2_eq_ppm);
        if (err == STATUS_OK) {
            printf("CO2eq: %d ppm, TVOC: %d ppb\r\n", co2_eq_ppm, tvoc_ppb);
        } else {
            printf("IAQ measurement error\r\n");
        }
        /* 精确延时 1 秒（满足 SGP30 要求） */
        vTaskDelay(pdMS_TO_TICKS(1000));
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
       
        vTaskDelay(pdMS_TO_TICKS(1000));
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
    
		 float voltage;
		uint8_t alarm;
    /* 3. 主循环：每秒读取一次并打印 */
    for (;;) {
        voltage = Get_MQ2_AO_Voltage();   // 读取模拟电压
      //  alarm = Get_MQ2_DO_Level();       // 读取数字报警信号
			printf("task9: %.2f V\r\n", voltage);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
/**
	* @brief 任务10 ,mq7  CO任务
  * @note   None
  * @param  None
  * @retval None
  */
static void app_task10_mq7(void *pvParameters)
{
    
		 float voltage;
		uint8_t alarm;
    /* 3. 主循环：每秒读取一次并打印 */
    for (;;) {
        voltage = Get_MQ7_AO_Voltage();   // 读取模拟电压
      //  alarm = Get_MQ2_DO_Level();       // 读取数字报警信号
			printf("task10: %.2f V\r\n", voltage);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
/**
	* @brief 任务11 ,mq135  空气质量任务
  * @note   None
  * @param  None
  * @retval None
  */
static void app_task11_mq135(void *pvParameters)
{
    
		 //float voltage;
		uint8_t alarm;
    /* 3. 主循环：每秒读取一次并打印 */
    for (;;) {
       // voltage = Get_MQ1_AO_Voltage();   // 读取模拟电压
        alarm = Get_MQ135_DO_Level();       // 读取数字报警信号
			printf("task11: %d V\r\n", alarm);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}