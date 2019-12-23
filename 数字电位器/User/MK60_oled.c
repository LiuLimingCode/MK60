/*!
 * @file       oled.c
 * @brief      OLED函数实现(SPI)
 * @author     llm
 * @注意：区别于普通的OLED底层基于软件模拟SPI，这个底层基于K60硬件SPI编写。
          优点是通讯速度远远大于前者(大概10倍)，CPU占用率远远小于前者。
    但是！务必确保电路设计时，OLED连接的SCK、SDA(D0,D1)选为PORT_cfg.h中的SPI固定引脚。(非常重要！！！)
          电路设计时在选择SPIX_SCK SPIX_SOUT 后最好不要再使用对应的SPIX_SIN引脚，因为该引脚会在SPI底层中配置为输入模式
          该底层涉及大量寄存器操作，故基本不允许跨芯片移植程序，如K60程序移植到KEA128芯片后该底层无法使用。(普通OLED底层移植性好)
 */

#include "MK60_oled.h"
#include "MK60_gpio.h"
#include "MK60_spi.h"
#include "charcode.h"

static void OLED_SetPos(uint8_t x, uint8_t y);
static void OLED_PinInit(void);
static void OLED_DLY_ms(uint16_t ms);
__STATIC_INLINE void SPI_Init(void);
__STATIC_INLINE void OLED_WriteCmd(uint8 cmd);
__STATIC_INLINE void OLED_WriteDat(uint8 data);

/*!
*  @描述       OLED显示屏清屏函数
*  @参数       NewState       0x00为关OLED，0x01为开OLED
*  示例:       OLED_Cmd (0x00);
*/
void OLED_Cmd(char NewState)
{
  if(NewState != 0) NewState = 1;
  OLED_WriteCmd(0xAE | NewState);
}

/*!
*  @描述       OLED显示屏清屏函数
*  @参数       color       0x00为全暗，0xff为全亮
*  示例:       OLED_ClearScreen (0x00);
*/
void OLED_ClearScreen(uint8_t color)
{
  uint16_t y,x;	
  for(y=0; y < OLED_PAGE_MAX; y++)
  {
    OLED_WriteCmd(0xb0 + y);
    OLED_WriteCmd(0x01);
    OLED_WriteCmd(0x10); 
    for(x=0; x < OLED_X_MAX; x++)
      OLED_WriteDat(color);
  }
}

/*!
*  @描述       OLED初始化
*  示例:       OLED_Init ();
*/
void OLED_Init (void)
{
  OLED_PinInit();
  SPI_Init();
	
  OLED_RSTL; //初始化OLED
        
  OLED_DLY_ms(50);
  OLED_RSTH;

  OLED_Cmd(0);//--turn off oled panel
  OLED_WriteCmd(0x00);//---set low column address
  OLED_WriteCmd(0x10);//---set high column address
  OLED_WriteCmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
  OLED_WriteCmd(0x81);//--set contrast control register
  OLED_WriteCmd(0xcf); // Set SEG Output Current Brightness
  OLED_WriteCmd(0xa1);//--Set SEG/Column Mapping     //
  OLED_WriteCmd(0xc8);//Set COM/Row Scan Direction  //
  OLED_WriteCmd(0xa6);//--set normal display
  OLED_WriteCmd(0xa8);//--set multiplex ratio(1 to 64)
  OLED_WriteCmd(0x3f);//--1/64 duty
  OLED_WriteCmd(0xd3);//-set display offset    Shift Mapping RAM Counter (0x00~0x3F)
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
  OLED_Cmd(1);//--turn on oled panel
  OLED_ClearScreen(BLACK);
  OLED_SetPos(0, 0);
}

/*!
*  @描述       显示一个6x8的字符
*  @参数       x       显示位置的横坐标(0~127)
*  @参数       y       显示位置的纵坐标(0~7)
*  @参数       data1   待显示的字符
*  示例:       gpio_init (0, 0, 'H');    //在屏幕(0,0)位置显示 H
*/
void OLED_P6x8Char(uint8_t x, uint8_t y, char ch)
{
  uint8_t c = 0, i = 0; 
  c =ch-32;
  if(x>126)
  {
    x=0;
    y++;
  }
  OLED_SetPos(x,y);    
  for(i=0;i<6;i++)
  {     
    OLED_WriteDat(F6x8[c][i]);  
  }
  x+=6;
}

/*!
*  @描述       显示一个6x8的字符串
*  @参数       x       显示位置的横坐标(0~127)
*  @参数       y       显示位置的纵坐标(0~7)
*  @参数       data1   待显示的字符串
*  示例:       gpio_init (0, 0, "HELLO");    //在屏幕(0,0)位置显示 HELLO
*/
void OLED_P6x8Str(uint8_t x, uint8_t y, const char *ch)
{
  uint8_t c = 0, i = 0, j = 0;      
  while (ch[j]!='\0')
  {    
    c =ch[j]-32;
    if(x>126)
    {
      x=0;
      y++;
    }
    OLED_SetPos(x,y);    
    for(i=0;i<6;i++)
    {     
      OLED_WriteDat(F6x8[c][i]);  
    }
    x+=6;
    j++;
  }
}

