/*!
 * @file       oled.h
 * @brief      OLED����ʵ��(SPI)
 * @author     llm
 * @ע�⣺��������ͨ��OLED�ײ��������ģ��SPI������ײ����K60Ӳ��SPI��д��
          �ŵ���ͨѶ�ٶ�ԶԶ����ǰ��(���10��)��CPUռ����ԶԶС��ǰ�ߡ�
    ���ǣ����ȷ����·���ʱ��OLED���ӵ�SCK��SDA(D0,D1)ѡΪPORT_cfg.h�е�SPI�̶����š�(�ǳ���Ҫ������)
          ��·���ʱ��ѡ��SPIX_SCK SPIX_SOUT ����ò�Ҫ��ʹ�ö�Ӧ��SPIX_SIN���ţ���Ϊ�����Ż���SPI�ײ�������Ϊ����ģʽ
          �õײ��漰�����Ĵ����������ʻ�����������оƬ��ֲ������K60������ֲ��KEA128оƬ��õײ��޷�ʹ�á�(��ͨOLED�ײ���ֲ�Ժ�)
 */

#ifndef __MK60_OLED_H
#define __MK60_OLED_H

#include "headfile.h"

/***************** ����ѡ�� *****************/

#define OLED_SPI       spi0                     //�����SCK��SDA(D0��D1)��PORT_cfg.h�и���SPIX_SCK��SPIX_SOUT

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