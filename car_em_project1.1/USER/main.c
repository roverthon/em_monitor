/**
  ******************************************************************************
  * @file    buzzer.h
  * @author  粤嵌温工+FZetc飞贼整理
  * @version V0.0.1
  * @date    2026.04.19
  * @brief   说明：
  *				LVGL配置与裁剪
  *          操作1：
  *            	1、保证stm32f4xx.h文件123行的HSE_VALUE时钟源值为8000000，保证system_stm32f4xx.c文件的316行的PLL_M系数为8
  *			   	2、GEC-M4板子上丝印层UART1跳线帽处，需要短接1-3和2-4，A-A口线要连接电脑和单片机USB口，保证调试通信和5V供电
  *            	3、串口助手的端口要打开连接开发板的那个串口端口，串口助手的波特率要和单片机串口程序保持一致
  *            	4、保证keil5软件和串口助手都是一个编码格式
  *
  *			 操作2：使用温工的porthelper.exe软件，生成字库(看笔记)，名字建议命名lvgl_font即可
  *			 操作3：使用温工的porthelper.exe软件，生成图库(看笔记)，名字以你图片名字命名即可	
  *
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
	*				任务3：app_task3_lvgl 界面任务
	*				任务4：app_task4_rtc 时间任务
	*				任务5：app_task5_adc 采集任务
	*				任务6：	
	*				事件标志组：
	*								event1_group_handle（rtc->lvgl)
	*				互斥锁：
	*								adc_rtc_sem_mutex_handle;（adc<->rtc)
	*				消息队列：
	*								
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


#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lv_demo_widgets.h"

#include "adc.h"
#include "FreeRTOS.h"		// 选编译配置文件，用于汇总所有源文件的编译选择控制
#include "task.h"			// 任务相关函数头文件
#include "semphr.h"			// 信号量头文件
#include "event_groups.h"	// 事件标志组头文件
#include "queue.h"			// 消息队列头文件
#include "rtc.h"
#include "lvgl.h"

#define  QUEUE_LEN    4   	/* 队列的长度，最大可包含多少个消息 */
#define  QUEUE_SIZE   64  	/* 队列中每个消息大小（字节） */


// 位的设置
#define BIT_0 (1<<0)
#define BIT_1 (1<<1)
#define BIT_2 (1<<2)
#define BIT_3 (1<<3)
#define BIT_4 (1<<4)
#define BIT_5 (1<<5)
#define BIT_6 (1<<6)
#define BIT_7 (1<<7)


/* 任务句柄 */
TaskHandle_t app_task1_init_handle      = NULL;
TaskHandle_t app_task2_lvgl_task_handle = NULL;
TaskHandle_t app_task3_lvgl_handle	    = NULL;
TaskHandle_t app_task4_rtc_handle	    = NULL;
TaskHandle_t app_task5_adc_handle	    = NULL;

/* 任务函数*/
static void app_task1_init(void *pvParameters);
static void app_task2_lvgl_task(void *pvParameters);
static void app_task3_lvgl(void *pvParameters);
static void app_task4_rtc(void *pvParameters);
static void app_task5_adc(void *pvParameters);

// 事件标志组（仅修改变量名）
static EventGroupHandle_t event1_group_handle;
// 互斥信号量
static SemaphoreHandle_t adc_rtc_sem_mutex_handle;
//消息队列
static QueueHandle_t adc_to_lvgl_msg_queue_handle = NULL;			
static QueueHandle_t rtc_to_lvgl_msg_queue_handle = NULL;

