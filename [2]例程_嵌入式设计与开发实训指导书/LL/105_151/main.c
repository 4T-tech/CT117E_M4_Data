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
uint8_t  ucState;                    	/* 系统状态 */
uint16_t usTms;    	 				        	/* 毫秒计时 */
uint16_t usSec;                      	/* 秒计时 */
uint16_t usCap2[2];			          		/* TIM2输入捕捉值 */
uint16_t usCap3[2];			          		/* TIM3输入捕捉值 */
int16_t  ssFreqa, ssFreqb;         		/* 频率值 */
int16_t  ssPeria, ssPerib;         		/* 周期值 */
uint16_t usPd=1000;			           		/* 突变参数 */
uint16_t usPh=5000;		            		/* 超限参数 */
int16_t  ssPx=0;			            		/* 校准参数 */
uint16_t usFreq[2][30];            		/* 频率值 */
uint8_t  ucIndex;                    	/* 频率索引 */
uint8_t  ucNda;                      	/* 通道A突变次数 */
uint8_t  ucNdb;                      	/* 通道B突变次数 */
uint8_t  ucNha;                      	/* 通道A超限次数 */
uint8_t  ucNhb;                      	/* 通道A超限次数 */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void KEY_Proc(void);              	  /* 按键处理 */
void LED_Proc(void);                	/* LED处理 */
void LCD_Proc(void);                	/* LCD处理 */
void TIM_Proc(void);                	/* TIM处理 */
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
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  LCD_Init();                       	/* LCD初始化 */
  LCD_Clear(Black);                 	/* LCD清屏 */
  LCD_SetTextColor(White);        	  /* 设置字符色 */
  LCD_SetBackColor(Black);        	  /* 设置背景色 */
  /* USER CODE END 2 */
 
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    KEY_Proc();                     	/* 按键处理 */
    LED_Proc();                     	/* LED处理 */
    LCD_Proc();                     	/* LCD处理 */
    TIM_Proc();                     	/* TIM处理 */
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/* USER CODE BEGIN 4 */
uint8_t  ucKey;                   	  /* 按键值 */
void KEY_Proc(void)               	  /* 按键处理 */
{
  uint8_t ucKey1 = 0, ucKey2 = 0;

  ucKey1 = KEY_Read();          	    /* 按键读取 */
  if (ucKey1 != ucKey)            	  /* 键值变化 */
	{
    ucKey2 = ucKey;               	  /* 保存键值 */
    ucKey = ucKey1;               	  /* 保存键值 */
	}
  else
	{
    ucKey1 = 0;                     	/* 清除键值 */
    ucKey2 = 0;                     	/* 清除键值 */
	}
	
  switch (ucKey1)
	{
    case 1:                         	/* B1键按下 */
     switch (ucState)
			{
        case 0x10:
          if (usPd < 1000)
		        usPd += 100;
          break;
        case 0x11:
          if (usPh < 10000)
            usPh += 100;
					break;
        case 0x12:
          if (ssPx < 1000)
            ssPx += 100;
			}
      break;
    case 2:                         	/* B2键按下 */
     switch (ucState)
			{
        case 0x10:
          if (usPd > 100)
		        usPd -= 100;
          break;
        case 0x11:
          if (usPh > 1000)
            usPh -= 100;
					break;
        case 0x12:
          if (ssPx > -1000)
            ssPx -= 100;
			}
      break;
    case 3:                         	/* B3键按下 */
      switch (ucState&0xf0)
			{
        case 0:
		      ucState ^= 1;
          break;
        case 0x10:
          if (++ucState == 0x13)
            ucState = 0x10;
          break;
        case 0x20:
          usTms = 0;
          usSec = 0;
			}
      break;
    case 4:                         	/* B4键按下 */
      ucState &= 0xf0;
      ucState += 0x10;
      if (ucState == 0x30)
        ucState = 0;
      LCD_Clear(Black);              	/* LCD清屏 */
	}
	if ((ucState == 0x20) && (ucKey2 == 3) && (usSec >= 1))
	{
    ucNda = ucNdb = ucNha = ucNhb = 0;
	}
}

uint8_t  ucLed;                      	/* LED值 */
void LED_Proc(void)               	  /* LED处理 */
{
  if ((ucState & 0xf0) == 0)
    ucLed |= 1;                 	    /* LD1点亮 */
	else
    ucLed &= ~1;                 	    /* LD1熄灭 */
		
  if (ssFreqa > usPh)
    ucLed |= 2;                 	    /* LD2点亮 */
	else
    ucLed &= ~2;                 	    /* LD2熄灭 */

  if (ssFreqb > usPh)
    ucLed |= 4;                 	    /* LD3点亮 */
	else
    ucLed &= ~4;                 	    /* LD3熄灭 */

  if ((ucNda >= 3)||(ucNdb >= 3))
    ucLed |= 0x80;               	    /* LD8点亮 */
	else
    ucLed &= ~0x80;              	    /* LD8熄灭 */

  LED_Disp(ucLed);                   	/* LED显示 */
}

