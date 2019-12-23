#include "headfile.h"

float pitch, roll, yaw;

void PIT0_IRQHandler(void)
{
	PIT_FlAG_CLR(pit0);

	MPU6050_DMPGetData(&pitch, &roll, &yaw);
	
	OLED_P6x8Flo(0, 0, pitch, -3);
	OLED_P6x8Flo(0, 1, roll, -3);
	OLED_P6x8Flo(0, 2, yaw, -3);
}

int main(void)
{
	get_clk();//上电后必须运行一次这个函数，获取各个频率信息
    
	OLED_Init();
	while(MPU6050_DMPInit());
	pit_init_ms(pit0, 10);
	set_irq_priority(PIT0_IRQn,1);
	enable_irq(PIT0_IRQn);
	
	while(1);
}

