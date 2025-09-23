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
#include "tim.h"
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
uint8_t ucT200;                     /* Ƶ�ʲ�����ʱ */
uint8_t ucStep;                     /* Ƶ�ʲ�����־ */
uint16_t usFreq=4000;               /* Ƶ��ֵ */
uint8_t ucMode=0;                   /* ���ģʽ(��Ƶģʽ) */
uint8_t ucMode1=0;                  /* ���ģʽ1(��Ƶģʽ) */
uint8_t ucPara='R';                 /* ����ѡ�� */
uint8_t ucDuty;                     /* ռ�ձ� */
uint16_t usV[4];                    /* ����, ����, �ٶ�, �ٶ�1 */
uint8_t ucR=1, ucK=1;               /* ���� */
uint8_t ucR1=1, ucK1=1;             /* ����1 */
uint8_t ucN;                        /* ģʽ�л����� */
uint16_t usMl, usMh;                /* �ٶ����ֵ */
uint16_t usT2s;                     /* �ٶ�ͳ�Ƽ�ʱ */
uint8_t ucLock;                     /* ����״̬ */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void KEY_Proc(void);              	/* �������� */
void LED_Proc(void);              	/* LED���� */
void LCD_Proc(void);              	/* LCD���� */
void ADC_Proc(void);              	/* ADC���� */
void TIM_Proc(void);                /* TIM���� */
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
  MX_ADC2_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  LCD_Init();                      	/* LCD��ʼ�� */
  LCD_Clear(Black);                	/* LCD���� */
  LCD_SetTextColor(White);        	/* �����ַ�ɫ */
  LCD_SetBackColor(Black);        	/* ���ñ���ɫ */
	
  TIM4_SetAutoReload(249);          /* PA11���Ƶ��4kHz */
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    KEY_Proc();                    	/* �������� */
    LED_Proc();                    	/* LED���� */
    LCD_Proc();                    	/* LCD���� */
    ADC_Proc();    	        				/* ADC���� */
    TIM_Proc();                     /* TIM���� */
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/* USER CODE BEGIN 4 */
uint8_t ucKey;                      /* ����ֵ */
uint8_t ucSec=5;                    /* ���ʱ */
void KEY_Proc(void)               	/* �������� */
{
  uint8_t ucKey_Val, ucKey_Dn, ucKey_Up;

  ucKey_Val = KEY_Read();          	/* ��ȡ���� */
  ucKey_Dn = ucKey_Val & (ucKey ^ ucKey_Val);
  ucKey_Up = ~ucKey_Val & (ucKey ^ ucKey_Val);
  ucKey = ucKey_Val;

  switch (ucKey_Dn) {
    case 1:                         /* B1������ */
      if (++ucState == 3)           /* �л�״̬ */
        ucState = 0;
      if (ucState == 1)
        ucPara = 'R';
      if (ucState == 2) {
        ucR = ucR1;                 /* ������Ч */
        ucK = ucK1; }
      LCD_Clear(Black);             /* LCD���� */
      break;
    case 2:                         /* B2������ */
      if ((ucState == 0) && (ucSec >= 5)) {
        ucMode ^= 1;                /* �л�ģʽ */
        if (ucMode == 0) {
          usFreq = 8000;            /* 8kHz -> 4kHz*/
          ucStep = 1; }             /* Ƶ�ʵݼ� */
        else {
          usFreq = 4000;            /* 4kHz -> 8kHz*/
          ucStep = 2; }             /* Ƶ�ʵݼ� */
        ucSec = 0;
        ucN++; }
      else {                        /* �л����� */
        if (ucPara == 'R')
          ucPara = 'K';
        else
          ucPara = 'R'; }
      break;
    case 3:                         /* B3������ */
      if (ucState == 1) {
        if (ucPara == 'R')
          if (++ucR1 == 11)         /* ������ */
            ucR1 = 1;
        if (ucPara == 'K')
          if (++ucK1 == 11)         /* ������ */
            ucK1 = 1; }
      break;
    case 4:                         /* B4������ */
      if (ucState == 0) {
        ucSec = 0;
        ucLock = 0;	}               /* ���� */
      if (ucState == 1) {
        if (ucPara == 'R')
          if (--ucR1 == 0)          /* ������ */
            ucR1 = 10;
        if (ucPara == 'K')
          if (--ucK1 == 0)          /* ������ */
            ucK1 = 10; }
  }
  if ((ucKey_Up == 4) && (ucState == 0) && (ucSec >= 2))
    ucLock = 1;		                  /* ���� */
}

