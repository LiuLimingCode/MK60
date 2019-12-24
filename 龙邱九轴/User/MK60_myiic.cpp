/*!
 * @文件       myiic.c
 * @功能       模拟IIC函数实现
 * @作者       刘力铭
 * @完成时间   2019-12
 */
 
#include "MK60_myiic.h"

// 其实整个IIC的底层包括所有的IIC器件,我都很想做成C++的形式,因为用C语言写很蠢
// 但是考虑到以后的移植问题就放弃了.但是这部分的程序我实在忍不了了
// Myiic_Base这个C++类只用到了最简单的C++语法,包括类初始化函数和类成员函数,
// 成员函数完全可以当成普通的C语言函数来看,只不过他们都共用sdaPin和sclPin变量
// 这个类可以用C语言的函数实现一样的功能,但没有必要.
class Myiic_Base
{
public:
	PTX_n sdaPin;
	PTX_n sclPin;

//----------------------------------------------------------
//  @描述       类初始化函数,用来初始化sdaPin和sclPin
//----------------------------------------------------------
	Myiic_Base(PTX_n sda, PTX_n scl)
	{
		sdaPin = sda;
		sclPin = scl;
	}

//----------------------------------------------------------
//  @描述       模拟IIC发送起始信号
//----------------------------------------------------------
	void IIC_Start(void)
	{
		MYIIC_GPIO_SET_OUT(sdaPin);
		
		MYIIC_GPIO_SET_BITS(sdaPin);
		MYIIC_GPIO_SET_BITS(sclPin);
		NOP();
		MYIIC_GPIO_RESET_BITS(sdaPin);
		NOP();
		MYIIC_GPIO_RESET_BITS(sclPin);
	}
	
//----------------------------------------------------------
//  @描述       模拟IIC发送终止信号
//----------------------------------------------------------
	void IIC_Stop(void)
	{
		MYIIC_GPIO_SET_OUT(sdaPin);
		
		MYIIC_GPIO_RESET_BITS(sclPin);
		MYIIC_GPIO_RESET_BITS(sdaPin);
		NOP();
		MYIIC_GPIO_SET_BITS(sclPin);
		NOP();
		MYIIC_GPIO_SET_BITS(sdaPin);
		NOP();
	}
	
//----------------------------------------------------------
//  @描述       模拟IIC检查从机应答信号
//  @返回       0 = 成功;1 = 失败
//----------------------------------------------------------
	uint8_t IIC_ChackAsk(void)
	{
		uint8_t ErrTime = 0;
		MYIIC_GPIO_SET_IN(sdaPin);
		
	//	MYIIC_GPIO_SET_BITS(sdaPin);
	//	NOP();
		MYIIC_GPIO_SET_BITS(sclPin);
		NOP();
		while(MYIIC_GPIO_READ(sdaPin))
		{
			ErrTime++;
			if(ErrTime > 254)
			{
				IIC_Stop();
				return 1;
			}
		}
		MYIIC_GPIO_RESET_BITS(sclPin);
		NOP();
		return 0;
	}
	
//----------------------------------------------------------
//  @描述       模拟IIC发送应答信号
//  @参数       NewState        SET = 应答,RESET = 非应答
//----------------------------------------------------------
	void IIC_SendAsk(char NewState)
	{
		MYIIC_GPIO_RESET_BITS(sclPin);
		MYIIC_GPIO_SET_OUT(sdaPin);
		
		(NewState == 1)? MYIIC_GPIO_RESET_BITS(sdaPin): MYIIC_GPIO_SET_BITS(sdaPin);
		NOP();
		MYIIC_GPIO_SET_BITS(sclPin);
		NOP();
		MYIIC_GPIO_RESET_BITS(sclPin);
		NOP();
	}
	
//----------------------------------------------------------
//  @描述       模拟IIC发送一个字节
//  @参数       data            发送的字节
//  @返回       0 = 成功;1 = 失败
//----------------------------------------------------------
	uint8_t IIC_SendByte(uint8_t data)
	{
		uint8_t temp;
		MYIIC_GPIO_SET_OUT(sdaPin);
		MYIIC_GPIO_RESET_BITS(sclPin);
		for(temp = 0; temp < 8; temp++)
		{
			(data & 0x80)? MYIIC_GPIO_SET_BITS(sdaPin): MYIIC_GPIO_RESET_BITS(sdaPin);
			data <<= 1;
			NOP();
			MYIIC_GPIO_SET_BITS(sclPin);
			NOP();
			MYIIC_GPIO_RESET_BITS(sclPin);
			NOP();
		}
		if(IIC_ChackAsk())
			return 1;
		return 0;
	}
	
//----------------------------------------------------------
//  @描述       模拟IIC接收一个字节
//  @参数       NewState        1 = 应答信号,0 = 非应答信号
//  @返回       读出来的字节
//----------------------------------------------------------
	uint8_t IIC_ReadByte(char NewState)
	{
		uint8_t temp = 0,data = 0;
		MYIIC_GPIO_SET_IN(sdaPin);
		
		for(temp = 0; temp < 8; ++temp)
		{
			MYIIC_GPIO_RESET_BITS(sclPin);
			NOP();
			MYIIC_GPIO_SET_BITS(sclPin);
			data <<= 1;
			if(MYIIC_GPIO_READ(sdaPin))
				data++;
			NOP();
		}
		(NewState == 1)? IIC_SendAsk(1): IIC_SendAsk(0);
		return data;
	}
};


