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
typedef struct
{
  uint8_t  ucVval;                 	/* ��ѹֵ */
  uint16_t usFval;                 	/* Ƶ��ֵ */
  uint8_t  ucDuty;                 	/* ռ�ձ� */
} sData;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t  ucState;                    	/* ϵͳ״̬ */
uint16_t usTms;                      	/* �����ʱ */
uint8_t  ucSec, ucSec1;             	/* ���ʱ */
uint8_t  ucDuty;                     	/* ռ�ձ� */
uint16_t usFval;                     	/* Ƶ��ֵ */
uint16_t usFmax=2000, usFmax1=2000;   /* Ƶ������ */
uint8_t  ucFnum, ucFflg;           	  /* Ƶ�ʱ�������, ��־ */
uint8_t  ucFdiv=1, ucFdiv1=1;       	/* ��Ƶϵ�� */
uint16_t ucVval;                     	/* ��ѹֵ */
uint8_t  ucVmax=30, ucVmax1=30;     	/* ��ѹ���� */
uint8_t  ucVnum, ucVflg;             	/* ��ѹ��������, ��־ */
uint8_t  ucVmin=9, ucVmin1=9;     	  /* �ط���Сֵ */
uint16_t usTval;                     	/* �¶�ֵ */
uint16_t ucTmax=30, ucTmax1=30;     	/* �¶����� */
uint8_t  ucTnum, ucTflg;             	/* �¶ȱ�������, ��־ */
uint8_t  ucTtim=6, ucTtim1=6;     	  /* �ط�ʱ�� */
sData    sDrec[11];                  	/* ��¼���ݣ�0~10�� */
uint8_t  ucDrec;                     	/* ��¼�ط�״̬ */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void KEY_Proc(void);              	/* �������� */
void LED_Proc(void);              	/* LED���� */
void LCD_Proc(void);              	/* LCD���� */
void DATA_Proc(void);              	/* ���ݴ��� */
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
  MX_TIM1_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  LCD_Init();                      	/* LCD��ʼ�� */
  LCD_Clear(Black);                	/* LCD���� */
  LCD_SetTextColor(White);        	/* �����ַ�ɫ */
  LCD_SetBackColor(Black);        	/* ���ñ���ɫ */
	
  TIM1_SetCompare1(0);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    KEY_Proc();                    	/* �������� */
    LED_Proc();                    	/* LED���� */
    LCD_Proc();                    	/* LCD���� */
    DATA_Proc();    		      			/* ���ݴ��� */
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/* USER CODE BEGIN 4 */
uint8_t  ucKey;                      	/* ����ֵ */
void KEY_Proc(void)               	/* �������� */
{
  uint8_t ucKey_Val = 0;

  if (ucDrec == 1)
    return;

  ucKey_Val = KEY_Read();         	/* ������ȡ */
  if (ucKey_Val != ucKey)        	  /* ��ֵ�仯 */
  {
    ucKey = ucKey_Val;            	/* �����ֵ */
    usTms = 0;                      /* �����ʱ */
    ucSec = 0;
  }
  else
    ucKey_Val = 0;                  /* �����ֵ */

  switch (ucKey_Val)
  {
    case 1:                         /* B1�������� */
      ucState &= 0xf0;
      ucState += 0x10;              /* �����л� */
      if (ucState == 0x20)
      {
        usFmax = usFmax1;           /* ����������Ч */
        ucVmax = ucVmax1;
        ucTmax = ucTmax1;
      }
      if (ucState == 0x40)
      {
        ucFdiv = ucFdiv1;           /* �طŲ�����Ч */
        ucVmin = ucVmin1;
        ucTtim = ucTtim1;
        ucState = 0;
      }
      LCD_Clear(Black);             /* LCD���� */
      break;
    case 2:                         /* B2�������� */
      switch (ucState & 0xf0)
      {
        case 0:
          ucDrec = 1;               /* ��¼���� */
          break;
        case 0x10:
          if (++ucState == 0x13)   	/* ���������л� */
            ucState = 0x10;
          break;
        case 0x20:
          ucFnum = ucVnum = ucTnum = 0;
          break;
        case 0x30:
          if (++ucState == 0x33)    /* �طŲ����л� */
            ucState = 0x30;
      }
      break;
    case 3:                         /* B3�������� */
      switch (ucState)
      {
        case 0:
          if (ucDrec == 2)
            ucDrec = 3;             /* �ط����� */
          break;
        case 0x10:
          if (usFmax1 < 10000)
            usFmax1 += 1000;
          break;
        case 0x11:
          if (ucVmax1 < 33)
            ucVmax1 += 3;
          break;
        case 0x12:
          if (ucTmax1 < 80)
            ucTmax1++;
          break;
        case 0x30:
          if (ucFdiv1 < 10)
            ucFdiv1++;
          break;
        case 0x31:
          if (ucVmin1 < 33)
            ucVmin1 += 3;
          break;
        case 0x32:
          if (ucTtim1 < 10)
            ucTtim1 += 2;
      }
      break;
    case 4:                         /* B4�������� */
      switch (ucState)
      {
        case 0:
          if (ucDrec == 2)
          {
            TIM1_SetAutoReload(999);/* 1KHz */
            ucDrec = 4;             /* �طŵ�ѹ */
          }
          break;
        case 0x10:
          if (usFmax1 > 1000)
            usFmax1 -= 1000;
          break;
        case 0x11:
          if (ucVmax1 > 0)
            ucVmax1 -= 3;
          break;
        case 0x12:
          if (ucTmax1 > 0)
            ucTmax1--;
          break;
        case 0x30:
          if (ucFdiv1 > 1)
            ucFdiv1--;
          break;
        case 0x31:
          if (ucVmin1 > 0)
            ucVmin1 -= 3;
          break;
        case 0x32:
          if (ucTtim1 > 2)
            ucTtim1 -= 2;
      }
  }
  if ((ucKey == 5) && (ucSec == 2))
  {
    ucState = 0;
    usFmax = usFmax1 = 2000;
    ucVmax = ucVmax1 = 30;
    ucTmax = ucTmax1 = 30;
    ucFdiv = ucFdiv1 = 1;
    ucVmin = ucVmin1 = 9;
    ucTtim = ucTtim1 = 6;
    ucFnum = 0;
    ucVnum = 0;
    ucTnum = 0;
    ucDrec = 0;
  }
}

