/*!
 * @�ļ�       fxos8700.c
 * @����       fxos8700����ʵ��,fxos8700��һ��������Խ��������ٶȼƴ�����������شŴ�����,
 *             ����ٶȴ���������������ԶԶ����MPU6050�ļ��ٶȴ�����.�شŴ������������Ⱥ͹�
 *             ��Ƶ��ԶԶ����MPU9050�ĵشŴ�����,����������������ߵĴ���,��оƬ����û��DMP
 *             ��Ԫ,�������Ҫ������̬�ǵĻ�,��Ҫ�������ӿ������˲��㷨.
 * @����       ������
 * @���ʱ��   2019��12��
 */
 
#ifndef __MK60_FXOS8700_H
#define __MK60_FXOS8700_H

#include "headfile.h"

#define FXOS8700_ADDR 0x1e

#define NO_ERR                  (uint8_t)0
#define ERR_IIC_CHECKACK_FAIL   (uint8_t)1
#define ERR_SELTTEST_FAIL       (uint8_t)2
#define ERR_FXOS8700INIT_FAIL   (uint8_t)3
#define ERR_DMPINIT_FAIL        (uint8_t)4

/********************** �Ĵ�����ַ **********************/

#define FXOS8700_STATUS               0x00		
#define FXOS8700_OUT_X_MSB            0x01
#define FXOS8700_OUT_X_LSB            0x02
#define FXOS8700_OUT_Y_MSB            0x03
#define FXOS8700_OUT_Y_LSB            0x04
#define FXOS8700_OUT_Z_MSB            0x05
#define FXOS8700_OUT_Z_LSB            0x06
#define FXOS8700_F_SETUP              0x09
#define FXOS8700_TRIG_CFG             0x0A
#define FXOS8700_SYSMOD               0x0B
#define FXOS8700_INT_SOURCE           0x0C
#define FXOS8700_WHO_AM_I             0x0D
#define FXOS8700_XYZ_DATA_CFG         0x0E
#define FXOS8700_HP_FILTER_CUTOFF     0x0F

#define FXOS8700_CTRL_REG1            0x2A
#define FXOS8700_CTRL_REG2            0x2B
#define FXOS8700_CTRL_REG3            0x2C
#define FXOS8700_CTRL_REG4            0x2D
#define FXOS8700_CTRL_REG5            0x2E

#define FXOS8700_M_DR_STATUS          0x32
#define FXOS8700_M_OUT_X_MSB          0x33
#define FXOS8700_M_OUT_X_LSB          0x34
#define FXOS8700_M_OUT_Y_MSB          0x35
#define FXOS8700_M_OUT_Y_LSB          0x36
#define FXOS8700_M_OUT_Z_MSB          0x37
#define FXOS8700_M_OUT_Z_LSB          0x38

#define FXOS8700_M_THS_CFG            0x52
#define FXOS8700_M_THS_SRC            0x53
#define FXOS8700_M_THS_X_MSB          0x54
#define FXOS8700_M_THS_X_LSB          0x55
#define FXOS8700_M_THS_Y_MSB          0x56
#define FXOS8700_M_THS_Y_LSB          0x57
#define FXOS8700_M_THS_Z_MSB          0x58
#define FXOS8700_M_THS_Z_LSB          0x59
#define FXOS8700_M_THS_COUNT          0x5A
#define FXOS8700_M_CTRL_REG1          0x5B
#define FXOS8700_M_CTRL_REG2          0x5C
#define FXOS8700_M_CTRL_REG3          0x5D

/********************** XYZ_DATA_CFG **********************/
typedef enum            //���ٶȼ��������
{ 
	FXOS8700_FS_2G = 0x00,    //int16������Ϊ��2G
	FXOS8700_FS_4G = 0x01,
	FXOS8700_FS_8G = 0x02,
}FXOS8700_FS_TypeDef;

/********************** CTRL_REG1 **********************/
typedef enum            //�����������
{ 
	FXOS8700_DR_800HZ = 0x00 << 3,
	FXOS8700_DR_400HZ = 0x01 << 3,
	FXOS8700_DR_200HZ = 0x02 << 3,
	FXOS8700_DR_100HZ = 0x03 << 3,
	FXOS8700_DR_50HZ = 0x04 << 3,
	FXOS8700_DR_25HZ = 0x05 << 3,
	FXOS8700_DR_12_5HZ = 0x06 << 3,
	FXOS8700_DR_6_25HZ = 0x07 << 3,
	FXOS8700_DR_1_5625HZ = 0x08 << 3,
}FXOS8700_DR_TypeDef;

