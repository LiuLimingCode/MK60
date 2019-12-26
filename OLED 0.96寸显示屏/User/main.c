#include "headfile.h"

uint8 Pit_flag = 0;

void PIT0_IRQHandler(void)
{
	PIT_FlAG_CLR(pit0);
	Pit_flag = 1;
	
}
int32 times1,times2,time,delay;
int main(void)
{
	
	get_clk();//上电后必须运行一次这个函数，获取各个频率信息
	pit_init_ms(pit0, 100);
	set_irq_priority(PIT0_IRQn, 1);
	enable_irq(PIT0_IRQn);
	EnableInterrupts;
  
	OLED_Init();
	
	while(1)
	{
		if(Pit_flag == 1)
		{
			Pit_flag = 0;
			delay = 0;
			times1 = PIT->CHANNEL[pit0].CVAL;
			if(PIT->CHANNEL[pit0].TFLG &  PIT_TFLG_TIF_MASK)
				delay += 1;
			
			OLED_P6x8Int(0, 0, 121, 3);
			OLED_P6x8Int(0, 1, 12, 3);
			OLED_P6x8Int(0, 2, 0, 3);
			OLED_P6x8Int(0, 3, -121, 3);
			OLED_P6x8Int(0, 4, 121, -3);
			OLED_P6x8Flo(0, 5, -12222.22, -5);
			OLED_P6x8Flo(0, 6, 122222.22, 6);
			OLED_P6x8Int(64, 0, 121, 3);
			OLED_P6x8Int(64, 1, 12, 3);
			OLED_P6x8Int(64, 2, 1, 2);
			OLED_P6x8Int(64, 3, -1, 1);
			OLED_P6x8Int(64, 4, 0, 1);
			OLED_P6x8Int(64, 5, -21, 2);
			OLED_P6x8Int(64, 6, -123, 2);
			times2 = PIT->CHANNEL[pit0].CVAL;
			if(PIT->CHANNEL[pit0].TFLG &  PIT_TFLG_TIF_MASK)
				delay += 1;
			time = times1 - times2;
			OLED_P6x8Flo(0, 7, (float)time/(100000 * bus_clk_mhz) * 100, 2);
			OLED_P6x8Str(64, 7, "ms");
		}
	}
}

