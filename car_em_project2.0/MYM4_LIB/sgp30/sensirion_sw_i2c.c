/*
 * Copyright (c) 2018, Sensirion AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of Sensirion AG nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "sensirion_arch_config.h"
#include "sensirion_common.h"
#include "sensirion_i2c.h"
#include "stm32f4xx.h"
#include "my_reg.h"
#include "my_config.h"

/* I2C 时钟周期，单位微秒，对应 100kHz 速率时可设为 10us */
#ifndef I2C_CLOCK_PERIOD_USEC
#define I2C_CLOCK_PERIOD_USEC  10
#endif

#define DELAY_USEC (I2C_CLOCK_PERIOD_USEC / 2)

/* 引脚定义：SDA -> PD6, SCL -> PD7 */
#define SDA_PIN         GPIO_Pin_6
#define SCL_PIN         GPIO_Pin_7
#define SDA_SCL_PORT    GPIOD

/**
 * 初始化 I2C 引脚：将 PD6 和 PD7 设置为推挽输出，初始高电平
 */
void sensirion_init_pins(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 使能 GPIOD 时钟（STM32F4 使用 AHB1 总线） */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    /* 配置 SDA(PD6) 和 SCL(PD7) 为推挽输出，速度 50MHz */
    GPIO_InitStructure.GPIO_Pin   = SDA_PIN | SCL_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(SDA_SCL_PORT, &GPIO_InitStructure);

    /* 初始设置为高电平 */
    GPIO_SetBits(SDA_SCL_PORT, SDA_PIN | SCL_PIN);
}

/**
 * 将 SDA 引脚配置为输入模式（无上拉/下拉）
 */
void SDA_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin   = SDA_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SDA_SCL_PORT, &GPIO_InitStructure);
}

/**
 * 将 SDA 引脚配置为输出模式（推挽）
 */
void SDA_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin   = SDA_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(SDA_SCL_PORT, &GPIO_InitStructure);
}

/**
 * 将 SDA 设为输入状态（释放总线，外部上拉电阻会将电平拉高）
 */
void sensirion_SDA_in(void)
{
    GPIO_SetBits(SDA_SCL_PORT, SDA_PIN);
}

/**
 * 将 SDA 设为输出状态并输出低电平
 */
void sensirion_SDA_out(void)
{
    GPIO_ResetBits(SDA_SCL_PORT, SDA_PIN);
}

/**
 * 读取 SDA 引脚的电平
 * @return 0 低电平，1 高电平
 */
u8 sensirion_SDA_read(void)
{
    return GPIO_ReadInputDataBit(SDA_SCL_PORT, SDA_PIN);
}

/**
 * 将 SCL 设为输入状态（释放总线）
 */
void sensirion_SCL_in(void)
{
    GPIO_SetBits(SDA_SCL_PORT, SCL_PIN);
}

/**
 * 将 SCL 设为输出状态并输出低电平
 */
void sensirion_SCL_out(void)
{
    GPIO_ResetBits(SDA_SCL_PORT, SCL_PIN);
}

/**
 * 微秒级延时（简单循环实现，需根据系统时钟调整）
 * @param useconds 延时微秒数
 */
void sensirion_sleep_usec(u32 useconds) {
    uint32_t ticks = (SystemCoreClock / 1000000U) * useconds;  // 微秒转 SysTick 周期
    uint32_t start = SysTick->VAL;
    uint32_t elapsed;
    do {
        elapsed = (start - SysTick->VAL) & 0x00FFFFFF;
    } while (elapsed < ticks);
}

/**
 * I2C 发送一个字节，并检查从机的应答（ACK/NACK）
 * @param data 要发送的字节
 * @return 0 表示收到 ACK，非 0 表示 NACK
 */
static s8 sensirion_i2c_write_byte(u8 data)
{
    s8 nack, i;

    SDA_OUT();
    /* 发送 8 个数据位，MSB 优先 */
    for (i = 7; i >= 0; i--) {
        sensirion_SCL_out();                     /* SCL 低电平，准备数据 */
        if ((data >> i) & 0x01)
            sensirion_SDA_in();                  /* 输出高电平（由外部上拉实现） */
        else
            sensirion_SDA_out();                 /* 输出低电平 */
        sensirion_sleep_usec(DELAY_USEC);
        sensirion_SCL_in();                      /* SCL 高电平，数据被采样 */
        sensirion_sleep_usec(DELAY_USEC);
    }

    /* 第 9 个时钟：读取从机应答 */
    sensirion_SCL_out();                         /* SCL 低电平 */
    sensirion_SDA_in();                          /* 释放 SDA 总线 */
    sensirion_sleep_usec(DELAY_USEC);
    SDA_IN();                                    /* 将 SDA 切换为输入，以便读取 */
    sensirion_SCL_in();                          /* SCL 高电平，读取应答 */
    nack = (sensirion_SDA_read() != 0);          /* 若 SDA 为高表示 NACK */
    sensirion_SCL_out();                         /* SCL 恢复低电平 */

    return nack;
}

