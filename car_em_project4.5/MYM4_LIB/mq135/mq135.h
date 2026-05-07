#ifndef __MQ135_H
#define __MQ135_H

#include "stm32f4xx.h"

void MQ135_Init(void);          // 初始化 PA8 为输入（读取 DO）
uint8_t Get_MQ135_DO_Level(void); // 读取数字输出电平（0/1）

#endif