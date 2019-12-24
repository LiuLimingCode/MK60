/*!
 * @文件       fxas2100.c
 * @描述       fxas2100函数实现,fxas2100是一款三轴角速度传感器,性能与MPU6050几乎相同
 * @作者       刘力铭
 * @完成时间   2019年12月
 */
 
#include "MK60_fxas2100.h"
#include "MK60_myiic.h"
#include <math.h>

//----------------------------------------------------------------------------
//  @描述       FXAS2100初始化函数
//  @返回       0 = 成功,其他 = 失败
//  @示例       if(FXAS2100_Init()) { ... }
//----------------------------------------------------------------------------
uint8_t FXAS2100_Init(void)
{
	uint8 IIC_ID; 
	IIC_Init(FXAS2100_SDA_Pin, FXAS2100_SCL_Pin);
	if(IIC_ReadRegister(FXAS2100_SDA_Pin, FXAS2100_SCL_Pin, FXAS2100_ADDR, FXAS2100_WHO_AM_I, &IIC_ID))return ERR_IIC_CHECKACK_FAIL;
	if(IIC_ID != 0xD7)return ERR_IIC_CHECKACK_FAIL;
	IIC_WriteRegister(FXAS2100_SDA_Pin, FXAS2100_SCL_Pin, FXAS2100_ADDR, FXAS2100_CTRL_REG1, FXAS2100_RST);
	systick_delay_ms(10);
	if(IIC_WriteRegister(FXAS2100_SDA_Pin, FXAS2100_SCL_Pin, FXAS2100_ADDR, FXAS2100_CTRL_REG0, FXAS2100_FS))return ERR_IIC_CHECKACK_FAIL;
	if(IIC_WriteRegister(FXAS2100_SDA_Pin, FXAS2100_SCL_Pin, FXAS2100_ADDR, FXAS2100_CTRL_REG1, FXAS2100_DR | FXAS2100_MODE_ACTIVE))return ERR_IIC_CHECKACK_FAIL;
	return NO_ERR;
}

//----------------------------------------------------------------------------
//  @描述       FXAS2100读取数据
//  @参数       Data_Type 读取数据的类型,见FXAS2100_DATA_TypeDef
//  @参数       data 读取数据存放的地址
//  @返回       0 = 成功,其他 = 失败
//  @示例       if(FXAS2100_ReadData(FXAS2100_DATA_X_GYRO, &gyroXData)) { ... }
//----------------------------------------------------------------------------
uint8_t FXAS2100_ReadData(FXAS2100_DATA_TypeDef Data_Type, int16_t *data)
{
	uint8_t Register_Arr;
	uint8_t dat_h, dat_l;
	Register_Arr = FXAS2100_OUT_X_MSB + Data_Type * 2;
	if(IIC_ReadRegister(FXAS2100_SDA_Pin, FXAS2100_SCL_Pin, FXAS2100_ADDR, Register_Arr, &dat_h))return ERR_IIC_CHECKACK_FAIL;
	if(IIC_ReadRegister(FXAS2100_SDA_Pin, FXAS2100_SCL_Pin, FXAS2100_ADDR, Register_Arr + 1, &dat_l))return ERR_IIC_CHECKACK_FAIL;
	*data = (int16_t)((dat_h << 8) | dat_l);
	return NO_ERR;
}

//----------------------------------------------------------------------------
//  @描述       FXAS2100读取三轴数据
//  @参数       GXdata X轴角速度数据存放地址
//  @参数       GYdata Y轴角速度数据存放地址
//  @参数       GZdata Z轴角速度数据存放地址
//  @返回       0 = 成功,其他 = 失败
//  @示例       if(FXAS2100_ReadALLData(&gyroXData, &gyroYData, &gyroZData)) { ... }
//----------------------------------------------------------------------------
uint8_t FXAS2100_ReadALLData(int16_t *GXdata, int16_t *GYdata, int16_t *GZdata)
{
	uint8_t dat_h, dat_l;
	if(IIC_ReadRegister(FXAS2100_SDA_Pin, FXAS2100_SCL_Pin, FXAS2100_ADDR, FXAS2100_OUT_X_MSB, &dat_h))return ERR_IIC_CHECKACK_FAIL;
	if(IIC_ReadRegister(FXAS2100_SDA_Pin, FXAS2100_SCL_Pin, FXAS2100_ADDR, FXAS2100_OUT_X_LSB, &dat_l))return ERR_IIC_CHECKACK_FAIL;
	*GXdata = (int16_t)((dat_h << 8) | dat_l);
	if(IIC_ReadRegister(FXAS2100_SDA_Pin, FXAS2100_SCL_Pin, FXAS2100_ADDR, FXAS2100_OUT_Y_MSB, &dat_h))return ERR_IIC_CHECKACK_FAIL;
	if(IIC_ReadRegister(FXAS2100_SDA_Pin, FXAS2100_SCL_Pin, FXAS2100_ADDR, FXAS2100_OUT_Y_LSB, &dat_l))return ERR_IIC_CHECKACK_FAIL;
	*GYdata = (int16_t)((dat_h << 8) | dat_l);
	if(IIC_ReadRegister(FXAS2100_SDA_Pin, FXAS2100_SCL_Pin, FXAS2100_ADDR, FXAS2100_OUT_Z_MSB, &dat_h))return ERR_IIC_CHECKACK_FAIL;
	if(IIC_ReadRegister(FXAS2100_SDA_Pin, FXAS2100_SCL_Pin, FXAS2100_ADDR, FXAS2100_OUT_Z_LSB, &dat_l))return ERR_IIC_CHECKACK_FAIL;
	*GZdata = (int16_t)((dat_h << 8) | dat_l);
	return NO_ERR;
}
