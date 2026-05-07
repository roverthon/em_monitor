#ifndef __MQ7_H
#define __MQ7_H

#include "stm32f4xx.h"

void MQ7_Init(void);
float Get_MQ7_AO_Voltage(void);
uint8_t Get_MQ7_DO_Level(void);

#endif