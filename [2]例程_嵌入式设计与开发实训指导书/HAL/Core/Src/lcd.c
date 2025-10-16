#include "gpio.h"
#include "fonts.h"

uint16_t TextColor = 0x0000, BackColor = 0xFFFF;

void LCD_Delay(uint16_t n)
{
  uint16_t i,j;
  for (i=0; i<n; ++i)
    for (j=0; j<3000; ++j);
}
/* 写寄存器（参考图3.11） */
void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue)
{
  LCD_Write(0, LCD_Reg);		          	/* 写索引 */
  LCD_Write(1, LCD_RegValue);       		/* 写数据 */
}
/* 设置光标（参考表3.4） */
void LCD_SetCursor(uint8_t Xpos, uint16_t Ypos)
{ 
  LCD_WriteReg(0x20, Xpos);	        		/* 水平地址 */
  LCD_WriteReg(0x21, Ypos);       			/* 垂直地址 */
}
/* 准备写RAM（参考表3.4） */
void LCD_WriteRAM_Prepare(void)
{
  LCD_Write(0, 0x22);	            			/* 写索引 */
}
/* 写RAM（参考表3.4） */
void LCD_WriteRAM(uint16_t RGB_Code)
{
  LCD_Write(1, RGB_Code);         			/* 写数据 */
}
/* 绘制字符 */
void LCD_DrawChar(uint8_t Xpos, uint16_t Ypos, uint16_t *ch)
{
  uint8_t index = 0, i = 0;

  for (index = 0; index < 24; index++)	/* 24行 */
  {
    LCD_SetCursor(Xpos, Ypos);
    LCD_WriteRAM_Prepare();
    for (i = 0; i < 16; i++)	    			/* 16列 */
    {
      if ((ch[index] & (1 << i)) == 0x00)
        LCD_WriteRAM(BackColor);	  		/* 0-背景色 */
      else
        LCD_WriteRAM(TextColor);		  	/* 1-字符色 */
    }
/* 下一行 */
//  Ypos--;                              	/* 水平模式0 */
//  Ypos++;                              	/* 水平模式3 */
    Xpos++;                              	/* 垂直模式1 */
//  Xpos--;                              	/* 垂直模式2 */
  }
}
/* 初始化 */
void LCD_Init(void)
{ 
  LCD_WriteReg(0x00,0x0001);		/* 8230上电 */
//LCD_WriteReg(0x03,0x1000);		/* BGR 水平模式0*/
//LCD_WriteReg(0x03,0x1030);		/* BGR 水平模式3*/
  LCD_WriteReg(0x03,0x1018);		/* BGR 垂直模式1*/
//LCD_WriteReg(0x03,0x1028);		/* BGR 垂直模式2*/
  LCD_WriteReg(0x07,0x0173);		/* 图形显示 */
  LCD_WriteReg(0x10,0x1090);		/* 电源控制1 */
  LCD_WriteReg(0x11,0x0227);		/* 电源控制2 */
  LCD_WriteReg(0x12,0x001d);		/* 电源控制3 */
  LCD_WriteReg(0x60,0x2700);		/* 输出控制 */
  LCD_WriteReg(0x61,0x0001);		/* 显示控制 */
  LCD_Delay(250);
}
/* 清屏 */
void LCD_Clear(uint16_t Color)
{
  uint32_t index = 0;
  LCD_SetCursor(0x00, 0x0000);
  LCD_WriteRAM_Prepare();
  for (index = 0; index < 76800; index++)
    LCD_WriteRAM(Color);
}
/* 设置字符色 */
void LCD_SetTextColor(uint16_t Color)
{
  TextColor = Color;
}
/* 设置背景色 */
void LCD_SetBackColor(uint16_t Color)
{
  BackColor = Color;
}
/* 显示字符 */
void LCD_DisplayChar(uint8_t Line, uint16_t Column, uint8_t Ascii)
{
  Ascii -= 32;
  LCD_DrawChar(Line, Column, &ASCII_Table[Ascii * 24]);
}
/* 显示字符串 */
void LCD_DisplayStringLine(uint8_t Line, uint8_t *ptr)
{
  uint8_t i=0, j;
  uint16_t k;
/* 字符数，起始位置 */
//j = 15; k = 239;			         		/* 水平模式0 */
//j = 15; k = 0;	          				/* 水平模式3 */
  j = 20; k = 319;		        			/* 垂直模式1 */
//j = 20; k = 0;		          			/* 垂直模式2 */
  while ((*ptr != 0) && (i < j))
  {
//  LCD_DisplayChar(k, Line, *ptr);	/* 水平模式 */
    LCD_DisplayChar(Line, k, *ptr);	/* 垂直模式 */
/* 下一个字符位置 */
    k -= 16;			          				/* 水平模式0, 垂直模式1 */
//  k += 16;		            				/* 水平模式3, 垂直模式2 */
    ptr++;				            			/* 下一个字符 */
    i++;
  }
}
