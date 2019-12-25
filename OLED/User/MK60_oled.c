/*!
 * @�ļ�       oled.c
 * @����       OLED����ʵ��
 * @����       ������
 * @���ʱ��   2019-12
 */

#include "MK60_oled.h"
#include "MK60_myspi.h"
#include "charcode.h"

static void OLED_WriteDat(uint8_t data);
static void OLED_WriteCmd(uint8_t cmd);
static void OLED_SetPos(uint8_t x, uint8_t y);
static void OLED_PinInit(void);

//----------------------------------------------------------------------------------------
//  @����       OLED��ʾ����������
//  @����       color       0x00Ϊȫ��,0xffΪȫ��
//  @ʾ��       OLED_ClearScreen(0x00);
//----------------------------------------------------------------------------------------
void OLED_ClearScreen(uint8_t color)
{
	uint16_t y, x;	
	for(y = 0; y < OLED_PAGE_MAX; ++y)
	{
		OLED_WriteCmd(0xb0 + y);
		OLED_WriteCmd(0x01);
		OLED_WriteCmd(0x10); 
		for( x= 0; x < OLED_X_MAX; ++x)
			OLED_WriteDat(color);
	}
}

//----------------------------------------------------------------------------------------
//  @����       OLED��ʼ��
//  @ʾ��       OLED_Init();
//----------------------------------------------------------------------------------------
void OLED_Init (void)
{
	OLED_PinInit();
	
	OLED_RSTL; //��ʼ��OLED
	
	for(int temp = 0; temp < 999999; ++temp){} //��ʱ

	OLED_RSTH;
	
	OLED_WriteCmd(0xAE);//--turn off oled panel
	OLED_WriteCmd(0x00);//---set low column address
	OLED_WriteCmd(0x10);//---set high column address
	OLED_WriteCmd(0x40);//--set start line address	Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WriteCmd(0x81);//--set contrast control register
	OLED_WriteCmd(0xcf); // Set SEG Output Current Brightness

#if (0 == OLED_DISPLAY_DIR)
	OLED_WriteCmd(0xa1);//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
	OLED_WriteCmd(0xc8);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
#else
    OLED_WriteCmd(0xa0);//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
	OLED_WriteCmd(0xc0);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
#endif
		
	OLED_WriteCmd(0xa6);//--set normal display
	OLED_WriteCmd(0xa8);//--set multiplex ratio(1 to 64)
	OLED_WriteCmd(0x3f);//--1/64 duty
	OLED_WriteCmd(0xd3);//-set display offset		Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WriteCmd(0x00);//-not offset
	OLED_WriteCmd(0xd5);//--set display clock divide ratio/oscillator frequency
	OLED_WriteCmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WriteCmd(0xd9);//--set pre-charge period
	OLED_WriteCmd(0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WriteCmd(0xda);//--set com pins hardware configuration
	OLED_WriteCmd(0x12);
	OLED_WriteCmd(0xdb);//--set vcomh
	OLED_WriteCmd(0x40);//Set VCOM Deselect Level
	OLED_WriteCmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WriteCmd(0x02);//
	OLED_WriteCmd(0x8d);//--set Charge Pump enable/disable
	OLED_WriteCmd(0x14);//--set(0x10) disable
	OLED_WriteCmd(0xa4);// Disable Entire Display On (0xa4/0xa5)
	OLED_WriteCmd(0xa6);// Disable Inverse Display On (0xa6/a7) 
	OLED_WriteCmd(0xAF);//--turn on oled panel
	OLED_ClearScreen(OLED_BLACK);
	OLED_SetPos(0, 0);
}

//----------------------------------------------------------------------------------------
//  @����       ��ʾһ��6x8���ַ�
//  @����       x       ��ʾλ�õĺ�����(0~127)
//  @����       y       ��ʾλ�õ�������(0~7)
//  @����       data1   ����ʾ���ַ�
//  @ʾ��       OLED_P6x8Char(0, 0, 'H');    //����Ļ(0,0)λ����ʾ H
//----------------------------------------------------------------------------------------
void OLED_P6x8Char(uint8_t x, uint8_t y, char ch)
{
	uint8_t c = 0, i = 0; 
	c = ch - 32;
	if(x> OLED_X_MAX - 6)
	{
		x = 0;
		++y;
	}
	OLED_SetPos(x, y);
	for(i = 0; i < 6; ++i)
	{
		OLED_WriteDat(F6x8[c][i]);	
	}
	x += 6;
}

//----------------------------------------------------------------------------------------
//  @����       ��ʾһ��6x8���ַ���
//              x       ��ʾλ�õĺ�����(0~127)
//              y       ��ʾλ�õ�������(0~7)
//              data1   ����ʾ���ַ���
//  @ʾ��       OLED_P6x8Str(0, 0, "HELLO");    //����Ļ(0,0)λ����ʾ HELLO
//----------------------------------------------------------------------------------------
void OLED_P6x8Str(uint8_t x, uint8_t y, const char *ch)
{
	uint8_t c = 0, i = 0, j = 0;
	while (ch[j] != '\0')
	{
		c = ch[j] - 32;
		if(x > OLED_X_MAX - 6)
		{
			x = 0;
			++y;
		}
		OLED_SetPos(x, y);
		for(i = 0; i < 6; ++i)
		{
			OLED_WriteDat(F6x8[c][i]);
		}
		x += 6;
		++j;
	}
}

//----------------------------------------------------------------------------------------
//  @����       ��ʾһ��6x8��������
//  @����       x       ��ʾλ�õĺ�����(0~127)
//  @����       y       ��ʾλ�õ�������(0~7)
//  @����       data1   ����ʾ������
//  @����       set     ��ʾ����λλ��,����ò���������,����ʾ����з���
//  @ʾ��       OLED_P6x8Int(0, 0, 123, 4);     //����Ļ(0,0)λ����ʾ 0123
//              OLED_P6x8Int(0, 0, -123, -3);   //����Ļ(0,0)λ����ʾ -123
//----------------------------------------------------------------------------------------
void OLED_P6x8Int(uint8_t x, uint8_t y, int16_t data1, int8 set)
{
	uint8_t result[7] = {0};
	
	if(set > 5) set = 5; //int16���������ʾ5λ
	if(set < -5) set = -5;
	
	if(data1 < 0)
	{
		data1 *= -1;
		result[0] = '-';
	}
	else
	{
		result[0] = '+';
	}
	
	result[1] = (uint8_t)((data1 % 100000) / 10000 + '0');
	result[2] = (uint8_t)((data1 % 10000) / 1000 + '0');
	result[3] = (uint8_t)((data1 % 1000) / 100 + '0');
	result[4] = (uint8_t)((data1 % 100) / 10 + '0');
	result[5] = (uint8_t)((data1 % 10) / 1 + '0');
	result[6] = '\0';
	
	if(set < 0)
	{
		result[5 + set] = result[0];
		OLED_P6x8Str(x, y, (char *)&(result[5 + set]));
	}
	else if(set > 0)
	{
		OLED_P6x8Str(x, y, (char *)&(result[6 - set]));
	}
}

//----------------------------------------------------------------------------------------
//  @����       ��ʾһ��6x8�ĸ�����
//  @����       x       ��ʾλ�õĺ�����(0~127)
//  @����       y       ��ʾλ�õ�������(0~7)
//  @����       data1   ����ʾ�ĸ�����
//  @����       set     ��ʾ������������λλ��,����ò���������,����ʾ����з���
//  @ʾ��       OLED_P6x8Flo(0, 0, 100.0/3, 3);    //����Ļ(0,0)λ����ʾ 033.33
//              OLED_P6x8Flo(0, 0, 100.0/3, -3);   //����Ļ(0,0)λ����ʾ +033.33
//----------------------------------------------------------------------------------------
void OLED_P6x8Flo(uint8_t x, uint8_t y, double data1, int8 set)
{
	uint8_t result[13] = {0};
	uint32_t data2;
	
	if(set > 8) set = 8; //����λ�����ʾ8λ
	if(set < -8) set = -8;
	
	if(data1 < 0)
	{			
		result[0] = '-';
		data1 *= -1;
	}
	else
	{
		result[0]='+';
	}
	
	data1 += 0.000001;
	data2 = (uint32_t)(data1 * 100);
	result[1] = (uint8_t)((data2 % 10000000000) / 1000000000 + '0');
	result[2] = (uint8_t)((data2 % 1000000000) / 100000000 + '0');
	result[3] = (uint8_t)((data2 % 100000000) / 10000000 + '0');
	result[4] = (uint8_t)((data2 % 10000000) / 1000000 + '0');
	result[5] = (uint8_t)((data2 % 1000000) / 100000 + '0');
	result[6] = (uint8_t)((data2 % 100000) / 10000 + '0');
	result[7] = (uint8_t)((data2 % 10000) / 1000 + '0');
	result[8] = (uint8_t)((data2 % 1000) / 100 + '0');
	result[9] = '.';
	result[10] = (uint8_t)((data2 % 100) / 10 + '0');
	result[11] = (uint8_t)((data2 % 10) / 1 + '0');
	result[12] = '\0';
	
	if(set < 0)
	{
		result[8 + set] = result[0];
		OLED_P6x8Str(x, y, (char *)&(result[8 + set]));
	}
	else if(set > 0)
	{
		OLED_P6x8Str(x, y, (char *)&(result[9 - set]));
	}
}

//----------------------------------------------------------------------------------------
//  @����       ��ʾһ��8x16�ı�׼ASCII�ַ�
//  @����       x       ��ʾλ�õĺ�����(0~127)
//  @����       y       ��ʾλ�õ�������(0~6)
//  @����       ch      ����ʾ���ַ�
//  @ʾ��       OLED_P8x16Char(0, 0, 'H');    //����Ļ(0,0)λ����ʾ H
//----------------------------------------------------------------------------------------
void OLED_P8x16Char(uint8_t x, uint8_t y, char ch)
{
	uint8_t c = 0, i = 0;
	
	c = ch - 32;
	if(x > OLED_X_MAX - 8)
	{
		x = 0;
		++y;
	}
	OLED_SetPos(x, y);
	for(i = 0; i < 8; ++i)
	{
		OLED_WriteDat(F8X16[c * 16 + i]);
	}
	OLED_SetPos(x, y + 1);
	for(i = 0; i < 8; ++i)
	{
		OLED_WriteDat(F8X16[c * 16 + i + 8]);
	}
	x += 8;
}

//----------------------------------------------------------------------------------------
//  @����       ��ʾһ��8x16�ı�׼ASCII�ַ���
//  @����       x       ��ʾλ�õĺ�����(0~127)
//  @����       y       ��ʾλ�õ�������(0~6)
//  @����       ch      ����ʾ���ַ���
//  @ʾ��       OLED_P8x16Str(0, 0, "HELLO"); //����Ļ(0,0)λ����ʾ HELLO
//----------------------------------------------------------------------------------------
void OLED_P8x16Str(uint8_t x, uint8_t y, const char *ch)
{
	uint8_t c = 0, i = 0, j = 0;
	while (ch[j] != '\0')
	{
		c = ch[j] - 32;
		if(x > OLED_X_MAX - 8)
		{
			x = 0;
			++y;
		}
		OLED_SetPos(x, y);
		for(i = 0; i < 8; ++i)
		{
			OLED_WriteDat(F8X16[c * 16 + i]);
		}
		OLED_SetPos(x, y + 1);
		for(i = 0; i < 8; ++i)
		{
			OLED_WriteDat(F8X16[c * 16 + i + 8]);
		}
		x += 8;
		++j;
	}
}

//----------------------------------------------------------------------------------------
//  @����       ��ʾһ��14x16�������ַ�
//  @����       x       ��ʾλ�õĺ�����(0~127)
//  @����       y       ��ʾλ�õ�������(0~6)
//  @����       ch      ����ʾ���ַ���
//  @ʾ��       OLED_P14x16CHCHAR(0, 0, "���");    //����Ļ(0,0)λ����ʾ ���
//  @ע��       ��ʾ����ʱ��Ҫ������ȡģ���ȡģ����,ȡģʱ����Ϊ �������,����ȡģ,�ֽڵ���,
//              �������charcode.h�е�Word14x16�����з���ʹ��
//----------------------------------------------------------------------------------------
void OLED_P14x16CHCHAR(uint8_t x, uint8_t y, const char *ch)
{
	uint8_t wordnum, i;
	while(*ch != '\0')
	{
		if(x > OLED_X_MAX - 14)
		{
			x = 0;
			++y;
		}
		OLED_SetPos(x, y);
		for(wordnum = 0; wordnum < sizeof(Word14x16) / 31; ++wordnum)
		{
			if(((*ch & 0xff) == (Word14x16[wordnum].word_name[0] & 0xff)) && ((*(ch + 1) & 0xff) == (Word14x16[wordnum].word_name[1] & 0xff)))
			{
				for(i = 0; i < 14; ++i)
				{
					OLED_WriteDat(Word14x16[wordnum].index[i]);
				}
				OLED_SetPos(x, y + 1);
				for(i = 14; i < 28; ++i)
				{
					OLED_WriteDat(Word14x16[wordnum].index[i]);
				}
				x += 14;
				ch += 2;
				break;
			}
		}
	}
}

//----------------------------------------------------------------------------------------
//  @����       ��ʾһ��ͼƬ
//  @����       x0      ��ʾλ�õ���������(0~127)
//  @����       y0      ��ʾλ�õ����������(0~7)
//  @����       x1      ��ʾλ�õ��պ�����(0~127)
//  @����       y1      ��ʾλ�õ��յ�������(0~7)
//  @����       bmp     ����ʾ��ͼƬ
//  @ʾ��       OLED_PrintBMP(0, 0, 100, 6, bmp); //����Ļ(0,0)λ����ʾһ����СΪ100*48��ͼƬ
//  @ע��       ��ʾͼƬʱ��Ҫ��ͼ��ȡģ���ȡģ,ȡģʱ����Ϊ ����ˮƽ,�ֽڴ�ֱ,��ɫ,
//              ���ߴ�128 * 64,�ֽ����������ݷ���
//----------------------------------------------------------------------------------------
void OLED_PrintBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const char bmp[])
{ 	
	uint16_t temp = 0;
	uint8_t x, y;
	for(y = y0; y <= y1; ++y)
	{
		OLED_SetPos(x0,y);				
		for(x = x0; x <= x1; ++x)
		{
			OLED_WriteDat(bmp[temp++]);
		}
	}
}

