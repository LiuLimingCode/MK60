/*!
 * @�ļ�       myiic.h
 * @����       ģ��IIC����ʵ��
 * @����       ������
 * @���ʱ��   2019-12
 */
 
#ifndef __MK60_MYIIC_H
#define __MK60_MYIIC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "headfile.h"

/***************** ��Ҫ�궨�� *****************/
#define NOP()   for(int tempdely = 0; tempdely < 2; ++tempdely);//����IICͨѶ����ʱʱ�䣬����ȶ���

//Ϊ������ֲ,����ֱ�Ӹ��ĺ궨��ʵ����ͬ����
#define MYIIC_GPIO_SET_BITS(x)    gpio_set(x ,1)       //����������ߵ�ƽ
#define MYIIC_GPIO_RESET_BITS(x)  gpio_set(x, 0)       //����������͵�ƽ
#define MYIIC_GPIO_SET_OUT(x)     gpio_init(x, GPO, 0) //����������Ϊ���ģʽ,Ĭ������͵�ƽ
#define MYIIC_GPIO_SET_IN(x)      gpio_init(x, GPI, 1) //����������Ϊ����ģʽ,Ĭ������
#define MYIIC_GPIO_READ(x)        gpio_get(x)          //��ȡ���������ƽ

/****************** �ɵ��ú��� ******************/
void IIC_Init(PTX_n sdaPin, PTX_n sclPin);
uint8_t IIC_ReadRegister(PTX_n sdaPin, PTX_n sclPin, uint8_t SlaveAddress, uint8_t REG_Address, uint8_t *data);
uint8_t IIC_ReadRegisterLen(PTX_n sdaPin, PTX_n sclPin, uint8_t SlaveAddress, uint8_t REG_Address, uint8_t len, uint8_t *buf);
uint8_t IIC_WriteRegister(PTX_n sdaPin, PTX_n sclPin, uint8_t SlaveAddress, uint8_t REG_Address, uint8_t REG_data);
uint8_t IIC_WriteRegisterLen(PTX_n sdaPin, PTX_n sclPin, uint8_t SlaveAddress, uint8_t REG_Address, uint8_t len, uint8_t *buf);
	
#ifdef __cplusplus
}
#endif

#endif
