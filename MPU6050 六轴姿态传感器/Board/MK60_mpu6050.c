 /*!
 * @文件       mpu6050.c
 * @功能       MPU6050函数实现,MPU6050是一款六轴姿态传感器,其角速度传感器略有漂移,无其他明显缺点.
 *             其加速度传感器噪声较大,数据必须通过处理后方可使用.芯片最大的优点是自带DMP单元,可以
 *             计算四元数,为单片机减少了很大的负担,但单片机和芯片DMP的通信必须妥善处理.
 * @作者       刘力铭
 * @完成时间   2019-12
 */
 
#include "MK60_mpu6050.h"
#include "MK60_myiic.h"
#if MPU6050_DMP_EN == 1
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include <math.h>
#endif

#if MPU6050_DMP_EN == 0
//----------------------------------------------------------------------------
//  @描述       MPU6050初始化函数
//  @返回       0 = 成功;1 = 失败
//  @示例       if(MPU6050_Init()) { ... }
//  @注意       为了不混淆初始化函数,只有在不开启DMP功能时,才能调用该函数
//----------------------------------------------------------------------------
uint8_t MPU6050_Init(void)
{
  IIC_Init(MPU6050_SDA_Pin, MPU6050_SCK_Pin);
  if(IIC_WriteData(MPU6050_SDA_Pin, MPU6050_SCK_Pin, MPU6050_ADDR, MPU6050_REGISTER_PWR_MGMT_1, 0x01))return ERR_IIC_CHECKACK_FAIL;
  if(IIC_WriteData(MPU6050_SDA_Pin, MPU6050_SCK_Pin, MPU6050_ADDR, MPU6050_REGISTER_SMPLRT_DIV, (1000 / MPU6050_OUTPUT_RATE - 1)))return ERR_IIC_CHECKACK_FAIL;   //200Hz
  if(IIC_WriteData(MPU6050_SDA_Pin, MPU6050_SCK_Pin, MPU6050_ADDR, MPU6050_REGISTER_CONFIG, DLPF_CFG_BANDWIDTH_94))return ERR_IIC_CHECKACK_FAIL;
  if(IIC_WriteData(MPU6050_SDA_Pin, MPU6050_SCK_Pin, MPU6050_ADDR, MPU6050_REGISTER_ACCEL_CONFIG, MPU6050_AFS_SEL))return ERR_IIC_CHECKACK_FAIL;
  if(IIC_WriteData(MPU6050_SDA_Pin, MPU6050_SCK_Pin, MPU6050_ADDR, MPU6050_REGISTER_GYRO_CONFIG, MPU6050_FS_SEL))return ERR_IIC_CHECKACK_FAIL;
  return NO_ERR;
}
#endif

//----------------------------------------------------------------------------
//  @描述       MPU6050读取数据
//  @参数       Data_Type读取数据的类型,见MPU6050_DATA_TypeDef
//  @参数       data 为读取数据存放的地址
//  @返回       0 = 成功,其他 = 失败
//  @示例       if(MPU6050_ReadData(MPU6050_GYRO_X, &gyroXData)) { ... }
//----------------------------------------------------------------------------
uint8_t MPU6050_ReadData(MPU6050_DATA_TypeDef Data_Type, int16_t *data)
{
	uint8_t Register_Arr;
	uint8_t dat_h, dat_l;
	Register_Arr = MPU6050_REGISTER_ACCEL_XOUTH + Data_Type * 2;
	if(IIC_ReadData(MPU6050_SDA_Pin, MPU6050_SCK_Pin, MPU6050_ADDR, Register_Arr, &dat_h))return ERR_IIC_CHECKACK_FAIL;
	if(IIC_ReadData(MPU6050_SDA_Pin, MPU6050_SCK_Pin, MPU6050_ADDR, Register_Arr + 1, &dat_l))return ERR_IIC_CHECKACK_FAIL;
	*data = (int16_t)((dat_h << 8) | dat_l);
	return NO_ERR;
}

#if MPU6050_DMP_EN == 1 //以下全为DMP函数

static unsigned short inv_orientation_matrix_to_scalar(const signed char *mtx);
static unsigned short inv_row_2_scale(const signed char *row);

static signed char gyro_orientation[9] = { 1, 0, 0,
                                           0, 1, 0,
                                           0, 0, 1};

