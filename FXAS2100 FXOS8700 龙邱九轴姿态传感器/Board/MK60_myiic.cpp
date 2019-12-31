/*!
 * @文件       myiic.c
 * @功能       模拟IIC函数实现,整个IIC底层由C++语言编写,因为一方面,IIC的底层由C语言编写过于繁琐
 *             一方面,我觉得这是一个很好的机会展示一下如何用C语言和C++混合编译,底层提供了C++的
 *             函数调用接口和C语言的调用接口,自行选择.
 * @作者       刘力铭
 * @完成时间   2019-12
 */
 
#include "MK60_myiic.h"

class IIC_Controller
{
public:/****************** C++风格可调用函数 ******************/

	//----------------------------------------------------------
	//  @描述       类初始化函数,用来初始化sdaPin和sckPin
	//  @参数       sda                 IIC通讯的数据引脚
	//  @参数       scl                 IIC通讯的时钟引脚
	//  @参数       slaveAddress        从机地址
	//----------------------------------------------------------
	IIC_Controller(PTX_n sda, PTX_n sck, uint8 addr)
	{
		sdaPin = sda;
		sckPin = sck;
		slaveAddress = addr;
	}

	//----------------------------------------------------------
	//  @描述       模拟IIC初始化,即初始化两个GPIO引脚
	//----------------------------------------------------------
	void init(void)
	{
		gpio_init(sdaPin, GPO, 1);
		gpio_init(sckPin, GPO, 1);
	}
	
	//----------------------------------------------------------
	//  @描述       模拟IIC发送寄存器数据
	//  @参数       registerAddress     寄存器地址
	//  @参数       data                写入的数据
	//  @返回       0 = 成功;1 = 失败
	//----------------------------------------------------------
	uint8 writeData(uint8 registerAddress, uint8 data)
	{
		start();
		if(sendByte(slaveAddress << 1 | 0))return 1;
		if(sendByte(registerAddress))return 1;
		if(sendByte(data))return 1;
		stop();
		return 0;
	}
	
	//----------------------------------------------------------
	//  @描述       模拟IIC连续发送寄存器数据
	//  @参数       registerAddress     寄存器地址
	//  @参数       len                 连续发送的次数
	//  @参数       buffer              发送的数据地址
	//  @返回       0 = 成功;1 = 失败
	//----------------------------------------------------------
	uint8 writeBuffer(uint8 registerAddress, uint8 len, uint8 *buffer)
	{
		uint8 temp;
		
		start();
		if(sendByte(slaveAddress << 1 | 0))return 1;
		if(sendByte(registerAddress))return 1;
		for(temp = 0; temp < len; ++temp)
		{
			if(sendByte(buffer[temp]))return 1;
		}
		stop();
		return 0;
	}
	
	//----------------------------------------------------------
	//  @描述       模拟IIC读取寄存器数据
	//  @参数       registerAddress     寄存器地址
	//  @参数       data                存储读取值的地址
	//  @返回       0 = 成功;1 = 失败
	//----------------------------------------------------------
	uint8 readData(uint8 registerAddress, uint8 *data)
	{
		start();
		if(sendByte(slaveAddress << 1 | 0)) return 1;
		if(sendByte(registerAddress)) return 1;
		start();
		if(sendByte(slaveAddress << 1 | 1)) return 1;
		*data = readByte(0);
		stop();
		return 0;
	}
	
	//----------------------------------------------------------
	//  @描述       模拟IIC连续读取寄存器数据
	//  @参数       registerAddress     寄存器地址
	//  @参数       len                 存储读取值的地址
	//  @参数       buffer              储存数据的地址
	//  @返回       0 = 成功;1 = 失败
	//----------------------------------------------------------
	uint8 readBuffer(uint8 registerAddress, uint8 len, uint8 *buffer)
	{
		start();
		if(sendByte(slaveAddress << 1 | 0))return 1;
		if(sendByte(registerAddress))return 1;
		start();
		if(sendByte(slaveAddress << 1 | 1)) return 1;
		while(len)
		{
			(len == 1) ? ((*buffer) = readByte(0)) : ((*buffer) = readByte(1));
			--len;
			++buffer;
		}
		stop();
		return 0;
	}


private:/*********************** 内部函数 ***********************/

