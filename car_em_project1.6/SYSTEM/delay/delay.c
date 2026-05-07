#include "delay.h"
#include "FreeRTOS.h"				
#include "task.h"

static u8  fac_us=0;			// us延时倍乘数			   
static u16 fac_ms=0;			// ms延时倍乘数,在os下,代表每个节拍的ms数
	   
	   
/**
  * @brief  自写延时函数
  * @note   利用执行语句产生的时间，进行多个语句执行，实现不精准延时
  * @param  None
  * @retval None
  */ 
void DELAY_MyNms(uint16_t nms)
{
	int32_t i = 0;
	while(nms--)
	{
		i = 12000;
		while(i--);
	}
}

/**
  * @brief  初始化延迟函数
  * @note   SYSTICK的时钟固定为AHB时钟，基础例程里面SYSTICK时钟频率为AHB/8，这里为了兼容FreeRTOS，所以将SYSTICK的时钟频率改为AHB的频率！
  * @param  SYSCLK:系统时钟频率
  * @retval None
  */ 

void DELAY_SysTickInit(uint8_t sysclk)
{
	u32 reload;
 	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);   	// 频率168MHZ
	fac_us=sysclk;										// 不论是否使用OS,fac_us都需要使用
	reload=sysclk;										// 每秒钟的计数次数 单位为M	   
	reload*=1000000/configTICK_RATE_HZ;					// 根据delay_ostickspersec设定溢出时间
														// reload为24位寄存器,最大值:16777216,在168M下,约合0.0998s左右	
	fac_ms=1000/configTICK_RATE_HZ;						// 代表OS可以延时的最少单位	   
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;			// 开启SYSTICK中断
	SysTick->LOAD=reload; 								// 每1/configTICK_RATE_HZ断一次	
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk; 			// 开启SYSTICK     
}								    


/**
  * @brief  系统的微秒 
  * @note   None
  * @param  None
  * @retval None
  */ 
void delay_us(uint32_t nus)
{
	uint32_t told,tnow,tcnt=0;
	uint32_t temp=0;
	uint32_t reload=SysTick->LOAD;					//系统定时器的重载值
	uint32_t ticks=nus*(SystemCoreClock/1000000);	//总共要等待的滴答数目
	told=SysTick->VAL;        						//刚进入时的计数器值
	
	//挂起所有任务[可选]
	vTaskSuspendAll();
	
	while(1)
	{
		//获取当前计数值
		tnow=SysTick->VAL;					
		
		if(tnow!=told)
		{	    
			//SYSTICK是一个递减的计数器
			if(tnow<told)
				tcnt+=told-tnow;			
			else 
				tcnt+=reload-tnow+told;
			
			told=tnow;
			
			//时间超过/等于要延迟的时间,则退出.
			if(tcnt>=ticks)
				break;						
		}  
		
		temp=SysTick->CTRL;
		
		//若定时器中途关闭了，跳出循环
		if((temp & 0x01)==0)			
			break;
	}
	//恢复所有任务[可选]
	xTaskResumeAll();
}

/**
  * @brief  系统的毫秒 
  * @note   建议不要使用这个ms延时，使用系统提供的vTaskDelay ms延时
  * @param  None
  * @retval None
  */ 
void delay_ms(u32 nms)
{	
	if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)//系统已经运行
	{		
		if(nms>=fac_ms)						//延时的时间大于OS的最少时间周期 
		{ 
   			vTaskDelay(nms/fac_ms);	 		//FreeRTOS延时 
		}
		nms%=fac_ms;						//OS已经无法提供这么小的延时了,采用普通方式延时    
	}
	delay_us((u32)(nms*1000));				//普通方式延时
}