RTC_DateTypeDef  RTC_DateStruct;					// RTC日期配置
RTC_TimeTypeDef  RTC_TimeStruct;					// RTC时间配置
	// adc外设相关变量区域
	uint16_t adc_light_val   = 0;
	uint16_t adc_voltage_val = 0;
	
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
	

	// 三、创建事件标志组
	event1_group_handle    = xEventGroupCreate();
	
	//创建互斥信号量
	adc_rtc_sem_mutex_handle = xSemaphoreCreateMutex();
	
	 // 三、创建消息队列、  
   adc_to_lvgl_msg_queue_handle  = xQueueCreate(QUEUE_LEN, QUEUE_SIZE); 
	
	
	/* 创建app_task_init任务 */
    xTaskCreate((TaskFunction_t)app_task1_init,          /* 任务入口函数 */
                (const char *)"app_task1_init",          /* 任务名字 */
                (uint16_t)512,                           /* 任务栈大小 */
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
	

	
	// 二、外设初始化区域(硬件初始化)
	// NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);			// 选择中断分组为第2组(抢占式优先级：4级， 响应式优先级：4级)，上面已经分好组了
	// DELAY_SysTickInit(168);									// 系统定时器的时钟源初始化(168MHZ或21MHZ)，被RTOS接管了，所以不弄了
	UART1_Init(115200);											// 串口1初始化为115200
	
	LED_Init();													// led、蜂鸣器、按键初始化
	BUZZER_Init();
	KEY_Init();
	RTC_ConfigInit();									// RTC时钟初始化并设置日期时间		
	
	lv_init();													// 初始化lvgl
	lv_port_disp_init();										// 初始化lvgl显示设备
	lv_port_indev_init();										// 初始化lvgl输入设备
	ADC3_IN5_LightInit();								// ADC3通道5的初始化(光敏传感器)
	// 三、外设具现化区域(任务创建的地方)
	// 1、创建app_task2_lvgl_task任务
	xReturned = xTaskCreate(app_task2_lvgl_task,"app_task2_lvgl_task", 512, NULL,  7, &app_task2_lvgl_task_handle);     
	if( xReturned == pdPASS )
    {
		printf("app_task2_lvgl_task create success!\r\n");
    }
	
	// 2、创建app_task3_lvgl界面任务
	xReturned = xTaskCreate(app_task3_lvgl,"app_task3_lvgl", 512, NULL,  6, &app_task3_lvgl_handle);     
	if( xReturned == pdPASS )
    {
		printf("app_task3_lvgl create success!\r\n");
    }
	
	// 3、创建app_task4_rtc时间任务
	xReturned = xTaskCreate(app_task4_rtc,"app_task4_rtc", 128, NULL,  6, &app_task4_rtc_handle);     
	if( xReturned == pdPASS )
    {
		printf("app_task4_rtc create success!\r\n");
    }
	// 4、创建app_task5_adc采集任务
	xReturned = xTaskCreate(app_task5_adc,"app_task5_adc", 128, NULL,  6, &app_task5_adc_handle);     
	if( xReturned == pdPASS )
    {
		printf("app_task5_adc create success!\r\n");
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
	

	// 1. 创建中心时间标签 shijian
	lv_obj_t *shijian = lv_label_create(lv_scr_act());  // 创建标签控件
	lv_obj_align(shijian, LV_ALIGN_CENTER, 0, 0);       // 屏幕设置
	lv_obj_set_width(shijian, 200);
	lv_obj_set_style_text_color(shijian, lv_color_black(), 0);     // 设置文字颜色为黑色
	char time_buf[32];
		// 1. 创建标签 guangzhao
	lv_obj_t *guangzhao = lv_label_create(lv_scr_act());  // 创建标签控件
	lv_obj_align(guangzhao, LV_ALIGN_CENTER, 0, 40);      		 // 屏幕设置
	lv_obj_set_width(shijian, 200);
	lv_obj_set_style_text_color(guangzhao, lv_color_black(), 0);     // 设置文字颜色为黑色
	char adc_buf[32];
    for (;;)
    {
			// 等待位0置为1，第一个pdTRUE表示执行本函数后会清除标志位，第二个pdFALSE表示逻辑或触发的（仅修改变量名）
			xEventGroupWaitBits(event1_group_handle, BIT_0, pdTRUE, pdFALSE, portMAX_DELAY);
			
			sprintf(time_buf, "20%02d-%02d-%02d\n%02d\n%02d:%02d:%02d",
					RTC_DateStruct.RTC_Year,
					RTC_DateStruct.RTC_Month,
					RTC_DateStruct.RTC_Date,
					RTC_DateStruct.RTC_WeekDay,
					RTC_TimeStruct.RTC_Hours,
					RTC_TimeStruct.RTC_Minutes,
					RTC_TimeStruct.RTC_Seconds
			);
			sprintf(adc_buf, "%02d\r\n",
					adc_voltage_val
			);
			// 更新标签文本
			lv_label_set_text(shijian, time_buf);
			lv_label_set_text(guangzhao, adc_buf);
       vTaskDelay(5);
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
		
    for (;;)
    {
		// 获取锁
		xSemaphoreTake(adc_rtc_sem_mutex_handle, portMAX_DELAY);   
					
		//1、获取当前日期(指得是你设置的日期)
		RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
		
		//2、获取当前时间(指得是你设置的日期)
		RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
		
		
						
			// 设置RTC事件标志组位0为1（仅修改变量名）
			xEventGroupSetBits(event1_group_handle,  BIT_0);	


				// 释放锁
			xSemaphoreGive(adc_rtc_sem_mutex_handle);
       vTaskDelay(5);
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
		
		delay_ms(200);
		
	
    for (;;)
    {
			// 获取锁
			xSemaphoreTake(adc_rtc_sem_mutex_handle, portMAX_DELAY);   
			
			//1、ADC转换值
			adc_light_val = ADC3_IN5_GetLightVal();
		
			//2、电压值
			adc_voltage_val = adc_light_val*3300/0xFFF;;
		
			// 释放锁
			xSemaphoreGive(adc_rtc_sem_mutex_handle);

       vTaskDelay(5);
    }
}