//----------------------------------------------------------
//  @描述       模拟IIC初始化,即初始化两个GPIO引脚
//  @参数       sdaPin          sda数据信号引脚号
//  @参数       sclPin          scl时钟信号引脚号
//----------------------------------------------------------
void IIC_Init(PTX_n sdaPin, PTX_n sclPin)
{
	gpio_init(sdaPin, GPO, 1);
	gpio_init(sclPin, GPO, 1);
}

//----------------------------------------------------------
//  @描述       模拟IIC发送寄存器数据
//  @参数       sdaPin          sda数据信号引脚号
//  @参数       sclPin          scl时钟信号引脚号
//  @参数       SlaveAddress    从机地址
//  @参数       REG_Address     寄存器地址
//  @参数       REG_data        写入的数据
//  @返回       0 = 成功;1 = 失败
//----------------------------------------------------------
uint8_t IIC_WriteRegister(PTX_n sdaPin, PTX_n sclPin, uint8_t SlaveAddress, uint8_t REG_Address, uint8_t REG_data)
{
	Myiic_Base base(sdaPin, sclPin);
	base.IIC_Start();
	if(base.IIC_SendByte(SlaveAddress << 1 | 0))return 1;
	if(base.IIC_SendByte(REG_Address))return 1;
	if(base.IIC_SendByte(REG_data))return 1;
	base.IIC_Stop();
	return 0;
}

//----------------------------------------------------------
//  @描述       模拟IIC连续发送寄存器数据
//  @参数       sdaPin          sda数据信号引脚号
//  @参数       sclPin          scl时钟信号引脚号
//  @参数       SlaveAddress    从机地址
//  @参数       REG_Address     寄存器地址
//  @参数       len             连续发送的次数
//  @参数       buf             发送的数据地址
//  @返回       0 = 成功;1 = 失败
//----------------------------------------------------------
uint8_t IIC_WriteRegisterLen(PTX_n sdaPin, PTX_n sclPin, uint8_t SlaveAddress, uint8_t REG_Address, uint8_t len, uint8_t *buf)
{
	uint8_t temp;
	Myiic_Base base(sdaPin, sclPin);
	base.IIC_Start();
	if(base.IIC_SendByte(SlaveAddress << 1 | 0))return 1;
	if(base.IIC_SendByte(REG_Address))return 1;
	for(temp = 0; temp < len; ++temp)
	{
		if(base.IIC_SendByte(buf[temp]))return 1;
	}
	base.IIC_Stop();
	return 0;
}

//----------------------------------------------------------
//  @描述       模拟IIC读取寄存器数据
//  @参数       sdaPin          sda数据信号引脚号
//  @参数       sclPin          scl时钟信号引脚号
//  @参数       SlaveAddress    从机地址
//  @参数       REG_Address     寄存器地址
//  @参数       data            存储读取值的地址
//  @返回       0 = 成功;1 = 失败
//----------------------------------------------------------
uint8_t IIC_ReadRegister(PTX_n sdaPin, PTX_n sclPin, uint8_t SlaveAddress, uint8_t REG_Address, uint8_t *data)
{
	Myiic_Base base(sdaPin, sclPin);
	base.IIC_Start();
	if(base.IIC_SendByte(SlaveAddress << 1 | 0)) return 1;
	if(base.IIC_SendByte(REG_Address)) return 1;
	base.IIC_Start();
	if(base.IIC_SendByte(SlaveAddress << 1 | 1)) return 1;
	*data = base.IIC_ReadByte(0);
	base.IIC_Stop();
	return 0;
}

//----------------------------------------------------------
//  @描述       模拟IIC连续读取寄存器数据
//  @参数       sdaPin          sda数据信号引脚号
//  @参数       sclPin          scl时钟信号引脚号
//  @参数       SlaveAddress    从机地址
//  @参数       REG_Address     寄存器地址
//  @参数       len             存储读取值的地址
//  @参数       buf             储存数据的地址
//  @返回       0 = 成功;1 = 失败
//----------------------------------------------------------
uint8_t IIC_ReadRegisterLen(PTX_n sdaPin, PTX_n sclPin, uint8_t SlaveAddress, uint8_t REG_Address, uint8_t len, uint8_t *buf)
{
	Myiic_Base base(sdaPin, sclPin);
	base.IIC_Start();
	if(base.IIC_SendByte(SlaveAddress << 1 | 0))return 1;
	if(base.IIC_SendByte(REG_Address))return 1;
	base.IIC_Start();
	if(base.IIC_SendByte(SlaveAddress << 1 | 1)) return 1;
	while(len)
	{
		(len == 1) ? ((*buf) = base.IIC_ReadByte(0)) : ((*buf) = base.IIC_ReadByte(1));
		--len;
		++buf;
	}
	base.IIC_Stop();
	return 0;
}

