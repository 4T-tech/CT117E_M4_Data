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
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "stdio.h"
#include "math.h"
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
uint8_t  ucState;                  	/* 状态 */
uint8_t  ucSec;                    	/* 秒计时 */
uint8_t  ucLed;                    	/* LED值 */
uint8_t  ucDsta;                   	/* 设备状态： 0-空闲，1-运行准备，2-运行处理，3-等待 */
uint16_t usCoor[100][2];           	/* 坐标数值 */
uint8_t  ucCnum;                   	/* 坐标数量 */
uint8_t  ucCno;                   	/* 坐标序号 */
uint16_t usCpx;                    	/* 设备位置x */
uint16_t usCpy;                    	/* 设备位置y */
uint8_t  ucScen;                   	/* 场景 */
uint16_t usSpeed;		  	        		/* 行进速度*10 */
uint8_t  ucR=10;                   	/* 参数R*10 */
uint8_t  ucB=10;                   	/* 参数B */
uint16_t usTS;		    	        		/* 累计行程*10 */
uint16_t usTT;		    	        		/* 累计时长*10 */
uint8_t  ucDuty[6]={10,30,50,70,90,95}; /* 占空比 */
uint8_t  ucDno;                   	/* 占空比序号 */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void KEY_Proc(void);              	/* 按键处理 */
void LED_Proc(void);              	/* LED处理 */
void LCD_Proc(void);              	/* LCD处理 */
void UART_Proc(void);              	/* UART处理 */
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
  MX_TIM4_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  LCD_Init();                      	/* LCD初始化 */
  LCD_Clear(Black);                	/* LCD清屏 */
  LCD_SetTextColor(White);        	/* 设置字符色 */
  LCD_SetBackColor(Black);        	/* 设置背景色 */

  TIM4_SetAutoReload(999);         	/* 配置场景1 */
  TIM1_SetAutoReload(999);
  TIM4_SetCompare1(0);
  TIM1_SetCompare1(0);
  ucLed |= 0x10;                  	/* LD5点亮 */
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    KEY_Proc();                    	/* 按键处理 */
    LED_Proc();                    	/* LED处理 */
    LCD_Proc();                    	/* LCD处理 */
    UART_Proc();    		      			/* UART处理 */
    DATA_Proc();    	        			/* 数据处理 */
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/* USER CODE BEGIN 4 */
uint8_t ucKey;                    	/* 按键值 */
void KEY_Proc(void)               	/* 按键处理 */
{
  uint8_t ucKey_Val = 0;

  ucKey_Val = KEY_Read();         	/* 按键读取 */
  if (ucKey_Val != ucKey)         	/* 键值变化 */
  {
    ucKey = ucKey_Val;            	/* 保存键值 */
    ucSec = 0;
  }	else
    ucKey_Val = 0;                 	/* 清除键值 */

  switch (ucKey_Val)
  {
    case 1:                        	/* B1键按下 */
      switch (ucDsta)
      {
        case 0:                   	/* 空闲状态 */
          if (ucCnum != 0)        	/* 有坐标数据 */
            ucDsta = 1;           	/* 运行准备 */
          break;
        case 2:                   	/* 运行处理 */
          ucDsta = 3;             	/* 等待状态 */
          break;
        case 3:                   	/* 等待状态 */
          ucDsta = 2;             	/* 运行处理 */
      }
      break;
    case 2:                        	/* B2键按下 */
      ucState &= 0xf0;
      ucState += 0x10;
		  if (ucState == 0x30)         	/* 切换界面 */
        ucState = 0;
      LCD_Clear(Black);
      break;
    case 3:                        	/* B3键按下 */
      if ((ucState&0xf0) == 0x10)  	/* 参数界面 */
        ucState ^= 1;             	/* 切换参数 */
      break;
    case 4:                        	/* B4键按下 */
      if (ucState == 0x10)         	/* 参数界面 */
        if (++ucR == 21)           	/* 调整R参数 */
          ucR = 10;      
      if (ucState == 0x11)         	/* 参数界面 */
      {
   	    ucB += 10;           	      /* 调整B参数 */
        if (ucB == 110)
          ucB = 10;      
      }
  }
  if ((ucKey == 5) && (ucSec > 2))  	/* B3/4键按下2s以上 */
    if ((ucState == 0) && (ucDsta == 0))
        usTS = usTT = 0;
}

