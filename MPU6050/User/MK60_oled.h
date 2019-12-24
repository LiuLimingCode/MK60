/*!
 * @�ļ�       oled.h
 * @����       OLED����ʵ��
 * @����       ������
 * @���ʱ��   2019-12
 */

#ifndef __MK60_OLED_H
#define __MK60_OLED_H

#include "headfile.h"

/***************** ��Ҫ�궨�� *****************/
#define OLED_X_MAX             128            //OLED�������ش�С
#define OLED_Y_MAX             64             //OLED�������ش�С
#define OLED_PAGE_MAX          (OLED_Y_MAX/8) //OLED��8������һ�����
#define OLED_WHITE             (uint8_t)0XFF  //��ɫ
#define OLED_BLACK             (uint8_t)0X00  //��ɫ

//�Ƿ�ѡ��Ӳ��SPI��0Ϊʹ��ģ��SPI,1Ϊʹ��Ӳ��SPI,Ӳ�����SPI������MK_myspi.h
#define OLED_SPI_HARDWARE      1

//�Ƿ�����Ƭѡ���ţ�0Ϊ������,1Ϊ����.����Ƭѡ���ſ�������SPIͨѶ�ȶ�,����Ҫ�ѵ�Ƭ���������ӵ�OLED��CS������.��������,OLED�ϵ�CS���ű���ӵ�
#define OLED_SPI_USE_CS	       0

/****************** ����ѡ�� ******************/
#define OLED_DC_Pin    A13
#define OLED_RES_Pin   A12

#define OLED_RSTH      gpio_set(OLED_RES_Pin, 1)
#define OLED_RSTL      gpio_set(OLED_RES_Pin, 0)

#define OLED_DCH       gpio_set(OLED_DC_Pin, 1)
#define OLED_DCL       gpio_set(OLED_DC_Pin, 0)

#if OLED_SPI_USE_CS == 1 //����Ƭѡ����,��������,����ĺ겻��Ҫ��
#define OLED_CS_Pin    A14
#define OLED_CSH       gpio_set(OLED_CS_Pin, 1)
#define OLED_CSL       gpio_set(OLED_CS_Pin, 0)
#endif

#if OLED_SPI_HARDWARE == 0 //ʹ��ģ��SPI,���������¿��Բ�����OLED_SPI
#define OLED_SDA_Pin   A16 //D1
#define OLED_SCL_Pin   A15 //D0
#else                      //ʹ��Ӳ��SPI,���������¿��Բ���OLED_SDA_Pin��OLED_SCL_Pin
#define OLED_SPI       spi0//�����SCK��SDA(D0��D1)������PORT_cfg.h�и���SPIX_SCK��SPIX_SOUT
#endif

/****************** �ɵ��ú��� ******************/
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