	//----------------------------------------------------------
	//  @描述       模拟IIC发送起始信号
	//----------------------------------------------------------
	void start(void)
	{
		MYIIC_GPIO_SET_OUT(sdaPin);
		
		MYIIC_GPIO_SET_BITS(sdaPin);
		MYIIC_GPIO_SET_BITS(sckPin);
		NOP();
		MYIIC_GPIO_RESET_BITS(sdaPin);
		NOP();
		MYIIC_GPIO_RESET_BITS(sckPin);
	}
	
	//----------------------------------------------------------
	//  @描述       模拟IIC发送终止信号
	//----------------------------------------------------------
	void stop(void)
	{
		MYIIC_GPIO_SET_OUT(sdaPin);
		
		MYIIC_GPIO_RESET_BITS(sckPin);
		MYIIC_GPIO_RESET_BITS(sdaPin);
		NOP();
		MYIIC_GPIO_SET_BITS(sckPin);
		NOP();
		MYIIC_GPIO_SET_BITS(sdaPin);
		NOP();
	}
	
	//----------------------------------------------------------
	//  @描述       模拟IIC检查从机应答信号
	//  @返回       0 = 成功;1 = 失败
	//----------------------------------------------------------
	uint8 checkAsk(void)
	{
		uint8 ErrTime = 0;
		MYIIC_GPIO_SET_IN(sdaPin);
		
	//	MYIIC_GPIO_SET_BITS(sdaPin);
	//	NOP();
		MYIIC_GPIO_SET_BITS(sckPin);
		NOP();
		while(MYIIC_GPIO_READ(sdaPin))
		{
			ErrTime++;
			if(ErrTime > 254)
			{
				stop();
				return 1;
			}
		}
		MYIIC_GPIO_RESET_BITS(sckPin);
		NOP();
		return 0;
	}
	
	//----------------------------------------------------------
	//  @描述       模拟IIC发送应答信号
	//  @参数       NewState        SET = 应答,RESET = 非应答
	//----------------------------------------------------------
	void sendAsk(char NewState)
	{
		MYIIC_GPIO_RESET_BITS(sckPin);
		MYIIC_GPIO_SET_OUT(sdaPin);
		
		(NewState == 1)? MYIIC_GPIO_RESET_BITS(sdaPin): MYIIC_GPIO_SET_BITS(sdaPin);
		NOP();
		MYIIC_GPIO_SET_BITS(sckPin);
		NOP();
		MYIIC_GPIO_RESET_BITS(sckPin);
		NOP();
	}
	
	//----------------------------------------------------------
	//  @描述       模拟IIC发送一个字节
	//  @参数       data            发送的字节
	//  @返回       0 = 成功;1 = 失败
	//----------------------------------------------------------
	uint8 sendByte(uint8 data)
	{
		uint8 temp;
		MYIIC_GPIO_SET_OUT(sdaPin);
		MYIIC_GPIO_RESET_BITS(sckPin);
		for(temp = 0; temp < 8; temp++)
		{
			(data & 0x80)? MYIIC_GPIO_SET_BITS(sdaPin): MYIIC_GPIO_RESET_BITS(sdaPin);
			data <<= 1;
			NOP();
			MYIIC_GPIO_SET_BITS(sckPin);
			NOP();
			MYIIC_GPIO_RESET_BITS(sckPin);
			NOP();
		}
		if(checkAsk())
			return 1;
		return 0;
	}
	
	//----------------------------------------------------------
	//  @描述       模拟IIC接收一个字节
	//  @参数       NewState        1 = 应答信号,0 = 非应答信号
	//  @返回       读出来的字节
	//----------------------------------------------------------
	uint8 readByte(char NewState)
	{
		uint8 temp = 0,data = 0;
		MYIIC_GPIO_SET_IN(sdaPin);
		
		for(temp = 0; temp < 8; ++temp)
		{
			MYIIC_GPIO_RESET_BITS(sckPin);
			NOP();
			MYIIC_GPIO_SET_BITS(sckPin);
			data <<= 1;
			if(MYIIC_GPIO_READ(sdaPin))
				data++;
			NOP();
		}
		(NewState == 1)? sendAsk(1): sendAsk(0);
		return data;
	}