uint8_t ucTled;                    	/* LED闪烁延时 */
void LED_Proc(void)               	/* LED处理 */
{
  if (ucTled < 200)
    return;                        	/* 200ms未到返回 */
  ucTled = 0;

  if (ucDsta == 0)                 	/* 空闲状态 */
    ucLed &= ~1;                  	/* LD1熄灭 */
  else if (ucDsta == 2)           	/* 开始运行 */
    ucLed |= 1;                    	/* LD1点亮 */
  else                            	/* 等待状态 */
    ucLed ^= 1;                   	/* LD1闪烁 */

  LED_Disp(ucLed);                 	/* LED显示 */
}

uint8_t  ucLcd[21];                	/* LCD值(\0结束) */
uint16_t usTlcd;                   	/* LCD刷新计时 */
void LCD_Proc(void)               	/* LCD处理 */
{
  if (usTlcd < 200)   	      			/* 200ms未到 */
    return;
  usTlcd = 0;

  switch (ucState&0xf0)
  {
    case 0:                       	/* 行程界面 */
      sprintf((char*)ucLcd, "       DATA");
      LCD_DisplayStringLine(Line1, ucLcd);

  		if (ucDsta == 0)
        sprintf((char *)ucLcd, "     ST=Idle");
      else if (ucDsta == 2)
        sprintf((char *)ucLcd, "     ST=Busy");
      else
        sprintf((char *)ucLcd, "     ST=Wait");
      LCD_DisplayStringLine(Line3, ucLcd);

      sprintf((char *)ucLcd, "     CP=%u,%u    ", usCpx, usCpy);
      LCD_DisplayStringLine(Line4, ucLcd);

      if (ucDsta == 0)
        sprintf((char *)ucLcd, "     TP=NF     ");
      else
				sprintf((char *)ucLcd, "     TP=%u,%u    ", usCoor[ucCno+1][0], usCoor[ucCno+1][1]);
      LCD_DisplayStringLine(Line5, ucLcd);

      if (ucDsta == 2)
        sprintf((char *)ucLcd, "     SE=%3.1f    ", usSpeed/10.0);
      else
        sprintf((char *)ucLcd, "     SE=0.0    ");
      LCD_DisplayStringLine(Line6, ucLcd);

      if (ucDsta == 0)
        sprintf((char *)ucLcd, "     RN=NF");
      else
        sprintf((char *)ucLcd, "     RN=%u ", ucCnum);
      LCD_DisplayStringLine(Line7, ucLcd);
      break;
    case 0x10:
      sprintf((char*)ucLcd, "       PARA");
      LCD_DisplayStringLine(Line1, ucLcd);
      sprintf((char *)ucLcd, "     R=%2.1f", ucR/10.0);
      if (ucState == 0x10)
        ucLcd[4] = '.';
      LCD_DisplayStringLine(Line3, ucLcd);
      sprintf((char *)ucLcd, "     B=%u ", ucB);
      if (ucState == 0x11)
        ucLcd[4] = '.';
      LCD_DisplayStringLine(Line4, ucLcd);
      break;
    case 0x20:
      sprintf((char*)ucLcd, "       RECD");
      LCD_DisplayStringLine(Line1, ucLcd);
      sprintf((char *)ucLcd, "     TS=%2.1f    ", usTS/10.0);
      LCD_DisplayStringLine(Line3, ucLcd);
      sprintf((char *)ucLcd, "     TT=%u      ", usTT/10);
      LCD_DisplayStringLine(Line4, ucLcd);
  }
}

