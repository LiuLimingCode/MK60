/*!
 * @文件       myspi.h
 * @功能       硬件SPI和软件SPI统一实现
 * @作者       刘力铭
 * @完成时间   2019-12
 */

#ifndef __MK60_MYSPI_H
#define __MK60_MYSPI_H

#include "headfile.h"

/****************** 硬件SPI和软件SPI介绍 ******************
##SPI是通讯协议,本质上是两个引脚(SDA数据引脚,SCL时钟引脚)以特
  定的方式改变电平信号,从而传输不同的数据.
##硬件SPI使用单片机内部的SPI模块传输数据
  优点：传输速度快,几乎不占用CPU
  缺点：单片机必须有SPI模块,并且编程设计到芯片的寄存器,不同型号
        的单片机编程方法不相同,不方便底层移植.在硬件设计的时候
        必须使用单片机中具有SPI功能的指定引脚.
##软件SPI使用引脚的高低电平变化功能(GPIO)模拟出SPI的时序
  缺点：传输速度慢,在传输过程中需要CPU控制,占用CPU
  优点：由于大部分单片机都有GPIO功能,底层移植简单,在硬件设计的
        时候可以使用单片机的任意的引脚.
*********************************************************/

void SPI_InitHardware(SPIn_e spi, uint32 baud);
void SPI_SendDataHardware(SPIn_e spi, uint8 data);
void SPI_InitSimulated(PTX_n sdaPin, PTX_n sclPin);
void SPI_SendDataSimulated(PTX_n sdaPin, PTX_n sclPin, uint8 data);

#endif
