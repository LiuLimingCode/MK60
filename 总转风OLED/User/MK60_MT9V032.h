/*!
 * @file       mt9v032.h
 * @brief      �����ת������ͷʵ��
 * @author     llm
 * @ע�⣺			 ���Ӳ���޸�������ͷ������,Ӧ�ý����������еĺ���������,��������ȷ����ʹ��
							 �жϷ�����ȫ��д��.c�ļ���,����������Լ�����
 */
#ifndef __MK60_MT9V032_H
#define __MK60_MT9V032_H

#include "headfile.h"

#define WIDTH   188//ͼ����   ��Χ1-752     K60�ɼ���������188
#define HEIGHT  120//ͼ��߶�	��Χ1-480

#define BLACK		0x00
#define WHITE		0xff


//����ͷ����ö��
typedef enum
{
    INIT = 0,               //����ͷ��ʼ������
    AUTO_EXP,               //�Զ��ع�����
    EXP_TIME,               //�ع�ʱ������
    FPS,                    //����ͷ֡������
    SET_COL,                //ͼ��������
    SET_ROW,                //ͼ��������
    LR_OFFSET,              //ͼ������ƫ������
    UD_OFFSET,              //ͼ������ƫ������
    GAIN,                   //ͼ��ƫ������
    CONFIG_FINISH,          //������λ����Ҫ����ռλ����
    
    SET_EXP_TIME = 0XF0,    //���������ع�ʱ������
    GET_STATUS,             //��ȡ����ͷ��������
    GET_VERSION,            //�̼��汾������
    
    SET_ADDR = 0XFE,        //�Ĵ�����ַ����
    SET_DATA                //�Ĵ�����������
}CMD;

extern uint8 mt9v032_finish_flag;  //һ��ͼ��ɼ���ɱ�־λ


//////////////////////////��������//////////////////////////

#define MT9V032_COF_UART    				uart3  													//��������ͷ��ʹ�õ��Ĵ���
#define MT9V032_COF_UART_INT				UART3_RX_TX_IRQHandler					//���ڵ��жϺ���
#define MT9V032_DMA_CH							DMA_CH0													//����ͷʹ�õ�DMAͨ��
#define MT9V032_DMA_CH_INT					DMA0_IRQHandler									//DMAͨ�����жϺ���
#define MT9V032_PCLK                C18                            	//����ͷ����ʱ��
#define MT9V032_VSYNC               C6                            	//����ͷ��ͬ���ź�
#define MT9V032_VSYNC_INT						PORTC_IRQHandler								//��ͬ���źŵ��жϺ���

#define MT9V032_DATAPORT 		C_IN_DATA(1)	                					//DMA���ݿ�
//--------------------------------------------------------------------------------------------------
//        ����ͷ���ݽӿ�
//        C_IN_DATA(1)
//        ��һ����ĸC������ʹ��C�ڣ�����ѡ��A��B��C��D��E.�ֱ��ӦA_IN_DATA��B_IN_DATA��C_IN_DATA��D_IN_DATA��E_IN_DATA
//        �ڶ�����������Ĳ���1������ʹ�õ���C8 - C15��,
//		  ����ѡ��0��1��2��3���ֱ��Ӧ����	�ö˿ڵ�X0-X7��X8-X15��X16-X23��X24-X31(����x������ǵ�һ����ĸ���������ڵľ���C8-C15)
//--------------------------------------------------------------------------------------------------

void MT9V032_Init(void *imgaddr);
void GrayscaleScaledHalf(uint8 *grayscale, uint8 *grayscaleHalf, uint16 width, uint16 height);
void GrayscaleToBinary(uint8 *Grayscale, uint8 *Binary, uint8 Threshold, uint32 size);
void GrayscaleSend(UARTn uartn, unsigned char *grayscale, uint16 width, uint16 height);
int OSTUThreshold(uint8 *grayscale, uint8 *binary, uint32 size);
#endif
