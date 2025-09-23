#include "gpio.h"
#include "fonts.h"

uint16_t TextColor = 0x0000, BackColor = 0xFFFF;

void LCD_Delay(uint16_t n)
{
  uint16_t i,j;
  for (i=0; i<n; ++i)
    for (j=0; j<3000; ++j);
}
/* д�Ĵ������ο�ͼ3.11�� */
void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue)
{
  LCD_Write(0, LCD_Reg);		          	/* д���� */
  LCD_Write(1, LCD_RegValue);       		/* д���� */
}
/* ���ù�꣨�ο���3.4�� */
void LCD_SetCursor(uint8_t Xpos, uint16_t Ypos)
{ 
  LCD_WriteReg(0x20, Xpos);	        		/* ˮƽ��ַ */
  LCD_WriteReg(0x21, Ypos);       			/* ��ֱ��ַ */
}
/* ׼��дRAM���ο���3.4�� */
void LCD_WriteRAM_Prepare(void)
{
  LCD_Write(0, 0x22);	            			/* д���� */
}
/* дRAM���ο���3.4�� */
void LCD_WriteRAM(uint16_t RGB_Code)
{
  LCD_Write(1, RGB_Code);         			/* д���� */
}
/* �����ַ� */
void LCD_DrawChar(uint8_t Xpos, uint16_t Ypos, uint16_t *ch)
{
  uint8_t index = 0, i = 0;

  for (index = 0; index < 24; index++)	/* 24�� */
  {
    LCD_SetCursor(Xpos, Ypos);
    LCD_WriteRAM_Prepare();
    for (i = 0; i < 16; i++)	    			/* 16�� */
    {
      if ((ch[index] & (1 << i)) == 0x00)
        LCD_WriteRAM(BackColor);	  		/* 0-����ɫ */
      else
        LCD_WriteRAM(TextColor);		  	/* 1-�ַ�ɫ */
    }
/* ��һ�� */
//  Ypos--;                              	/* ˮƽģʽ0 */
//  Ypos++;                              	/* ˮƽģʽ3 */
    Xpos++;                              	/* ��ֱģʽ1 */
//  Xpos--;                              	/* ��ֱģʽ2 */
  }
}
/* ��ʼ�� */
void LCD_Init(void)
{ 
  LCD_WriteReg(0x00,0x0001);		/* 8230�ϵ� */
//LCD_WriteReg(0x03,0x1000);		/* BGR ˮƽģʽ0*/
//LCD_WriteReg(0x03,0x1030);		/* BGR ˮƽģʽ3*/
  LCD_WriteReg(0x03,0x1018);		/* BGR ��ֱģʽ1*/
//LCD_WriteReg(0x03,0x1028);		/* BGR ��ֱģʽ2*/
  LCD_WriteReg(0x07,0x0173);		/* ͼ����ʾ */
  LCD_WriteReg(0x10,0x1090);		/* ��Դ����1 */
  LCD_WriteReg(0x11,0x0227);		/* ��Դ����2 */
  LCD_WriteReg(0x12,0x001d);		/* ��Դ����3 */
  LCD_WriteReg(0x60,0x2700);		/* ������� */
  LCD_WriteReg(0x61,0x0001);		/* ��ʾ���� */
  LCD_Delay(250);
}
/* ���� */
void LCD_Clear(uint16_t Color)
{
  uint32_t index = 0;
  LCD_SetCursor(0x00, 0x0000);
  LCD_WriteRAM_Prepare();
  for (index = 0; index < 76800; index++)
    LCD_WriteRAM(Color);
}
/* �����ַ�ɫ */
void LCD_SetTextColor(uint16_t Color)
{
  TextColor = Color;
}
/* ���ñ���ɫ */
void LCD_SetBackColor(uint16_t Color)
{
  BackColor = Color;
}
/* ��ʾ�ַ� */
void LCD_DisplayChar(uint8_t Line, uint16_t Column, uint8_t Ascii)
{
  Ascii -= 32;
  LCD_DrawChar(Line, Column, &ASCII_Table[Ascii * 24]);
}
/* ��ʾ�ַ��� */
void LCD_DisplayStringLine(uint8_t Line, uint8_t *ptr)
{
  uint8_t i=0, j;
  uint16_t k;
/* �ַ�������ʼλ�� */
//j = 15; k = 239;			         		/* ˮƽģʽ0 */
//j = 15; k = 0;	          				/* ˮƽģʽ3 */
  j = 20; k = 319;		        			/* ��ֱģʽ1 */
//j = 20; k = 0;		          			/* ��ֱģʽ2 */
  while ((*ptr != 0) && (i < j))
  {
//  LCD_DisplayChar(k, Line, *ptr);	/* ˮƽģʽ */
    LCD_DisplayChar(Line, k, *ptr);	/* ��ֱģʽ */
/* ��һ���ַ�λ�� */
    k -= 16;			          				/* ˮƽģʽ0, ��ֱģʽ1 */
//  k += 16;		            				/* ˮƽģʽ3, ��ֱģʽ2 */
    ptr++;				            			/* ��һ���ַ� */
    i++;
  }
}
