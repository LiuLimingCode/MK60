/*!
 * @文件       fxos8700.c
 * @描述       fxos8700函数实现,fxos8700是一款性能优越的三轴加速度计传感器和三轴地磁传感器,
 *             其加速度传感器的数据噪声远远低于MPU6050的加速度传感器.地磁传感器的灵敏度和工
 *             作频率远远超过MPU9050的地磁传感器,可以用来检测起跑线的磁铁,但芯片本身没有DMP
 *             单元,所以如果要计算姿态角的话,需要另外增加卡尔曼滤波算法.
 * @作者       刘力铭
 * @完成时间   2019年12月
 */
 
#include "MK60_fxos8700.h"
#include "MK60_myiic.h"

//----------------------------------------------------------------------------
//  @描述       FXOS8700初始化函数
//  @返回       0 = 成功,其他 = 失败
//  @示例       if(FXOS8700_Init()) { ... }
//----------------------------------------------------------------------------
uint8_t FXOS8700_Init(void)
{
	uint8 IIC_ID;
	IIC_Init(FXOS8700_SDA_Pin, FXOS8700_SCL_Pin);
	if(IIC_WriteRegister(FXOS8700_SDA_Pin, FXOS8700_SCL_Pin, FXOS8700_ADDR, FXOS8700_M_CTRL_REG1, FXOS8700_M_HMS_HYBRID_MODE))return ERR_IIC_CHECKACK_FAIL;
	if(IIC_ReadRegister(FXOS8700_SDA_Pin, FXOS8700_SCL_Pin, FXOS8700_ADDR, FXOS8700_WHO_AM_I, &IIC_ID))return ERR_IIC_CHECKACK_FAIL;
	if(IIC_ID != 0xC7)return ERR_IIC_CHECKACK_FAIL;
	IIC_WriteRegister(FXOS8700_SDA_Pin, FXOS8700_SCL_Pin, FXOS8700_ADDR, FXOS8700_CTRL_REG2, FXOS8700_ST);
	systick_delay_ms(10);
	if(IIC_WriteRegister(FXOS8700_SDA_Pin, FXOS8700_SCL_Pin, FXOS8700_ADDR, FXOS8700_XYZ_DATA_CFG, FXOS8700_FS))return ERR_IIC_CHECKACK_FAIL;
	if(IIC_WriteRegister(FXOS8700_SDA_Pin, FXOS8700_SCL_Pin, FXOS8700_ADDR, FXOS8700_CTRL_REG1, FXOS8700_DR | FXOS8700_INOISE | FXOS8700_ACTIVE))return ERR_IIC_CHECKACK_FAIL;
	return NO_ERR;
}

//----------------------------------------------------------------------------
//  @描述       FXOS8700读取数据
//  @参数       Data_Type 读取数据的类型,见FXOS8700_DATA_TypeDef
//  @参数       data 读取数据存放的地址
//  @返回       0 = 成功,其他 = 失败
//  @示例       if(FXOS8700_ReadData(FXOS8700_DATA_X_ACCEL, &accelXData)) { ... }
//----------------------------------------------------------------------------
uint8_t FXOS8700_ReadData(FXOS8700_DATA_TypeDef Data_Type, int16_t *data)
{
	uint8_t Register_Arr;
	uint8_t dat_h, dat_l;
	Register_Arr = (Data_Type < FXOS8700_DATA_X_MAGNET) ? (FXOS8700_OUT_X_MSB + Data_Type * 2) : (FXOS8700_M_OUT_X_MSB + (Data_Type - 3) * 2);
	if(IIC_ReadRegister(FXOS8700_SDA_Pin, FXOS8700_SCL_Pin, FXOS8700_ADDR, Register_Arr, &dat_h))return ERR_IIC_CHECKACK_FAIL;
	if(IIC_ReadRegister(FXOS8700_SDA_Pin, FXOS8700_SCL_Pin, FXOS8700_ADDR, Register_Arr + 1, &dat_l))return ERR_IIC_CHECKACK_FAIL;
	*data = (int16_t)((dat_h << 8) | dat_l);
	return NO_ERR;
}
