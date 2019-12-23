#include "headfile.h"

#define CODER_R		B18
#define CODER_L		A4
#define CODER_DIR_R				B19
#define CODER_DIR_L				A5

int g_nSpeedLeft;
int g_nSpeedRight;

void PIT0_IRQHandler(void)
{
	PIT_FlAG_CLR(pit0);
	
	g_nSpeedLeft = ((gpio_get(CODER_DIR_L) == 1) ? DMA_count_get(DMA_CH10) : -DMA_count_get(DMA_CH10));
	DMA_count_reset(DMA_CH10);
	g_nSpeedRight = ((gpio_get(CODER_DIR_R) == 0) ? DMA_count_get(DMA_CH11) : -DMA_count_get(DMA_CH11));
	DMA_count_reset(DMA_CH11);
	
	OLED_P6x8Int(0, 0, g_nSpeedLeft, -3);
	OLED_P6x8Int(0, 1, g_nSpeedRight, -3);
}

int main(void)
{
	get_clk();//上电后必须运行一次这个函数，获取各个频率信息
    
	OLED_Init();
	
	gpio_init(CODER_DIR_L, GPI, 1);
	gpio_init(CODER_DIR_R, GPI, 1);
  DMA_count_Init(DMA_CH10, CODER_L, 2000, DMA_rising_down_keepon);
	DMA_count_Init(DMA_CH11, CODER_R, 2000, DMA_rising_down_keepon);
	
	pit_init_ms(pit0, 10);
	set_irq_priority(PIT0_IRQn,1);
	enable_irq(PIT0_IRQn);
	
	while(1);
}

