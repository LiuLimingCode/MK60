/*!
 * @�ļ�       myspi.h
 * @����       Ӳ��SPI�����SPIͳһʵ��
 * @����       ������
 * @���ʱ��   2019-12
 */

#ifndef __MK60_MYSPI_H
#define __MK60_MYSPI_H

#include "headfile.h"

/****************** Ӳ��SPI�����SPI���� ******************
##SPI��ͨѶЭ��,����������������(SDA��������,SCLʱ������)����
  ���ķ�ʽ�ı��ƽ�ź�,�Ӷ����䲻ͬ������.
##Ӳ��SPIʹ�õ�Ƭ���ڲ���SPIģ�鴫������
  �ŵ㣺�����ٶȿ�,������ռ��CPU
  ȱ�㣺��Ƭ��������SPIģ��,���ұ����Ƶ�оƬ�ļĴ���,��ͬ�ͺ�
        �ĵ�Ƭ����̷�������ͬ,������ײ���ֲ.��Ӳ����Ƶ�ʱ��
        ����ʹ�õ�Ƭ���о���SPI���ܵ�ָ������.
##���SPIʹ�����ŵĸߵ͵�ƽ�仯����(GPIO)ģ���SPI��ʱ��
  ȱ�㣺�����ٶ���,�ڴ����������ҪCPU����,ռ��CPU
  �ŵ㣺���ڴ󲿷ֵ�Ƭ������GPIO����,�ײ���ֲ��,��Ӳ����Ƶ�
        ʱ�����ʹ�õ�Ƭ�������������.
*********************************************************/

void SPI_InitHardware(SPIn_e spi, uint32 baud);
void SPI_SendDataHardware(SPIn_e spi, uint8 data);
void SPI_InitSimulated(PTX_n sdaPin, PTX_n sclPin);
void SPI_SendDataSimulated(PTX_n sdaPin, PTX_n sclPin, uint8 data);

#endif
