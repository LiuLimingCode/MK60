/*!
 * @文件       myspi.c
 * @功能       硬件SPI和软件SPI统一实现
 * @作者       刘力铭
 * @完成时间   2019-12
 */

#include "MK60_myspi.h"

//----------------------------------------------------------------------
//  @描述       硬件SPI初始化
//  @参数       spi       需要使用的spi模块号
//  @参数       baud      传输波特率,即spi传输速度
//  @注意       移植直接更改函数内容以到达实现相同功能即可,
//              若不打算移植,直接把函数内容清空,工程内不要调用即可
//----------------------------------------------------------------------
void SPI_InitHardware(SPIn_e spi, uint32 baud)
{
	(void)spi_init(spi, NOT_PCS, MASTER, baud);
}

//----------------------------------------------------------------------
//  @描述       硬件SPI发送一格字节的数据
//  @参数       spi       需要使用的spi模块号
//  @参数       data      需要发送的数据
//  @注意       移植直接更改函数内容以到达实现相同功能即可,
//              若不打算移植,直接把函数内容清空,工程内不要调用即可
//----------------------------------------------------------------------
void SPI_SendDataHardware(SPIn_e spi, uint8 data)
{
	while((SPIN[spi]->SR & SPI_SR_TCF_MASK) == 1){} //等待传输完成
	SPIN[spi]->SR = SPI_SR_TCF_MASK;
	SPIN[spi]->PUSHR = (0
						| SPI_PUSHR_CTAS(0)
						| SPI_PUSHR_CONT_MASK         
						| SPI_PUSHR_TXDATA(data));
}

//----------------------------------------------------------------------
//  @描述       软件SPI初始化,即初始化两个GPIO引脚
//  @参数       sdaPin    sda数据信号引脚号
//  @参数       sclPin    scl时钟信号引脚号
//  @注意       移植直接更改函数内容以到达实现相同功能即可
//----------------------------------------------------------------------
void SPI_InitSimulated(PTX_n sdaPin, PTX_n sclPin)
{
	gpio_init(sclPin, GPO, 1);
	gpio_init(sdaPin, GPO, 1);
}

//----------------------------------------------------------------------
//  @描述       软件SPI发送一格字节的数据,即设置GPIO电平变换来模拟SPI时序
//  @参数       sdaPin    sda数据信号引脚号
//  @参数       sclPin    scl时钟信号引脚号
//  @参数       data      需要发送的数据
//  @注意       移植直接更改函数内容以到达实现相同功能即可
//----------------------------------------------------------------------
void SPI_SendDataSimulated(PTX_n sdaPin, PTX_n sclPin, uint8 data)
{
#define MYSPI_GPIO_SET_BITS(x)    gpio_set(x ,1) //为方便移植,可以直接更改宏定义实现相同功能
#define MYSPI_GPIO_RESET_BITS(x)  gpio_set(x, 0)
	uint8_t temp = 8;
	
	MYSPI_GPIO_SET_BITS(sclPin);
	while (temp--)
	{
		if (data & 0x80) MYSPI_GPIO_SET_BITS(sdaPin);
		else MYSPI_GPIO_RESET_BITS(sdaPin); 
		MYSPI_GPIO_SET_BITS(sclPin);
		MYSPI_GPIO_RESET_BITS(sclPin);
		data <<= 1;
	}
}
