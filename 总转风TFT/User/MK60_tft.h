/*!
 * @file       tft.h
 * @brief      逐飞1.8寸TFT屏(基于ST7735S)函数实现(SPI)
 * @author     刘力铭
 * @注意：区别于普通的TFT底层基于软件模拟SPI，这个底层基于K60硬件SPI编写。
          优点是通讯速度远远大于前者，CPU占用率远远小于前者。
    但是！务必确保电路设计时，TFT连接的SCK、SDA(D0,D1)选为PORT_cfg.h中的SPI固定引脚。(非常重要！！！)
          电路设计时在选择SPIX_SCK SPIX_SOUT 后最好不要再使用对应的SPIX_SIN引脚，因为该引脚会在SPI底层中配置为输入模式
          该底层涉及大量寄存器操作，故基本不允许跨芯片移植程序，如K60程序移植到KEA128芯片后该底层无法使用。(普通TFT底层移植性好)
 */
#ifndef __MK60_TFT_H
#define __MK60_TFT_H

#include "headfile.h"

/***************** 引脚选择 *****************/

#define TFT_SPI       spi0                     //具体的SCK、SDA(D0、D1)在PORT_cfg.h中更改SPIX_SCK、SPIX_SOUT

#define TFT_DC_Pin    A13

#define TFT_RES_Pin   A12

#define TFT_RSTH 	gpio_set(TFT_RES_Pin, 1)
#define TFT_RSTL 	gpio_set(TFT_RES_Pin, 0)

#define TFT_DCH  	gpio_set(TFT_DC_Pin, 1)
#define TFT_DCL  	gpio_set(TFT_DC_Pin, 0)

#define	TFT_X_MAX	128	//液晶X方宽度
#define TFT_Y_MAX	160     //液晶Y方宽度
#define GBP454_RED     	0XF800    //红色
#define GBP454_GREEN   	0X07E0    //绿色
#define GBP454_BLUE    	0X001F    //蓝色
#define GBP454_BRED    	0XF81F
#define GBP454_GRED    	0XFFE0    //灰色
#define GBP454_GBLUE   	0X07FF    //
#define GBP454_BLACK   	0X0000    //黑色
#define GBP454_WHITE   	0XFFFF    //白色
#define GBP454_YELLOW  	0xFFE0    //黄色

void TFT_ClearScreen(uint16 color);
void TFT_Init(void);
void TFT_P8x16Char(uint16 x, uint16 y, uint8 dat, uint16 PENCOLOR, uint16 BGCOLOR);
void TFT_P8x16Str(uint16 x, uint16 y, char *dat, uint16 PENCOLOR, uint16 BGCOLOR);
void TFT_P8x16Int(uint16 x, uint16 y, int16_t data1, int8 set, uint16 PENCOLOR, uint16 BGCOLOR);
void TFT_P6x8Flo(uint8_t x, uint8_t y, float data1, int8 set, uint16 PENCOLOR, uint16 BGCOLOR);
void TFT_PrintBMP(uint8_t x, uint8_t y, uint8 size_x, uint8 size_y, const unsigned char *bmp);
void TFT_PrintGrayImage(uint8_t x, uint8_t y, uint8 size_x, uint8 size_y, const unsigned char *Image, int8 set);

#endif
