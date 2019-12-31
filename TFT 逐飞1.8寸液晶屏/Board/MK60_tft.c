/*!
 * @�ļ�       tft.c
 * @����       ���1.8��TFT��(����ST7735S)����ʵ��(SPI)
 * @����       ������
 * @���ʱ��   2019-12
 */
 
#include "MK60_tft.h"
#include "TFT_code.h"

static void TFT_PinInit(void);
static void TFT_WriteCmd(uint8 cmd);
static void TFT_WriteDat8Bits(uint8 data);
static void TFT_WriteDat16Bits(uint16 data);
static void TFT_SetRegion(uint16 x_start, uint16 y_start, uint16 x_end, uint16 y_end);

//----------------------------------------------------------------------------------------
//  @����       TFT��ʾ����������
//  @����       color       ������ɫ,��ʽΪRGB454
//  @ʾ��       TFT_ClearScreen(GBP454_WHITE);
//----------------------------------------------------------------------------------------
void TFT_ClearScreen(uint16 color)
{
	uint8 i, j;
	TFT_SetRegion(0, 0, TFT_X_MAX - 1, TFT_Y_MAX - 1);
	for (i = 0; i < TFT_Y_MAX; ++i)
		for (j = 0; j < TFT_X_MAX; ++j)
			TFT_WriteDat16Bits(color);
}
   
//----------------------------------------------------------------------------------------
//  @����       TFT��ʼ��
//  @ʾ��       TFT_Init();
//----------------------------------------------------------------------------------------
void TFT_Init(void)
{	
	TFT_PinInit();
	
	TFT_RSTL;
	for(int temp = 0; temp < 999999; ++temp){}
	TFT_RSTH;
	for(int temp = 0; temp < 999999; ++temp){}
	TFT_WriteCmd(0x11);
	for(int temp = 0; temp < 999999; ++temp){}
	
	TFT_WriteCmd(0xB1); 
	TFT_WriteDat8Bits(0x01); 
	TFT_WriteDat8Bits(0x2C); 
	TFT_WriteDat8Bits(0x2D); 
	
	TFT_WriteCmd(0xB2); 
	TFT_WriteDat8Bits(0x01); 
	TFT_WriteDat8Bits(0x2C); 
	TFT_WriteDat8Bits(0x2D); 
	
	TFT_WriteCmd(0xB3); 
	TFT_WriteDat8Bits(0x01); 
	TFT_WriteDat8Bits(0x2C); 
	TFT_WriteDat8Bits(0x2D); 
	TFT_WriteDat8Bits(0x01); 
	TFT_WriteDat8Bits(0x2C); 
	TFT_WriteDat8Bits(0x2D); 
	
	TFT_WriteCmd(0xB4);
	TFT_WriteDat8Bits(0x07); 
	
	TFT_WriteCmd(0xC0); 
	TFT_WriteDat8Bits(0xA2); 
	TFT_WriteDat8Bits(0x02); 
	TFT_WriteDat8Bits(0x84); 
	TFT_WriteCmd(0xC1); 
	TFT_WriteDat8Bits(0xC5); 
	
	TFT_WriteCmd(0xC2); 
	TFT_WriteDat8Bits(0x0A); 
	TFT_WriteDat8Bits(0x00); 
	
	TFT_WriteCmd(0xC3); 
	TFT_WriteDat8Bits(0x8A); 
	TFT_WriteDat8Bits(0x2A); 
	TFT_WriteCmd(0xC4); 
	TFT_WriteDat8Bits(0x8A); 
	TFT_WriteDat8Bits(0xEE); 
	
	TFT_WriteCmd(0xC5);
	TFT_WriteDat8Bits(0x0E); 
	
	TFT_WriteCmd(0x36);
	TFT_WriteDat8Bits(0xC0); 
	TFT_WriteDat8Bits(0xC8); 
	
	TFT_WriteCmd(0xe0); 
	TFT_WriteDat8Bits(0x0f); 
	TFT_WriteDat8Bits(0x1a); 
	TFT_WriteDat8Bits(0x0f); 
	TFT_WriteDat8Bits(0x18); 
	TFT_WriteDat8Bits(0x2f); 
	TFT_WriteDat8Bits(0x28); 
	TFT_WriteDat8Bits(0x20); 
	TFT_WriteDat8Bits(0x22); 
	TFT_WriteDat8Bits(0x1f); 
	TFT_WriteDat8Bits(0x1b); 
	TFT_WriteDat8Bits(0x23); 
	TFT_WriteDat8Bits(0x37); 
	TFT_WriteDat8Bits(0x00); 	
	TFT_WriteDat8Bits(0x07); 
	TFT_WriteDat8Bits(0x02); 
	TFT_WriteDat8Bits(0x10); 
	
	TFT_WriteCmd(0xe1); 
	TFT_WriteDat8Bits(0x0f); 
	TFT_WriteDat8Bits(0x1b); 
	TFT_WriteDat8Bits(0x0f); 
	TFT_WriteDat8Bits(0x17); 
	TFT_WriteDat8Bits(0x33); 
	TFT_WriteDat8Bits(0x2c); 
	TFT_WriteDat8Bits(0x29); 
	TFT_WriteDat8Bits(0x2e); 
	TFT_WriteDat8Bits(0x30); 
	TFT_WriteDat8Bits(0x30); 
	TFT_WriteDat8Bits(0x39); 
	TFT_WriteDat8Bits(0x3f); 
	TFT_WriteDat8Bits(0x00); 
	TFT_WriteDat8Bits(0x07); 
	TFT_WriteDat8Bits(0x03); 
	TFT_WriteDat8Bits(0x10);
	
	TFT_WriteCmd(0x2a);
	TFT_WriteDat8Bits(0x00);
	TFT_WriteDat8Bits(0x00 + 2);
	TFT_WriteDat8Bits(0x00);
	TFT_WriteDat8Bits(0x80+2);
	
	TFT_WriteCmd(0x2b);
	TFT_WriteDat8Bits(0x00);
	TFT_WriteDat8Bits(0x00 + 3);
	TFT_WriteDat8Bits(0x00);
	TFT_WriteDat8Bits(0x80 + 3);
	
	TFT_WriteCmd(0xF0); 
	TFT_WriteDat8Bits(0x01); 
	TFT_WriteCmd(0xF6);
	TFT_WriteDat8Bits(0x00); 
	
	TFT_WriteCmd(0x3A);
	TFT_WriteDat8Bits(0x05); 
	
	TFT_WriteCmd(0x29);
	
	TFT_ClearScreen(TFT_GBP454_WHITE);//ȫ��
}

