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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "stdio.h"
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
uint8_t ucState;                    /* ϵͳ״̬ */
uint8_t ucSec;                      /* ���ʱ */
uint8_t ucPsd[3]={'@','@','@'};     /* ��������ֵ */
uint8_t ucPsw[3]={'1','2','3'};     /* ��ǰ����ֵ */
uint8_t ucNum;                      /* ���������� */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void KEY_Proc(void);              	/* �������� */
void LED_Proc(void);              	/* LED���� */
void LCD_Proc(void);              	/* LCD���� */
void UART_Proc(void);              	/* UART���� */
void TIM_Proc(void);              	/* TIM���� */
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

  /* MCU Configuration--------------------------------------------------------*/

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
//  MX_ADC1_Init();
//  MX_ADC2_Init();
//  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  LCD_Init();                      	/* LCD��ʼ�� */
  LCD_Clear(Black);                	/* LCD���� */
  LCD_SetTextColor(White);        	/* �����ַ�ɫ */
  LCD_SetBackColor(Black);        	/* ���ñ���ɫ */
	
  TIM4_SetCompare1(500);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    KEY_Proc();                    	/* �������� */
    LED_Proc();                    	/* LED���� */
    LCD_Proc();                    	/* LCD���� */
    UART_Proc();    		      			/* UART���� */
    TIM_Proc();    	        				/* TIM���� */
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/* USER CODE BEGIN 4 */
uint8_t ucKey;                      /* ����ֵ */
void KEY_Proc(void)               	/* �������� */
{
  uint8_t ucKey_Val = 0;

  if (ucState == 1)
    return;

  ucKey_Val = KEY_Read();         	/* ������ȡ */
  if (ucKey_Val != ucKey)         	/* ��ֵ�仯 */
    ucKey = ucKey_Val;            	/* �����ֵ */
  else
    ucKey_Val = 0;                 	/* �����ֵ */

  switch (ucKey_Val) {
    case 1:                             /* B1������ */
    case 2:                             /* B2������ */
    case 3:                             /* B3������ */
      ucKey_Val--;
      if (ucPsd[ucKey_Val] == '@')
        ucPsd[ucKey_Val] = '0';
      else {
        ucPsd[ucKey_Val]++;
        if (ucPsd[ucKey_Val] == ':')
          ucPsd[ucKey_Val] = '0'; }
      break;
    case 4:                             /* B4������ */
      if ((ucPsd[0] == ucPsw[0])
        &&(ucPsd[1] == ucPsw[1])
        &&(ucPsd[2] == ucPsw[2])) {
        ucState = 1;
        TIM4_SetAutoReload(499);
        TIM4_SetCompare1(50);
        ucNum = 0; }
      else {
        ucPsd[0] = ucPsd[1] = ucPsd[2] = '@';
        ucNum++; }
      ucSec = 0;
  }
}

uint8_t ucLed;                      /* LEDֵ */
uint8_t ucTled;                     /* LED��˸��ʱ */
void LED_Proc(void)               	/* LED���� */
{
  if (ucTled < 100)
    return;
  ucTled = 0;

  if (ucState == 1)
    ucLed |= 1;
  else {
    ucLed &= ~1;
    if ((ucNum >= 3) && (ucSec < 5))
      ucLed ^= 2;
    else
      ucLed &= ~2; }
  LED_Disp(ucLed);                 	/* LED��ʾ */
}

uint8_t ucLcd[21];                  /* LCDֵ(\0����) */
uint16_t usTlcd;                    /* LCDˢ�¼�ʱ */
uint16_t usCapt[2];                 /* TIM2��׽ֵ */
void LCD_Proc(void)               	/* LCD���� */
{
  if (usTlcd < 500)   	      			/* 500msδ�� */
    return;
  usTlcd = 0;

  if (ucState == 0) {
    LCD_DisplayStringLine(Line1, (unsigned char*)"       PSD");
    sprintf((char*)ucLcd, "    B1:%c %c  ", ucPsd[0], ucPsw[0]);
    LCD_DisplayStringLine(Line3, ucLcd);
    sprintf((char*)ucLcd, "    B2:%c %c", ucPsd[1], ucPsw[1]);
    LCD_DisplayStringLine(Line4, ucLcd);
    sprintf((char*)ucLcd, "    B3:%c %c", ucPsd[2], ucPsw[2]);
    LCD_DisplayStringLine(Line5, ucLcd); }
  else {
    LCD_DisplayStringLine(Line1, (unsigned char*)"       STA");
    LCD_DisplayStringLine(Line3, (unsigned char*)"    F:2000Hz");
    LCD_DisplayStringLine(Line4, (unsigned char*)"    D:10% ");
    LCD_DisplayStringLine(Line5, (unsigned char*)"          ");
  }
  TIM2_GetCapture(usCapt);         	/* TIM2��׽�������ã� */
  sprintf((char*)ucLcd, "   %03uHz %02u%%",
    1000000/usCapt[0], (usCapt[0]-usCapt[1])*100/usCapt[0]);
  LCD_DisplayStringLine(Line7, ucLcd);
}

uint8_t ucUrx[20];                  /* UART����ֵ */
void UART_Proc(void)              	/* UART���� */
{
  if (UART_Receive(ucUrx, 7) == 0) {	/* ������� */
    if ((ucUrx[0] == ucPsw[0])   	  /* ������ȷ */
      &&(ucUrx[1] == ucPsw[1])
      &&(ucUrx[2] == ucPsw[2])) {
      ucPsw[0] = ucUrx[4];       	  /* �޸����� */
      ucPsw[1] = ucUrx[5];
      ucPsw[2] = ucUrx[6]; } }
}

int fputc(int ch, FILE *f)		    	/* printf()ʵ�� */
{
  UART_Transmit((uint8_t *)&ch, 1);
  return ch;
}

void TIM_Proc(void)               	/* TIM���� */
{
  if ((ucSec >= 5) && (ucState == 1)) {
    ucState = 0;
    TIM4_SetAutoReload(999);
    TIM4_SetCompare1(500);
    ucPsd[0] = ucPsd[1] = ucPsd[2] = '@'; }
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
