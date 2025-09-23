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
uint8_t  ucState;                  	/* ״̬ */
uint8_t  ucSec;                    	/* ���ʱ */
uint8_t  ucLed;                    	/* LEDֵ */
uint8_t  ucDsta;                   	/* �豸״̬�� 0-���У�1-����׼����2-���д���3-�ȴ� */
uint16_t usCoor[100][2];           	/* ������ֵ */
uint8_t  ucCnum;                   	/* �������� */
uint8_t  ucCno;                   	/* ������� */
uint16_t usCpx;                    	/* �豸λ��x */
uint16_t usCpy;                    	/* �豸λ��y */
uint8_t  ucScen;                   	/* ���� */
uint16_t usSpeed;		  	        		/* �н��ٶ�*10 */
uint8_t  ucR=10;                   	/* ����R*10 */
uint8_t  ucB=10;                   	/* ����B */
uint16_t usTS;		    	        		/* �ۼ��г�*10 */
uint16_t usTT;		    	        		/* �ۼ�ʱ��*10 */
uint8_t  ucDuty[6]={10,30,50,70,90,95}; /* ռ�ձ� */
uint8_t  ucDno;                   	/* ռ�ձ���� */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void KEY_Proc(void);              	/* �������� */
void LED_Proc(void);              	/* LED���� */
void LCD_Proc(void);              	/* LCD���� */
void UART_Proc(void);              	/* UART���� */
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
  MX_TIM4_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  LCD_Init();                      	/* LCD��ʼ�� */
  LCD_Clear(Black);                	/* LCD���� */
  LCD_SetTextColor(White);        	/* �����ַ�ɫ */
  LCD_SetBackColor(Black);        	/* ���ñ���ɫ */

  TIM4_SetAutoReload(999);         	/* ���ó���1 */
  TIM1_SetAutoReload(999);
  TIM4_SetCompare1(0);
  TIM1_SetCompare1(0);
  ucLed |= 0x10;                  	/* LD5���� */
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    KEY_Proc();                    	/* �������� */
    LED_Proc();                    	/* LED���� */
    LCD_Proc();                    	/* LCD���� */
    UART_Proc();    		      			/* UART���� */
    DATA_Proc();    	        			/* ���ݴ��� */
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/* USER CODE BEGIN 4 */
uint8_t ucKey;                    	/* ����ֵ */
void KEY_Proc(void)               	/* �������� */
{
  uint8_t ucKey_Val = 0;

  ucKey_Val = KEY_Read();         	/* ������ȡ */
  if (ucKey_Val != ucKey)         	/* ��ֵ�仯 */
  {
    ucKey = ucKey_Val;            	/* �����ֵ */
    ucSec = 0;
  }	else
    ucKey_Val = 0;                 	/* �����ֵ */

  switch (ucKey_Val)
  {
    case 1:                        	/* B1������ */
      switch (ucDsta)
      {
        case 0:                   	/* ����״̬ */
          if (ucCnum != 0)        	/* ���������� */
            ucDsta = 1;           	/* ����׼�� */
          break;
        case 2:                   	/* ���д��� */
          ucDsta = 3;             	/* �ȴ�״̬ */
          break;
        case 3:                   	/* �ȴ�״̬ */
          ucDsta = 2;             	/* ���д��� */
      }
      break;
    case 2:                        	/* B2������ */
      ucState &= 0xf0;
      ucState += 0x10;
		  if (ucState == 0x30)         	/* �л����� */
        ucState = 0;
      LCD_Clear(Black);
      break;
    case 3:                        	/* B3������ */
      if ((ucState&0xf0) == 0x10)  	/* �������� */
        ucState ^= 1;             	/* �л����� */
      break;
    case 4:                        	/* B4������ */
      if (ucState == 0x10)         	/* �������� */
        if (++ucR == 21)           	/* ����R���� */
          ucR = 10;      
      if (ucState == 0x11)         	/* �������� */
      {
   	    ucB += 10;           	      /* ����B���� */
        if (ucB == 110)
          ucB = 10;      
      }
  }
  if ((ucKey == 5) && (ucSec > 2))  	/* B3/4������2s���� */
    if ((ucState == 0) && (ucDsta == 0))
        usTS = usTT = 0;
}