/*!
*  @描述       显示一个6x8的整数数
*  @参数       x       显示位置的横坐标(0~127)
*  @参数       y       显示位置的纵坐标(0~7)
*  @参数       data1   待显示的整数
*  @参数       set     显示整数位位数，如果该参数带负号，则显示结果有符号
*  示例:       gpio_init (0, 0, 123, 4);    //在屏幕(0,0)位置显示 0123
               gpio_init (0, 0, -123, -3);    //在屏幕(0,0)位置显示 -123
*/
void OLED_P6x8Int(uint8_t x, uint8_t y, int16_t data1, int8 set)
{
  uint8_t result[7];
  if(data1<0)
  {
    data1 *= -1;
    result[0] = '-';
  }
  else
  {
    result[0] = '+';
  }
  result[1] = (uint8_t)((data1%100000)/10000+48);
  result[2] = (uint8_t)((data1%10000)/1000+48);
  result[3] = (uint8_t)((data1%1000)/100+48);
  result[4] = (uint8_t)((data1%100)/10+48);
  result[5] = (uint8_t)(data1%10+48);
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

/*!
*  @描述       显示一个6x8的浮点数
*  @参数       x       显示位置的横坐标(0~127)
*  @参数       y       显示位置的纵坐标(0~7)
*  @参数       data1   待显示的浮点数
*  @参数       set     显示浮点数的整数位位数，如果该参数带负号，则显示结果有符号
*  示例:       gpio_init (0, 0, 100.0/3, 2);    //在屏幕(0,0)位置显示 33.33
               gpio_init (0, 0, 100.0/3, -2);    //在屏幕(0,0)位置显示 +33.33
*/
void OLED_P6x8Flo(uint8_t x, uint8_t y, float data1, int8 set)
{
  uint8_t result[9];
  uint32_t data2;
  if(data1<0)
  {			
    result[0] = '-';
    data1 *= -1;
  }
  else
  {
    result[0]='+';
  }
  data1 += 0.000001;
  data2 = (uint32_t)(data1*100);
  result[1] = (uint8_t)((data2%1000000)/100000+48);
  result[2] = (uint8_t)((data2%100000)/10000+48);
  result[3] = (uint8_t)((data2%10000)/1000+48);
  result[4] = (uint8_t)((data2%1000)/100+48);
  result[5] = '.';
  result[6] = (uint8_t)((data2%100)/10+48);
  result[7] = (uint8_t)(data2%10+48);
  result[8] = '\0';
  
  if(set < 0)
  {
    result[4 + set] = result[0];
    OLED_P6x8Str(x, y, (char *)&(result[4 + set]));
  }
  else if(set > 0)
  {
    OLED_P6x8Str(x, y, (char *)&(result[5 - set]));
  }
}

/*!
*  @描述       显示一个8x16的标准ASCII字符
*  @参数       x       显示位置的横坐标(0~127)
*  @参数       y       显示位置的纵坐标(0~6)
*  @参数       ch      待显示的字符
*  示例:       gpio_init (0, 0, 'H');    //在屏幕(0,0)位置显示 H
*/
void OLED_P8x16Char(uint8_t x, uint8_t y, char ch)
{
  uint8_t c = 0, i = 0;
  
  c = ch - 32;
  if(x > 120)
  {
    x=0;
    y++;
  }
  OLED_SetPos(x,y);
  for(i=0;i<8;i++)
  {
    OLED_WriteDat(F8X16[c*16+i]);
  }
  OLED_SetPos(x,y+1);
  for(i=0;i<8;i++)
  {
    OLED_WriteDat(F8X16[c*16+i+8]);
  }
  x+=8;
}

/*!
*  @描述       显示一个8x16的标准ASCII字符串
*  @参数       x       显示位置的横坐标(0~127)
*  @参数       y       显示位置的纵坐标(0~6)
*  @参数       ch      待显示的字符串
*  示例:       gpio_init (0, 0, "HELLO");    //在屏幕(0,0)位置显示 HELLO
*/
void OLED_P8x16Str(uint8_t x, uint8_t y, const char *ch)
{
  uint8_t c = 0, i = 0, j = 0;
  while (ch[j] != '\0')
  {    
    c = ch[j] - 32;
    if(x > 120)
    {
      x=0;
      y++;
    }
    OLED_SetPos(x,y);
    for(i=0;i<8;i++)
    {
      OLED_WriteDat(F8X16[c*16+i]);
    }
    OLED_SetPos(x,y+1);
    for(i=0;i<8;i++)
    {
      OLED_WriteDat(F8X16[c*16+i+8]);
    }
    x+=8;
    j++;
  }
}

/*!
*  @描述       显示一个14x16的中文字符
*  @参数       x       显示位置的横坐标(0~127)
*  @参数       y       显示位置的纵坐标(0~6)
*  @参数       ch      待显示的字符串
*  示例:       gpio_init (0, 0, "你好");    //在屏幕(0,0)位置显示 你好
*  注意        显示汉字时需要用文字取模软件取模字体，取模时设置为 宋体五号，纵向取模，字节倒叙，再添加至charcode.h中的Word14x16数组中方可使用
*/
void OLED_P14x16CHCHAR(uint8_t x, uint8_t y, const char *ch)
{
  uint8_t wordnum, i;
  while(*ch != '\0')
  {
    if(x > 114)
    {
      x = 0;
      ++ y;
    }
    OLED_SetPos(x, y);
    for(wordnum = 0; wordnum < sizeof(Word14x16)/31; ++wordnum)
    {
      if(((*ch & 0xff) == (Word14x16[wordnum].word_name[0] & 0xff)) && ((*(ch+1) & 0xff) == (Word14x16[wordnum].word_name[1] & 0xff)))
      {
        for(i=0; i<14; i++)
        {
          OLED_WriteDat(Word14x16[wordnum].index[i]);
        }
        OLED_SetPos(x, y+1);
        for(i=14; i<28; i++)
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

/*!
*  @描述       显示一个图片
*  @参数       x0      显示位置的起点横坐标(0~127)
*  @参数       y0      显示位置的起点纵坐标(0~7)
*  @参数       x1      显示位置的终横坐标(0~127)
*  @参数       y1      显示位置的终点纵坐标(0~7)
*  @参数       bmp     待显示的图片
*  示例:       gpio_init (0, 0, 100, 6, bmp);    //在屏幕(0,0)位置显示一个大小为100 * 48 的图片
*  注意:       显示图片时需要用图像取模软件取模，取模时设置为 数据水平，字节垂直，单色，最大尺寸128 * 64，字节内像素数据反序
*/
void OLED_PrintBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const char bmp[])
{ 	
  uint16_t temp=0;
  uint8_t x,y;
  for(y = y0; y <= y1; ++y)
  {
    OLED_SetPos(x0,y);				
    for(x = x0; x <= x1; ++x)
    {      
      OLED_WriteDat(bmp[temp++]);	    	
    }
  }
}

/*!
*  @描述       显示一个二值化图片
*  @参数       size_x		原图片的X大小
*  @参数       size_y		原图片的Y大小
*  @参数       binary		待显示的二值化图片
*  示例:       gpio_init (94, 60, Binary);    //在屏幕(0,0)位置显示一个大小为96 * 60 的图片
*/
void OLED_PrintBinary(uint16 size_x, uint16 size_y, uint8 * binary)
{
	uint8 data;
	for(int16 index_y = 0; index_y < 8; ++index_y)
	{
		OLED_SetPos(0, index_y);
		for(int16 index_x = 0; index_x < 128; ++index_x)
		{
			for(int8 index = 7; index >= 0; --index)
			{
				data <<= 1;
				data |= (binary + ((index_y  * 8 + index) * size_y) / 64 * size_x)[(index_x * size_x) / 128] & 0x01;
			}
			OLED_WriteDat(data);
		}
	}
}

/****************** 以下为内部函数，不可调用 ******************/
__STATIC_INLINE void SPI_Init(void)
{
	(void)spi_init(OLED_SPI, NOT_PCS, MASTER, bus_clk_mhz *1000000);
}

__STATIC_INLINE void OLED_WriteCmd(uint8 cmd)
{
  OLED_DCL;
  while((SPIN[OLED_SPI]->SR & SPI_SR_TCF_MASK) == 1);//等待传输完成
  SPIN[OLED_SPI]->SR = SPI_SR_TCF_MASK;
  SPIN[OLED_SPI]->PUSHR = (0
													| SPI_PUSHR_CTAS(0)          //选择CTAR0寄存器
													| SPI_PUSHR_CONT_MASK         
													| SPI_PUSHR_TXDATA(cmd));
}

__STATIC_INLINE void OLED_WriteDat(uint8 data)
{
  OLED_DCH;
  while((SPIN[OLED_SPI]->SR & SPI_SR_TCF_MASK) == 1);//等待传输完成
  SPIN[OLED_SPI]->SR = SPI_SR_TCF_MASK;
  SPIN[OLED_SPI]->PUSHR = (0
													| SPI_PUSHR_CTAS(0)          //选择CTAR0寄存器
													| SPI_PUSHR_CONT_MASK         
													| SPI_PUSHR_TXDATA(data));
}

static void OLED_DLY_ms(uint16_t ms)
{                         
  unsigned int a;
  while(ms)
  {
    a=222;
    while(a--);
    ms--;
  }
  return;
}

static void OLED_PinInit(void)
{
  gpio_init(OLED_RES_Pin, GPO, 1);
  gpio_init(OLED_DC_Pin, GPO, 1);
}

static void OLED_SetPos(uint8_t x, uint8_t y)//设置坐标
{ 
  OLED_WriteCmd(0xb0+y);
  OLED_WriteCmd(((x&0xf0)>>4)|0x10);
  OLED_WriteCmd((x&0x0f)|0x01); 
}