	//因为每一个IIC芯片的数据引脚、时钟引脚和从机地址是由硬件决定的,不会在编程的过程中发生改变
	PTX_n sdaPin;
	PTX_n sckPin;
	uint8 slaveAddress;
};

//----------------------------------------------------------
//  @描述       模拟IIC初始化,即初始化两个GPIO引脚
//  @参数       sda       IIC通讯的数据引脚
//  @参数       scl       IIC通讯的时钟引脚
//----------------------------------------------------------
void IIC_Init(PTX_n sdaPin, PTX_n sckPin)
{
	IIC_Controller controller(sdaPin, sckPin, 0);
	controller.init();
}

//----------------------------------------------------------
//  @描述       模拟IIC发送寄存器数据
//  @参数       sda                 IIC通讯的数据引脚
//  @参数       scl                 IIC通讯的时钟引脚
//  @参数       slaveAddress        从机地址
//  @参数       registerAddress     寄存器地址
//  @参数       data                写入的数据
//  @返回       0 = 成功;1 = 失败
//----------------------------------------------------------
uint8 IIC_WriteData(PTX_n sdaPin, PTX_n sckPin, uint8 slaveAddress, uint8 registerAddress, uint8 data)
{
	IIC_Controller controller(sdaPin, sckPin, slaveAddress);
	return controller.writeData(registerAddress, data);
}

//----------------------------------------------------------
//  @描述       模拟IIC连续发送寄存器数据
//  @参数       sda                 IIC通讯的数据引脚
//  @参数       scl                 IIC通讯的时钟引脚
//  @参数       slaveAddress        从机地址
//  @参数       registerAddress     寄存器地址
//  @参数       len                 连续发送的次数
//  @参数       buffer              发送的数据地址
//  @返回       0 = 成功;1 = 失败
//----------------------------------------------------------
uint8 IIC_WriteBuffer(PTX_n sdaPin, PTX_n sckPin, uint8 slaveAddress, uint8 registerAddress, uint8 len, uint8 *buffer)
{
	IIC_Controller controller(sdaPin, sckPin, slaveAddress);
	return controller.writeBuffer(registerAddress, len, buffer);
}

//----------------------------------------------------------
//  @描述       模拟IIC读取寄存器数据
//  @参数       sda                 IIC通讯的数据引脚
//  @参数       scl                 IIC通讯的时钟引脚
//  @参数       slaveAddress        从机地址
//  @参数       registerAddress     寄存器地址
//  @参数       data                存储读取值的地址
//  @返回       0 = 成功;1 = 失败
//----------------------------------------------------------
uint8 IIC_ReadData(PTX_n sdaPin, PTX_n sckPin, uint8 slaveAddress, uint8 registerAddress, uint8 *data)
{
	IIC_Controller controller(sdaPin, sckPin, slaveAddress);
	return controller.readData(registerAddress, data);
}

//----------------------------------------------------------
//  @描述       模拟IIC连续读取寄存器数据
//  @参数       sda                 IIC通讯的数据引脚
//  @参数       scl                 IIC通讯的时钟引脚
//  @参数       slaveAddress        从机地址
//  @参数       registerAddress     寄存器地址
//  @参数       len                 存储读取值的地址
//  @参数       buffer              储存数据的地址
//  @返回       0 = 成功;1 = 失败
//----------------------------------------------------------
uint8 IIC_ReadBuffer(PTX_n sdaPin, PTX_n sckPin, uint8 slaveAddress, uint8 registerAddress, uint8 len, uint8 *buffer)
{
	IIC_Controller controller(sdaPin, sckPin, slaveAddress);
	return controller.readBuffer(registerAddress, len, buffer);
}
