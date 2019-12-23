/*!
 * @file       oled.h
 * @brief      OLED函数实现
 * @author     llm
 */

#ifndef __MK60_OLED_H
#define __MK60_OLED_H

#include "headfile.h"

/***************** 引脚选择 *****************/
#define OLED_SDA_Pin   A16

#define OLED_SCL_Pin   A15                                    //D0

#define OLED_DC_Pin    A13

#define OLED_RES_Pin   A12

#define OLED_SCLH 	gpio_set(OLED_SCL_Pin, 1)
#define OLED_SCLL 	gpio_set(OLED_SCL_Pin, 0)

#define OLED_SDAH 	gpio_set(OLED_SDA_Pin, 1)
#define OLED_SDAL 	gpio_set(OLED_SDA_Pin, 0)

#define OLED_RSTH 	gpio_set(OLED_RES_Pin, 1)
#define OLED_RSTL 	gpio_set(OLED_RES_Pin, 0)

#define OLED_DCH  	gpio_set(OLED_DC_Pin, 1)
#define OLED_DCL  	gpio_set(OLED_DC_Pin, 0)

#define OLED_X_MAX 				128
#define OLED_Y_MAX 				64
#define OLED_PAGE_MAX  		(OLED_Y_MAX/8)
#define WHITE 						(uint8_t)0XFF
#define BLACK							(uint8_t)0X00

void OLED_Init(void);
void OLED_Cmd(char NewState);
void OLED_ClearScreen(uint8_t color);
void OLED_P6x8Char(uint8_t x, uint8_t y, char ch);
void OLED_P6x8Str(uint8_t x, uint8_t y, const char *ch);
void OLED_P6x8Int(uint8_t x, uint8_t y, int16_t data1, int8 set);
void OLED_P6x8Flo(uint8_t x, uint8_t y, float data1, int8 set);
void OLED_P8x16Char(uint8_t x, uint8_t y, char ch);
void OLED_P8x16Str(uint8_t x, uint8_t y, const char *ch);
void OLED_P14x16CHCHAR(uint8_t x, uint8_t y, const char *ch);
void OLED_PrintBMP(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1, const char bmp[]);
void OLED_PrintBinary(uint16 size_x, uint16 size_y, uint8 * binary);

#endif