uint8_t  ucLed;                      	/* LEDֵ */
uint8_t  ucTled;                     	/* LED��˸��ʱ */
void LED_Proc(void)               	/* LED���� */
{
  if (ucTled < 100)	            		/* ��˸��� */
    return;
  ucTled = 0;

  if (ucDrec == 1)
    ucLed ^= 1;                     /* LD1��˸ */
  else
    ucLed &= ~1;                    /* LD1Ϩ�� */

  if (ucDrec == 3)
    ucLed ^= 2;                     /* LD2��˸ */
  else
    ucLed &= ~2;                    /* LD2Ϩ�� */

  if (ucDrec == 4)
    ucLed ^= 4;                     /* LD3��˸ */
  else
    ucLed &= ~4;                    /* LD3Ϩ�� */

  if (usFval >= usFmax)
    ucLed |= 8;                     /* LD4���� */
  else
    ucLed &= ~8;                    /* LD4Ϩ�� */

  if (ucVval >= ucVmax)
    ucLed |= 0x10;                 	/* LD5���� */
  else
    ucLed &= ~0x10;                 /* LD5Ϩ�� */

  if ((usTval>>4) >= ucTmax)
    ucLed |= 0x20;                  /* LD6���� */
  else
    ucLed &= ~0x20;                	/* LD6Ϩ�� */
  LED_Disp(ucLed);                 	/* LED��ʾ */
}

