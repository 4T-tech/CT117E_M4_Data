/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t ucSec;                     	/* 秒计时 */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void KEY_Proc(void);              	/* 按键处理 */
void LED_Proc(void);              	/* LED处理 */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
//  MX_DMA_Init();
//  MX_ADC1_Init();
//  MX_ADC2_Init();
//  MX_TIM1_Init();
//  MX_TIM2_Init();
//  MX_TIM3_Init();
//  MX_TIM4_Init();
//  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    KEY_Proc();                     /* 按键处理 */
    LED_Proc();                     /* LED处理 */
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}
/* USER CODE BEGIN 4 */
uint8_t ucKey, ucDir;             	/* 按键值，LED显示流水方向 */
void KEY_Proc(void)               	/* 按键处理 */
{
  uint8_t ucKey_Val = 0;

  ucKey_Val = KEY_Read();         	/* 按键读取 */
  if (ucKey_Val != ucKey)         	/* 键值变化 */
    ucKey = ucKey_Val;            	/* 保存键值 */
  else
    ucKey_Val = 0;                 	/* 清除键值 */

  switch (ucKey_Val)
  {
    case 1:                         	/* B1键按下 */
      ucDir ^= 1;                   	/* 改变流水方向 */
      break;
    case 2:                         	/* B2键按下 */
      break;
  }
}

uint8_t ucLed, ucSec1;            	/* LED值，LED显示延时 */
void LED_Proc(void)               	/* LED处理 */
{
  if (ucSec1 == ucSec)
    return;                         	/* 1s未到返回 */
  ucSec1 = ucSec;
  if (ucDir == 0)                   	/* LED值左环移 */
  {
    ucLed <<= 1; 
    if (ucLed == 0)
      ucLed = 1;
  } else {                           	/* LED值右环移 */
    ucLed >>= 1;
    if (ucLed == 0)
      ucLed = 0x80;
  }
  LED_Disp(ucLed);                 	/* LED显示 */
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
