/*!
 * @file       oled.c
 * @brief      OLED����ʵ��(SPI)
 * @author     llm
 * @ע�⣺��������ͨ��OLED�ײ�������ģ��SPI������ײ����K60Ӳ��SPI��д��
          �ŵ���ͨѶ�ٶ�ԶԶ����ǰ��(���10��)��CPUռ����ԶԶС��ǰ�ߡ�
    ���ǣ����ȷ����·���ʱ��OLED���ӵ�SCK��SDA(D0,D1)ѡΪPORT_cfg.h�е�SPI�̶����š�(�ǳ���Ҫ������)
          ��·���ʱ��ѡ��SPIX_SCK SPIX_SOUT ����ò�Ҫ��ʹ�ö�Ӧ��SPIX_SIN���ţ���Ϊ�����Ż���SPI�ײ�������Ϊ����ģʽ
          �õײ��漰�����Ĵ����������ʻ����������оƬ��ֲ������K60������ֲ��KEA128оƬ��õײ��޷�ʹ�á�(��ͨOLED�ײ���ֲ�Ժ�)
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
*  @����       OLED��ʾ����������
*  @����       NewState       0x00Ϊ��OLED��0x01Ϊ��OLED
*  ʾ��:       OLED_Cmd (0x00);
*/
void OLED_Cmd(char NewState)
{
  if(NewState != 0) NewState = 1;
  OLED_WriteCmd(0xAE | NewState);
}