uint8_t ucLed;                      /* LEDֵ */
uint8_t ucTled;                     /* LED��˸��ʱ */
void LED_Proc(void)               	/* LED���� */
{
  if (ucTled < 100)
    return;
  ucTled = 0;

  if (ucState == 0)
    ucLed |= 1;                     /* LD1���� */
  else
    ucLed &= ~1;                    /* LD1Ϩ�� */

  if (ucStep >= 1)
    ucLed ^= 2;                     /* LD2��˸ */
  else
    ucLed &= ~2;                    /* LD2Ϩ�� */
	
  if (ucLock == 1)
    ucLed |= 4;                     /* LD3���� */
  else
    ucLed &= ~4;                    /* LD3Ϩ�� */
  LED_Disp(ucLed);                 	/* LED��ʾ */
}

uint8_t ucLcd[21];                  /* LCDֵ(\0����) */
uint16_t usTlcd;                    /* LCDˢ�¼�ʱ */
void LCD_Proc(void)               	/* LCD���� */
{
  if (usTlcd < 100)                 /* 100msδ�� */
    return;
  usTlcd = 0;

  switch (ucState) {
    case 0:                         /* ���ݽ��� */
      LCD_DisplayStringLine(Line1, (unsigned char*)"        DATA");
      if (ucMode1 == 0)
        LCD_DisplayStringLine(Line3, (unsigned char*)"     M=L");
      else
        LCD_DisplayStringLine(Line3, (unsigned char*)"     M=H");
//    sprintf((char*)ucLcd, "     P=%02u%% ", ucDuty);
      sprintf((char*)ucLcd, "     P=%02u%%   %02u%% ",
        ucDuty, (usV[0]-usV[1])*usFreq/10000);
      LCD_DisplayStringLine(Line4, ucLcd);
//    sprintf((char*)ucLcd, "     V=%3.1f ", usV[2]/10.0);
      sprintf((char*)ucLcd, "     V=%3.1f %4u", usV[2]/10.0, usFreq);
      LCD_DisplayStringLine(Line5, ucLcd);
      break;
    case 1:                         /* �������� */
      LCD_DisplayStringLine(Line1, (unsigned char*)"        PARA");
      sprintf((char*)ucLcd, "     R=%u ", ucR1);
      if (ucPara == 'R') ucLcd[4] = '.';
      LCD_DisplayStringLine(Line3, ucLcd);
      sprintf((char*)ucLcd, "     K=%u ", ucK1);
      if (ucPara == 'K') ucLcd[4] = '.';
      LCD_DisplayStringLine(Line4, ucLcd);
      break;
    case 2:                         /* ��¼���� */
      LCD_DisplayStringLine(Line1, (unsigned char*)"        RECD");
      sprintf((char*)ucLcd, "     N=%u ", ucN);
      LCD_DisplayStringLine(Line3, ucLcd);
      sprintf((char*)ucLcd, "     MH=%4.1f ", usMh/10.0);
      LCD_DisplayStringLine(Line4, ucLcd);
      sprintf((char*)ucLcd, "     ML=%3.1f ", usMl/10.0);
      LCD_DisplayStringLine(Line5, ucLcd);
  }
}

void ADC_Proc(void)               	/* ADC���� */
{
  uint8_t ucAdc;                    /* ADC��ѹ*10 */

  if (ucLock == 1)
    return;

  ucAdc = ADC2_Read()*33/4095;      /* ADC��ȡ */
  if (ucAdc < 10)
    ucDuty = 10;
  else if (ucAdc > 30)
    ucDuty = 85;
  else
    ucDuty = (375*ucAdc-2750)/100;
  TIM4_SetCompare1(10000*ucDuty/usFreq);
}

void TIM_Proc(void)                 /* TIM���� */
{
  TIM2_GetCapture(usV);             /* TIM2��׽��������� */
  usV[2]=628000*ucR/ucK/usV[0];     /* �ٶ�ֵ����*10 */
  if (usV[2] != usV[3]) {
    usT2s = 0;                      /* 2s��ʱ��ʼ */
    usV[3] = usV[2]; }

  if (usT2s > 2000) {               /* 2s��ʱ�� */
    if (ucMode1 == 0) {
      if (usV[2] > usMl)
        usMl = usV[2]; }            /* ��¼��Ƶ���ֵ */
    else
      if (usV[2] > usMh)
        usMh = usV[2]; }            /* ��¼��Ƶ���ֵ */

  if (ucStep == 0)
    return;
  if (ucT200 < 200)                 /* ����200ms */
    return;
  ucT200 = 0;

  if (ucStep == 1)
    usFreq -= 160;                  /* Ƶ�ʵݼ� */
  else
    usFreq += 160;                  /* Ƶ�ʵݼ� */
  TIM4_SetAutoReload(1000000/usFreq-1);
  TIM4_SetCompare1(10000*ucDuty/usFreq);
  if ((usFreq <= 4000) ||(usFreq >= 8000)) {
    ucStep = 0;                     /* �������� */
    ucMode1 = ucMode;	}
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
