/*!
 * @file       fxas2100.h
 * @brief      fxas2100函数实现
 * @author     刘力铭
 */
#ifndef _MK60_FXAS2100_H
#define _MK60_FXAS2100_H

#include "headfile.h"

#define FXAS2100_ADDR 0x20

#define NO_ERR									(uint8_t)0
#define ERR_IIC_CHECKACK_FAIL		(uint8_t)1
#define ERR_SELTTEST_FAIL				(uint8_t)2
#define ERR_FXAS2100INIT_FAIL		(uint8_t)3
#define ERR_DMPINIT_FAIL				(uint8_t)4

typedef enum
{
	FXAS2100_DATA_X_GYRO = 0,
	FXAS2100_DATA_Y_GYRO = 1,
	FXAS2100_DATA_Z_GYRO = 2,
}FXAS2100_DATA_TypeDef;

/******************* FXAS2100寄存器地址 *******************/

#define FXAS2100_STATUS								0x00		//
#define FXAS2100_OUT_X_MSB						0x01		//存储最近X轴加速度传感器的测量值(高8位)
#define FXAS2100_OUT_X_LSB						0x02		//存储最近X轴加速度传感器的测量值(低8位)
#define FXAS2100_OUT_Y_MSB						0x03		//存储最近Y轴加速度传感器的测量值(高8位)
#define FXAS2100_OUT_Y_LSB						0x04		//存储最近Y轴加速度传感器的测量值(低8位)
#define FXAS2100_OUT_Z_MSB						0x05		//存储最近Z轴加速度传感器的测量值(高8位)
#define FXAS2100_OUT_Z_LSB						0x06		//存储最近Z轴加速度传感器的测量值(低8位)
#define FXAS2100_DR_STATUS						0x07
#define FXAS2100_F_STATUS							0x08
#define FXAS2100_F_SETUP							0x09
#define FXAS2100_F_EVENT							0x0A
#define FXAS2100_INT_SRC_FLAG					0x0B
#define FXAS2100_WHO_AM_I							0x0C
#define FXAS2100_CTRL_REG0						0x0D
#define FXAS2100_RT_CFG								0x0E
#define FXAS2100_RT_SRC								0x0F
#define FXAS2100_RT_THS								0x10
#define FXAS2100_RT_COUNT							0x11
#define FXAS2100_TEMP									0x12
#define FXAS2100_CTRL_REG1						0x13
#define FXAS2100_CTRL_REG2						0x14
#define FXAS2100_CTRL_REG3						0x15

/********************** CTRL_REG0 **********************/
//主要是设置陀螺仪的带宽设置的灵敏度，具体看数据手册
typedef enum															//陀螺仪输出速率设置
{ 
	FXAS2100_FS_2000 = 0x00,
	FXAS2100_FS_1000 = 0x01,
	FXAS2100_FS_500 = 0x02,
	FXAS2100_FS_250 = 0x03,
}FXAS2100_FS_TypeDef;

/********************** CTRL_REG1 **********************/
#define FXAS2100_RST								0x40					//软件复位
#define FXAS2100_ST									0x20					//自检使能

typedef enum															//陀螺仪输出速率设置
{ 
	FXAS2100_DR_800HZ = 0x00<<2,
	FXAS2100_DR_400HZ = 0x01<<2,
	FXAS2100_DR_200HZ = 0x02<<2,
	FXAS2100_DR_100HZ = 0x03<<2,
	FXAS2100_DR_50HZ = 0x04<<2,
	FXAS2100_DR_25HZ = 0x05<<2,
	FXAS2100_DR_12_5HZ = 0x06<<2,
}FXAS2100_DR_TypeDef;

typedef enum															//陀螺仪模式设置
{ 
	FXAS2100_MODE_STANDBY = 0x00,
	FXAS2100_MODE_READY = 0x01,
	FXAS2100_MODE_ACTIVE = 0x02,
}FXAS2100_MODE_TypeDef;

uint8_t FXAS2100_Init(void);
uint8_t FXAS2100_ReadData(FXAS2100_DATA_TypeDef Data_Type, int16_t *data);
uint8_t FXAS2100_ReadALLData(int16_t *GXdata, int16_t *GYdata, int16_t *GZdata);

#endif