/*!
*  @����       OLED��ʾ����������
*  @����       color       0x00Ϊȫ����0xffΪȫ��
*  ʾ��:       OLED_ClearScreen (0x00);
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
*  @����       OLED��ʼ��
*  ʾ��:       OLED_Init ();
*/
void OLED_Init (void)
{
  OLED_PinInit();
  SPI_Init();
	
  OLED_RSTL; //��ʼ��OLED
        
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
*  @����       ��ʾһ��6x8���ַ�
*  @����       x       ��ʾλ�õĺ�����(0~127)
*  @����       y       ��ʾλ�õ�������(0~7)
*  @����       data1   ����ʾ���ַ�
*  ʾ��:       gpio_init (0, 0, 'H');    //����Ļ(0,0)λ����ʾ H
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
*  @����       ��ʾһ��6x8���ַ���
*  @����       x       ��ʾλ�õĺ�����(0~127)
*  @����       y       ��ʾλ�õ�������(0~7)
*  @����       data1   ����ʾ���ַ���
*  ʾ��:       gpio_init (0, 0, "HELLO");    //����Ļ(0,0)λ����ʾ HELLO
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
*  @����       ��ʾһ��6x8��������
*  @����       x       ��ʾλ�õĺ�����(0~127)
*  @����       y       ��ʾλ�õ�������(0~7)
*  @����       data1   ����ʾ������
*  @����       set     ��ʾ����λλ��������ò��������ţ�����ʾ����з���
*  ʾ��:       gpio_init (0, 0, 123, 4);    //����Ļ(0,0)λ����ʾ 0123
               gpio_init (0, 0, -123, -3);    //����Ļ(0,0)λ����ʾ -123
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
*  @����       ��ʾһ��6x8�ĸ�����
*  @����       x       ��ʾλ�õĺ�����(0~127)
*  @����       y       ��ʾλ�õ�������(0~7)
*  @����       data1   ����ʾ�ĸ�����
*  @����       set     ��ʾ������������λλ��������ò��������ţ�����ʾ����з���
*  ʾ��:       gpio_init (0, 0, 100.0/3, 2);    //����Ļ(0,0)λ����ʾ 33.33
               gpio_init (0, 0, 100.0/3, -2);    //����Ļ(0,0)λ����ʾ +33.33
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
*  @����       ��ʾһ��8x16�ı�׼ASCII�ַ�
*  @����       x       ��ʾλ�õĺ�����(0~127)
*  @����       y       ��ʾλ�õ�������(0~6)
*  @����       ch      ����ʾ���ַ�
*  ʾ��:       gpio_init (0, 0, 'H');    //����Ļ(0,0)λ����ʾ H
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
*  @����       ��ʾһ��8x16�ı�׼ASCII�ַ���
*  @����       x       ��ʾλ�õĺ�����(0~127)
*  @����       y       ��ʾλ�õ�������(0~6)
*  @����       ch      ����ʾ���ַ���
*  ʾ��:       gpio_init (0, 0, "HELLO");    //����Ļ(0,0)λ����ʾ HELLO
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
*  @����       ��ʾһ��14x16�������ַ�
*  @����       x       ��ʾλ�õĺ�����(0~127)
*  @����       y       ��ʾλ�õ�������(0~6)
*  @����       ch      ����ʾ���ַ���
*  ʾ��:       gpio_init (0, 0, "���");    //����Ļ(0,0)λ����ʾ ���
*  ע��        ��ʾ����ʱ��Ҫ������ȡģ���ȡģ���壬ȡģʱ����Ϊ ������ţ�����ȡģ���ֽڵ����������charcode.h�е�Word14x16�����з���ʹ��
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
*  @����       ��ʾһ��ͼƬ
*  @����       x0      ��ʾλ�õ���������(0~127)
*  @����       y0      ��ʾλ�õ����������(0~7)
*  @����       x1      ��ʾλ�õ��պ�����(0~127)
*  @����       y1      ��ʾλ�õ��յ�������(0~7)
*  @����       bmp     ����ʾ��ͼƬ
*  ʾ��:       gpio_init (0, 0, 100, 6, bmp);    //����Ļ(0,0)λ����ʾһ����СΪ100 * 48 ��ͼƬ
*  ע��:       ��ʾͼƬʱ��Ҫ��ͼ��ȡģ���ȡģ��ȡģʱ����Ϊ ����ˮƽ���ֽڴ�ֱ����ɫ�����ߴ�128 * 64���ֽ����������ݷ���
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
*  @����       ��ʾһ����ֵ��ͼƬ
*  @����       size_x		ԭͼƬ��X��С
*  @����       size_y		ԭͼƬ��Y��С
*  @����       binary		����ʾ�Ķ�ֵ��ͼƬ
*  ʾ��:       gpio_init (94, 60, Binary);    //����Ļ(0,0)λ����ʾһ����СΪ96 * 60 ��ͼƬ
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

/****************** ����Ϊ�ڲ����������ɵ��� ******************/
__STATIC_INLINE void SPI_Init(void)
{
	(void)spi_init(OLED_SPI, NOT_PCS, MASTER, bus_clk_mhz *1000000);
}

__STATIC_INLINE void OLED_WriteCmd(uint8 cmd)
{
  OLED_DCL;
  while((SPIN[OLED_SPI]->SR & SPI_SR_TCF_MASK) == 1);//�ȴ��������
  SPIN[OLED_SPI]->SR = SPI_SR_TCF_MASK;
  SPIN[OLED_SPI]->PUSHR = (0
													| SPI_PUSHR_CTAS(0)          //ѡ��CTAR0�Ĵ���
													| SPI_PUSHR_CONT_MASK         
													| SPI_PUSHR_TXDATA(cmd));
}

__STATIC_INLINE void OLED_WriteDat(uint8 data)
{
  OLED_DCH;
  while((SPIN[OLED_SPI]->SR & SPI_SR_TCF_MASK) == 1);//�ȴ��������
  SPIN[OLED_SPI]->SR = SPI_SR_TCF_MASK;
  SPIN[OLED_SPI]->PUSHR = (0
													| SPI_PUSHR_CTAS(0)          //ѡ��CTAR0�Ĵ���
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

static void OLED_SetPos(uint8_t x, uint8_t y)//��������
{ 
  OLED_WriteCmd(0xb0+y);
  OLED_WriteCmd(((x&0xf0)>>4)|0x10);
  OLED_WriteCmd((x&0x0f)|0x01); 
}
