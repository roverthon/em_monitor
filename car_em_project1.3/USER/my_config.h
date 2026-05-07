/**
  ******************************************************************************
  * @file    my_config.h
  * @author  FZetc飞贼
  * @version V0.0.1
  * @date    2026.03.20
  * @brief   说明：
  *				1、配置工程信息
  *				2、位带操作的信息
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
 
#ifndef __MY_CONFIG_H		// 定义以防止递归包含
#define __MY_CONFIG_H


// 一、其它头文件
#include "stm32f4xx.h"                
#include "stm32f4xx_adc.h"
#include "stm32f4xx_crc.h"
#include "stm32f4xx_dbgmcu.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_flash.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_iwdg.h"
#include "stm32f4xx_pwr.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_rtc.h"
#include "stm32f4xx_sdio.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_wwdg.h"
#include "misc.h" 
#include "stm32f4xx_cryp.h"
#include "stm32f4xx_hash.h"
#include "stm32f4xx_rng.h"
#include "stm32f4xx_can.h"
#include "stm32f4xx_dac.h"
#include "stm32f4xx_dcmi.h"
#include "stm32f4xx_fsmc.h"


// 二、宏定义(函数、变量、常量)
#define ON  1		// 开
#define OFF 0		// 关

#define ORDER  1	// 顺序
#define REVERS 2	// 逆序

// (1)、RCC外设寄存器
#define RCC_AHB1ENR	 *((volatile unsigned int*)(0x40023800+0x30))			// AHB1外设寄存器

// (2)、GPIO外设寄存器
// 1、GPIOA域的寄存器
#define GPIOA_MODER    *((volatile unsigned int*)(0x40020000+0x00))			// 端口模式寄存器
#define GPIOA_OTYPER   *((volatile unsigned int*)(0x40020000+0x04))			// 端口输出类型寄存器
#define GPIOA_OSPEEDR  *((volatile unsigned int*)(0x40020000+0x08))			// 端口输出速度寄存器
#define GPIOA_PUPDR    *((volatile unsigned int*)(0x40020000+0x0C))			// 端口上拉/下拉寄存器
#define GPIOA_IDR      *((volatile unsigned int*)(0x40020000+0x10))			// 端口输入数据寄存器
#define GPIOA_ODR      *((volatile unsigned int*)(0x40020000+0x14))			// 端口输出数据寄存器
#define GPIOA_BSRR     *((volatile unsigned int*)(0x40020000+0x18))			// 端口置位/复位寄存器

// 2、GPIOF域的寄存器
#define GPIOF_MODER    *((volatile unsigned int*)(0x40021400+0x00))			// 端口模式寄存器
#define GPIOF_OTYPER   *((volatile unsigned int*)(0x40021400+0x04))			// 端口输出类型寄存器
#define GPIOF_OSPEEDR  *((volatile unsigned int*)(0x40021400+0x08))			// 端口输出速度寄存器
#define GPIOF_PUPDR    *((volatile unsigned int*)(0x40021400+0x0C))			// 端口上拉/下拉寄存器
#define GPIOF_IDR      *((volatile unsigned int*)(0x40021400+0x10))			// 端口输入数据寄存器
#define GPIOF_ODR      *((volatile unsigned int*)(0x40021400+0x14))			// 端口输出数据寄存器
#define GPIOF_BSRR     *((volatile unsigned int*)(0x40021400+0x18))			// 端口置位/复位寄存器

// (3)、前任总结的位带操作的方式
// 1、IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

// 2、IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+20) //0x40020014
#define GPIOB_ODR_Addr    (GPIOB_BASE+20) //0x40020414 
#define GPIOC_ODR_Addr    (GPIOC_BASE+20) //0x40020814 
#define GPIOD_ODR_Addr    (GPIOD_BASE+20) //0x40020C14 
#define GPIOE_ODR_Addr    (GPIOE_BASE+20) //0x40021014 
#define GPIOF_ODR_Addr    (GPIOF_BASE+20) //0x40021414    
#define GPIOG_ODR_Addr    (GPIOG_BASE+20) //0x40021814   
#define GPIOH_ODR_Addr    (GPIOH_BASE+20) //0x40021C14    
#define GPIOI_ODR_Addr    (GPIOI_BASE+20) //0x40022014     

#define GPIOA_IDR_Addr    (GPIOA_BASE+16) //0x40020010 
#define GPIOB_IDR_Addr    (GPIOB_BASE+16) //0x40020410 
#define GPIOC_IDR_Addr    (GPIOC_BASE+16) //0x40020810 
#define GPIOD_IDR_Addr    (GPIOD_BASE+16) //0x40020C10 
#define GPIOE_IDR_Addr    (GPIOE_BASE+16) //0x40021010 
#define GPIOF_IDR_Addr    (GPIOF_BASE+16) //0x40021410 
#define GPIOG_IDR_Addr    (GPIOG_BASE+16) //0x40021810 
#define GPIOH_IDR_Addr    (GPIOH_BASE+16) //0x40021C10 
#define GPIOI_IDR_Addr    (GPIOI_BASE+16) //0x40022010 

// 3、IO口操作,只对单一的IO口!(确保n的值小于16!)
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入

#define PHout(n)   BIT_ADDR(GPIOH_ODR_Addr,n)  //输出 
#define PHin(n)    BIT_ADDR(GPIOH_IDR_Addr,n)  //输入

#define PIout(n)   BIT_ADDR(GPIOI_ODR_Addr,n)  //输出 
#define PIin(n)    BIT_ADDR(GPIOI_IDR_Addr,n)  //输入


// 三、自定义的数据类型(结构体、联合体、枚举等)

// 四、全局变量声明

// 五、函数声明
extern void MY_CONFIG_Init(void);

// 六、静态变量、静态函数定义

#endif 	/* __MY_CONFIG_H */
