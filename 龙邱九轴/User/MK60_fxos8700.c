/*!
 * @�ļ�       fxos8700.c
 * @����       fxos8700����ʵ��,fxos8700��һ��������Խ��������ٶȼƴ�����������شŴ�����,
 *             ����ٶȴ���������������ԶԶ����MPU6050�ļ��ٶȴ�����.�شŴ������������Ⱥ͹�
 *             ��Ƶ��ԶԶ����MPU9050�ĵشŴ�����,����������������ߵĴ���,��оƬ����û��DMP
 *             ��Ԫ,�������Ҫ������̬�ǵĻ�,��Ҫ�������ӿ������˲��㷨.
 * @����       ������
 * @���ʱ��   2019��12��
 */
 
#include "MK60_fxos8700.h"
#include "MK60_myiic.h"

//----------------------------------------------------------------------------
//  @����       FXOS8700��ʼ������
//  @����       0 = �ɹ�,���� = ʧ��
//  @ʾ��       if(FXOS8700_Init()) { ... }
//----------------------------------------------------------------------------
uint8_t FXOS8700_Init(void)
{
	uint8 IIC_ID;
	IIC_Init(FXOS8700_SDA_Pin, FXOS8700_SCL_Pin);
	if(IIC_WriteData(FXOS8700_SDA_Pin, FXOS8700_SCL_Pin, FXOS8700_ADDR, FXOS8700_M_CTRL_REG1, FXOS8700_M_HMS_HYBRID_MODE))return ERR_IIC_CHECKACK_FAIL;
	if(IIC_ReadData(FXOS8700_SDA_Pin, FXOS8700_SCL_Pin, FXOS8700_ADDR, FXOS8700_WHO_AM_I, &IIC_ID))return ERR_IIC_CHECKACK_FAIL;
	if(IIC_ID != 0xC7)return ERR_IIC_CHECKACK_FAIL;
	IIC_WriteData(FXOS8700_SDA_Pin, FXOS8700_SCL_Pin, FXOS8700_ADDR, FXOS8700_CTRL_REG2, FXOS8700_ST);
	systick_delay_ms(10);
	if(IIC_WriteData(FXOS8700_SDA_Pin, FXOS8700_SCL_Pin, FXOS8700_ADDR, FXOS8700_XYZ_DATA_CFG, FXOS8700_FS))return ERR_IIC_CHECKACK_FAIL;
	if(IIC_WriteData(FXOS8700_SDA_Pin, FXOS8700_SCL_Pin, FXOS8700_ADDR, FXOS8700_CTRL_REG1, FXOS8700_DR | FXOS8700_INOISE | FXOS8700_ACTIVE))return ERR_IIC_CHECKACK_FAIL;
	return NO_ERR;
}

//----------------------------------------------------------------------------
//  @����       FXOS8700��ȡ����
//  @����       Data_Type ��ȡ���ݵ�����,��FXOS8700_DATA_TypeDef
//  @����       data ��ȡ���ݴ�ŵĵ�ַ
//  @����       0 = �ɹ�,���� = ʧ��
//  @ʾ��       if(FXOS8700_ReadData(FXOS8700_DATA_X_ACCEL, &accelXData)) { ... }
//----------------------------------------------------------------------------
uint8_t FXOS8700_ReadData(FXOS8700_DATA_TypeDef Data_Type, int16_t *data)
{
	uint8_t Register_Arr;
	uint8_t dat_h, dat_l;
	Register_Arr = (Data_Type < FXOS8700_DATA_X_MAGNET) ? (FXOS8700_OUT_X_MSB + Data_Type * 2) : (FXOS8700_M_OUT_X_MSB + (Data_Type - 3) * 2);
	if(IIC_ReadData(FXOS8700_SDA_Pin, FXOS8700_SCL_Pin, FXOS8700_ADDR, Register_Arr, &dat_h))return ERR_IIC_CHECKACK_FAIL;
	if(IIC_ReadData(FXOS8700_SDA_Pin, FXOS8700_SCL_Pin, FXOS8700_ADDR, Register_Arr + 1, &dat_l))return ERR_IIC_CHECKACK_FAIL;
	*data = (int16_t)((dat_h << 8) | dat_l);
	return NO_ERR;
}
