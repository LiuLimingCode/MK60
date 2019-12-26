#include "headfile.h"

//以下为数字电位器操作界面参考程序

#define KU              B1
#define KD              C5
#define KC              B17
#define KL              B19
#define KR              B9 	
#define SW4             A5
#define SW3             A12
#define SW2             A13 
#define AD_CH_L_1       ADC1_SE7a
#define AD_CH_L__       ADC1_DP0
#define AD_CH_M_1       ADC0_DP1
#define AD_CH_R__       ADC0_DP0
#define AD_CH_R_1       ADC1_DP1

void FLASH_InitMCP41(void);
void Normalized_MCP41(void);

#define	FLASH_SAVE_MCP41        (FLASH_SECTOR_NUM - 23)
#define FLASH_SAVE_ADC_NORMAL   (FLASH_SECTOR_NUM - 25)
#define	OLED_SHOW(x)            (int16)(25.5 * (x - 1))

typedef enum
{
  L_1 = 0,
  L__,
  M_1,
  R__,
  R_1,
  ADC_NUM,
}ADC_POSITION;

uint8 MCP41_Flash_Save[8];//用于保存MCP41返回的电阻值
ADCn_Ch MCP41_ADC[ADC_NUM] = {AD_CH_L_1, AD_CH_L__, AD_CH_M_1, AD_CH_R__, AD_CH_R_1};//设定的通道
uint8 MCP41_Set[ADC_NUM] = {80, 100, 80, 100, 80};//各个电感的设定值(对应上面设定的通道号)
int16 ADC_MinData[ADC_NUM];//保存数字点位器设置后的电感最小值

int main()
{
  OLED_Init();
  MCP41_Init();
  FLASH_Init();
  gpio_init(SW4, GPI, 1);
  gpio_init(KL, GPI, 1);
  gpio_init(KR, GPI, 1);
  gpio_init(KU, GPI, 1);
  gpio_init(KD, GPI, 1);
  gpio_init(KC, GPI, 1);
  
  
  gpio_init(MCP41_CS0, GPO, 1);
  gpio_init(MCP41_CS1, GPO, 1);
  gpio_init(MCP41_CS2, GPO, 1);
  gpio_init(MCP41_CS3, GPO, 1);
  gpio_init(MCP41_CS4, GPO, 1);
  
  FLASH_InitMCP41();
  
  if(gpio_get(SW4) == 0)
    Normalized_MCP41();
  
  while(1)
  {
    OLED_P6x8Int(OLED_SHOW(1), 3, adc_once(AD_CH_L_1, ADC_8bit), 3);
    OLED_P6x8Int(OLED_SHOW(2), 3, adc_once(AD_CH_L__, ADC_8bit), 3);
    OLED_P6x8Int(OLED_SHOW(3), 3, adc_once(AD_CH_M_1, ADC_8bit), 3);
    OLED_P6x8Int(OLED_SHOW(4), 3, adc_once(AD_CH_R__, ADC_8bit), 3);
    OLED_P6x8Int(OLED_SHOW(5), 3, adc_once(AD_CH_R_1, ADC_8bit), 3);//显示电感读数
    systick_delay_ms(10);
  }
}

/**
 * @备注    数字电位器人机界面操作函数，MCP41_Set数组为电感的预设值
 */
