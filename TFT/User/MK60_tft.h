/*!
 * @�ļ�       tft.h
 * @����       ���1.8��TFT��(����ST7735S)����ʵ��(SPI)
 * @����       ������
 * @���ʱ��   2019-12
 */
 
#ifndef __MK60_TFT_H
#define __MK60_TFT_H

#include "headfile.h"

/***************** ��Ҫ�궨�� *****************/
#define	TFT_X_MAX	128               //Һ��X�����
#define TFT_Y_MAX	160               //Һ��Y�����
#define TFT_GBP454_RED     	0XF800    //��ɫ
#define TFT_GBP454_GREEN   	0X07E0    //��ɫ
#define TFT_GBP454_BLUE    	0X001F    //��ɫ
#define TFT_GBP454_BRED    	0XF81F
#define TFT_GBP454_GRED    	0XFFE0    //��ɫ
#define TFT_GBP454_GBLUE   	0X07FF    //
#define TFT_GBP454_BLACK   	0X0000    //��ɫ
#define TFT_GBP454_WHITE   	0XFFFF    //��ɫ
#define TFT_GBP454_YELLOW  	0xFFE0    //��ɫ

//�Ƿ�ѡ��Ӳ��SPI��0Ϊʹ��ģ��SPI,1Ϊʹ��Ӳ��SPI,Ӳ�����SPI������MK_myspi.h
#define TFT_SPI_HARDWARE      1

//�Ƿ�����Ƭѡ���ţ�0Ϊ������,1Ϊ����.����Ƭѡ���ſ�������SPIͨѶ�ȶ�,����Ҫ�ѵ�Ƭ���������ӵ�OLED��CS������.��������,OLED�ϵ�CS���ű���ӵ�
#define TFT_SPI_USE_CS        0

/***************** ����ѡ�� *****************/
#define TFT_DC_Pin    A13
#define TFT_RES_Pin   A12

#define TFT_RSTH      gpio_set(TFT_RES_Pin, 1)
#define TFT_RSTL      gpio_set(TFT_RES_Pin, 0)

#define TFT_DCH       gpio_set(TFT_DC_Pin, 1)
#define TFT_DCL       gpio_set(TFT_DC_Pin, 0)

#if TFT_SPI_USE_CS == 1 //����Ƭѡ����,��������,����ĺ겻��Ҫ��
#define TFT_CS_Pin    A14
#define TFT_CSH       gpio_set(TFT_CS_Pin, 1)
#define TFT_CSL       gpio_set(TFT_CS_Pin, 0)
#endif

#if TFT_SPI_HARDWARE == 0 //ʹ��ģ��SPI,���������¿��Բ�����TFT_SPI
#define TFT_SDA_Pin   A16 //D1
#define TFT_SCL_Pin   A15 //D0
#else                     //ʹ��Ӳ��SPI,���������¿��Բ���TFT_SDA_Pin��TFT_SCL_Pin
#define TFT_SPI       spi0//�����SCK��SDA(D0��D1)������PORT_cfg.h�и���SPIX_SCK��SPIX_SOUT
#endif

/****************** �ɵ��ú��� ******************/
void TFT_Init(void);
void TFT_ClearScreen(uint16 color);
void TFT_P8x16Char(uint16 x, uint16 y, uint8 dat, uint16 PENCOLOR, uint16 BGCOLOR);
void TFT_P8x16Str(uint16 x, uint16 y, char *dat, uint16 PENCOLOR, uint16 BGCOLOR);
void TFT_P8x16Int(uint16 x, uint16 y, int16_t data1, int8 set, uint16 PENCOLOR, uint16 BGCOLOR);
void TFT_P6x8Flo(uint8 x, uint8 y, float data1, int8 set, uint16 PENCOLOR, uint16 BGCOLOR);
void TFT_PrintBMP(uint8 x, uint8 y, uint8 size_x, uint8 size_y, const unsigned char *bmp) ;

#endif
