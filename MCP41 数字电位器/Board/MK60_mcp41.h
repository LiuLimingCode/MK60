 /*!
 * @�ļ�       mcp41.h
 * @����       mcp41���ֵ�λ������ʵ��
 * @����       ������
 * @���ʱ��   2019-12
 */
 
#ifndef __MK60_MCP41_H
#define __MK60_MCP41_H

#include "headfile.h"

/****************** SPI����ѡ�� ******************/
#define MCP41_SCL_Pin    E2
#define MCP41_SDA_Pin    E1

/****************** Ƭѡ����ѡ�� ******************/
#define MCP41_CS0       C0
#define MCP41_CS1       C0
#define MCP41_CS2       C0
#define MCP41_CS3       C0
#define MCP41_CS4       C0
#define MCP41_CS5       0xff
#define MCP41_CS6       0xff
#define MCP41_CS7       0xff

/****************** ADCͨ��ѡ�� ******************/
#define MCP41_P0       ADC0_SE8
#define MCP41_P1       ADC0_SE8
#define MCP41_P2       ADC0_SE8
#define MCP41_P3       ADC0_SE8
#define MCP41_P4       ADC0_SE8
#define MCP41_P5       0xff
#define MCP41_P6       0xff
#define MCP41_P7       0xff

void MCP41_Init(void);
void MCP41_SetCmd(uint8 Set_Num ,uint8 data);
uint8 MCP41_SetValue(uint8 Set_Num, uint8 Ask_Value);

#endif
