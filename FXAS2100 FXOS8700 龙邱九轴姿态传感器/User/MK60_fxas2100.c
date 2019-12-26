/*!
 * @�ļ�       fxas2100.c
 * @����       fxas2100����ʵ��,fxas2100��һ��������ٶȴ�����,������MPU6050������ͬ
 * @����       ������
 * @���ʱ��   2019��12��
 */
 
#include "MK60_fxas2100.h"
#include "MK60_myiic.h"
#include <math.h>

//----------------------------------------------------------------------------
//  @����       FXAS2100��ʼ������
//  @����       0 = �ɹ�,���� = ʧ��
//  @ʾ��       if(FXAS2100_Init()) { ... }
//----------------------------------------------------------------------------
uint8_t FXAS2100_Init(void)
{
	uint8 IIC_ID; 
	IIC_Init(FXAS2100_SDA_Pin, FXAS2100_SCL_Pin);
	if(IIC_ReadData(FXAS2100_SDA_Pin, FXAS2100_SCL_Pin, FXAS2100_ADDR, FXAS2100_WHO_AM_I, &IIC_ID))return ERR_IIC_CHECKACK_FAIL;
	if(IIC_ID != 0xD7)return ERR_IIC_CHECKACK_FAIL;
	IIC_WriteData(FXAS2100_SDA_Pin, FXAS2100_SCL_Pin, FXAS2100_ADDR, FXAS2100_CTRL_REG1, FXAS2100_RST);
	systick_delay_ms(10);
	if(IIC_WriteData(FXAS2100_SDA_Pin, FXAS2100_SCL_Pin, FXAS2100_ADDR, FXAS2100_CTRL_REG0, FXAS2100_FS))return ERR_IIC_CHECKACK_FAIL;
	if(IIC_WriteData(FXAS2100_SDA_Pin, FXAS2100_SCL_Pin, FXAS2100_ADDR, FXAS2100_CTRL_REG1, FXAS2100_DR | FXAS2100_MODE_ACTIVE))return ERR_IIC_CHECKACK_FAIL;
	return NO_ERR;
}

//----------------------------------------------------------------------------
//  @����       FXAS2100��ȡ����
//  @����       Data_Type ��ȡ���ݵ�����,��FXAS2100_DATA_TypeDef
//  @����       data ��ȡ���ݴ�ŵĵ�ַ
//  @����       0 = �ɹ�,���� = ʧ��
//  @ʾ��       if(FXAS2100_ReadData(FXAS2100_DATA_X_GYRO, &gyroXData)) { ... }
//----------------------------------------------------------------------------
uint8_t FXAS2100_ReadData(FXAS2100_DATA_TypeDef Data_Type, int16_t *data)
{
	uint8_t Register_Arr;
	uint8_t dat_h, dat_l;
	Register_Arr = FXAS2100_OUT_X_MSB + Data_Type * 2;
	if(IIC_ReadData(FXAS2100_SDA_Pin, FXAS2100_SCL_Pin, FXAS2100_ADDR, Register_Arr, &dat_h))return ERR_IIC_CHECKACK_FAIL;
	if(IIC_ReadData(FXAS2100_SDA_Pin, FXAS2100_SCL_Pin, FXAS2100_ADDR, Register_Arr + 1, &dat_l))return ERR_IIC_CHECKACK_FAIL;
	*data = (int16_t)((dat_h << 8) | dat_l);
	return NO_ERR;
}

//----------------------------------------------------------------------------
//  @����       FXAS2100��ȡ��������
//  @����       GXdata X����ٶ����ݴ�ŵ�ַ
//  @����       GYdata Y����ٶ����ݴ�ŵ�ַ
//  @����       GZdata Z����ٶ����ݴ�ŵ�ַ
//  @����       0 = �ɹ�,���� = ʧ��
//  @ʾ��       if(FXAS2100_ReadALLData(&gyroXData, &gyroYData, &gyroZData)) { ... }
//----------------------------------------------------------------------------
uint8_t FXAS2100_ReadALLData(int16_t *GXdata, int16_t *GYdata, int16_t *GZdata)
{
	uint8_t dat_h, dat_l;
	if(IIC_ReadData(FXAS2100_SDA_Pin, FXAS2100_SCL_Pin, FXAS2100_ADDR, FXAS2100_OUT_X_MSB, &dat_h))return ERR_IIC_CHECKACK_FAIL;
	if(IIC_ReadData(FXAS2100_SDA_Pin, FXAS2100_SCL_Pin, FXAS2100_ADDR, FXAS2100_OUT_X_LSB, &dat_l))return ERR_IIC_CHECKACK_FAIL;
	*GXdata = (int16_t)((dat_h << 8) | dat_l);
	if(IIC_ReadData(FXAS2100_SDA_Pin, FXAS2100_SCL_Pin, FXAS2100_ADDR, FXAS2100_OUT_Y_MSB, &dat_h))return ERR_IIC_CHECKACK_FAIL;
	if(IIC_ReadData(FXAS2100_SDA_Pin, FXAS2100_SCL_Pin, FXAS2100_ADDR, FXAS2100_OUT_Y_LSB, &dat_l))return ERR_IIC_CHECKACK_FAIL;
	*GYdata = (int16_t)((dat_h << 8) | dat_l);
	if(IIC_ReadData(FXAS2100_SDA_Pin, FXAS2100_SCL_Pin, FXAS2100_ADDR, FXAS2100_OUT_Z_MSB, &dat_h))return ERR_IIC_CHECKACK_FAIL;
	if(IIC_ReadData(FXAS2100_SDA_Pin, FXAS2100_SCL_Pin, FXAS2100_ADDR, FXAS2100_OUT_Z_LSB, &dat_l))return ERR_IIC_CHECKACK_FAIL;
	*GZdata = (int16_t)((dat_h << 8) | dat_l);
	return NO_ERR;
}
