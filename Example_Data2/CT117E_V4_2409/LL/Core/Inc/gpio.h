/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   This file contains all the function prototypes for
  *          the gpio.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */
uint8_t KEY_Read(void);             	/* ������ȡ */
void LED_Disp(uint8_t ucLed);       	/* LED��ʾ */
void LCD_Write(uint8_t RS, uint16_t Value);	/* LCDд */
void SDA_Input_Mode(void);						/* SDA����ģʽ */
void SDA_Output_Mode(void);						/* SDA���ģʽ */
uint8_t SDA_Input(void);			  			/* SDA���� */
void SDA_Output(uint8_t);				  		/* SDA��� */
void SCL_Output(uint8_t);				  		/* SCL���� */
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