uint8_t  ucUsec;                   	/* UART发送延时 */
uint8_t  ucUrx[20];               	/* UART接收值 */
uint8_t  ucUrn;                    	/* UART接收字节数（最高位为接收完成标志） */
void UART_Proc(void)              	/* UART处理 */
{
  uint8_t  i, j;
  uint8_t  m, n;
  uint16_t x, y;

  if (ucUsec != ucSec)         			/* 测试用 */
  {
    ucUsec = ucSec;
    printf("%03u\r\n", ucSec);
  }

  if (ucUrn < 0x80)               	/* 接收未完成 */
    return;
  ucUrn &= 0x7f;
	
  switch (ucUrx[0])
  {
    case '(':                     	/* 设置坐标 */
      if (ucDsta == 0)            	/* 空闲状态 */
      {
        m = n = 0;
	  		for (i=1; i<ucUrn; i++)   	/* 解析数据 */
        {
          if ((ucUrx[i] == ',') || (ucUrx[i] == ')'))
          {
            ucUrx[i] = 0;
            switch (i-m-1) 	        /* 数据位数 */
            {
              case 3:
                ucUrx[i] += (ucUrx[i-3]-'0')*100;
              case 2:
                ucUrx[i] += (ucUrx[i-2]-'0')*10;
              case 1:
                ucUrx[i] += ucUrx[i-1]-'0';
            }
            if ((n & 1) == 0)
              usCoor[(n>>1)+1][0] = ucUrx[i];  /* x坐标 */
            else
              usCoor[(n>>1)+1][1] = ucUrx[i];  /* y坐标 */
            m = i;
            n++;                    /* 坐标个数 */
          }
        }
        if ((n & 1) == 0)          	/* 偶数个 */
        {
          ucCnum = n>>1;
          printf("Got it");
        } else
          printf("Error");
      } else
        printf("Busy");
      break;
    case '{':                     	/* 删除坐标 */
      if (ucCnum > 1)             	/* 坐标数量大于1 */
      {
        m = n = 0;
	  		for (i=1; i<ucUrn; i++)   	/* 解析数据 */
        {
          if ((ucUrx[i] == ',') || (ucUrx[i] == ')'))
          {
            ucUrx[i] = 0;
            switch (i-m-1) 	        /* 数据位数 */
            {
              case 3:
                ucUrx[i] += (ucUrx[i-3]-'0')*100;
              case 2:
                ucUrx[i] += (ucUrx[i-2]-'0')*10;
              case 1:
                ucUrx[i] += ucUrx[i-1]-'0';
            }
            if ((n & 1) == 0)
              x = ucUrx[i];         /* x坐标 */
            else
              y = ucUrx[i];         /* y坐标 */
            m = i;
            n++;                    /* 坐标个数 */
          }
        }
	  		for (i=ucCno+2; i<ucCno+ucCnum; i++)   	/* 查找数据 */
        {
          if ((usCoor[i][0] == x) && (usCoor[i][1] == y))
          {
	  		    for (j=i; j<ucCno+ucCnum; j++)   	/* 删除数据 */
            {
              usCoor[j][0] = usCoor[j+1][0];   /* 数据前移 */
              usCoor[j][1] = usCoor[j+1][1];   /* 数据前移 */
            }
            ucCnum--;
            printf("Got it");
            break;
          }
        }
        if (i == ucCno+ucCnum)
          printf("Nonexistent");
      } else
        printf("Nonexistent");
      break;
    case '[':                     	/* 设置场景 */
			if (ucDsta == 2)            	/* 运行处理 */
      {
        if ((ucUrx[1]>0x30) && (ucUrx[1]<0x35) && (ucUrx[2]==']'))
        {                         	/* 数据有效 */
          ucScen = ucUrx[1];        /* 保存场景值 */
          switch (ucScen)
          {
            case '1':             	/* 场景1 */
              TIM4_SetAutoReload(999);
              TIM1_SetAutoReload(999);
              TIM4_SetCompare1(ucDuty[ucDno]*10);
              TIM1_SetCompare1((ucDuty[ucDno]-5)*10);
              ucLed |= 0x10;       	/* LD5点亮 */
              ucLed &= 0x1f;       	/* LD6~8熄灭 */
						break;
            case '2':             	/* 场景2 */
              TIM4_SetAutoReload(249);
              TIM1_SetAutoReload(999);
              TIM4_SetCompare1(ucDuty[ucDno]*2.5);
              TIM1_SetCompare1((ucDuty[ucDno]-5)*10);
              ucLed |= 0x20;       	/* LD6点亮 */
              ucLed &= 0x2f;       	/* LD5/7/8熄灭 */
						  break;
            case '3':             	/* 场景3 */
              TIM4_SetAutoReload(999);
              TIM1_SetAutoReload(249);
              TIM4_SetCompare1(ucDuty[ucDno]*10);
              TIM1_SetCompare1((ucDuty[ucDno]-5)*2.5);
              ucLed |= 0x40;       	/* LD7点亮 */
              ucLed &= 0x4f;       	/* LD5/6/8熄灭 */
						  break;
            case '4':             	/* 场景4 */
              TIM4_SetAutoReload(249);
              TIM1_SetAutoReload(249);
              TIM4_SetCompare1(ucDuty[ucDno]*2.5);
              TIM1_SetCompare1((ucDuty[ucDno]-5)*2.5);
              ucLed |= 0x80;       	/* LD8点亮 */
              ucLed &= 0x8f;       	/* LD5~7熄灭 */
          }
          printf("Got it");
        } else
          printf("Error");
      } else {
        TIM1_SetCompare1(0);      	/* 输出低电平 */
        TIM4_SetCompare1(0);
        printf("Device offilne");
      }
      break;
    case '?':                     	/* 查询状态 */
      switch (ucDsta)
      {
        case 0:
          printf("Idle");
          break;
        case 2:
          printf("Busy");
          break;
        case 3:
          printf("Wait");
      }
      break;
    case '#':                     	/* 查询位置 */
      printf("(%u,%u)", usCpx, usCpy);
      break;
    default:
      printf("Error");
  }
  ucUrn = 0;
}

