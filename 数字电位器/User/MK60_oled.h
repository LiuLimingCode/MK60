/*!
 * @file       oled.h
 * @brief      OLED函数实现(SPI)
 * @author     llm
 * @注意：区别于普通的OLED底层基于软件模拟SPI，这个底层基于K60硬件SPI编写。
          优点是通讯速度远远大于前者(大概10倍)，CPU占用率远远小于前者。
    但是！务必确保电路设计时，OLED连接的SCK、SDA(D0,D1)选为PORT_cfg.h中的SPI固定引脚。(非常重要！！！)
          电路设计时在选择SPIX_SCK SPIX_SOUT 后最好不要再使用对应的SPIX_SIN引脚，因为该引脚会在SPI底层中配置为输入模式
          该底层涉及大量寄存器操作，故基本不允许跨芯片移植程序，如K60程序移植到KEA128芯片后该底层无法使用。(普通OLED底层移植性好)
 */

#ifndef __MK60_OLED_H
#define __MK60_OLED_H

#include "headfile.h"

/***************** 引脚选择 *****************/

#define OLED_SPI       spi0                     //具体的SCK、SDA(D0、D1)在PORT_cfg.h中更改SPIX_SCK、SPIX_SOUT

#define OLED_DC_Pin    A13

#define OLED_RES_Pin   A12

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
