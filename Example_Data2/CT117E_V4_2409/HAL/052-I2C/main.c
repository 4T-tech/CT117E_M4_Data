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
#include "lcd.h"
#include "stdio.h"
#include "i2c.h"
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
uint8_t ucSec;                     	/* ���ʱ */
uint8_t ucCnt;    		        			/* �������� */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void KEY_Proc(void);              	/* �������� */
void LED_Proc(void);              	/* LED���� */
void LCD_Proc(void);              	/* LCD���� */
void UART_Proc(void);              	/* UART���� */
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
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  LCD_Init();                      	/* LCD��ʼ�� */
  LCD_Clear(Black);                	/* LCD���� */
  LCD_SetTextColor(White);        	/* �����ַ�ɫ */
  LCD_SetBackColor(Black);        	/* ���ñ���ɫ */

  MEM_Read((uint8_t *)&ucCnt, 0, 1);	/* �洢���� */
  ucCnt++;
  MEM_Write((uint8_t *)&ucCnt, 0, 1);	/* �洢��д */
	/* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    KEY_Proc();                     /* �������� */
    LED_Proc();                     /* LED���� */
    LCD_Proc();                    	/* LCD���� */
    UART_Proc();    	      				/* UART���� */
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}
/* USER CODE BEGIN 4 */
uint8_t ucKey, ucDir;             	/* ����ֵ��LED��ʾ��ˮ���� */
void KEY_Proc(void)               	/* �������� */
{
  uint8_t ucKey_Val = 0;

  ucKey_Val = KEY_Read();         	/* ������ȡ */
  if (ucKey_Val != ucKey)         	/* ��ֵ�仯 */
    ucKey = ucKey_Val;            	/* �����ֵ */
  else
    ucKey_Val = 0;                 	/* �����ֵ */

  switch (ucKey_Val)
  {
    case 1:                         	/* B1������ */
      ucDir ^= 1;                   	/* �ı���ˮ���� */
      break;
    case 2:                         	/* B2������ */
      break;
  }
}

uint8_t ucLed, ucSec1;            	/* LEDֵ��LED��ʾ��ʱ */
void LED_Proc(void)               	/* LED���� */
{
  if (ucSec1 == ucSec)
    return;                         	/* 1sδ������ */
  ucSec1 = ucSec;
  if (ucDir == 0)                   	/* LEDֵ���� */
  {
    ucLed <<= 1; 
    if (ucLed == 0)
      ucLed = 1;
  } else {                           	/* LEDֵ�һ��� */
    ucLed >>= 1;
    if (ucLed == 0)
      ucLed = 0x80;
  }
  LED_Disp(ucLed);                 	/* LED��ʾ */
}

uint8_t  ucLcd[21];                 	/* LCDֵ(\0����) */
uint16_t usTlcd;                   	/* LCDˢ�¼�ʱ */
void LCD_Proc(void)               	/* LCD���� */
{
  if (usTlcd < 500)   				/* 500msδ�� */
    return;
  usTlcd = 0;

  sprintf((char*)ucLcd, " SEC:%03u   CNT:%03u ", ucSec, ucCnt);
  LCD_DisplayStringLine(Line1, ucLcd);
}

uint8_t ucUrx[20], ucSec2;        	/* UART����ֵ��������ʱ */
void UART_Proc(void)              	/* UART���� */
{
  if (ucSec2 != ucSec)
  {                           			/* 1s�� */
    ucSec2 = ucSec;

    printf("%s %03u\r\n", ucLcd, ucSec);
  }

  if (UART_Receive(ucUrx, 2) == 0)  /* ���յ��ַ� */
    ucSec = (ucUrx[0]-0x30)*10+ucUrx[1]-0x30;
}

int fputc(int ch, FILE *f)		    	/* printf()ʵ�� */
{
  UART_Transmit((uint8_t *)&ch, 1);
  return ch;
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