//----------------------------------------------------------------------------------------
//  @����       ��ʾһ����ֵ��ͼƬ
//  @����       size_x		ԭͼƬ��X��С
//  @����       size_y		ԭͼƬ��Y��С
//  @����       binary		����ʾ�Ķ�ֵ��ͼƬ
//  @ʾ��       OLED_PrintBinary(94, 60, Binary); //����Ļ(0,0)λ����ʾһ����СΪ96*60��ͼƬ
//----------------------------------------------------------------------------------------
void OLED_PrintBinary(uint16_t size_x, uint16_t size_y, uint8_t* binary)
{
	uint8_t data;
	for(int16_t index_y = 0; index_y < 8; ++index_y)
	{
		OLED_SetPos(0, index_y);
		for(int16_t index_x = 0; index_x < 128; ++index_x)
		{
			for(int8_t index = 7; index >= 0; --index)
			{
				data <<= 1;
				data |= (binary + ((index_y * 8 + index) * size_y) / 64 * size_x)[(index_x * size_x) / 128] & 0x01;
			}
			OLED_WriteDat(data);
		}
	}
}

/****************** ����Ϊ�ڲ�����,���ɵ��� ******************/

//��κ궨��������ǣ�����OLED_SPI_HARDWARE����,ΪOLED_SPI_INIT��OLED_SPI_SEND_DATA��ѡ��Ӧ����
#if OLED_SPI_HARDWARE == 0 //ʹ��ģ��SPI
#define OLED_SPI_INIT()       SPI_InitSimulated(OLED_SDA_Pin, OLED_SCL_Pin)
#define OLED_SPI_SEND_DATA(x) SPI_SendDataSimulated(OLED_SDA_Pin, OLED_SCL_Pin, x)
#else //ʹ��Ӳ��SPI
#define OLED_SPI_INIT()       SPI_InitHardware(OLED_SPI, bus_clk_mhz *1000000)
#define OLED_SPI_SEND_DATA(x) SPI_SendDataHardware(OLED_SPI, x)
#endif

static void OLED_PinInit(void) //��ʼ��
{
	OLED_SPI_INIT();
	gpio_init(OLED_RES_Pin, GPO, 1);
	gpio_init(OLED_DC_Pin, GPO, 1);
}

static void OLED_WriteDat(uint8_t data) //д����
{
#if OLED_SPI_USE_CS == 1
	OLED_CSL();
#endif
	
	OLED_DCH;
	OLED_SPI_SEND_DATA(data);
	
#if OLED_SPI_USE_CS == 1
	OLED_CSH();
#endif
}
static void OLED_WriteCmd(uint8_t cmd) //д����
{
#if OLED_SPI_USE_CS == 1
	OLED_CSL();
#endif
	
	OLED_DCL;
	OLED_SPI_SEND_DATA(cmd);
	
#if OLED_SPI_USE_CS == 1
	OLED_CSH();
#endif
}

static void OLED_SetPos(uint8_t x, uint8_t y) //��������
{ 
	OLED_WriteCmd(0xb0 + y);
	OLED_WriteCmd(((x & 0xf0) >> 4) | 0x10);
	OLED_WriteCmd((x & 0x0f) | 0x01); 
}
