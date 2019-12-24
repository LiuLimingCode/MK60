/*!
 * @�ļ�       myiic.c
 * @����       ģ��IIC����ʵ��
 * @����       ������
 * @���ʱ��   2019-12
 */
 
#include "MK60_myiic.h"

// ��ʵ����IIC�ĵײ�������е�IIC����,�Ҷ���������C++����ʽ,��Ϊ��C����д�ܴ�
// ���ǿ��ǵ��Ժ����ֲ����ͷ�����.�����ⲿ�ֵĳ�����ʵ���̲�����
// Myiic_Base���C++��ֻ�õ�����򵥵�C++�﷨,�������ʼ�����������Ա����,
// ��Ա������ȫ���Ե�����ͨ��C���Ժ�������,ֻ�������Ƕ�����sdaPin��sclPin����
// ����������C���Եĺ���ʵ��һ���Ĺ���,��û�б�Ҫ.
class Myiic_Base
{
public:
	PTX_n sdaPin;
	PTX_n sclPin;

//----------------------------------------------------------
//  @����       ���ʼ������,������ʼ��sdaPin��sclPin
//----------------------------------------------------------
	Myiic_Base(PTX_n sda, PTX_n scl)
	{
		sdaPin = sda;
		sclPin = scl;
	}

//----------------------------------------------------------
//  @����       ģ��IIC������ʼ�ź�
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
//  @����       ģ��IIC������ֹ�ź�
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
//  @����       ģ��IIC���ӻ�Ӧ���ź�
//  @����       0 = �ɹ�;1 = ʧ��
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
//  @����       ģ��IIC����Ӧ���ź�
//  @����       NewState        SET = Ӧ��,RESET = ��Ӧ��
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
//  @����       ģ��IIC����һ���ֽ�
//  @����       data            ���͵��ֽ�
//  @����       0 = �ɹ�;1 = ʧ��
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
//  @����       ģ��IIC����һ���ֽ�
//  @����       NewState        1 = Ӧ���ź�,0 = ��Ӧ���ź�
//  @����       ���������ֽ�
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
//  @����       ģ��IIC��ʼ��,����ʼ������GPIO����
//  @����       sdaPin          sda�����ź����ź�
//  @����       sclPin          sclʱ���ź����ź�
//----------------------------------------------------------
void IIC_Init(PTX_n sdaPin, PTX_n sclPin)
{
	gpio_init(sdaPin, GPO, 1);
	gpio_init(sclPin, GPO, 1);
}

//----------------------------------------------------------
//  @����       ģ��IIC���ͼĴ�������
//  @����       sdaPin          sda�����ź����ź�
//  @����       sclPin          sclʱ���ź����ź�
//  @����       SlaveAddress    �ӻ���ַ
//  @����       REG_Address     �Ĵ�����ַ
//  @����       REG_data        д�������
//  @����       0 = �ɹ�;1 = ʧ��
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
//  @����       ģ��IIC�������ͼĴ�������
//  @����       sdaPin          sda�����ź����ź�
//  @����       sclPin          sclʱ���ź����ź�
//  @����       SlaveAddress    �ӻ���ַ
//  @����       REG_Address     �Ĵ�����ַ
//  @����       len             �������͵Ĵ���
//  @����       buf             ���͵����ݵ�ַ
//  @����       0 = �ɹ�;1 = ʧ��
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
//  @����       ģ��IIC��ȡ�Ĵ�������
//  @����       sdaPin          sda�����ź����ź�
//  @����       sclPin          sclʱ���ź����ź�
//  @����       SlaveAddress    �ӻ���ַ
//  @����       REG_Address     �Ĵ�����ַ
//  @����       data            �洢��ȡֵ�ĵ�ַ
//  @����       0 = �ɹ�;1 = ʧ��
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
//  @����       ģ��IIC������ȡ�Ĵ�������
//  @����       sdaPin          sda�����ź����ź�
//  @����       sclPin          sclʱ���ź����ź�
//  @����       SlaveAddress    �ӻ���ַ
//  @����       REG_Address     �Ĵ�����ַ
//  @����       len             �洢��ȡֵ�ĵ�ַ
//  @����       buf             �������ݵĵ�ַ
//  @����       0 = �ɹ�;1 = ʧ��
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