uint8_t ucTled;                    	/* LED��˸��ʱ */
void LED_Proc(void)               	/* LED���� */
{
  if (ucTled < 200)
    return;                        	/* 200msδ������ */
  ucTled = 0;

  if (ucDsta == 0)                 	/* ����״̬ */
    ucLed &= ~1;                  	/* LD1Ϩ�� */
  else if (ucDsta == 2)           	/* ��ʼ���� */
    ucLed |= 1;                    	/* LD1���� */
  else                            	/* �ȴ�״̬ */
    ucLed ^= 1;                   	/* LD1��˸ */

  LED_Disp(ucLed);                 	/* LED��ʾ */
}

uint8_t  ucLcd[21];                	/* LCDֵ(\0����) */
uint16_t usTlcd;                   	/* LCDˢ�¼�ʱ */
void LCD_Proc(void)               	/* LCD���� */
{
  if (usTlcd < 200)   	      			/* 200msδ�� */
    return;
  usTlcd = 0;

  switch (ucState&0xf0)
  {
    case 0:                       	/* �г̽��� */
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

uint8_t  ucUsec;                   	/* UART������ʱ */
uint8_t  ucUrx[20];               	/* UART����ֵ */
uint8_t  ucUrn;                    	/* UART�����ֽ��������λΪ������ɱ�־�� */
void UART_Proc(void)              	/* UART���� */
{
  uint8_t  i, j;
  uint8_t  m, n;
  uint16_t x, y;

  if (ucUsec != ucSec)         			/* ������ */
  {
    ucUsec = ucSec;
    printf("%03u\r\n", ucSec);
  }

  if (ucUrn < 0x80)               	/* ����δ��� */
    return;
  ucUrn &= 0x7f;
	
  switch (ucUrx[0])
  {
    case '(':                     	/* �������� */
      if (ucDsta == 0)            	/* ����״̬ */
      {
        m = n = 0;
	  		for (i=1; i<ucUrn; i++)   	/* �������� */
        {
          if ((ucUrx[i] == ',') || (ucUrx[i] == ')'))
          {
            ucUrx[i] = 0;
            switch (i-m-1) 	        /* ����λ�� */
            {
              case 3:
                ucUrx[i] += (ucUrx[i-3]-'0')*100;
              case 2:
                ucUrx[i] += (ucUrx[i-2]-'0')*10;
              case 1:
                ucUrx[i] += ucUrx[i-1]-'0';
            }
            if ((n & 1) == 0)
              usCoor[(n>>1)+1][0] = ucUrx[i];  /* x���� */
            else
              usCoor[(n>>1)+1][1] = ucUrx[i];  /* y���� */
            m = i;
            n++;                    /* ������� */
          }
        }
        if ((n & 1) == 0)          	/* ż���� */
        {
          ucCnum = n>>1;
          printf("Got it");
        } else
          printf("Error");
      } else
        printf("Busy");
      break;
    case '{':                     	/* ɾ������ */
      if (ucCnum > 1)             	/* ������������1 */
      {
        m = n = 0;
	  		for (i=1; i<ucUrn; i++)   	/* �������� */
        {
          if ((ucUrx[i] == ',') || (ucUrx[i] == ')'))
          {
            ucUrx[i] = 0;
            switch (i-m-1) 	        /* ����λ�� */
            {
              case 3:
                ucUrx[i] += (ucUrx[i-3]-'0')*100;
              case 2:
                ucUrx[i] += (ucUrx[i-2]-'0')*10;
              case 1:
                ucUrx[i] += ucUrx[i-1]-'0';
            }
            if ((n & 1) == 0)
              x = ucUrx[i];         /* x���� */
            else
              y = ucUrx[i];         /* y���� */
            m = i;
            n++;                    /* ������� */
          }
        }
	  		for (i=ucCno+2; i<ucCno+ucCnum; i++)   	/* �������� */
        {
          if ((usCoor[i][0] == x) && (usCoor[i][1] == y))
          {
	  		    for (j=i; j<ucCno+ucCnum; j++)   	/* ɾ������ */
            {
              usCoor[j][0] = usCoor[j+1][0];   /* ����ǰ�� */
              usCoor[j][1] = usCoor[j+1][1];   /* ����ǰ�� */
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
    case '[':                     	/* ���ó��� */
			if (ucDsta == 2)            	/* ���д��� */
      {
        if ((ucUrx[1]>0x30) && (ucUrx[1]<0x35) && (ucUrx[2]==']'))
        {                         	/* ������Ч */
          ucScen = ucUrx[1];        /* ���泡��ֵ */
          switch (ucScen)
          {
            case '1':             	/* ����1 */
              TIM4_SetAutoReload(999);
              TIM1_SetAutoReload(999);
              TIM4_SetCompare1(ucDuty[ucDno]*10);
              TIM1_SetCompare1((ucDuty[ucDno]-5)*10);
              ucLed |= 0x10;       	/* LD5���� */
              ucLed &= 0x1f;       	/* LD6~8Ϩ�� */
						break;
            case '2':             	/* ����2 */
              TIM4_SetAutoReload(249);
              TIM1_SetAutoReload(999);
              TIM4_SetCompare1(ucDuty[ucDno]*2.5);
              TIM1_SetCompare1((ucDuty[ucDno]-5)*10);
              ucLed |= 0x20;       	/* LD6���� */
              ucLed &= 0x2f;       	/* LD5/7/8Ϩ�� */
						  break;
            case '3':             	/* ����3 */
              TIM4_SetAutoReload(999);
              TIM1_SetAutoReload(249);
              TIM4_SetCompare1(ucDuty[ucDno]*10);
              TIM1_SetCompare1((ucDuty[ucDno]-5)*2.5);
              ucLed |= 0x40;       	/* LD7���� */
              ucLed &= 0x4f;       	/* LD5/6/8Ϩ�� */
						  break;
            case '4':             	/* ����4 */
              TIM4_SetAutoReload(249);
              TIM1_SetAutoReload(249);
              TIM4_SetCompare1(ucDuty[ucDno]*2.5);
              TIM1_SetCompare1((ucDuty[ucDno]-5)*2.5);
              ucLed |= 0x80;       	/* LD8���� */
              ucLed &= 0x8f;       	/* LD5~7Ϩ�� */
          }
          printf("Got it");
        } else
          printf("Error");
      } else {
        TIM1_SetCompare1(0);      	/* ����͵�ƽ */
        TIM4_SetCompare1(0);
        printf("Device offilne");
      }
      break;
    case '?':                     	/* ��ѯ״̬ */
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
    case '#':                     	/* ��ѯλ�� */
      printf("(%u,%u)", usCpx, usCpy);
      break;
    default:
      printf("Error");
  }
  ucUrn = 0;
}

int fputc(int ch, FILE *f)		    	/* printf()ʵ�� */
{
  UART_Transmit((uint8_t *)&ch, 1);
  return ch;
}

uint8_t  ucTdat;			           		/* ���ݲɼ���ʱ */
uint8_t  ucVol;			             		/* ��ѹֵ*10 */
uint16_t usCapt[2];			        		/* TIM���벶׽ֵ */
int16_t  ssDistx;			          		/* ����x */
int16_t  ssDisty;			          		/* ����y */
uint16_t usDist;			          		/* 2������*10 */
uint16_t usDcur;			          		/* ��ǰ���� */
void DATA_Proc(void)               	/* ���ݴ��� */
{
  if (ucDsta == 1)                	/* ����׼�� */
  {
    ssDistx = usCoor[ucCno+1][0]-usCoor[ucCno][0];
		ssDisty = usCoor[ucCno+1][1]-usCoor[ucCno][1];
		usDist = sqrt(pow(ssDistx, 2)+pow(ssDisty, 2)) * 10;
    usDcur = 0;
    ucDsta = 2;               	    /* ���д��� */
  }

  if (ucTdat < 100)   	      			/* 100msδ�� */
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
  usSpeed = 31400*ucR/usCapt[0]+ucB*10;  /* �ٶ�*10 */

  if (ucDsta == 2)                	/* ���д��� */
  {
    usTT++;                         /* ʱ��+100ms */
    usTS += usSpeed / 10;           /* 100ms���о���: (usSpeed/10)*100ms/1000*10 */
    usDcur += usSpeed / 10; 
    if (usDcur < usDist)
		{                             	/* ���������㵱ǰλ�� */
			usCpx = usCoor[ucCno][0] + usDcur*ssDistx/usDist;
			usCpy = usCoor[ucCno][1] + usDcur*ssDisty/usDist;
    } else {
      ucCno++;
      if (--ucCnum != 0)
      {
        usDcur = 0; 
        usCpx = usCoor[ucCno][0];
        usCpy = usCoor[ucCno][1];
			  ucDsta = 1;    	            /* ����׼�� */
      } else {
        usCoor[0][0] = usCoor[ucCno][0];
        usCoor[0][1] = usCoor[ucCno][1];
        usCpx = usCoor[0][0];
        usCpy = usCoor[0][1];
        ucCno = 0;
        ucDsta = 0;    	            /* ����״̬ */
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