#define FXOS8700_INOISE  0x04
#define FXOS8700_ACTIVE  0x01

/********************** CTRL_REG2 **********************/

#define FXOS8700_ST       0x80
#define FXOS8700_RST      0x40

/********************** CTRL_REG4 **********************/
#define FXOS8700_INT_EN_ASLP    0x80
#define FXOS8700_INT_EN_FIFO    0x40
#define FXOS8700_INT_EN_TRANS   0x20
#define FXOS8700_INT_EN_INDPRT  0x10
#define FXOS8700_INT_EN_PULSE   0x08
#define FXOS8700_INT_EN_FFMT    0x04
#define FXOS8700_INT_EN_VECM    0x02
#define FXOS8700_INT_EN_DRDY    0x01

/********************** CTRL_REG5 **********************/
#define FXOS8700_INT_CFG_ASLP   0x80
#define FXOS8700_INT_CFG_FIFO   0x40
#define FXOS8700_INT_CFG_TRANS  0x20
#define FXOS8700_INT_CFG_INDPRT 0x10
#define FXOS8700_INT_CFG_PULSE  0x08
#define FXOS8700_INT_CFG_FFMT   0x04
#define FXOS8700_INT_CFG_VECM   0x02
#define FXOS8700_INT_CFG_DRDY   0x01

/********************** M_THS_CFG **********************/
#define FXOS8700_M_THS_ELE      0x80
#define FXOS8700_M_THS_OAE      0x40
#define FXOS8700_M_THS_ZEFE     0x20
#define FXOS8700_M_THS_YEFE     0x10
#define FXOS8700_M_THS_XEFE     0x08
#define FXOS8700_M_THS_WAKE_EN  0x04
#define FXOS8700_M_THS_INT_EM   0x02
#define FXOS8700_M_THS_INT_CFG  0x01

/********************** M_THS_SRC **********************/
#define FXOS8700_M_THS_EA       0x80
#define FXOS8700_M_THS_ZHE      0x20
#define FXOS8700_M_THS_ZHP      0x10
#define FXOS8700_M_THS_YHE      0x08
#define FXOS8700_M_THS_YHP      0x04
#define FXOS8700_M_THS_XHE      0x02
#define FXOS8700_M_THS_XHP      0x01

/********************** M_CTRL_REG1 **********************/
typedef enum            //���ٶȼ������������
{ 
	FXOS8700_M_HMS_ONLY_ACCEL = 0x00,
	FXOS8700_M_HMS_ONLY_MAGNET = 0x01,
	FXOS8700_M_HMS_HYBRID_MODE = 0x03,
}FXOS8700_M_HMS_TypeDef;

/********************** �������� **********************/
typedef enum
{ 
	FXOS8700_DATA_X_ACCEL = 0,    //X����ٶ�
	FXOS8700_DATA_Y_ACCEL = 1,    //Y����ٶ�
	FXOS8700_DATA_Z_ACCEL = 2,    //Z����ٶ�
	FXOS8700_DATA_X_MAGNET = 3,   //X����ٶ�
	FXOS8700_DATA_Y_MAGNET = 4,   //Y����ٶ�
	FXOS8700_DATA_Z_MAGNET = 5,   //Z����ٶ�
}FXOS8700_DATA_TypeDef;

/***************** ��Ҫ�궨�� *****************/
#define FXOS8700_FS     FXOS8700_FS_2G    //���ü��ٶȼƵ�����,��FXOS8700_FS_TypeDef
#define FXOS8700_DR     FXOS8700_DR_800HZ //���ü��ٵļƵ��������Ƶ��,��FXOS8700_DR_TypeDef

/****************** ����ѡ�� ******************/
#define FXOS8700_SDA_Pin       B3
#define FXOS8700_SCL_Pin       B2

/****************** �ɵ��ú��� ******************/
uint8_t FXOS8700_Init(void);
uint8_t FXOS8700_ReadData(FXOS8700_DATA_TypeDef Data_Type, int16_t *data);

#endif
