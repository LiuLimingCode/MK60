/*!
 * @file       myspi.h
 * @brief      Ӳ��SPI�����SPIͳһʵ��
 * @author     llm
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
      �ĵ�Ƭ����̷�������ͬ,������ײ���ֲ.
##���SPIʹ�����ŵĸߵ͵�ƽ�仯����(GPIO)ģ���SPI��ʱ��
  ȱ�㣺�����ٶ���,�ڴ����������ҪCPU����,ռ��CPU
	�ŵ㣺���ڴ󲿷ֵ�Ƭ������GPIO����,�ײ���ֲ��
*********************************************************/

void SPI_InitHardware(SPIn_e Spi, uint32 baud);
void SPI_SendDataHardware(uint8 data, SPIn_e Spi);
void SPI_InitSimulated(PTX_n SdaPin, PTX_n SclPin);
void SPI_SendDataSimulated(PTX_n sdaPin, PTX_n sclPin, uint8 data);

#endif
