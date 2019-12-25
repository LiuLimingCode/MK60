 /*!
 * @�ļ�       mpu6050.h
 * @����       MPU6050����ʵ��,MPU6050��һ��������̬������,����ٶȴ���������Ư��,����������ȱ��.
 *             ����ٶȴ����������ϴ�,���ݱ���ͨ������󷽿�ʹ��.оƬ�����ŵ����Դ�DMP��Ԫ,����
 *             ������Ԫ��,Ϊ��Ƭ�������˺ܴ�ĸ���,����Ƭ����оƬDMP��ͨ�ű������ƴ���.
 * @����       ������
 * @���ʱ��   2019-12
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

/********************** �Ĵ�����ַ **********************/

#define MPU6050_REGISTER_SELF_TEST_X      0x0D    //X���Բ�ֵ
#define MPU6050_REGISTER_SELF_TEST_Y      0x0E    //Y���Բ�ֵ
#define MPU6050_REGISTER_SELF_TEST_Z      0x0F    //Z���Բ�ֵ
#define MPU6050_REGISTER_SELF_TEST_A      0x10    //���ٶ��Բ�ֵ
#define MPU6050_REGISTER_SMPLRT_DIV       0x19    //�����Ƶ
#define MPU6050_REGISTER_CONFIG           0x1A    //��ͨ�˲���
#define MPU6050_REGISTER_GYRO_CONFIG      0x1B    //X,Y,Z���������Լ�,ѡ������������
#define MPU6050_REGISTER_ACCEL_CONFIG     0x1C    //X,Y,Z����ٶȴ������Լ�,���ٶȴ���������ѡ��
#define MPU6050_REGISTER_FIFO_EN          0x23    //FIFOʹ�ܼĴ���
#define MPU6050_REGISTER_INT_ENABLE       0x38    //�ж�ʹ�ܼĴ���
#define MPU6050_REGISTER_INT_STATUS       0x3A    //�ж�״̬�Ĵ���
#define MPU6050_REGISTER_ACCEL_XOUTH      0x3B    //�洢���X����ٶȴ������Ĳ���ֵ(��8λ)
#define MPU6050_REGISTER_ACCEL_XOUTL      0x3C    //�洢���X����ٶȴ������Ĳ���ֵ(��8λ)
#define MPU6050_REGISTER_ACCEL_YOUTH      0x3D    //�洢���Y����ٶȴ������Ĳ���ֵ(��8λ)
#define MPU6050_REGISTER_ACCEL_YOUTL      0x3E    //�洢���Y����ٶȴ������Ĳ���ֵ(��8λ)
#define MPU6050_REGISTER_ACCEL_ZOUTH      0x3F    //�洢���Z����ٶȴ������Ĳ���ֵ(��8λ)
#define MPU6050_REGISTER_ACCEL_ZOUTL      0x40    //�洢���Z����ٶȴ������Ĳ���ֵ(��8λ)
#define MPU6050_REGISTER_TEMP_OUTH        0x41    //�洢����¶ȴ������Ĳ���ֵ(��8λ)
#define MPU6050_REGISTER_TEMP_OUTL        0x42    //�洢����¶ȴ������Ĳ���ֵ(��8λ)
#define MPU6050_REGISTER_GYRO_XOUTH       0x43    //�洢���X�������ǵĲ���ֵ(��8λ)
#define MPU6050_REGISTER_GYRO_XOUTL       0x44    //�洢���X�������ǵĲ���ֵ(��8λ)
#define MPU6050_REGISTER_GYRO_YOUTH       0x45    //�洢���Y�������ǵĲ���ֵ(��8λ)
#define MPU6050_REGISTER_GYRO_YOUTL       0x46    //�洢���Y�������ǵĲ���ֵ(��8λ)
#define MPU6050_REGISTER_GYRO_ZOUTH       0x47    //�洢���Z�������ǵĲ���ֵ(��8λ)
#define MPU6050_REGISTER_GYRO_ZOUTL       0x48    //�洢���Z�������ǵĲ���ֵ(��8λ)
#define MPU6050_REGISTER_PWR_MGMT_1       0x6B    //��Դ����1
#define MPU6050_REGISTER_PWR_MGMT_2       0x6C    //��Դ����2
#define MPU6050_REGISTER_WHO_AM_I         0x75    //�ӻ���ַ�Ĵ���

/********************** CONFIG **********************/
typedef enum            //�˲�������
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

#define XG_ST   0x80    //����X���������Լ�
#define YG_ST   0x40
#define ZG_ST   0x20
#define XYZG_ST 0xE0