void Normalized_MCP41(void)
{
	int8 _index = 1, _index_old = 1;
	
	OLED_P6x8Str((OLED_X_MAX - 1 - 6 * (sizeof("Normalized") - 1)) / 2, 0, "Normalized");
	OLED_P6x8Str(OLED_SHOW(1), 1, "L_1");
	OLED_P6x8Str(OLED_SHOW(2), 1, "L__");
	OLED_P6x8Str(OLED_SHOW(3), 1, "M_1");
	OLED_P6x8Str(OLED_SHOW(4), 1, "R__");
	OLED_P6x8Str(OLED_SHOW(5), 1, "R_1");//显示界面
        
	while(1)
	{
		OLED_P6x8Int(OLED_SHOW(1), 3, adc_once(AD_CH_L_1, ADC_8bit), 3);
		OLED_P6x8Int(OLED_SHOW(2), 3, adc_once(AD_CH_L__, ADC_8bit), 3);
		OLED_P6x8Int(OLED_SHOW(3), 3, adc_once(AD_CH_M_1, ADC_8bit), 3);
		OLED_P6x8Int(OLED_SHOW(4), 3, adc_once(AD_CH_R__, ADC_8bit), 3);
		OLED_P6x8Int(OLED_SHOW(5), 3, adc_once(AD_CH_R_1, ADC_8bit), 3);//显示电感读数
		
		if(gpio_get(KL) == 0) _index--;//光标左右移动
		if(gpio_get(KR) == 0) _index++;
		if(gpio_get(KU) == 0)//如果按下了上键，那么将设定的电阻值全部保存到FLASH
		{
			FLASH_EraseSector(FLASH_SAVE_MCP41);//FLASH必须先擦除再写入
			for(int8 temp = 0; temp < 8; temp++)
				FLASH_WriteSector(FLASH_SAVE_MCP41, 32 * temp, MCP41_Flash_Save[temp]);//写入电阻值
                        FLASH_EraseSector(FLASH_SAVE_ADC_NORMAL);
			for(int8 temp = 0; temp < ADC_NUM; temp++)
				FLASH_WriteSector(FLASH_SAVE_ADC_NORMAL, 32 * temp, ADC_MinData[temp]);//写入ADC最小值
			break;
		}
		if(gpio_get(KD) == 0)//如果按了下键，那么不保存设定值，用原来保存的电阻值重新设置电位器
		{
			FLASH_InitMCP41();
			break;
		}
		if(gpio_get(KC) == 0)//如果按下确认键，那么开始调整电阻值
		{
			OLED_P6x8Str(OLED_SHOW(_index), 2, " & ");//调整过程中显示 & 
			MCP41_Flash_Save[_index - 1] = MCP41_SetValue(_index - 1, MCP41_Set[_index  - 1]);
			systick_delay_ms(50);
                        ADC_MinData[_index - 1] = adc_read(MCP41_ADC[_index - 1], ADC_8bit,16);//保存此时的最小值
			OLED_P6x8Str(OLED_SHOW(_index), 2, " * ");//显示 * 表示调整结束
			OLED_P6x8Int(OLED_SHOW(_index), 4, MCP41_Set[_index - 1], 3);//显示调整值用于参考
		}
		
		if(_index == 0) _index = ADC_NUM;//光标控制
		if(_index == ADC_NUM + 1) _index = 1;
		
		OLED_P6x8Str(OLED_SHOW(_index), 2, " * ");
		if(_index != _index_old)
			OLED_P6x8Str(OLED_SHOW(_index_old), 2, "   ");
		
		_index_old = _index;
		systick_delay_ms(50);
		
		for(int temp = 0; temp < 3; ++temp)//增加电感读数显示的频率
		{
			OLED_P6x8Int(OLED_SHOW(1), 3, adc_once(AD_CH_L_1, ADC_8bit), 3);
			OLED_P6x8Int(OLED_SHOW(2), 3, adc_once(AD_CH_L__, ADC_8bit), 3);
			OLED_P6x8Int(OLED_SHOW(3), 3, adc_once(AD_CH_M_1, ADC_8bit), 3);
			OLED_P6x8Int(OLED_SHOW(4), 3, adc_once(AD_CH_R__, ADC_8bit), 3);
			OLED_P6x8Int(OLED_SHOW(5), 3, adc_once(AD_CH_R_1, ADC_8bit), 3);//显示电感读数
			systick_delay_ms(50);
		}
	}
        
	OLED_ClearScreen(OLED_BLACK);//while结束后清屏
	while(gpio_get(KU) == 0);//等待松手
	while(gpio_get(KD) == 0);
}

//从FLSAH中读出电阻的保存值并设置电位器
void FLASH_InitMCP41(void)
{
	for(int8 temp = 0; temp < 8; temp++)
	{
		MCP41_Flash_Save[temp] = flash_read(FLASH_SAVE_MCP41, 32 * temp, uint8);
	}
	for(int8 temp = 0; temp < ADC_NUM; temp++)
	{
		MCP41_SetCmd(temp, MCP41_Flash_Save[temp]);
	}
	for(uint8 temp= 0; temp < ADC_NUM; temp++)
	{
		ADC_MinData[temp] = flash_read(FLASH_SAVE_ADC_NORMAL, 32 * temp, int16);
		OLED_P6x8Int(OLED_SHOW(temp + 1) + 6, 2, ADC_MinData[temp] ,3);
	}
}
