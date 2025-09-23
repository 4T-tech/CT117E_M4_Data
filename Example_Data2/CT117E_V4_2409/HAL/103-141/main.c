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
uint8_t ucState;                    /* 系统状态 */
uint8_t ucT200;                     /* 频率步进计时 */
uint8_t ucStep;                     /* 频率步进标志 */
uint16_t usFreq=4000;               /* 频率值 */
uint8_t ucMode=0;                   /* 输出模式(低频模式) */
uint8_t ucMode1=0;                  /* 输出模式1(低频模式) */
uint8_t ucPara='R';                 /* 参数选择 */
uint8_t ucDuty;                     /* 占空比 */
uint16_t usV[4];                    /* 脉宽, 周期, 速度, 速度1 */
uint8_t ucR=1, ucK=1;               /* 参数 */
uint8_t ucR1=1, ucK1=1;             /* 参数1 */
uint8_t ucN;                        /* 模式切换次数 */
uint16_t usMl, usMh;                /* 速度最大值 */
uint16_t usT2s;                     /* 速度统计计时 */
uint8_t ucLock;                     /* 锁定状态 */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void KEY_Proc(void);              	/* 按键处理 */
void LED_Proc(void);              	/* LED处理 */
void LCD_Proc(void);              	/* LCD处理 */
void ADC_Proc(void);              	/* ADC处理 */
void TIM_Proc(void);                /* TIM处理 */
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
  LCD_Init();                      	/* LCD初始化 */
  LCD_Clear(Black);                	/* LCD清屏 */
  LCD_SetTextColor(White);        	/* 设置字符色 */
  LCD_SetBackColor(Black);        	/* 设置背景色 */
	
  TIM4_SetAutoReload(249);          /* PA11输出频率4kHz */
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    KEY_Proc();                    	/* 按键处理 */
    LED_Proc();                    	/* LED处理 */
    LCD_Proc();                    	/* LCD处理 */
    ADC_Proc();    	        				/* ADC处理 */
    TIM_Proc();                     /* TIM处理 */
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/* USER CODE BEGIN 4 */
uint8_t ucKey;                      /* 按键值 */
uint8_t ucSec=5;                    /* 秒计时 */
void KEY_Proc(void)               	/* 按键处理 */
{
  uint8_t ucKey_Val, ucKey_Dn, ucKey_Up;

  ucKey_Val = KEY_Read();          	/* 读取按键 */
  ucKey_Dn = ucKey_Val & (ucKey ^ ucKey_Val);
  ucKey_Up = ~ucKey_Val & (ucKey ^ ucKey_Val);
  ucKey = ucKey_Val;

  switch (ucKey_Dn) {
    case 1:                         /* B1键按下 */
      if (++ucState == 3)           /* 切换状态 */
        ucState = 0;
      if (ucState == 1)
        ucPara = 'R';
      if (ucState == 2) {
        ucR = ucR1;                 /* 参数生效 */
        ucK = ucK1; }
      LCD_Clear(Black);             /* LCD清屏 */
      break;
    case 2:                         /* B2键按下 */
      if ((ucState == 0) && (ucSec >= 5)) {
        ucMode ^= 1;                /* 切换模式 */
        if (ucMode == 0) {
          usFreq = 8000;            /* 8kHz -> 4kHz*/
          ucStep = 1; }             /* 频率递减 */
        else {
          usFreq = 4000;            /* 4kHz -> 8kHz*/
          ucStep = 2; }             /* 频率递加 */
        ucSec = 0;
        ucN++; }
      else {                        /* 切换参数 */
        if (ucPara == 'R')
          ucPara = 'K';
        else
          ucPara = 'R'; }
      break;
    case 3:                         /* B3键按下 */
      if (ucState == 1) {
        if (ucPara == 'R')
          if (++ucR1 == 11)         /* 参数加 */
            ucR1 = 1;
        if (ucPara == 'K')
          if (++ucK1 == 11)         /* 参数加 */
            ucK1 = 1; }
      break;
    case 4:                         /* B4键按下 */
      if (ucState == 0) {
        ucSec = 0;
        ucLock = 0;	}               /* 解锁 */
      if (ucState == 1) {
        if (ucPara == 'R')
          if (--ucR1 == 0)          /* 参数键 */
            ucR1 = 10;
        if (ucPara == 'K')
          if (--ucK1 == 0)          /* 参数减 */
            ucK1 = 10; }
  }
  if ((ucKey_Up == 4) && (ucState == 0) && (ucSec >= 2))
    ucLock = 1;		                  /* 锁定 */
}