//----------------------------------------------------------------------------
//  @描述       MPU6050初始化函数
//  @返回       0 = 成功,其他 = 失败
//  @示例       while(MPU6050_DMPInit());
//  @注意       DMP默认配置陀螺仪量程为±2000,加速度计为±2G
//----------------------------------------------------------------------------
uint8_t MPU6050_DMPInit(void)
{
	uint8_t err=0;
	IIC_Init(MPU6050_SDA_Pin, MPU6050_SCK_Pin); //初始化IIC总线
	if(mpu_init()==0)	//初始化MPU6050
	{
		err = mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL); //设置需要的传感器
		if(err)return ERR_MPU6050INIT_FAIL; 
		err = mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL); //设置FIFO
		if(err)return ERR_MPU6050INIT_FAIL; 
		err = mpu_set_sample_rate(MPU6050_OUTPUT_RATE);	//设置采样率200HZ
		if(err)return ERR_MPU6050INIT_FAIL; 
		err = dmp_load_motion_driver_firmware();		//加载DMP固件
		if(err)return ERR_DMPINIT_FAIL; 
		err = dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_orientation)); //设置陀螺仪方向
		if(err)return ERR_MPU6050INIT_FAIL; 
		err = dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_TAP | DMP_FEATURE_ANDROID_ORIENT | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO | DMP_FEATURE_GYRO_CAL); //设置DMP功能
		if(err)return ERR_DMPINIT_FAIL; 
		err = dmp_set_fifo_rate(MPU6050_DMP_RATE); //设置DMP输出速率
		if(err)return ERR_DMPINIT_FAIL;   
		//err = MPU6050_SelfTest(); //自检,建议屏蔽否则每次初始化移动陀螺仪会导致自检失败
		//if(err)return ERR_SELTTEST_FAIL;    
		err = mpu_set_dmp_state(1);	//使能DMP
		if(err)return ERR_DMPINIT_FAIL;     
	}else return ERR_MPU6050INIT_FAIL;
	return NO_ERR;
}

//----------------------------------------------------------------------------
//  @描述       MPU6050自检函数
//  @返回       0 = 成功,其他 = 失败
//  @示例       if(MPU6050_SelfTest()) { ... }
//  @注意       在调用自检函数时,必须保证陀螺仪静止才会返回成功,
//              建议只在怀疑传感器出问题时才调用该函数
//----------------------------------------------------------------------------
uint8_t MPU6050_SelfTest(void)
{
	int result;
	long gyro[3], accel[3]; 
	result = mpu_run_self_test(gyro, accel);
	if (result == 0x3) 
	{
		float sens;
		unsigned short accel_sens;
		mpu_get_gyro_sens(&sens);
		gyro[0] = (long)(gyro[0] * sens);
		gyro[1] = (long)(gyro[1] * sens);
		gyro[2] = (long)(gyro[2] * sens);
		dmp_set_gyro_bias(gyro);
		mpu_get_accel_sens(&accel_sens);
		accel_sens = 0;
		accel[0] *= accel_sens;
		accel[1] *= accel_sens;
		accel[2] *= accel_sens;
		dmp_set_accel_bias(accel);
		return NO_ERR;
	}else return ERR_SELTTEST_FAIL;
}


//----------------------------------------------------------------------------
//  @描述       MPU6050读取DMP处理之后的数据
//  @参数       pitch:俯仰角存放地址   精度:0.1°   范围: -90.0° <--->  +90.0°
//  @参数       roll :横滚角存放地址   精度:0.1°   范围:-180.0° <---> +180.0°
//  @参数       yaw  :航向角存放地址   精度:0.1°   范围:-180.0° <---> +180.0°
//  @返回       0 = 成功,其他 = 失败
//  @示例       if(MPU6050_DMPGetData(&pitch, &roll, &yaw)) { ... }
//  @注意       若读DMP时间太慢或太慢,会导致FIFO溢出,造成dmp_read_fifo()函数调用
//              错误函数调用错误以后会自动调用mpu_reset_fifo(),故解决办法为:
//              1.定时器定时调用MPU6050_DMPGetData(),并且调用频率和MPU6050_DMP_RATE一致
//              2.在一次调用失败以后延时再一次调用while(MPU6050_DMPGetData())
//----------------------------------------------------------------------------
uint8_t MPU6050_DMPGetData(float *pitch, float *roll, float *yaw)
{
	float q0 = 1.0f,q1 = 0.0f,q2 = 0.0f,q3 = 0.0f;
	unsigned long sensor_timestamp;
	short gyro[3], accel[3], sensors;
	unsigned char more;
	long quat[4];
	if(dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors, &more))return 1;
	if(sensors&INV_WXYZ_QUAT) 
	{
		q0 = quat[0] / q30;
		q1 = quat[1] / q30;
		q2 = quat[2] / q30;
		q3 = quat[3] / q30; 
		*pitch = asin(-2 * q1 * q3 + 2 * q0* q2)* 57.3; // pitch
		*roll  = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1)* 57.3; // roll
		*yaw   = atan2(2 * (q1 * q2 + q0 * q3), q0 * q0 + q1 * q1 - q2 * q2 - q3 * q3) * 57.3; //yaw
	}else return 2;
	return NO_ERR;
}

//方向转换
static unsigned short inv_row_2_scale(const signed char *row)
{
    unsigned short b;

    if (row[0] > 0)
        b = 0;
    else if (row[0] < 0)
        b = 4;
    else if (row[1] > 0)
        b = 1;
    else if (row[1] < 0)
        b = 5;
    else if (row[2] > 0)
        b = 2;
    else if (row[2] < 0)
        b = 6;
    else
        b = 7;      // error
    return b;
}

//陀螺仪方向控制
static unsigned short inv_orientation_matrix_to_scalar( const signed char *mtx)
{
    unsigned short scalar; 
    /*
       XYZ  010_001_000 Identity Matrix
       XZY  001_010_000
       YXZ  010_000_001
       YZX  000_010_001
       ZXY  001_000_010
       ZYX  000_001_010
     */
    scalar = inv_row_2_scale(mtx);
    scalar |= inv_row_2_scale(mtx + 3) << 3;
    scalar |= inv_row_2_scale(mtx + 6) << 6;
    return scalar;
}
#endif