/**
 * I2C 读取一个字节
 * @param ack 非 0 表示主机发送 ACK（继续读取），0 表示主机发送 NACK（结束读取）
 * @return 读取到的字节
 */
static u8 sensirion_i2c_read_byte(u8 ack)
{
    s8 i;
    u8 data = 0;

    sensirion_SDA_in();
    SDA_IN();                                    /* 确保 SDA 为输入模式 */
    for (i = 7; i >= 0; i--) {
        sensirion_sleep_usec(DELAY_USEC);
        sensirion_SCL_in();                      /* SCL 高电平，读取数据位 */
        data |= (sensirion_SDA_read() != 0) << i;
        sensirion_SCL_out();                     /* SCL 低电平，准备下一位 */
    }

    SDA_OUT();                                   /* 切换回输出模式，准备发送应答 */
    if (ack)
        sensirion_SDA_out();                     /* ACK：输出低电平 */
    else
        sensirion_SDA_in();                      /* NACK：释放总线（高电平） */
    sensirion_sleep_usec(DELAY_USEC);
    sensirion_SCL_in();                          /* 第 9 个时钟，从机采样应答 */
    sensirion_sleep_usec(DELAY_USEC);
    sensirion_SCL_out();                         /* SCL 恢复低电平 */
    sensirion_SDA_in();                          /* 释放 SDA */

    return data;
}

/**
 * I2C 起始条件
 */
static void sensirion_i2c_start(void)
{
    SDA_OUT();
    sensirion_SDA_out();                         /* SDA 低电平 */
    sensirion_sleep_usec(DELAY_USEC);
    sensirion_SCL_out();                         /* SCL 低电平（启动信号结束时） */
    sensirion_sleep_usec(DELAY_USEC);
}

/**
 * I2C 停止条件
 */
static void sensirion_i2c_stop(void)
{
    SDA_OUT();
    sensirion_SDA_out();                         /* SDA 低电平 */
    sensirion_sleep_usec(DELAY_USEC);
    sensirion_SCL_in();                          /* SCL 高电平 */
    sensirion_sleep_usec(DELAY_USEC);
    sensirion_SDA_in();                          /* SDA 高电平（停止信号） */
    sensirion_sleep_usec(DELAY_USEC);
}

/**
 * 向 I2C 从机写入数据
 * @param address 7 位从机地址
 * @param data    数据缓冲区
 * @param count   字节数
 * @return 0 成功，非 0 失败
 */
s8 sensirion_i2c_write(u8 address, const u8* data, u16 count)
{
    s8 ret;
    u16 i;

    sensirion_i2c_start();
    ret = sensirion_i2c_write_byte(address << 1);
    if (ret != STATUS_OK) {
        sensirion_i2c_stop();
        return ret;
    }
    for (i = 0; i < count; i++) {
        ret = sensirion_i2c_write_byte(data[i]);
        if (ret != STATUS_OK) {
            sensirion_i2c_stop();
            break;
        }
    }
    sensirion_i2c_stop();
    return ret;
}

/**
 * 从 I2C 从机读取数据
 * @param address 7 位从机地址
 * @param data    数据缓冲区，用于存放读取的字节
 * @param count   要读取的字节数
 * @return 0 成功，非 0 失败
 */
s8 sensirion_i2c_read(u8 address, u8* data, u16 count)
{
    s8 ret;
    u16 i;

    sensirion_i2c_start();
    ret = sensirion_i2c_write_byte((address << 1) | 1);
    if (ret != STATUS_OK) {
        sensirion_i2c_stop();
        return ret;
    }
    /* 前 count-1 个字节发送 ACK，最后一个字节发送 NACK */
    for (i = 0; i < count; i++) {
        data[i] = sensirion_i2c_read_byte(i < count - 1 ? 1 : 0);
    }
    sensirion_i2c_stop();
    return ret;
}

/**
 * 初始化 I2C 外设（配置引脚）
 */
void sensirion_i2c_init(void)
{
    sensirion_init_pins();
}
