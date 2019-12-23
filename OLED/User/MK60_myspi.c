#include "MK60_myspi.h"

void SPI_InitHardware(SPIn_e spi, uint32 baud)
{
	(void)spi_init(spi, NOT_PCS, MASTER, baud);
}

void SPI_SendDataHardware(uint8 data, SPIn_e spi)
{
	while((SPIN[spi]->SR & SPI_SR_TCF_MASK) == 1);//等待传输完成
  SPIN[spi]->SR = SPI_SR_TCF_MASK;
  SPIN[spi]->PUSHR = (0
													| SPI_PUSHR_CTAS(0)          //选择CTAR0寄存器
													| SPI_PUSHR_CONT_MASK         
													| SPI_PUSHR_TXDATA(data));
}

void SPI_InitSimulated(PTX_n sdaPin, PTX_n sclPin)
{
	gpio_init(sclPin, GPO, 1);
  gpio_init(sdaPin, GPO, 1);
}

#define MYSPI_GPIO_SET_BITS(x)    gpio_set(x ,1)
#define MYSPI_GPIO_RESET_BITS(x)  gpio_set(x, 0)

void SPI_SendDataSimulated(PTX_n sdaPin, PTX_n sclPin, uint8 data)
{
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
