/*!
 * @文件       tft.h
 * @功能       逐飞1.8寸TFT屏(基于ST7735S)函数实现(SPI)
 * @作者       刘力铭
 * @完成时间   2019-12
 */
 
#ifndef __MK60_TFT_H
#define __MK60_TFT_H

#include "headfile.h"

/***************** 重要宏定义 *****************/
#define	TFT_X_MAX	128               //液晶X方宽度
#define TFT_Y_MAX	160               //液晶Y方宽度
#define TFT_GBP454_RED     	0XF800    //红色
#define TFT_GBP454_GREEN   	0X07E0    //绿色
#define TFT_GBP454_BLUE    	0X001F    //蓝色
#define TFT_GBP454_BRED    	0XF81F
#define TFT_GBP454_GRED    	0XFFE0    //灰色
#define TFT_GBP454_GBLUE   	0X07FF    //
#define TFT_GBP454_BLACK   	0X0000    //黑色
#define TFT_GBP454_WHITE   	0XFFFF    //白色
#define TFT_GBP454_YELLOW  	0xFFE0    //黄色

//是否选用硬件SPI：0为使用模拟SPI,1为使用硬件SPI,硬件软件SPI描述见MK_myspi.h
#define TFT_SPI_HARDWARE      1

//是否启用片选引脚：0为不启用,1为启用.增加片选引脚可以增加SPI通讯稳定,但需要把单片机引脚连接到OLED的CS引脚上.若不启用,OLED上的CS引脚必须接地
#define TFT_SPI_USE_CS        0

/***************** 引脚选择 *****************/
#define TFT_DC_Pin    A13
#define TFT_RES_Pin   A12

#define TFT_RSTH      gpio_set(TFT_RES_Pin, 1)
#define TFT_RSTL      gpio_set(TFT_RES_Pin, 0)

#define TFT_DCH       gpio_set(TFT_DC_Pin, 1)
#define TFT_DCL       gpio_set(TFT_DC_Pin, 0)

#if TFT_SPI_USE_CS == 1 //启用片选引脚,若不启用,下面的宏不需要填
#define TFT_CS_Pin    A14
#define TFT_CSH       gpio_set(TFT_CS_Pin, 1)
#define TFT_CSL       gpio_set(TFT_CS_Pin, 0)
#endif

#if TFT_SPI_HARDWARE == 0 //使用模拟SPI,在这个情况下可以不定义TFT_SPI
#define TFT_SDA_Pin   A16 //D1
#define TFT_SCL_Pin   A15 //D0
#else                     //使用硬件SPI,在这个情况下可以不定TFT_SDA_Pin和TFT_SCL_Pin
#define TFT_SPI       spi0//具体的SCK、SDA(D0、D1)引脚在PORT_cfg.h中更改SPIX_SCK、SPIX_SOUT
#endif

/****************** 可调用函数 ******************/
void TFT_Init(void);
void TFT_ClearScreen(uint16 color);
void TFT_P8x16Char(uint16 x, uint16 y, uint8 dat, uint16 PENCOLOR, uint16 BGCOLOR);
void TFT_P8x16Str(uint16 x, uint16 y, char *dat, uint16 PENCOLOR, uint16 BGCOLOR);
void TFT_P8x16Int(uint16 x, uint16 y, int16_t data1, int8 set, uint16 PENCOLOR, uint16 BGCOLOR);
void TFT_P6x8Flo(uint8 x, uint8 y, float data1, int8 set, uint16 PENCOLOR, uint16 BGCOLOR);
void TFT_PrintBMP(uint8 x, uint8 y, uint8 size_x, uint8 size_y, const unsigned char *bmp) ;

#endif