//----------------------------------------------------------------------------------------
//  @����       ��ʾһ��8x16���ַ�
//  @����       x          ��ʾλ�õĺ�����(0~127)
//  @����       y          ��ʾλ�õ�������(0~159)
//  @����       dat        ����ʾ���ַ�
//  @����       PENCOLOR   ���ʵ���ɫ,��ʽΪRGB454
//  @����       BGCOLOR    ��������ɫ,��ʽΪRGB454
//  @ʾ��       TFT_P8x16Char(0, 0, 'H', GBP454_BLACK, GBP454_WHITE); //����Ļ(0,0)λ����ʾH,������ɫΪ��ɫ,������ɫΪ��ɫ
//----------------------------------------------------------------------------------------
void TFT_P8x16Char(uint16 x, uint16 y, uint8 dat, uint16 PENCOLOR, uint16 BGCOLOR)
{
	uint8 i, j;
	uint8 temp;
	
	if(x > TFT_X_MAX - 8)
	{
		x = 0;
		y += 16;
	}
	
	for(i = 0; i < 16; ++i)
	{
		TFT_SetRegion(x, y + i, x + 7, y + i);
		temp = TFT_ASCII8x18[dat - 32][i];
		for(j	=0; j < 8; ++j)
		{
			if(temp & 0x01)	
				TFT_WriteDat16Bits(PENCOLOR);
			else
				TFT_WriteDat16Bits(BGCOLOR);
			temp >>= 1;
		}
	}
}

