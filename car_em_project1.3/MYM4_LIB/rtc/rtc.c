#include "rtc.h"

//1、RTC配置初始化
void RTC_ConfigInit(void)
{
    /* 0、外设信息配置结构体 */
	RTC_InitTypeDef  RTC_InitStructure;		//RTC基础配置

	/* 1、使能电源管理时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
		
	/* 2、允许使用RTC时钟 */
	PWR_BackupAccessCmd(ENABLE);

	/* 3、重置RTC域(注意：重置后，需重新开启RTC时钟) */
	//RCC_BackupResetCmd(ENABLE);
	//RCC_BackupResetCmd(DISABLE);
	
	/* 4、使能RTC时钟 */
	RCC_RTCCLKCmd(ENABLE);
	

#if SELECT_RTC_CLK_LSI 
	/* 4、使能内部低速时钟振荡器 */
	RCC_LSICmd(ENABLE);

	/* 5、等到内部低速时钟振荡器准备好 */ 	
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);

	/* 6、选择RTC时钟源 */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

	/* 7、等待直到RTC时间和日期寄存器(RTC_TR和RTC_DR)完成与RTCAPB时钟同步 */
	RTC_WaitForSynchro();
	
	/* 8、配置RTC数据寄存器和RTC预分频器 */
	/* 公式：ck_spre(1Hz) = RTCCLK(LSI) /(AsynchPrediv + 1)*(SynchPrediv + 1)*/
	RTC_InitStructure.RTC_AsynchPrediv = 0x7F;					//异步分频系数(127+1)
	RTC_InitStructure.RTC_SynchPrediv  = 0xF9;					//同步分频系数(249+1)
	RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;		//24小时格式
	RTC_Init(&RTC_InitStructure);								//将配置的外设信息，写入到相对应的寄存器中
	
	
#elif SELECT_RTC_CLK_LSE
	
	/* 5、使能外部低速时钟振荡器 */
	RCC_LSEConfig(RCC_LSE_ON);

	/* 6、等到外部低速时钟振荡器准备好 */ 	
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);

	/* 6、选择RTC时钟源 */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	

	/* 7、等待直到RTC时间和日期寄存器(RTC_TR和RTC_DR)完成与RTCAPB时钟同步 */
	RTC_WaitForSynchro();
	
	/* 8、配置RTC数据寄存器和RTC预分频器 */
	/* 公式：ck_spre(1Hz) = RTCCLK(LSE) /(AsynchPrediv + 1)*(SynchPrediv + 1)*/
	RTC_InitStructure.RTC_AsynchPrediv = 0x7F;					//异步分频系数(127+1)
	RTC_InitStructure.RTC_SynchPrediv  = 0xFF;					//同步分频系数(255+1)
	RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;		//24小时格式
	RTC_Init(&RTC_InitStructure);								//将配置的外设信息，写入到相对应的寄存器中
	
#elif SELECT_RTC_CLK_HSE
 
    // 4. 开启外部高速时钟HSE
    RCC_HSEConfig(RCC_HSE_ON);
    
    // 等待HSE就绪
    while(RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);
    
    // 5. 配置RTC时钟源为HSE的128分频
    RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div128);
    
    // 6. 使能RTC时钟
    RCC_RTCCLKCmd(ENABLE);
    
    // 7. 等待RTC寄存器同步
    RTC_WaitForSynchro();
    
    // 8. 等待上一次操作完成
    RTC_WaitForLastTask();
    
    // 9. 设置RTC预分频器
    RTC_EnterConfigMode();
    
    // 设置分频值以获得1Hz时钟
    // 假设HSE=8MHz，分频128后=62.5kHz
    // 再通过异步分频器分频到1Hz
    RTC_SetPrescaler(62499);  // 62.5kHz / 62500 = 1Hz
    
    // 退出配置模式
    RTC_ExitConfigMode();
    
    // 10. 等待操作完成
    RTC_WaitForLastTask();
	
#endif

}


//2、设置RTC的日期和时间
void RTC_SetDateTime(uint8_t Year, uint8_t Month, uint8_t Date, uint8_t Hours, uint8_t Minutes, uint8_t Seconds, uint8_t weekDay)
{
	/* 0、外设信息配置结构体 */
	RTC_DateTypeDef  RTC_DateStruct;					//RTC日期配置
	RTC_TimeTypeDef  RTC_TimeStruct;					//RTC时间配置
	
	/* 1、设置日期(年月日、星期) */
	RTC_DateStruct.RTC_Year		=	Year;				//年份(0-99)
	RTC_DateStruct.RTC_Month	=	Month;				//月份
	RTC_DateStruct.RTC_Date		=	Date;				//日期(1-31)
	RTC_DateStruct.RTC_WeekDay	=	weekDay; 			//星期
	RTC_SetDate(RTC_Format_BIN, &RTC_DateStruct);		//将配置的外设信息，写入到相对应的寄存器中
	
	/* 2、设置时间(时分秒) */
	RTC_TimeStruct.RTC_Hours    =  Hours;				//小时(24小时制：0-23，12小时制：0-11) 				
	RTC_TimeStruct.RTC_Minutes  =  Minutes;				//分钟(0-59) 
	RTC_TimeStruct.RTC_Seconds  =  Seconds;  			//秒数(0-59) 
	//RTC_TimeStruct.RTC_H12    =  RTC_H12_PM;
	RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);		//将配置的外设信息，写入到相对应的寄存器中

}


