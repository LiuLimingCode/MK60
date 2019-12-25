/*!
 * @�ļ�       myiic.c
 * @����       ģ��IIC����ʵ��,����IIC�ײ���C++���Ա�д,��Ϊһ����,IIC�ĵײ���C���Ա�д���ڷ���
 *             һ����,�Ҿ�������һ���ܺõĻ���չʾһ�������C���Ժ�C++��ϱ���,�ײ��ṩ��C++��
 *             �������ýӿں�C���Եĵ��ýӿ�,����ѡ��.
 * @����       ������
 * @���ʱ��   2019-12
 */
 
#include "MK60_myiic.h"

class IIC_Controller
{
public:/****************** C++���ɵ��ú��� ******************/

	//----------------------------------------------------------
	//  @����       ���ʼ������,������ʼ��sdaPin��sckPin
	//  @����       sda                 IICͨѶ����������
	//  @����       scl                 IICͨѶ��ʱ������
	//  @����       slaveAddress        �ӻ���ַ
	//----------------------------------------------------------
	IIC_Controller(PTX_n sda, PTX_n sck, uint8 addr)
	{
		sdaPin = sda;
		sckPin = sck;
		slaveAddress = addr;
	}

	//----------------------------------------------------------
	//  @����       ģ��IIC��ʼ��,����ʼ������GPIO����
	//----------------------------------------------------------
	void init(void)
	{
		gpio_init(sdaPin, GPO, 1);
		gpio_init(sckPin, GPO, 1);
	}
	
	//----------------------------------------------------------
	//  @����       ģ��IIC���ͼĴ�������
	//  @����       registerAddress     �Ĵ�����ַ
	//  @����       data                д�������
	//  @����       0 = �ɹ�;1 = ʧ��
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
	//  @����       ģ��IIC�������ͼĴ�������
	//  @����       registerAddress     �Ĵ�����ַ
	//  @����       len                 �������͵Ĵ���
	//  @����       buffer              ���͵����ݵ�ַ
	//  @����       0 = �ɹ�;1 = ʧ��
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
	//  @����       ģ��IIC��ȡ�Ĵ�������
	//  @����       registerAddress     �Ĵ�����ַ
	//  @����       data                �洢��ȡֵ�ĵ�ַ
	//  @����       0 = �ɹ�;1 = ʧ��
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
	//  @����       ģ��IIC������ȡ�Ĵ�������
	//  @����       registerAddress     �Ĵ�����ַ
	//  @����       len                 �洢��ȡֵ�ĵ�ַ
	//  @����       buffer              �������ݵĵ�ַ
	//  @����       0 = �ɹ�;1 = ʧ��
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


private:/*********************** �ڲ����� ***********************/

	//----------------------------------------------------------
	//  @����       ģ��IIC������ʼ�ź�
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
	//  @����       ģ��IIC������ֹ�ź�
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
	//  @����       ģ��IIC���ӻ�Ӧ���ź�
	//  @����       0 = �ɹ�;1 = ʧ��
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
	//  @����       ģ��IIC����Ӧ���ź�
	//  @����       NewState        SET = Ӧ��,RESET = ��Ӧ��
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
	//  @����       ģ��IIC����һ���ֽ�
	//  @����       data            ���͵��ֽ�
	//  @����       0 = �ɹ�;1 = ʧ��
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
	//  @����       ģ��IIC����һ���ֽ�
	//  @����       NewState        1 = Ӧ���ź�,0 = ��Ӧ���ź�
	//  @����       ���������ֽ�
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

	//��Ϊÿһ��IICоƬ���������š�ʱ�����źʹӻ���ַ����Ӳ��������,�����ڱ�̵Ĺ����з����ı�
	PTX_n sdaPin;
	PTX_n sckPin;
	uint8 slaveAddress;
};

//----------------------------------------------------------
//  @����       ģ��IIC��ʼ��,����ʼ������GPIO����
//  @����       sda       IICͨѶ����������
//  @����       scl       IICͨѶ��ʱ������
//----------------------------------------------------------
void IIC_Init(PTX_n sdaPin, PTX_n sckPin)
{
	IIC_Controller controller(sdaPin, sckPin, 0);
	controller.init();
}

//----------------------------------------------------------
//  @����       ģ��IIC���ͼĴ�������
//  @����       sda                 IICͨѶ����������
//  @����       scl                 IICͨѶ��ʱ������
//  @����       slaveAddress        �ӻ���ַ
//  @����       registerAddress     �Ĵ�����ַ
//  @����       data                д�������
//  @����       0 = �ɹ�;1 = ʧ��
//----------------------------------------------------------
uint8 IIC_WriteData(PTX_n sdaPin, PTX_n sckPin, uint8 slaveAddress, uint8 registerAddress, uint8 data)
{
	IIC_Controller controller(sdaPin, sckPin, slaveAddress);
	return controller.writeData(registerAddress, data);
}

//----------------------------------------------------------
//  @����       ģ��IIC�������ͼĴ�������
//  @����       sda                 IICͨѶ����������
//  @����       scl                 IICͨѶ��ʱ������
//  @����       slaveAddress        �ӻ���ַ
//  @����       registerAddress     �Ĵ�����ַ
//  @����       len                 �������͵Ĵ���
//  @����       buffer              ���͵����ݵ�ַ
//  @����       0 = �ɹ�;1 = ʧ��
//----------------------------------------------------------
uint8 IIC_WriteBuffer(PTX_n sdaPin, PTX_n sckPin, uint8 slaveAddress, uint8 registerAddress, uint8 len, uint8 *buffer)
{
	IIC_Controller controller(sdaPin, sckPin, slaveAddress);
	return controller.writeBuffer(registerAddress, len, buffer);
}

//----------------------------------------------------------
//  @����       ģ��IIC��ȡ�Ĵ�������
//  @����       sda                 IICͨѶ����������
//  @����       scl                 IICͨѶ��ʱ������
//  @����       slaveAddress        �ӻ���ַ
//  @����       registerAddress     �Ĵ�����ַ
//  @����       data                �洢��ȡֵ�ĵ�ַ
//  @����       0 = �ɹ�;1 = ʧ��
//----------------------------------------------------------
uint8 IIC_ReadData(PTX_n sdaPin, PTX_n sckPin, uint8 slaveAddress, uint8 registerAddress, uint8 *data)
{
	IIC_Controller controller(sdaPin, sckPin, slaveAddress);
	return controller.readData(registerAddress, data);
}

//----------------------------------------------------------
//  @����       ģ��IIC������ȡ�Ĵ�������
//  @����       sda                 IICͨѶ����������
//  @����       scl                 IICͨѶ��ʱ������
//  @����       slaveAddress        �ӻ���ַ
//  @����       registerAddress     �Ĵ�����ַ
//  @����       len                 �洢��ȡֵ�ĵ�ַ
//  @����       buffer              �������ݵĵ�ַ
//  @����       0 = �ɹ�;1 = ʧ��
//----------------------------------------------------------
uint8 IIC_ReadBuffer(PTX_n sdaPin, PTX_n sckPin, uint8 slaveAddress, uint8 registerAddress, uint8 len, uint8 *buffer)
{
	IIC_Controller controller(sdaPin, sckPin, slaveAddress);
	return controller.readBuffer(registerAddress, len, buffer);
}
