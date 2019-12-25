/*!
 * @�ļ�       myiic.h
 * @����       ģ��IIC����ʵ��,����IIC�ײ���C++���Ա�д,��Ϊһ����,IIC�ĵײ���C���Ա�д���ڷ���
 *             һ����,�Ҿ�������һ���ܺõĻ���չʾһ�������C���Ժ�C++��ϱ���.
 * @����       ������
 * @���ʱ��   2019-12
 */
 
#ifndef __MK60_MYIIC_H
#define __MK60_MYIIC_H

#ifdef __cplusplus  //ע��C���Ժ�C++���
extern "C" {
#endif

#include "headfile.h"

/***************** ��Ҫ�궨�� *****************/
#define NOP()   for(int tempdely = 0; tempdely < 2; ++tempdely);//����IICͨѶ����ʱʱ�䣬����ȶ���

//Ϊ������ֲ,����ֱ�Ӹ��ĺ궨��ʵ����ͬ����
#define MYIIC_GPIO_SET_BITS(x)    gpio_set(x, 1)       //����������ߵ�ƽ
#define MYIIC_GPIO_RESET_BITS(x)  gpio_set(x, 0)       //����������͵�ƽ
#define MYIIC_GPIO_SET_OUT(x)     gpio_init(x, GPO, 0) //����������Ϊ���ģʽ,Ĭ������͵�ƽ
#define MYIIC_GPIO_SET_IN(x)      gpio_init(x, GPI, 1) //����������Ϊ����ģʽ,Ĭ������
#define MYIIC_GPIO_READ(x)        gpio_get(x)          //��ȡ���������ƽ

/****************** C���Է��ɵ��ú��� ******************/
void IIC_Init(PTX_n sdaPin, PTX_n sckPin);
uint8 IIC_WriteData(PTX_n sdaPin, PTX_n sckPin, uint8 slaveAddress, uint8 registerAddress, uint8 data);
uint8 IIC_WriteBuffer(PTX_n sdaPin, PTX_n sckPin, uint8 slaveAddress, uint8 registerAddress, uint8 len, uint8 *buffer);
uint8 IIC_ReadData(PTX_n sdaPin, PTX_n sckPin, uint8 slaveAddress, uint8 registerAddress, uint8 *data);
uint8 IIC_ReadBuffer(PTX_n sdaPin, PTX_n sckPin, uint8 slaveAddress, uint8 registerAddress, uint8 len, uint8 *buffer);

#ifdef __cplusplus  //ע��C���Ժ�C++���
}
#endif

#endif
