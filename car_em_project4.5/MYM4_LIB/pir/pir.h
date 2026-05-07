#ifndef __PIR_H
#define __PIR_H
#include "stm32f4xx.h"

// Òý½Å¶¨Òå PE5
#define PIR_PORT        GPIOE
#define PIR_PIN         GPIO_Pin_5
#define PIR_RCC_CLK     RCC_AHB1Periph_GPIOE

void PIR_Init(void);
uint8_t PIR_Scan(void);

#endif