//----------------------------------------------------------------------------------------
//  @����       ��ʾһ��8x16���ַ���
//  @����       x          ��ʾλ�õĺ�����(0~127)
//  @����       y          ��ʾλ�õ�������(0~159)
//  @����       dat        ����ʾ���ַ���
//  @����       PENCOLOR   ���ʵ���ɫ,��ʽΪRGB454
//  @����       BGCOLOR    ��������ɫ,��ʽΪRGB454
//  @ʾ��       TFT_P8x16Str(0, 0, "Hello", GBP454_BLACK, GBP454_WHITE); //����Ļ(0,0)λ����ʾ Hello,������ɫΪ��ɫ,������ɫΪ��ɫ
//----------------------------------------------------------------------------------------
void TFT_P8x16Str(uint16 x, uint16 y, char *dat, uint16 PENCOLOR, uint16 BGCOLOR)
{
	uint16 index = 0;
	uint8 i, j, temp;
	while(dat[index] != '\0')
	{
		if(x > TFT_X_MAX - 8)
		{
			x = 0;
			y += 16;
		}
		
		for(i = 0; i < 16; ++i)
		{
			TFT_SetRegion(x, y + i, x + 7, y + i);
			temp = TFT_ASCII8x18[dat[index] - 32][i];
			for(j = 0; j < 8; ++j)
			{
				if(temp & 0x01)	
					TFT_WriteDat16Bits(PENCOLOR);
				else
					TFT_WriteDat16Bits(BGCOLOR);
				temp >>= 1;
			}
		}
		x += 8;
		++index;
	}
}

//----------------------------------------------------------------------------------------
//  @����       ��ʾһ��8x16��������
//  @����       x          ��ʾλ�õĺ�����(0~127)
//  @����       y          ��ʾλ�õ�������(0~159)
//  @����       data1      ����ʾ������
//  @����       set        ��ʾ����λλ��,����ò���������,����ʾ����з���,��Ϊ0���Զ��������ʵ���ʾֵ
//  @����       PENCOLOR   ���ʵ���ɫ,��ʽΪRGB454
//  @����       BGCOLOR    ��������ɫ,��ʽΪRGB454
//  @ʾ��       TFT_P8x16Int(0, 0, 123, 4, GBP454_BLACK, GBP454_WHITE); //����Ļ(0,0)λ����ʾ 0123,������ɫΪ��ɫ,������ɫΪ��ɫ
//              TFT_P8x16Int(0, 0, -123, -3, GBP454_BLACK, GBP454_WHITE); //����Ļ(0,0)λ����ʾ -123,������ɫΪ��ɫ,������ɫΪ��ɫ
//----------------------------------------------------------------------------------------
void TFT_P8x16Int(uint16 x, uint16 y, int16_t data1, int8 set, uint16 PENCOLOR, uint16 BGCOLOR)
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
		TFT_P8x16Str(x, y, (char *)&(result[5 + set]), PENCOLOR, BGCOLOR);
	}
	else if(set > 0)
	{
		TFT_P8x16Str(x, y, (char *)&(result[6 - set]), PENCOLOR, BGCOLOR);
	}
}

//----------------------------------------------------------------------------------------
//  @����       ��ʾһ��8x16�ĸ�����
//  @����       x          ��ʾλ�õĺ�����(0~127)
//  @����       y          ��ʾλ�õ�������(0~159)
//  @����       data1      ����ʾ�ĸ�����
//  @����       set        ��ʾ������������λλ��,����ò���������,����ʾ����з���,��Ϊ0���Զ��������ʵ���ʾֵ
//  @����       PENCOLOR   ���ʵ���ɫ,��ʽΪRGB454
//  @����       BGCOLOR    ��������ɫ,��ʽΪRGB454
//  @ʾ��       TFT_P6x8Flo(0, 0, 100.0/3, 2, GBP454_BLACK, GBP454_WHITE); //����Ļ(0,0)λ����ʾ 33.33,������ɫΪ��ɫ,������ɫΪ��ɫ
//              TFT_P6x8Flo(0, 0, 100.0/3, -2, GBP454_BLACK, GBP454_WHITE); //����Ļ(0,0)λ����ʾ +33.33,������ɫΪ��ɫ,������ɫΪ��ɫ
//----------------------------------------------------------------------------------------
void TFT_P6x8Flo(uint8_t x, uint8_t y, double data1, int8 set, uint16 PENCOLOR, uint16 BGCOLOR)
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
		TFT_P8x16Str(x, y, (char *)&(result[8 + set]), PENCOLOR, BGCOLOR);
	}
	else if(set > 0)
	{
		TFT_P8x16Str(x, y, (char *)&(result[9 - set]), PENCOLOR, BGCOLOR);
	}
}

