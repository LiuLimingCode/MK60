/*!
 * @文件       myiic.h
 * @功能       模拟IIC函数实现,整个IIC底层由C++语言编写,因为一方面,IIC的底层由C语言编写过于繁琐
 *             一方面,我觉得这是一个很好的机会展示一下如何用C语言和C++混合编译.
 * @作者       刘力铭
 * @完成时间   2019-12
 */
 
#ifndef __MK60_MYIIC_H
#define __MK60_MYIIC_H

#ifdef __cplusplus  //注明C语言和C++混编
extern "C" {
#endif

#include "headfile.h"

/***************** 重要宏定义 *****************/
#define NOP()   for(int tempdely = 0; tempdely < 2; ++tempdely);//增加IIC通讯的延时时间，提高稳定性

//为方便移植,可以直接更改宏定义实现相同功能
#define MYIIC_GPIO_SET_BITS(x)    gpio_set(x, 1)       //将引脚输出高电平
#define MYIIC_GPIO_RESET_BITS(x)  gpio_set(x, 0)       //将引脚输出低电平
#define MYIIC_GPIO_SET_OUT(x)     gpio_init(x, GPO, 0) //将引脚设置为输出模式,默认输出低电平
#define MYIIC_GPIO_SET_IN(x)      gpio_init(x, GPI, 1) //将引脚设置为输入模式,默认上拉
#define MYIIC_GPIO_READ(x)        gpio_get(x)          //读取引脚输入电平

/****************** C语言风格可调用函数 ******************/
void IIC_Init(PTX_n sdaPin, PTX_n sckPin);
uint8 IIC_WriteData(PTX_n sdaPin, PTX_n sckPin, uint8 slaveAddress, uint8 registerAddress, uint8 data);
uint8 IIC_WriteBuffer(PTX_n sdaPin, PTX_n sckPin, uint8 slaveAddress, uint8 registerAddress, uint8 len, uint8 *buffer);
uint8 IIC_ReadData(PTX_n sdaPin, PTX_n sckPin, uint8 slaveAddress, uint8 registerAddress, uint8 *data);
uint8 IIC_ReadBuffer(PTX_n sdaPin, PTX_n sckPin, uint8 slaveAddress, uint8 registerAddress, uint8 len, uint8 *buffer);

#ifdef __cplusplus  //注明C语言和C++混编
}
#endif

#endif
