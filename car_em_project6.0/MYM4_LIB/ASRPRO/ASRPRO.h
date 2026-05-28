/**
  ******************************************************************************
  * @file    ASRPRO.h
  * @author  XIXE
  * @version V0.0.1
  * @date    2026.02.05
  * @brief   
  *
  ******************************************************************************
  * @attention
  *
  * 本文档只供学习使用，不得商用，违者必究
  *
  * 有疑问或者建议： 43821980@qq.com
  *
  ******************************************************************************
  */

#ifndef __ASRPRO_H__
#define __ASRPRO_H__

typedef enum {
	ASRPRO_OPEN = 1,
	
}ASRPRO_RecvCmd;

typedef enum {
	ASRPRO_WELCOME = 0,
	ASRPRO_RFID_ERROR,
	ASRPRO_PASSWD_ERROR,
	ASRPRO_AS608_ERROR,
}ASRPRO_SendCmd;

void ASRPRO_Init(void);

#endif	/* __LED_H__ */
