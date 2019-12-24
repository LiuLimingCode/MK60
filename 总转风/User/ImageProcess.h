/*!
 * @�ļ�       ImageProcess.c
 * @����       ����ͼ��Ĵ�����,��Ϊ������ĳ��򴫳бȽ�����,��ϣ��ÿ���������Ա�ܹ�
 *             �ܽ����õ�ͼ�����㷨������ļ���.������ļ�Ϊ������������ͷͼ�������
 */
 
#ifndef __IMAGEPROCESS_H
#define __IMAGEPROCESS_H

#include "headfile.h"

/***************** ��Ҫ�궨�� *****************/
#define IMAGE_BLACK     0xff
#define IMAGE_WHITE     0x00

/****************** �ɵ��ú��� ******************/
void GrayscaleToBinary(uint8 *grayscale, uint8 *binary, uint8 threshold, uint32 size);
uint8 OSTUThreshold(uint8 *image, uint16 col, uint16 row);
void GrayscaleSend(UARTn uartn, uint8 *sendImage, uint32 size);

#endif
