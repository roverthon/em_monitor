#ifndef __RTC_H					//定义以防止递归包含
#define __RTC_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "my_config.h"
#include "my_reg.h"

#define SELECT_RTC_CLK_LSI 1
#define SELECT_RTC_CLK_LSE 0
#define SELECT_RTC_CLK_HSE 0

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
extern void RTC_ConfigInit(void);																							// 1、RTC配置初始化
extern void RTC_SetDateTime(uint8_t Year, uint8_t Month, uint8_t Date, uint8_t Hours, uint8_t Minutes, uint8_t Seconds, uint8_t weekDay);	// 2、设置RTC的日期和时间

#endif /* __RTC_H */
