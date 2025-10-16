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
uint8_t  ucState;                   /* 系统状态 */
uint8_t  ucSec=6;                   /* 秒计时 */
uint8_t  ucLcd[21];                 /* LCD值(\0结束) */
uint8_t  ucXcnt, ucYcnt;           	/* 商品购买数量 */
uint8_t  ucXprice=10, ucYprice=10;	/* 商品价格（*10） */
uint8_t  ucXrep=10, ucYrep=10;     	/* 商品库存数量 */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void KEY_Proc(void);              	/* 按键处理 */
void LED_Proc(void);              	/* LED处理 */
void LCD_Proc(void);              	/* LCD处理 */
void UART_Proc(void);              	/* UART处理 */
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
  MX_TIM2_Init();
  MX_TIM4_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  LCD_Init();                      	/* LCD初始化 */
  LCD_Clear(Black);                	/* LCD清屏 */
  LCD_SetTextColor(White);        	/* 设置字符色 */
  LCD_SetBackColor(Black);        	/* 设置背景色 */
	
  TIM4_SetAutoReload(499);         	/* 频率2KHz */
  TIM4_SetCompare1(25);            	/* 占空比5% */
  MEM_Read(ucLcd, 0, 4);	        	/* 读取参数 */
  if (ucLcd[0] < 10)
    ucXrep = ucLcd[0];
  if (ucLcd[1] < 10)
    ucYrep = ucLcd[1];
  if ((ucLcd[2] >= 10) && (ucLcd[2] <= 20))
    ucXprice = ucLcd[2];
  if ((ucLcd[3] >= 10) && (ucLcd[3] <= 20))
    ucYprice = ucLcd[3];
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    KEY_Proc();                    	/* 按键处理 */
    LED_Proc();                    	/* LED处理 */
    LCD_Proc();                    	/* LCD处理 */
    UART_Proc();    		      			/* UART处理 */
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/* USER CODE BEGIN 4 */
uint8_t  ucKey;                     /* 按键值 */
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
    case 1:                         /* B1按键按下 */
      if (++ucState == 3)
        ucState = 0;
      break;
    case 2:                         /* B2按键按下 */
      switch (ucState)
      {
        case 0:
          if (++ucXcnt == ucXrep + 1)
            ucXcnt = 0;
          break;
        case 1:
          if (++ucXprice == 21)
            ucXprice = 10;
          ucLcd[0] = ucXprice;
          MEM_Write(ucLcd, 2, 1);	  /* 保存X价格 */
          break;
        case 2:
          ++ucXrep;
          ucLcd[0] = ucXrep;
          MEM_Write(ucLcd, 0, 1);	  /* 保存X库存 */
      }
      break;
    case 3:                         /* B3按键按下 */
      switch (ucState)
      {
        case 0:
          if (++ucYcnt == ucYrep + 1)
            ucYcnt = 0;
          break;
        case 1:
          if (++ucYprice == 21)
            ucYprice = 10;
          ucLcd[0] = ucYprice;
          MEM_Write(ucLcd, 3, 1);	  /* 保存Y价格 */
          break;
        case 2:
          ++ucYrep;
          ucLcd[0] = ucYrep;
          MEM_Write(ucLcd, 1, 1);	  /* 保存Y库存 */
      }
      break;
    case 4:                         /* B4按键按下 */
      if (ucState == 0)
      {
        ucXrep -= ucXcnt;
        ucYrep -= ucYcnt;
        ucLcd[0] = ucXrep;
        ucLcd[1] = ucYrep;
        ucLcd[2] = ucXprice;
        ucLcd[3] = ucYprice;
        MEM_Write(ucLcd, 0, 2);	   /* 保存库存数量 */
        printf("X:%u,Y:%u,Z:%2.1f\r\n", ucXcnt, ucYcnt,
          (ucXcnt*ucXprice+ucYcnt*ucYprice)/10.0);
        ucSec = 0;
        ucXcnt = 0;
        ucYcnt = 0;
      }
  }	
}

uint8_t  ucLed;                     /* LED值 */
uint8_t  ucTled;                    /* LED闪烁计时 */
void LED_Proc(void)               	/* LED处理 */
{
  if (ucTled < 100)
    return;
  ucTled = 0;

  if (ucSec < 5)
  {
    ucLed |= 1;                     /* LD1点亮 */
    TIM4_SetCompare1(150);         	/* 占空比30% */
  }
  else
  {
    ucLed &= ~1;                    /* LD1熄灭 */
    TIM4_SetCompare1(25);          	/* 占空比5% */
  }
  if ((ucXrep == 0) && (ucYrep == 0))	/* 库存数量为0 */
    ucLed ^= 2;                     /* LD2闪烁 */
  else
    ucLed &= ~2;                    /* LD2熄灭 */
  LED_Disp(ucLed);                 	/* LED显示 */
}

uint16_t usTlcd;                    /* LCD刷新计时 */
uint16_t usCapt[2];                	/* TIM输入捕捉值 */
void LCD_Proc(void)               	/* LCD处理 */
{
  if (usTlcd < 500)   	      			/* 500ms未到 */
    return;
  usTlcd = 0;

  switch (ucState)
  {
    case 0:
      LCD_DisplayStringLine(Line1, (uint8_t *)"        SHOP ");
      sprintf((char*)ucLcd, "     X:%1u  ", ucXcnt);
      LCD_DisplayStringLine(Line3, ucLcd);
      sprintf((char*)ucLcd, "     Y:%1u  ", ucYcnt);
      LCD_DisplayStringLine(Line4, ucLcd);
      break;
    case 1:
      LCD_DisplayStringLine(Line1, (uint8_t *)"        PRICE");
      sprintf((char*)ucLcd, "     X:%2.1f  ", ucXprice/10.0);
      LCD_DisplayStringLine(Line3, ucLcd);
      sprintf((char*)ucLcd, "     Y:%2.1f  ", ucYprice/10.0);
      LCD_DisplayStringLine(Line4, ucLcd);
      break;
    case 2:
      LCD_DisplayStringLine(Line1, (uint8_t *)"        REP  ");
      sprintf((char*)ucLcd, "     X:%1u  ", ucXrep);
      LCD_DisplayStringLine(Line3, ucLcd);
      sprintf((char*)ucLcd, "     Y:%1u  ", ucYrep);
      LCD_DisplayStringLine(Line4, ucLcd);
  }
  TIM2_GetCapture(usCapt);
  sprintf((char *)ucLcd, " FRE:%4uHz DUT:%02u%%  ",
    1000000/usCapt[0], (usCapt[0]-usCapt[1])*100/usCapt[0]);
  LCD_DisplayStringLine(Line6, ucLcd);
}

uint8_t  ucUrx[2];                  /* UART接收值 */
void UART_Proc(void)              	/* UART处理 */
{
  if (UART_Receive(ucUrx, 1) == 0)	/* 接收到字符 */
    if (ucUrx[0] == '?')
      printf("X:%2.1f,Y:%2.1f\r\n", ucXprice/10.0, ucYprice/10.0);
}

int fputc(int ch, FILE *f)		    	/* printf()实现 */
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