uint8_t  ucLcd[21];                  	/* LCD值(\0结束) */
uint16_t usTlcd;                    	/* LCD刷新计时 */
void LCD_Proc(void)   	            	/* LCD处理 */
{
  if (usTlcd < 100)   		        		/* 100ms未到 */
    return;
  usTlcd = 0;

  switch (ucState)
	{
		case 0:   		        		        /* 频率数据界面 */
		  sprintf((char*)ucLcd, "        DATA        ");
      LCD_DisplayStringLine(Line1, ucLcd);
		  if (ssFreqa < 0)
        sprintf((char *)ucLcd, "     A=NULL        ");
		  else if (ssFreqa < 1000)
        sprintf((char *)ucLcd, "     A=%uHz        ", ssFreqa);
			else
        sprintf((char *)ucLcd, "     A=%4.2fKHz      ", ssFreqa/1000.0);
      LCD_DisplayStringLine(Line3, ucLcd);
		  if (ssFreqb < 0)
        sprintf((char *)ucLcd, "     B=NULL        ");
		  else if (ssFreqb < 1000)
        sprintf((char *)ucLcd, "     B=%uHz        ", ssFreqb);
			else
        sprintf((char *)ucLcd, "     B=%4.2fKHz      ", ssFreqb/1000.0);
      LCD_DisplayStringLine(Line4, ucLcd);
      break;
    case 1:   		        		        /* 周期数据界面 */
		  sprintf((char*)ucLcd, "        DATA        ");
      LCD_DisplayStringLine(Line1, ucLcd);
		  if (ssPeria < 0)
        sprintf((char *)ucLcd, "     A=NULL        ");
		  else if (ssPeria < 1000)
        sprintf((char *)ucLcd, "     A=%uuS        ", ssPeria);
			else
        sprintf((char *)ucLcd, "     A=%4.2fmS      ", ssPeria/1000.0);
      LCD_DisplayStringLine(Line3, ucLcd);
		  if (ssPerib < 0)
        sprintf((char *)ucLcd, "     B=NULL        ");
		  else if (ssPerib < 1000)
        sprintf((char *)ucLcd, "     B=%uuS        ", ssPerib);
			else
        sprintf((char *)ucLcd, "     B=%4.2fmS      ", ssPerib/1000.0);
      LCD_DisplayStringLine(Line4, ucLcd);
      break;
    case 0x10:   		        		        /* 参数界面 */
    case 0x11:
    case 0x12:
		  sprintf((char*)ucLcd, "        PARA        ");
      LCD_DisplayStringLine(Line1, ucLcd);
      sprintf((char *)ucLcd, "     PD=%uHz        ", usPd);
      if (ucState == 0x10)
				ucLcd[4]='.';
  		LCD_DisplayStringLine(Line3, ucLcd);
      sprintf((char *)ucLcd, "     PH=%uHz        ", usPh);
      if (ucState == 0x11)
				ucLcd[4]='.';
      LCD_DisplayStringLine(Line4, ucLcd);
      sprintf((char *)ucLcd, "     PX=%dHz        ", ssPx);
      if (ucState == 0x12)
				ucLcd[4]='.';
      LCD_DisplayStringLine(Line5, ucLcd);
      break;
    case 0x20:   		        		        /* 记录界面 */
		  sprintf((char*)ucLcd, "        RECD        ");
      LCD_DisplayStringLine(Line1, ucLcd);
      sprintf((char *)ucLcd, "     NDA=%u        ", ucNda);
      LCD_DisplayStringLine(Line3, ucLcd);
      sprintf((char *)ucLcd, "     NDB=%u        ", ucNdb);
      LCD_DisplayStringLine(Line4, ucLcd);
      sprintf((char *)ucLcd, "     NHA=%u        ", ucNha);
      LCD_DisplayStringLine(Line5, ucLcd);
      sprintf((char *)ucLcd, "     NHB=%u        ", ucNhb);
      LCD_DisplayStringLine(Line6, ucLcd);
  }
}

uint16_t usTtim;                    	/* TIM刷新计时 */
uint8_t  ucPhab;                     	/* 超限标志 */
void TIM_Proc(void)   	            	/* TIM处理 */
{
  uint16_t usFmaxa, usFmaxb;       		/* 频率最大值 */
  uint16_t usFmina, usFminb;       		/* 频率最小值 */

  if (usTtim < 100)   		        		/* 100ms未到 */
    return;
  usTtim = 0;

	TIM2_GetCapture(usCap3);
  TIM3_GetCapture(usCap2);
	ssFreqa = 1000000/usCap2[0] + ssPx;
	ssFreqb = 1000000/usCap3[0] + ssPx;
	ssPeria = 1000000/ssFreqa;
	ssPerib = 1000000/ssFreqb;

	if (ssFreqa > usPh)   		        	/* 频率A超限 */
	{
		if ((ucPhab & 1) == 0)
	  {
		  ucNha++;
		  ucPhab |= 1;
	  }
	}
  else
		ucPhab &= ~1;

	if (ssFreqb > usPh)   		        	/* 频率B超限 */
	{
		if ((ucPhab & 2) == 0)
	  {
		  ucNhb++;
		  ucPhab |= 2;
	  }
	}	
  else
	  ucPhab &= ~2;

	if (ssFreqa >= 0)
		usFreq[0][ucIndex] = ssFreqa;   	/* 保存频率A */
	else
		usFreq[0][ucIndex] = 0;

	if (ssFreqb >= 0)
		usFreq[1][ucIndex] = ssFreqb;   	/* 保存频率B */
	else
		usFreq[1][ucIndex] = 0;
		
	if (++ucIndex == 30)
	{
    ucIndex = 0;
    usFmaxa = usFmaxb = 0;
    usFmina = usFminb = 65535;
    for (int i=0; i<30; i++)   	      /* 统计最大值和最小值 */
		{
	    if (usFmaxa <= usFreq[0][i])
		    usFmaxa = usFreq[0][i];
	    if (usFmaxb <= usFreq[1][i])
		    usFmaxb = usFreq[1][i];
	    if (usFmina >= usFreq[0][i])
		    usFmina = usFreq[0][i];
	    if (usFminb >= usFreq[1][i])
		    usFminb = usFreq[1][i];
	  }
    if (usFmaxa - usFmina > usPd)   	/* 频率A突变 */
		  ucNda++;
    if (usFmaxb - usFminb > usPd)   	/* 频率B突变 */
		  ucNdb++;
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
