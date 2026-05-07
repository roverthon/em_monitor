#ifndef __MQ2_H
#define __MQ2_H

#include "stm32f4xx.h"

void MQ2_Init(void);          // 初始化 PA4(ADC) 和 PC7(GPIO输入)
float Get_MQ2_AO_Voltage(void);  // 读取模拟电压值
uint8_t Get_MQ2_DO_Level(void);  // 读取数字输出电平（0/1）

#endif