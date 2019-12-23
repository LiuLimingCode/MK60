/*!
 * @file       tft.h
 * @brief      ���1.8��TFT��(����ST7735S)����ʵ��(SPI)
 * @author     ������
 * @ע�⣺��������ͨ��TFT�ײ�������ģ��SPI������ײ����K60Ӳ��SPI��д��
          �ŵ���ͨѶ�ٶ�ԶԶ����ǰ�ߣ�CPUռ����ԶԶС��ǰ�ߡ�
    ���ǣ����ȷ����·���ʱ��TFT���ӵ�SCK��SDA(D0,D1)ѡΪPORT_cfg.h�е�SPI�̶����š�(�ǳ���Ҫ������)
          ��·���ʱ��ѡ��SPIX_SCK SPIX_SOUT ����ò�Ҫ��ʹ�ö�Ӧ��SPIX_SIN���ţ���Ϊ�����Ż���SPI�ײ�������Ϊ����ģʽ
          �õײ��漰�����Ĵ����������ʻ����������оƬ��ֲ������K60������ֲ��KEA128оƬ��õײ��޷�ʹ�á�(��ͨTFT�ײ���ֲ�Ժ�)
 */
#ifndef __MK60_TFT_H
#define __MK60_TFT_H

#include "headfile.h"

/***************** ����ѡ�� *****************/

#define TFT_SPI       spi0                     //�����SCK��SDA(D0��D1)��PORT_cfg.h�и���SPIX_SCK��SPIX_SOUT

#define TFT_DC_Pin    A13

#define TFT_RES_Pin   A12

#define TFT_RSTH 	gpio_set(TFT_RES_Pin, 1)
#define TFT_RSTL 	gpio_set(TFT_RES_Pin, 0)

#define TFT_DCH  	gpio_set(TFT_DC_Pin, 1)
#define TFT_DCL  	gpio_set(TFT_DC_Pin, 0)

#define	TFT_X_MAX	128	//Һ��X�����
#define TFT_Y_MAX	160     //Һ��Y�����
#define GBP454_RED     	0XF800    //��ɫ
#define GBP454_GREEN   	0X07E0    //��ɫ
#define GBP454_BLUE    	0X001F    //��ɫ
#define GBP454_BRED    	0XF81F
#define GBP454_GRED    	0XFFE0    //��ɫ
#define GBP454_GBLUE   	0X07FF    //
#define GBP454_BLACK   	0X0000    //��ɫ
#define GBP454_WHITE   	0XFFFF    //��ɫ
#define GBP454_YELLOW  	0xFFE0    //��ɫ

void TFT_ClearScreen(uint16 color);
void TFT_Init(void);
void TFT_P8x16Char(uint16 x, uint16 y, uint8 dat, uint16 PENCOLOR, uint16 BGCOLOR);
void TFT_P8x16Str(uint16 x, uint16 y, char *dat, uint16 PENCOLOR, uint16 BGCOLOR);
void TFT_P8x16Int(uint16 x, uint16 y, int16_t data1, int8 set, uint16 PENCOLOR, uint16 BGCOLOR);
void TFT_P6x8Flo(uint8_t x, uint8_t y, float data1, int8 set, uint16 PENCOLOR, uint16 BGCOLOR);
void TFT_PrintBMP(uint8_t x, uint8_t y, uint8 size_x, uint8 size_y, const unsigned char *bmp);
void TFT_PrintGrayImage(uint8_t x, uint8_t y, uint8 size_x, uint8 size_y, const unsigned char *Image, int8 set);

#endif
