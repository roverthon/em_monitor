#include "uart.h"
volatile uint8_t u1_recvbuf[128];
volatile uint8_t u1_count;
volatile uint8_t u1_flag;


// 关闭半主机模式
#pragma import(__use_no_semihosting_swi)

// 实现printf函数的重定向
struct __FILE 
{ 
	int handle; /* Add whatever you need here */ 
};
FILE __stdout;
FILE __stdin;

// printf函数的底层实现(fputc函数)
int fputc(int ch, FILE *f) 		// FILE *f这个没什么用，但是fputc函数的格式是如此，所以要那么写
{
	// 发送单个数据
    USART_SendData(USART1, ch);	

	// 检查串口1是否还有未发送的数据，有的话，循环等待
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == 0); 	
	
	// return ch 这个没什么用，但是fputc函数的格式是如此，所以要那么写
	return ch;
}

// 实现_sys_exit以避免半主机退出
void _sys_exit(int x) 
{
	x = x;		// 防止警告
	while(1);	// 卡住，不退出
}



/**
  * @brief  初始化USART1外设
  * @note   GEC-M4板子：
  *				UART1_1(丝印层)->RXD(网络标号)  =短接=  USART1_TX(网络标号）->PA9(芯片引脚标号)
  *				UART1_2(丝印层)->TXD(网络标号)  =短接=  USART1_RX(网络标号 )->PA10(芯片引脚标号)
  * @param  None 
  * @retval None
  */
void UART1_Init(uint32_t baudrate)
{
	// 0、片内外设信息初始化结构体
	GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;
	
	// 1、使能USART1的硬件时钟、相应的GPIO时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	// 2、将GPIO引脚PA9(TX1)、PA10(RX1)连接到所需外设(USART1)的复用模式下
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,  GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
	
	// 3、配置GPIO片内外设的引脚PA9(TX1)、PA10(RX1)
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	// 4、USART1的外设信息配置
    USART_InitStructure.USART_BaudRate 				= baudrate;							// 波特率：函数传参过来
    USART_InitStructure.USART_WordLength 			= USART_WordLength_8b;				// 数据位：8位
    USART_InitStructure.USART_StopBits 				= USART_StopBits_1;					// 停止位：1
    USART_InitStructure.USART_Parity 				= USART_Parity_No;					// 校验位：不需要奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl 	= USART_HardwareFlowControl_None;	// 硬件流：不需要硬件流
    USART_InitStructure.USART_Mode 					= USART_Mode_Rx | USART_Mode_Tx;	// 模式：既要收又要发
    USART_Init(USART1, &USART_InitStructure);											// 使用该函数，将配置的信息写入到相应的寄存器中			
	
	
	// 5、给USART1外设在NVIC上进行配置
	NVIC_InitStructure.NVIC_IRQChannel 					 = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority 		 = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd 				 = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
	// 6、使能USART1中断
	 USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	// 7、可以不用DMA(当你的CPU运算串口数据力有未殆时，可以使用DMA来负担，现在不需要)
	// DMA_Init();
	// DMA_Cmd();
	
	// 8、使能USART1外设
	USART_Cmd(USART1, ENABLE);

}


/**
  * @brief  UART1的发送数据(下位机->上位机)
  * @note   下位机(单片机)发送数据信息给上位机(电脑的串口助手、手机蓝牙的APP、手机WiFi的APP), 发送的是字符串数据
  * @param  None 
  * @retval None
  */
void UART1_SendStr(const char* str)			// "nihao"
{
	while(*str != '\0')
	{
		USART_SendData(USART1, *str++);								// 发送一个字符
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == 0); 	// 等待字符发送结束
	}
}


/**
  * @brief  UART1的接收数据中断服务函数(上位机->下位机)
  * @note   下位机(单片机)接收来自上位机(电脑的串口助手、手机蓝牙的APP、手机WiFi的APP)的信息，并进行处理
  * @param  None 
  * @retval None
  */
void USART1_IRQHandler(void)		// "shijiename,woxiangdainikankan#"	
{
	uint8_t r_data = 0;
	if( USART_GetITStatus(USART1, USART_IT_RXNE) == SET)		// 判断是否有数据发送到了uart1中了(串口接收中断)
	{
		// 接收来自上位机的数据并存放，设置接收完毕标志位
		r_data               = USART_ReceiveData(USART1);
		u1_recvbuf[u1_count] = r_data;
		
		if(r_data == '#')
			u1_flag = 1;
		else
			u1_count++;
		
		// 回发信息给上位机
		USART_SendData(USART1, r_data);								
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == 0); 	
		
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);			// 清除串口接收中断标志位
	}		
}
