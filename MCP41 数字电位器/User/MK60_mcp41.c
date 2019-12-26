 /*!
 * @�ļ�       mcp41.c
 * @����       mcp41���ֵ�λ������ʵ��
 * @����       ������
 * @���ʱ��   2019-12
 */
 
#include "MK60_mcp41.h"
#include "MK60_myspi.h"
#include "MK60_oled.h"

static uint8 CSX[8] = {MCP41_CS0, MCP41_CS1, MCP41_CS2, MCP41_CS3, MCP41_CS4, MCP41_CS5, MCP41_CS6, MCP41_CS7};
static uint8 ADC_CX[8] = {MCP41_P0, MCP41_P1, MCP41_P2, MCP41_P3, MCP41_P4, MCP41_P5, MCP41_P6, MCP41_P7};

//--------------------------------------------------------------
//  @����       MCP41���ֵ�λ�����ų�ʼ��
//  @ʾ��       MCP41_Init ();
//--------------------------------------------------------------
void MCP41_Init(void)
{
	SPI_InitSimulated(MCP41_SDA_Pin, MCP41_SCL_Pin);
	for(int8 temp = 0; CSX[temp] != 0xff && temp < 8; temp++)
		gpio_init((PTX_n)CSX[temp], GPO, 1);
	for(int8 temp = 0; ADC_CX[temp] != 0xff && temp < 8; temp++)
		adc_init((ADCn_Ch)ADC_CX[temp]);
}

//--------------------------------------------------------------
//  @����       MCP41���ֵ�λ��д�����ֵ
//  @����       Set_Num   ���õ�·��(0~7)
//  @����       data      ���õĵ���ֵ(0~255)
//  @ʾ��       MCP41_SetCmd (2, 0x7F);
//--------------------------------------------------------------
void MCP41_SetCmd(uint8 Set_Num ,uint8 data)
{
	gpio_set((PTX_n)CSX[Set_Num], 0);
	SPI_SendDataSimulated(MCP41_SDA_Pin, MCP41_SCL_Pin, 0x11);
	SPI_SendDataSimulated(MCP41_SDA_Pin, MCP41_SCL_Pin, 0xff - data);
	gpio_set((PTX_n)CSX[Set_Num], 1);
}

//--------------------------------------------------------------
//  @����       MCP41���ֵ�λ������ADC����ֵ
//  @����       Set_Num   ���õ�·��(0~7)
//  @����       data      ���õĶ���ֵ(0~255)
//  @����       ���õĵ���ֵ
//  @ʾ��       MCP41_SetValue (2, 100);
//--------------------------------------------------------------
uint8 MCP41_SetValue(uint8 Set_Num, uint8 Ask_Value)
{
	uint16 Temp_ADC_Data;
	int16 Set_Data = 0xff;
	MCP41_SetCmd(Set_Num, Set_Data);
	
	while(1)
	{
		if(Set_Data <= 0x00) break;
		systick_delay_ms(5);
		Temp_ADC_Data = adc_once((ADCn_Ch)ADC_CX[Set_Num], ADC_8bit);
		Set_Data --;
		MCP41_SetCmd(Set_Num, Set_Data);
		if(Temp_ADC_Data <= Ask_Value)
			break;
	}
	MCP41_SetCmd(Set_Num, Set_Data + 1);
	systick_delay_ms(5);
	return Set_Data + 1;
}