//----------------------------------------------------------------------------------------
//  @����       ��ʾһ��BMPͼ��
//  @����       x          ��ʾλ�õĺ�����(0~127)
//  @����       y          ��ʾλ�õ�������(0~159)
//  @����       size_x     ͼ���X��С
//  @����       size_y     ͼ���Y��С
//  @����       bmp        ��ʾ��ͼ��
//  @ʾ��       TFT_PrintBMP(0, 0, 128, 123, gImage_123); //��(0,0)����ʾһ��128*123��С��ͼƬgImage_123
//  @ע��       ��ʾͼƬʱ��Ҫ��ͼ��ȡģ���ȡģ,ȡģʱ����Ϊˮƽɨ�衢16λ���ɫ�����߶ȿ��128*160��������ͼ��ͷ����
//----------------------------------------------------------------------------------------
void TFT_PrintBMP(uint8 x, uint8 y, uint8 size_x, uint8 size_y, const unsigned char *Image) 
{
	uint32 i, j;
	
	if(x + size_x <= TFT_X_MAX)
		TFT_SetRegion(x, y, x + size_x - 1, y + size_y - 1);		//��������
	else
		TFT_SetRegion(x, y, TFT_X_MAX - 1, y + size_y - 1);
	
	for(i = 0; i < size_y; ++i)
	{
		if(y + i >= TFT_Y_MAX) break;
		for(j = 0; j < size_x; ++j)
		{
			if(x + j >= TFT_X_MAX) continue;
			TFT_WriteDat16Bits(Image[(j + i * size_x) * 2 + 1] << 8 | Image[(j + i * size_x) * 2]);  						
		}
	}
}

/****************** ����Ϊ�ڲ�����,���ɵ��� ******************/

//��κ궨��������ǣ�����TFT_SPI_HARDWARE����,ΪTFT_SPI_INIT��TFT_SPI_SEND_DATA��ѡ��Ӧ����
#if TFT_SPI_HARDWARE == 0 //ʹ��ģ��SPI
#define TFT_SPI_INIT()       SPI_InitSimulated(TFT_SDA_Pin, TFT_SCL_Pin)
#define TFT_SPI_SEND_DATA(x) SPI_SendDataSimulated(TFT_SDA_Pin, TFT_SCL_Pin, x)
#else //ʹ��Ӳ��SPI
#define TFT_SPI_INIT()       SPI_InitHardware(TFT_SPI, bus_clk_mhz *1000000)
#define TFT_SPI_SEND_DATA(x) SPI_SendDataHardware(TFT_SPI, x)
#endif

static void TFT_PinInit(void) //��ʼ��
{
	TFT_SPI_INIT();
	gpio_init(TFT_RES_Pin, GPO, 1);
	gpio_init(TFT_DC_Pin, GPO, 1);
}

static void TFT_WriteCmd(uint8 cmd) //д����
{
	TFT_DCL;
	TFT_SPI_SEND_DATA(cmd);
}

static void TFT_WriteDat8Bits(uint8 data) //д8λ����
{
	TFT_DCH;
	TFT_SPI_SEND_DATA(data);
}

static void TFT_WriteDat16Bits(uint16 data) //д16λ����
{
	TFT_DCH;
	
	TFT_SPI_SEND_DATA(data >> 8); //���͸�8λ
	TFT_SPI_SEND_DATA(data & 0xFF); //���͵�8λ
}

static void TFT_SetRegion(uint16 x_start, uint16 y_start, uint16 x_end, uint16 y_end) //���û滭����
{
	TFT_WriteCmd(0x2a);
	TFT_WriteDat8Bits(0x02);
	TFT_WriteDat8Bits(x_start + 2);
	TFT_WriteDat8Bits(0x02);
	TFT_WriteDat8Bits(x_end + 2);
	
	TFT_WriteCmd(0x2b);
	TFT_WriteDat8Bits(0x01);
	TFT_WriteDat8Bits(y_start + 1);
	TFT_WriteDat8Bits(0x01);
	TFT_WriteDat8Bits(y_end + 1);	
	
	TFT_WriteCmd(0x2c);
}