uint8_t  ucLcd[21];                  	/* LCDֵ(\0����) */
uint16_t usTlcd;                    	/* LCDˢ�¼�ʱ */
void LCD_Proc(void)               	/* LCD���� */
{
  if (usTlcd < 500)   	      			/* 500msδ�� */
    return;
  usTlcd = 0;

  switch (ucState&0xf0)
  {
    case 0:                         /* ʵʱ���ݽ��� */
      LCD_DisplayStringLine(Line1, (uint8_t *)"        DATA");
      sprintf((char*)ucLcd, "     F=%3u  ", usFval);
      LCD_DisplayStringLine(Line3, ucLcd);
      sprintf((char*)ucLcd, "     D=%2u%% ", ucDuty);
      LCD_DisplayStringLine(Line4, ucLcd);
      sprintf((char*)ucLcd, "     V=%2.1f ", ucVval/10.0);
      LCD_DisplayStringLine(Line5, ucLcd);
      sprintf((char*)ucLcd, "     T=%3.1f", usTval/16.0);
      LCD_DisplayStringLine(Line6, ucLcd);
      sprintf((char*)ucLcd, "     R=%1u ", ucDrec);
      LCD_DisplayStringLine(Line7, ucLcd);
      break;
    case 0x10:                      /* ������������ */
      LCD_DisplayStringLine(Line1, (uint8_t *)"        PARA");
      sprintf((char*)ucLcd, "     FH=%3u  ", usFmax1);
      if (ucState == 0x10) ucLcd[4] = '.';
      LCD_DisplayStringLine(Line3, ucLcd);
      sprintf((char*)ucLcd, "     VH=%2.1f ", ucVmax1/10.0);
      if (ucState == 0x11) ucLcd[4] = '.';
      LCD_DisplayStringLine(Line4, ucLcd);
      sprintf((char*)ucLcd, "     TH=%02u ", ucTmax1);
      if (ucState == 0x12) ucLcd[4] = '.';
      LCD_DisplayStringLine(Line5, ucLcd);
      break;
    case 0x20:                      /* ����ͳ�ƽ��� */
      LCD_DisplayStringLine(Line1, (uint8_t *)"        RECD");
      sprintf((char*)ucLcd, "     FN=%1u  ", ucFnum);
      LCD_DisplayStringLine(Line3, ucLcd);
      sprintf((char*)ucLcd, "     VN=%1u  ", ucVnum);
      LCD_DisplayStringLine(Line4, ucLcd);
      sprintf((char*)ucLcd, "     TN=%1u  ", ucTnum);
      LCD_DisplayStringLine(Line5, ucLcd);
       break;
    case 0x30:                      /* ������������ */
      LCD_DisplayStringLine(Line1, (uint8_t *)"        FSET");
      sprintf((char*)ucLcd, "     FP=%1u ", ucFdiv1);
      if (ucState == 0x30) ucLcd[4] = '.';
      LCD_DisplayStringLine(Line3, ucLcd);
      sprintf((char*)ucLcd, "     VP=%2.1f ", ucVmin1/10.0);
      if (ucState == 0x31) ucLcd[4] = '.';
      LCD_DisplayStringLine(Line4, ucLcd);
      sprintf((char*)ucLcd, "     TT=%1u ", ucTtim1);
      if (ucState == 0x32) ucLcd[4] = '.';
      LCD_DisplayStringLine(Line5, ucLcd);
  }
}

void DATA_Proc(void)               	/* ���ݴ��� */
{
  uint16_t usCapt[2];

  if (ucSec1 == ucSec)              /* 1sδ�� */
    return;
  ucSec1 = ucSec;

//  usTval = DSB_Read();
  if ((usTval>>4) >= ucTmax)
  {
    if (ucTflg == 0)
    {
      ucTnum++;
      ucTflg = 1;
    }
  }
  else
    ucTflg = 0;

  ucVval = ADC2_Read()*33/4095;
  if (ucVval >= ucVmax)
  {
    if (ucVflg == 0)
    {
      ucVnum++;
      ucVflg = 1;
    }
  }
  else
    ucVflg = 0;

  TIM2_GetCapture(usCapt);
  usFval = 1000000/usCapt[1];
  ucDuty = usCapt[0]*100/usCapt[1];
  if (usFval >= usFmax)
  {
    if (ucFflg == 0)
    {
      ucFnum++;
      ucFflg = 1;
    }
  }
  else
    ucFflg = 0;

  switch (ucDrec)
  {
    case 1:                         /* ��¼���� */
      sDrec[ucSec].ucVval = ucVval; 
      sDrec[ucSec].usFval = usFval; 
      sDrec[ucSec].ucDuty = ucDuty;
      if (ucSec == ucTtim)
        ucDrec = 2;                 /* ��ɼ�¼ */
      break;
    case 3:                         /* �ط����� */
      usCapt[1] = 1000000*ucFdiv/sDrec[ucSec].usFval;
      usCapt[0] = sDrec[ucSec].ucDuty*usCapt[1]/100;
      TIM1_SetAutoReload(usCapt[1]-1);
      TIM1_SetCompare1(usCapt[0]);
      if (ucSec == ucTtim)
      {
        TIM1_SetCompare1(0);
        ucDrec = 2;                 /* ��ɻط� */
      }
      break;
    case 4:                         /* �طŵ�ѹ */
      if (sDrec[ucSec].ucVval < ucVmin)
        TIM1_SetCompare1(100);      /* ռ�ձ�10% */
      else
        TIM1_SetCompare1(100+900*(sDrec[ucSec].ucVval-ucVmin)/(33-ucVmin));
      if (ucSec == ucTtim)
        ucDrec = 2;                 /* ��ɻط� */
  }
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