typedef enum            //����������
{ 
	FS_SEL_250 = 0x00,    //int16������Ϊ��250��/s
	FS_SEL_500 = 0x08,
	FS_SEL_1000 = 0x10,
	FS_SEL_2000 = 0x18,
}FS_SEL_TypeDef;

/********************** ACCEL_CONFIG **********************/

#define XA_ST    0x80   //����X����ٶȴ������Լ�
#define YA_ST    0x40
#define ZA_ST    0x20
#define XYZA_ST  0xE0

typedef enum            //���ٶȴ���������
{ 
	AFS_SEL_2G = 0x00,    //int16������Ϊ��2G
	AFS_SEL_4G = 0x08,
	AFS_SEL_8G = 0x10,
	AFS_SEL_16G = 0x18,
}AFS_SEL_TypeDef;

/********************** FIFO_EN **********************/

#define TEMP_FIFO_EN		0x80  //FIFO�����¶ȴ���������
#define XG_FIFO_EN			0x40
#define YG_FIFO_EN			0x20
#define ZG_FIFO_EN			0x10
#define ACCEL_FIFO_EN		0x08
#define SLV1_FIFO_EN		0x04
#define SLV2_FIFO_EN		0x02
#define SLV3_FIFO_EN		0x01

/********************** PWR_MGMT_1 **********************/

#define DEVICE_RESET    0x80  //��1����λ���еļĴ���,��λ����Ժ��Զ���0
#define SLEEP           0x40  //����˯��ģʽ(�͹���ģʽ)
#define CYCLE           0x20  //����λΪ1��SLEEPΪ0ʱ,����ѭ��ģʽ.�豸��˯��ģʽ�ͻ���֮��ѭ��
#define TEMP_DIS        0x08  //�ر��¶ȴ�����

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

/********************** �������� **********************/
typedef enum
{ 
	MPU6050_ACCEL_X = 0, //X����ٶ�
	MPU6050_ACCEL_Y = 1, //Y����ٶ�
	MPU6050_ACCEL_Z = 2, //Z����ٶ�
	MPU6050_TEMPERATURE = 3, //�¶�ֵ
	MPU6050_GYRO_X = 4, //X����ٶ�
	MPU6050_GYRO_Y = 5, //Y����ٶ�
	MPU6050_GYRO_Z = 6, //Z����ٶ�
}MPU6050_DATA_TypeDef;

/***************** ��Ҫ�궨�� *****************/
#define MPU6050_DMP_EN        1           //ͨ����������ж��Ƿ���DMP���ܣ��رպ���Ҫ����DMP��ͷ�ļ�

#if MPU6050_DMP_EN == 0       //��������������ǵ�DMP����,�����ѡ������

#define MPU6050_AFS_SEL       AFS_SEL_2G  //���ü��ٶȼƵ�����,��AFS_SEL_TypeDef
#define MPU6050_FS_SEL        FS_SEL_2000 //���ý��ٶȼƵ�����,��FS_SEL_TypeDef
#define MPU6050_OUTPUT_RATE   200         //���ô����������Ƶ��(��λHz),���ڼĴ������㹫ʽΪ(1000/MPU6050_OUTPUT_RATE - 1)Ϊ����,
                                          //���Ը�ֵ���ȡ1000, 500, 200, 100,����ʵ�������������趨ֵ����ڲ���

#else                         //������������ǵ�DMP����,�޷���������,����������Ϊ��2000�����ٶȼ�Ϊ��2G

#define MPU6050_OUTPUT_RATE   200         //(�Ƽ�200)���ô����������Ƶ��(��λHz),���ȡ1000, 500, 200, 100
#define MPU6050_DMP_RATE      100         //(�Ƽ�100)����DMP���ݴ������Ƶ��(��λHz),ע���ֵ�ǳ���Ҫ������ҪС�ڵ���MPU6050_OUTPUT_RATE,
                                          //���,��������Ը�Ƶ�������Եص���MPU6050_DMPGetData��������ȡDMP��ֵ,����DMP�����ݻ���������Ϊ
                                          //���ݶ�ȡƵ�ʹ���������,����Ϊ���ݶ�ȡ����������ݿɶ�.�ⶼ����MPU6050_DMPGetData����ʧ��,����
                                          //DMP�ĵײ���,һ������ʧ��,�ײ�Ḵλ������,CPU����뼸ʮ���������.
#endif

/****************** ����ѡ�� ******************/
#define MPU6050_SDA_Pin       B3
#define MPU6050_SCK_Pin       B2

/****************** �ɵ��ú��� ******************/
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
