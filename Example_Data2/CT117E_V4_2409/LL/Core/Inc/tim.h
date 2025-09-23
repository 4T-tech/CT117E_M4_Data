/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.h
  * @brief   This file contains all the function prototypes for
  *          the tim.c file
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
#ifndef __TIM_H__
#define __TIM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_TIM1_Init(void);
void MX_TIM2_Init(void);
void MX_TIM3_Init(void);
void MX_TIM4_Init(void);

/* USER CODE BEGIN Prototypes */
void TIM1_SetAutoReload(uint16_t usAuto); 	/* 设置TIM1自动重装值 */
void TIM4_SetAutoReload(uint16_t usAuto); 	/* 设置TIM4自动重装值 */
void TIM1_SetCompare1(uint16_t usComp);   	/* 设置TIM1输出比较值1 */
void TIM4_SetCompare1(uint16_t usComp);   	/* 设置TIM4输出比较值1 */
void TIM2_GetCapture(uint16_t *pusBuf);    	/* 获取TIM2输入捕获值 */
void TIM3_GetCapture(uint16_t *pusBuf);   	/* 获取TIM3输入捕获值 */
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __TIM_H__ */

