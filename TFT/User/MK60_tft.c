/*!
 * @文件       tft.c
 * @功能       逐飞1.8寸TFT屏(基于ST7735S)函数实现(SPI)
 * @作者       刘力铭
 * @完成时间   2019-12
 */
 
#include "MK60_tft.h"
#include "TFT_code.h"

static void TFT_PinInit(void);
static void TFT_WriteCmd(uint8 cmd);
static void TFT_WriteDat8Bits(uint8 data);
static void TFT_WriteDat16Bits(uint16 data);
static void TFT_SetRegion(uint16 x_start, uint16 y_start, uint16 x_end, uint16 y_end);

//----------------------------------------------------------------------------------------
//  @描述       TFT显示屏清屏函数
//  @参数       color       清屏颜色,格式为RGB454
//  @示例       TFT_ClearScreen(GBP454_WHITE);
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
//  @描述       TFT初始化
//  @示例       TFT_Init();
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
	
	TFT_ClearScreen(TFT_GBP454_WHITE);//全白
}

//----------------------------------------------------------------------------------------
//  @描述       显示一个8x16的字符
//  @参数       x          显示位置的横坐标(0~127)
//  @参数       y          显示位置的纵坐标(0~159)
//  @参数       dat        待显示的字符
//  @参数       PENCOLOR   画笔的颜色,格式为RGB454
//  @参数       BGCOLOR    背景的颜色,格式为RGB454
//  @示例       TFT_P8x16Char(0, 0, 'H', GBP454_BLACK, GBP454_WHITE); //在屏幕(0,0)位置显示H,画笔颜色为黑色,背景颜色为白色
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
//  @描述       显示一个8x16的字符串
//  @参数       x          显示位置的横坐标(0~127)
//  @参数       y          显示位置的纵坐标(0~159)
//  @参数       dat        待显示的字符串
//  @参数       PENCOLOR   画笔的颜色,格式为RGB454
//  @参数       BGCOLOR    背景的颜色,格式为RGB454
//  @示例       TFT_P8x16Str(0, 0, "Hello", GBP454_BLACK, GBP454_WHITE); //在屏幕(0,0)位置显示 Hello,画笔颜色为黑色,背景颜色为白色
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
//  @描述       显示一个8x16的整数数
//  @参数       x          显示位置的横坐标(0~127)
//  @参数       y          显示位置的纵坐标(0~159)
//  @参数       data1      待显示的整数
//  @参数       set        显示整数位位数,如果该参数带负号,则显示结果有符号,若为0则自动调整合适的显示值
//  @参数       PENCOLOR   画笔的颜色,格式为RGB454
//  @参数       BGCOLOR    背景的颜色,格式为RGB454
//  @示例       TFT_P8x16Int(0, 0, 123, 4, GBP454_BLACK, GBP454_WHITE); //在屏幕(0,0)位置显示 0123,画笔颜色为黑色,背景颜色为白色
//              TFT_P8x16Int(0, 0, -123, -3, GBP454_BLACK, GBP454_WHITE); //在屏幕(0,0)位置显示 -123,画笔颜色为黑色,背景颜色为白色
//----------------------------------------------------------------------------------------
void TFT_P8x16Int(uint16 x, uint16 y, int16_t data1, int8 set, uint16 PENCOLOR, uint16 BGCOLOR)
{
	uint8_t result[7] = {0};
	
	if(set > 5) set = 5; //int16整数最多显示5位
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
//  @描述       显示一个8x16的浮点数
//  @参数       x          显示位置的横坐标(0~127)
//  @参数       y          显示位置的纵坐标(0~159)
//  @参数       data1      待显示的浮点数
//  @参数       set        显示浮点数的整数位位数,如果该参数带负号,则显示结果有符号,若为0则自动调整合适的显示值
//  @参数       PENCOLOR   画笔的颜色,格式为RGB454
//  @参数       BGCOLOR    背景的颜色,格式为RGB454
//  @示例       TFT_P6x8Flo(0, 0, 100.0/3, 2, GBP454_BLACK, GBP454_WHITE); //在屏幕(0,0)位置显示 33.33,画笔颜色为黑色,背景颜色为白色
//              TFT_P6x8Flo(0, 0, 100.0/3, -2, GBP454_BLACK, GBP454_WHITE); //在屏幕(0,0)位置显示 +33.33,画笔颜色为黑色,背景颜色为白色
//----------------------------------------------------------------------------------------
void TFT_P6x8Flo(uint8_t x, uint8_t y, double data1, int8 set, uint16 PENCOLOR, uint16 BGCOLOR)
{
	uint8_t result[13] = {0};
	uint32_t data2;
	
	if(set > 8) set = 8; //整数位最多显示8位
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
//  @描述       显示一个BMP图像
//  @参数       x          显示位置的横坐标(0~127)
//  @参数       y          显示位置的纵坐标(0~159)
//  @参数       size_x     图像的X大小
//  @参数       size_y     图像的Y大小
//  @参数       bmp        显示的图像
//  @示例       TFT_PrintBMP(0, 0, 128, 123, gImage_123); //在(0,0)处显示一个128*123大小的图片gImage_123
//  @注意       显示图片时需要用图像取模软件取模,取模时设置为水平扫描、16位真彩色、最大高度宽度128*160、不包含图像头数据
//----------------------------------------------------------------------------------------
void TFT_PrintBMP(uint8 x, uint8 y, uint8 size_x, uint8 size_y, const unsigned char *Image) 
{
	uint32 i, j;
	
	if(x + size_x <= TFT_X_MAX)
		TFT_SetRegion(x, y, x + size_x - 1, y + size_y - 1);		//坐标设置
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

/****************** 以下为内部函数,不可调用 ******************/

//这段宏定义的作用是：根据TFT_SPI_HARDWARE定义,为TFT_SPI_INIT和TFT_SPI_SEND_DATA挑选对应函数
#if TFT_SPI_HARDWARE == 0 //使用模拟SPI
#define TFT_SPI_INIT()       SPI_InitSimulated(TFT_SDA_Pin, TFT_SCL_Pin)
#define TFT_SPI_SEND_DATA(x) SPI_SendDataSimulated(TFT_SDA_Pin, TFT_SCL_Pin, x)
#else //使用硬件SPI
#define TFT_SPI_INIT()       SPI_InitHardware(TFT_SPI, bus_clk_mhz *1000000)
#define TFT_SPI_SEND_DATA(x) SPI_SendDataHardware(TFT_SPI, x)
#endif

static void TFT_PinInit(void) //初始化
{
	TFT_SPI_INIT();
	gpio_init(TFT_RES_Pin, GPO, 1);
	gpio_init(TFT_DC_Pin, GPO, 1);
}

static void TFT_WriteCmd(uint8 cmd) //写命令
{
	TFT_DCL;
	TFT_SPI_SEND_DATA(cmd);
}

static void TFT_WriteDat8Bits(uint8 data) //写8位数据
{
	TFT_DCH;
	TFT_SPI_SEND_DATA(data);
}

static void TFT_WriteDat16Bits(uint16 data) //写16位数据
{
	TFT_DCH;
	
	TFT_SPI_SEND_DATA(data >> 8); //发送高8位
	TFT_SPI_SEND_DATA(data & 0xFF); //发送低8位
}

static void TFT_SetRegion(uint16 x_start, uint16 y_start, uint16 x_end, uint16 y_end) //设置绘画区域
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