int fputc(int ch, FILE *f)		    	/* printf()实现 */
{
  UART_Transmit((uint8_t *)&ch, 1);
  return ch;
}

uint8_t  ucTdat;			           		/* 数据采集计时 */
uint8_t  ucVol;			             		/* 电压值*10 */
uint16_t usCapt[2];			        		/* TIM输入捕捉值 */
int16_t  ssDistx;			          		/* 距离x */
int16_t  ssDisty;			          		/* 距离y */
uint16_t usDist;			          		/* 2点间距离*10 */
uint16_t usDcur;			          		/* 当前距离 */
void DATA_Proc(void)               	/* 数据处理 */
{
  if (ucDsta == 1)                	/* 运行准备 */
  {
    ssDistx = usCoor[ucCno+1][0]-usCoor[ucCno][0];
		ssDisty = usCoor[ucCno+1][1]-usCoor[ucCno][1];
		usDist = sqrt(pow(ssDistx, 2)+pow(ssDisty, 2)) * 10;
    usDcur = 0;
    ucDsta = 2;               	    /* 运行处理 */
  }

  if (ucTdat < 100)   	      			/* 100ms未到 */
    return;
  ucTdat = 0;

  ucVol = ADC2_Read()*33/4095;
  if (ucVol < 5)
    ucDno = 0;
  else if (ucVol < 10)
    ucDno = 1;
  else if (ucVol < 15)
    ucDno = 2;
  else if (ucVol < 20)
    ucDno = 3;
  else if (ucVol < 25)
    ucDno = 4;
  else
    ucDno = 5;
	
  if ((ucScen == 1) || (ucScen == 3))
    TIM4_SetCompare1(ucDuty[ucDno]*10);
  else
    TIM4_SetCompare1(ucDuty[ucDno]*2.5);

  if ((ucScen == 1) || (ucScen == 2))
    TIM1_SetCompare1((ucDuty[ucDno]-5)*10);
  else
    TIM1_SetCompare1((ucDuty[ucDno]-5)*2.5);

  TIM2_GetCapture(usCapt);
  usSpeed = 31400*ucR/usCapt[0]+ucB*10;  /* 速度*10 */

  if (ucDsta == 2)                	/* 运行处理 */
  {
    usTT++;                         /* 时长+100ms */
    usTS += usSpeed / 10;           /* 100ms运行距离: (usSpeed/10)*100ms/1000*10 */
    usDcur += usSpeed / 10; 
    if (usDcur < usDist)
		{                             	/* 按比例计算当前位置 */
			usCpx = usCoor[ucCno][0] + usDcur*ssDistx/usDist;
			usCpy = usCoor[ucCno][1] + usDcur*ssDisty/usDist;
    } else {
      ucCno++;
      if (--ucCnum != 0)
      {
        usDcur = 0; 
        usCpx = usCoor[ucCno][0];
        usCpy = usCoor[ucCno][1];
			  ucDsta = 1;    	            /* 运行准备 */
      } else {
        usCoor[0][0] = usCoor[ucCno][0];
        usCoor[0][1] = usCoor[ucCno][1];
        usCpx = usCoor[0][0];
        usCpy = usCoor[0][1];
        ucCno = 0;
        ucDsta = 0;    	            /* 空闲状态 */
      }
    }
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
