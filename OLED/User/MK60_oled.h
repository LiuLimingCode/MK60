/*!
 * @文件       oled.h
 * @功能       OLED函数实现
 * @作者       刘力铭
 * @完成时间   2019-12
 */

#ifndef __MK60_OLED_H
#define __MK60_OLED_H

#include "headfile.h"

/***************** 重要宏定义 *****************/
#define OLED_X_MAX             128            //OLED的列像素大小
#define OLED_Y_MAX             64             //OLED的行像素大小
#define OLED_PAGE_MAX          (OLED_Y_MAX/8) //OLED将8行像素一起控制
#define OLED_WHITE             (uint8_t)0XFF  //白色
#define OLED_BLACK             (uint8_t)0X00  //黑色

//是否选用硬件SPI：0为使用模拟SPI,1为使用硬件SPI,硬件软件SPI描述见MK_myspi.h
#define OLED_SPI_HARDWARE      1

//是否启用片选引脚：0为不启用,1为启用.增加片选引脚可以增加SPI通讯稳定,但需要把单片机引脚连接到OLED的CS引脚上.若不启用,OLED上的CS引脚必须接地
#define OLED_SPI_USE_CS	       0

/****************** 引脚选择 ******************/
#define OLED_DC_Pin    A13
#define OLED_RES_Pin   A12

#define OLED_RSTH      gpio_set(OLED_RES_Pin, 1)
#define OLED_RSTL      gpio_set(OLED_RES_Pin, 0)

#define OLED_DCH       gpio_set(OLED_DC_Pin, 1)
#define OLED_DCL       gpio_set(OLED_DC_Pin, 0)

#if OLED_SPI_USE_CS == 1 //启用片选引脚,若不启用,下面的宏不需要填
#define OLED_CS_Pin    A14
#define OLED_CSH       gpio_set(OLED_CS_Pin, 1)
#define OLED_CSL       gpio_set(OLED_CS_Pin, 0)
#endif

#if OLED_SPI_HARDWARE == 0 //使用模拟SPI,在这个情况下可以不定义OLED_SPI
#define OLED_SDA_Pin   A16 //D1
#define OLED_SCL_Pin   A15 //D0
#else                      //使用硬件SPI,在这个情况下可以不定OLED_SDA_Pin和OLED_SCL_Pin
#define OLED_SPI       spi0//具体的SCK、SDA(D0、D1)引脚在PORT_cfg.h中更改SPIX_SCK、SPIX_SOUT
#endif

/****************** 可调用函数 ******************/
void OLED_Init(void);
void OLED_ClearScreen(uint8_t color);
void OLED_P6x8Char(uint8_t x, uint8_t y, char ch);
void OLED_P6x8Str(uint8_t x, uint8_t y, const char *ch);
void OLED_P6x8Int(uint8_t x, uint8_t y, int16_t data1, int8 set);
void OLED_P6x8Flo(uint8_t x, uint8_t y, double data1, int8 set);
void OLED_P8x16Char(uint8_t x, uint8_t y, char ch);
void OLED_P8x16Str(uint8_t x, uint8_t y, const char *ch);
void OLED_P14x16CHCHAR(uint8_t x, uint8_t y, const char *ch);
void OLED_PrintBMP(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1, const char bmp[]);
void OLED_PrintBinary(uint16 size_x, uint16 size_y, uint8 * binary);

#endif
