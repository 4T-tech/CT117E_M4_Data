#ifndef __LCD_H
#define __LCD_H
#include "main.h"
/* LCD颜色 */
#define White          0xFFFF
#define Black          0x0000
#define Grey           0xF7DE
#define Blue           0x001F
#define Blue2          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0
/* 行定义 */
#define Line0          0				/* 239 */
#define Line1          24	  		/* 215 */
#define Line2          48		  	/* 191 */
#define Line3          72	  		/* 167 */
#define Line4          96		  	/* 143 */
#define Line5          120			/* 119 */
#define Line6          144			/* 95 */
#define Line7          168			/* 71 */
#define Line8          192			/* 47 */
#define Line9          216			/* 23 */
/* 列定义 */
#define Column0        319
#define Column1        303
#define Column2        287
#define Column3        271
#define Column4        255
#define Column5        239
#define Column6        223
#define Column7        207
#define Column8        191
#define Column9        175
#define Column10       159
#define Column11       143
#define Column12       127
#define Column13       111
#define Column14       95
#define Column15       79
#define Column16       63
#define Column17       47
#define Column18       31
#define Column19       15
/* 函数声明 */
void LCD_Init(void);
void LCD_Clear(uint16_t Color);
void LCD_SetTextColor(uint16_t Color);
void LCD_SetBackColor(uint16_t Color);
void LCD_DisplayChar(uint8_t Line, uint16_t Column, uint8_t Ascii);
void LCD_DisplayStringLine(uint8_t Line, uint8_t *ptr);
#endif /* __LCD_H */
