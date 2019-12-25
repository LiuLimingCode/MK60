 /*!
 * @文件       mpu6050.h
 * @功能       MPU6050函数实现,MPU6050是一款六轴姿态传感器,其角速度传感器略有漂移,无其他明显缺点.
 *             其加速度传感器噪声较大,数据必须通过处理后方可使用.芯片最大的优点是自带DMP单元,可以
 *             计算四元数,为单片机减少了很大的负担,但单片机和芯片DMP的通信必须妥善处理.
 * @作者       刘力铭
 * @完成时间   2019-12
 */
 
#ifndef __MK60_MPU6050_H
#define __MK60_MPU6050_H

#ifdef __cplusplus
extern "C" {
#endif

#include "headfile.h"

#define MPU6050_ADDR 0x68
#define q30  1073741824.0f

#define NO_ERR                   (uint8_t)0
#define ERR_IIC_CHECKACK_FAIL    (uint8_t)1
#define ERR_SELTTEST_FAIL        (uint8_t)2
#define ERR_MPU6050INIT_FAIL     (uint8_t)3
#define ERR_DMPINIT_FAIL         (uint8_t)4

/********************** 寄存器地址 **********************/

#define MPU6050_REGISTER_SELF_TEST_X      0x0D    //X轴自测值
#define MPU6050_REGISTER_SELF_TEST_Y      0x0E    //Y轴自测值
#define MPU6050_REGISTER_SELF_TEST_Z      0x0F    //Z轴自测值
#define MPU6050_REGISTER_SELF_TEST_A      0x10    //加速度自测值
#define MPU6050_REGISTER_SMPLRT_DIV       0x19    //输出分频
#define MPU6050_REGISTER_CONFIG           0x1A    //低通滤波器
#define MPU6050_REGISTER_GYRO_CONFIG      0x1B    //X,Y,Z轴陀螺仪自检,选择陀螺仪量程
#define MPU6050_REGISTER_ACCEL_CONFIG     0x1C    //X,Y,Z轴加速度传感器自检,加速度传感器量程选择
#define MPU6050_REGISTER_FIFO_EN          0x23    //FIFO使能寄存器
#define MPU6050_REGISTER_INT_ENABLE       0x38    //中断使能寄存器
#define MPU6050_REGISTER_INT_STATUS       0x3A    //中断状态寄存器
#define MPU6050_REGISTER_ACCEL_XOUTH      0x3B    //存储最近X轴加速度传感器的测量值(高8位)
#define MPU6050_REGISTER_ACCEL_XOUTL      0x3C    //存储最近X轴加速度传感器的测量值(低8位)
#define MPU6050_REGISTER_ACCEL_YOUTH      0x3D    //存储最近Y轴加速度传感器的测量值(高8位)
#define MPU6050_REGISTER_ACCEL_YOUTL      0x3E    //存储最近Y轴加速度传感器的测量值(低8位)
#define MPU6050_REGISTER_ACCEL_ZOUTH      0x3F    //存储最近Z轴加速度传感器的测量值(高8位)
#define MPU6050_REGISTER_ACCEL_ZOUTL      0x40    //存储最近Z轴加速度传感器的测量值(低8位)
#define MPU6050_REGISTER_TEMP_OUTH        0x41    //存储最近温度传感器的测量值(高8位)
#define MPU6050_REGISTER_TEMP_OUTL        0x42    //存储最近温度传感器的测量值(低8位)
#define MPU6050_REGISTER_GYRO_XOUTH       0x43    //存储最近X轴陀螺仪的测量值(高8位)
#define MPU6050_REGISTER_GYRO_XOUTL       0x44    //存储最近X轴陀螺仪的测量值(低8位)
#define MPU6050_REGISTER_GYRO_YOUTH       0x45    //存储最近Y轴陀螺仪的测量值(高8位)
#define MPU6050_REGISTER_GYRO_YOUTL       0x46    //存储最近Y轴陀螺仪的测量值(低8位)
#define MPU6050_REGISTER_GYRO_ZOUTH       0x47    //存储最近Z轴陀螺仪的测量值(高8位)
#define MPU6050_REGISTER_GYRO_ZOUTL       0x48    //存储最近Z轴陀螺仪的测量值(低8位)
#define MPU6050_REGISTER_PWR_MGMT_1       0x6B    //电源管理1
#define MPU6050_REGISTER_PWR_MGMT_2       0x6C    //电源管理2
#define MPU6050_REGISTER_WHO_AM_I         0x75    //从机地址寄存器

/********************** CONFIG **********************/
typedef enum            //滤波器设置
{ 
	DLPF_CFG_BANDWIDTH_260 = 0x00,
	DLPF_CFG_BANDWIDTH_184 = 0x01,
	DLPF_CFG_BANDWIDTH_94 = 0x02,
	DLPF_CFG_BANDWIDTH_44 = 0x03,
	DLPF_CFG_BANDWIDTH_21 = 0x04,
	DLPF_CFG_BANDWIDTH_10 = 0x05,
	DLPF_CFG_BANDWIDTH_5 = 0x06,
}DLPF_CFG_TypeDef;

/********************** GYRO_CONFIG **********************/

#define XG_ST   0x80    //开启X轴陀螺仪自检
#define YG_ST   0x40
#define ZG_ST   0x20
#define XYZG_ST 0xE0

typedef enum            //陀螺仪量程
{ 
	FS_SEL_250 = 0x00,    //int16满量程为±250°/s
	FS_SEL_500 = 0x08,
	FS_SEL_1000 = 0x10,
	FS_SEL_2000 = 0x18,
}FS_SEL_TypeDef;

/********************** ACCEL_CONFIG **********************/

#define XA_ST    0x80   //开启X轴加速度传感器自检
#define YA_ST    0x40
#define ZA_ST    0x20
#define XYZA_ST  0xE0

typedef enum            //加速度传感器量程
{ 
	AFS_SEL_2G = 0x00,    //int16满量程为±2G
	AFS_SEL_4G = 0x08,
	AFS_SEL_8G = 0x10,
	AFS_SEL_16G = 0x18,
}AFS_SEL_TypeDef;

/********************** FIFO_EN **********************/

#define TEMP_FIFO_EN		0x80  //FIFO储存温度传感器数据
#define XG_FIFO_EN			0x40
#define YG_FIFO_EN			0x20
#define ZG_FIFO_EN			0x10
#define ACCEL_FIFO_EN		0x08
#define SLV1_FIFO_EN		0x04
#define SLV2_FIFO_EN		0x02
#define SLV3_FIFO_EN		0x01

/********************** PWR_MGMT_1 **********************/

#define DEVICE_RESET    0x80  //置1来复位所有的寄存器,复位完成以后自动置0
#define SLEEP           0x40  //进入睡眠模式(低功耗模式)
#define CYCLE           0x20  //当此位为1而SLEEP为0时,进入循环模式.设备在睡眠模式和唤醒之间循环
#define TEMP_DIS        0x08  //关闭温度传感器

typedef enum
{
	CLKSEL_8MHZ = 0x00,
	CLKSEL_PLL_X = 0x01,
	CLKSEL_PLL_Y = 0x02,
	CLKSEL_PLL_Z = 0x03,
	CLKSEL_PLL_32_768KHZ = 0x04,
	CLKSEL_PLL_19_2MHZ = 0x05,
	CLKSEL_STOP = 0x07,
}CLKSEL_TypeDef;

/********************** 数据类型 **********************/
typedef enum
{ 
	MPU6050_ACCEL_X = 0, //X轴加速度
	MPU6050_ACCEL_Y = 1, //Y轴加速度
	MPU6050_ACCEL_Z = 2, //Z轴加速度
	MPU6050_TEMPERATURE = 3, //温度值
	MPU6050_GYRO_X = 4, //X轴角速度
	MPU6050_GYRO_Y = 5, //Y轴角速度
	MPU6050_GYRO_Z = 6, //Z轴角速度
}MPU6050_DATA_TypeDef;

/***************** 重要宏定义 *****************/
#define MPU6050_DMP_EN        1           //通过这个宏来判断是否开启DMP功能，关闭后不需要包含DMP的头文件

#if MPU6050_DMP_EN == 0       //如果不开启陀螺仪的DMP功能,则可以选择量程

#define MPU6050_AFS_SEL       AFS_SEL_2G  //设置加速度计的量程,见AFS_SEL_TypeDef
#define MPU6050_FS_SEL        FS_SEL_2000 //设置角速度计的量程,见FS_SEL_TypeDef
#define MPU6050_OUTPUT_RATE   200         //设置传感器输出的频率(单位Hz),由于寄存器计算公式为(1000/MPU6050_OUTPUT_RATE - 1)为整数,
                                          //所以该值最好取1000, 500, 200, 100,否则实际输出速率与该设定值会存在差异

#else                         //如果开启陀螺仪的DMP功能,无法设置量程,陀螺仪量程为±2000，加速度计为±2G

#define MPU6050_OUTPUT_RATE   200         //(推荐200)设置传感器输出的频率(单位Hz),最好取1000, 500, 200, 100
#define MPU6050_DMP_RATE      100         //(推荐100)设置DMP数据处理输出频率(单位Hz),注意该值非常重要首先他要小于等于MPU6050_OUTPUT_RATE,
                                          //其次,程序必须以该频率周期性地调用MPU6050_DMPGetData函数来读取DMP数值,否则DMP的数据缓冲区会因为
                                          //数据读取频率过慢而爆满,或因为数据读取过快而无数据可读.这都导致MPU6050_DMPGetData调用失败,而在
                                          //DMP的底层中,一旦调用失败,底层会复位传感器,CPU会进入几十毫秒的死等.
#endif

/****************** 引脚选择 ******************/
#define MPU6050_SDA_Pin       B3
#define MPU6050_SCK_Pin       B2

/****************** 可调用函数 ******************/
uint8_t MPU6050_ReadData(MPU6050_DATA_TypeDef Data_Type, int16_t *data);
#if MPU6050_DMP_EN == 0
uint8_t MPU6050_Init(void);
#endif
#if MPU6050_DMP_EN == 1
uint8_t MPU6050_DMPInit(void);
uint8_t MPU6050_SelfTest(void);
uint8_t MPU6050_DMPGetData(float *pitch,float *roll,float *yaw);
#endif

#ifdef __cplusplus
}
#endif

#endif
