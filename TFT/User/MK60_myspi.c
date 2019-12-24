/*!
 * @�ļ�       myspi.c
 * @����       Ӳ��SPI�����SPIͳһʵ��
 * @����       ������
 * @���ʱ��   2019-12
 */

#include "MK60_myspi.h"

//----------------------------------------------------------------------
//  @����       Ӳ��SPI��ʼ��
//  @����       spi       ��Ҫʹ�õ�spiģ���
//  @����       baud      ���䲨����,��spi�����ٶ�
//  @ע��       ��ֱֲ�Ӹ��ĺ��������Ե���ʵ����ͬ���ܼ���,
//              ����������ֲ,ֱ�ӰѺ����������,�����ڲ�Ҫ���ü���
//----------------------------------------------------------------------
void SPI_InitHardware(SPIn_e spi, uint32 baud)
{
	(void)spi_init(spi, NOT_PCS, MASTER, baud);
}

//----------------------------------------------------------------------
//  @����       Ӳ��SPI����һ���ֽڵ�����
//  @����       spi       ��Ҫʹ�õ�spiģ���
//  @����       data      ��Ҫ���͵�����
//  @ע��       ��ֱֲ�Ӹ��ĺ��������Ե���ʵ����ͬ���ܼ���,
//              ����������ֲ,ֱ�ӰѺ����������,�����ڲ�Ҫ���ü���
//----------------------------------------------------------------------
void SPI_SendDataHardware(SPIn_e spi, uint8 data)
{
	while((SPIN[spi]->SR & SPI_SR_TCF_MASK) == 1){} //�ȴ��������
	SPIN[spi]->SR = SPI_SR_TCF_MASK;
	SPIN[spi]->PUSHR = (0
						| SPI_PUSHR_CTAS(0)
						| SPI_PUSHR_CONT_MASK         
						| SPI_PUSHR_TXDATA(data));
}

//----------------------------------------------------------------------
//  @����       ���SPI��ʼ��,����ʼ������GPIO����
//  @����       sdaPin    sda�����ź����ź�
//  @����       sclPin    sclʱ���ź����ź�
//  @ע��       ��ֱֲ�Ӹ��ĺ��������Ե���ʵ����ͬ���ܼ���
//----------------------------------------------------------------------
void SPI_InitSimulated(PTX_n sdaPin, PTX_n sclPin)
{
	gpio_init(sclPin, GPO, 1);
	gpio_init(sdaPin, GPO, 1);
}

//----------------------------------------------------------------------
//  @����       ���SPI����һ���ֽڵ�����,������GPIO��ƽ�任��ģ��SPIʱ��
//  @����       sdaPin    sda�����ź����ź�
//  @����       sclPin    sclʱ���ź����ź�
//  @����       data      ��Ҫ���͵�����
//  @ע��       ��ֱֲ�Ӹ��ĺ��������Ե���ʵ����ͬ���ܼ���
//----------------------------------------------------------------------
void SPI_SendDataSimulated(PTX_n sdaPin, PTX_n sclPin, uint8 data)
{
#define MYSPI_GPIO_SET_BITS(x)    gpio_set(x ,1) //Ϊ������ֲ,����ֱ�Ӹ��ĺ궨��ʵ����ͬ����
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