uint8_t ucLed;                      /* LED值 */
uint8_t ucTled;                     /* LED闪烁延时 */
void LED_Proc(void)               	/* LED处理 */
{
  if (ucTled < 100)
    return;
  ucTled = 0;

  if (ucState == 0)
    ucLed |= 1;                     /* LD1点亮 */
  else
    ucLed &= ~1;                    /* LD1熄灭 */

  if (ucStep >= 1)
    ucLed ^= 2;                     /* LD2闪烁 */
  else
    ucLed &= ~2;                    /* LD2熄灭 */
	
  if (ucLock == 1)
    ucLed |= 4;                     /* LD3点亮 */
  else
    ucLed &= ~4;                    /* LD3熄灭 */
  LED_Disp(ucLed);                 	/* LED显示 */
}

uint8_t ucLcd[21];                  /* LCD值(\0结束) */
uint16_t usTlcd;                    /* LCD刷新计时 */
void LCD_Proc(void)               	/* LCD处理 */
{
  if (usTlcd < 100)                 /* 100ms未到 */
    return;
  usTlcd = 0;

  switch (ucState) {
    case 0:                         /* 数据界面 */
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
    case 1:                         /* 参数界面 */
      LCD_DisplayStringLine(Line1, (unsigned char*)"        PARA");
      sprintf((char*)ucLcd, "     R=%u ", ucR1);
      if (ucPara == 'R') ucLcd[4] = '.';
      LCD_DisplayStringLine(Line3, ucLcd);
      sprintf((char*)ucLcd, "     K=%u ", ucK1);
      if (ucPara == 'K') ucLcd[4] = '.';
      LCD_DisplayStringLine(Line4, ucLcd);
      break;
    case 2:                         /* 记录界面 */
      LCD_DisplayStringLine(Line1, (unsigned char*)"        RECD");
      sprintf((char*)ucLcd, "     N=%u ", ucN);
      LCD_DisplayStringLine(Line3, ucLcd);
      sprintf((char*)ucLcd, "     MH=%4.1f ", usMh/10.0);
      LCD_DisplayStringLine(Line4, ucLcd);
      sprintf((char*)ucLcd, "     ML=%3.1f ", usMl/10.0);
      LCD_DisplayStringLine(Line5, ucLcd);
  }
}

void ADC_Proc(void)               	/* ADC处理 */
{
  uint8_t ucAdc;                    /* ADC电压*10 */

  if (ucLock == 1)
    return;

  ucAdc = ADC2_Read()*33/4095;      /* ADC读取 */
  if (ucAdc < 10)
    ucDuty = 10;
  else if (ucAdc > 30)
    ucDuty = 85;
  else
    ucDuty = (375*ucAdc-2750)/100;
  TIM4_SetCompare1(10000*ucDuty/usFreq);
}

void TIM_Proc(void)                 /* TIM处理 */
{
  TIM2_GetCapture(usV);             /* TIM2捕捉脉宽和周期 */
  usV[2]=628000*ucR/ucK/usV[0];     /* 速度值计算*10 */
  if (usV[2] != usV[3]) {
    usT2s = 0;                      /* 2s计时开始 */
    usV[3] = usV[2]; }

  if (usT2s > 2000) {               /* 2s计时到 */
    if (ucMode1 == 0) {
      if (usV[2] > usMl)
        usMl = usV[2]; }            /* 记录低频最大值 */
    else
      if (usV[2] > usMh)
        usMh = usV[2]; }            /* 记录高频最大值 */

  if (ucStep == 0)
    return;
  if (ucT200 < 200)                 /* 步进200ms */
    return;
  ucT200 = 0;

  if (ucStep == 1)
    usFreq -= 160;                  /* 频率递减 */
  else
    usFreq += 160;                  /* 频率递加 */
  TIM4_SetAutoReload(1000000/usFreq-1);
  TIM4_SetCompare1(10000*ucDuty/usFreq);
  if ((usFreq <= 4000) ||(usFreq >= 8000)) {
    ucStep = 0;                     /* 步进结束 */
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
