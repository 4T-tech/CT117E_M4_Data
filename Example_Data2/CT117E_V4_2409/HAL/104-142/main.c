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
  uint8_t  ucVval;                 	/* 电压值 */
  uint16_t usFval;                 	/* 频率值 */
  uint8_t  ucDuty;                 	/* 占空比 */
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
uint8_t  ucState;                    	/* 系统状态 */
uint16_t usTms;                      	/* 毫秒计时 */
uint8_t  ucSec, ucSec1;             	/* 秒计时 */
uint8_t  ucDuty;                     	/* 占空比 */
uint16_t usFval;                     	/* 频率值 */
uint16_t usFmax=2000, usFmax1=2000;   /* 频率上限 */
uint8_t  ucFnum, ucFflg;           	  /* 频率报警次数, 标志 */
uint8_t  ucFdiv=1, ucFdiv1=1;       	/* 分频系数 */
uint16_t ucVval;                     	/* 电压值 */
uint8_t  ucVmax=30, ucVmax1=30;     	/* 电压上限 */
uint8_t  ucVnum, ucVflg;             	/* 电压报警次数, 标志 */
uint8_t  ucVmin=9, ucVmin1=9;     	  /* 回放最小值 */
uint16_t usTval;                     	/* 温度值 */
uint16_t ucTmax=30, ucTmax1=30;     	/* 温度上限 */
uint8_t  ucTnum, ucTflg;             	/* 温度报警次数, 标志 */
uint8_t  ucTtim=6, ucTtim1=6;     	  /* 回放时间 */
sData    sDrec[11];                  	/* 记录数据（0~10） */
uint8_t  ucDrec;                     	/* 记录回放状态 */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void KEY_Proc(void);              	/* 按键处理 */
void LED_Proc(void);              	/* LED处理 */
void LCD_Proc(void);              	/* LCD处理 */
void DATA_Proc(void);              	/* 数据处理 */
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
  LCD_Init();                      	/* LCD初始化 */
  LCD_Clear(Black);                	/* LCD清屏 */
  LCD_SetTextColor(White);        	/* 设置字符色 */
  LCD_SetBackColor(Black);        	/* 设置背景色 */
	
  TIM1_SetCompare1(0);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    KEY_Proc();                    	/* 按键处理 */
    LED_Proc();                    	/* LED处理 */
    LCD_Proc();                    	/* LCD处理 */
    DATA_Proc();    		      			/* 数据处理 */
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/* USER CODE BEGIN 4 */
uint8_t  ucKey;                      	/* 按键值 */
void KEY_Proc(void)               	/* 按键处理 */
{
  uint8_t ucKey_Val = 0;

  if (ucDrec == 1)
    return;

  ucKey_Val = KEY_Read();         	/* 按键读取 */
  if (ucKey_Val != ucKey)        	  /* 键值变化 */
  {
    ucKey = ucKey_Val;            	/* 保存键值 */
    usTms = 0;                      /* 毫秒计时 */
    ucSec = 0;
  }
  else
    ucKey_Val = 0;                  /* 清除键值 */

  switch (ucKey_Val)
  {
    case 1:                         /* B1按键按下 */
      ucState &= 0xf0;
      ucState += 0x10;              /* 界面切换 */
      if (ucState == 0x20)
      {
        usFmax = usFmax1;           /* 报警参数生效 */
        ucVmax = ucVmax1;
        ucTmax = ucTmax1;
      }
      if (ucState == 0x40)
      {
        ucFdiv = ucFdiv1;           /* 回放参数生效 */
        ucVmin = ucVmin1;
        ucTtim = ucTtim1;
        ucState = 0;
      }
      LCD_Clear(Black);             /* LCD清屏 */
      break;
    case 2:                         /* B2按键按下 */
      switch (ucState & 0xf0)
      {
        case 0:
          ucDrec = 1;               /* 记录数据 */
          break;
        case 0x10:
          if (++ucState == 0x13)   	/* 报警参数切换 */
            ucState = 0x10;
          break;
        case 0x20:
          ucFnum = ucVnum = ucTnum = 0;
          break;
        case 0x30:
          if (++ucState == 0x33)    /* 回放参数切换 */
            ucState = 0x30;
      }
      break;
    case 3:                         /* B3按键按下 */
      switch (ucState)
      {
        case 0:
          if (ucDrec == 2)
            ucDrec = 3;             /* 回放脉冲 */
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
    case 4:                         /* B4按键按下 */
      switch (ucState)
      {
        case 0:
          if (ucDrec == 2)
          {
            TIM1_SetAutoReload(999);/* 1KHz */
            ucDrec = 4;             /* 回放电压 */
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

uint8_t  ucLed;                      	/* LED值 */
uint8_t  ucTled;                     	/* LED闪烁延时 */
void LED_Proc(void)               	/* LED处理 */
{
  if (ucTled < 100)	            		/* 闪烁间隔 */
    return;
  ucTled = 0;

  if (ucDrec == 1)
    ucLed ^= 1;                     /* LD1闪烁 */
  else
    ucLed &= ~1;                    /* LD1熄灭 */

  if (ucDrec == 3)
    ucLed ^= 2;                     /* LD2闪烁 */
  else
    ucLed &= ~2;                    /* LD2熄灭 */

  if (ucDrec == 4)
    ucLed ^= 4;                     /* LD3闪烁 */
  else
    ucLed &= ~4;                    /* LD3熄灭 */

  if (usFval >= usFmax)
    ucLed |= 8;                     /* LD4点亮 */
  else
    ucLed &= ~8;                    /* LD4熄灭 */

  if (ucVval >= ucVmax)
    ucLed |= 0x10;                 	/* LD5点亮 */
  else
    ucLed &= ~0x10;                 /* LD5熄灭 */

  if ((usTval>>4) >= ucTmax)
    ucLed |= 0x20;                  /* LD6点亮 */
  else
    ucLed &= ~0x20;                	/* LD6熄灭 */
  LED_Disp(ucLed);                 	/* LED显示 */
}

uint8_t  ucLcd[21];                  	/* LCD值(\0结束) */
uint16_t usTlcd;                    	/* LCD刷新计时 */
void LCD_Proc(void)               	/* LCD处理 */
{
  if (usTlcd < 500)   	      			/* 500ms未到 */
    return;
  usTlcd = 0;

  switch (ucState&0xf0)
  {
    case 0:                         /* 实时数据界面 */
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
    case 0x10:                      /* 报警参数界面 */
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
    case 0x20:                      /* 报警统计界面 */
      LCD_DisplayStringLine(Line1, (uint8_t *)"        RECD");
      sprintf((char*)ucLcd, "     FN=%1u  ", ucFnum);
      LCD_DisplayStringLine(Line3, ucLcd);
      sprintf((char*)ucLcd, "     VN=%1u  ", ucVnum);
      LCD_DisplayStringLine(Line4, ucLcd);
      sprintf((char*)ucLcd, "     TN=%1u  ", ucTnum);
      LCD_DisplayStringLine(Line5, ucLcd);
       break;
    case 0x30:                      /* 报警参数界面 */
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

void DATA_Proc(void)               	/* 数据处理 */
{
  uint16_t usCapt[2];

  if (ucSec1 == ucSec)              /* 1s未到 */
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
    case 1:                         /* 记录数据 */
      sDrec[ucSec].ucVval = ucVval; 
      sDrec[ucSec].usFval = usFval; 
      sDrec[ucSec].ucDuty = ucDuty;
      if (ucSec == ucTtim)
        ucDrec = 2;                 /* 完成记录 */
      break;
    case 3:                         /* 回放脉冲 */
      usCapt[1] = 1000000*ucFdiv/sDrec[ucSec].usFval;
      usCapt[0] = sDrec[ucSec].ucDuty*usCapt[1]/100;
      TIM1_SetAutoReload(usCapt[1]-1);
      TIM1_SetCompare1(usCapt[0]);
      if (ucSec == ucTtim)
      {
        TIM1_SetCompare1(0);
        ucDrec = 2;                 /* 完成回放 */
      }
      break;
    case 4:                         /* 回放电压 */
      if (sDrec[ucSec].ucVval < ucVmin)
        TIM1_SetCompare1(100);      /* 占空比10% */
      else
        TIM1_SetCompare1(100+900*(sDrec[ucSec].ucVval-ucVmin)/(33-ucVmin));
      if (ucSec == ucTtim)
        ucDrec = 2;                 /* 完成回放 */
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
