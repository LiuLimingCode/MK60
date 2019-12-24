#include "headfile.h"

float pitch, roll, yaw;
int16 gyroX, gyroY, gyroZ;

void PIT0_IRQHandler(void)
{
	PIT_FlAG_CLR(pit0);

	MPU6050_DMPGetData(&pitch, &roll, &yaw);
	MPU6050_ReadData(MPU6050_GYRO_X, &gyroX);
	MPU6050_ReadData(MPU6050_GYRO_Y, &gyroY);
	MPU6050_ReadData(MPU6050_GYRO_Z, &gyroZ);
	
	OLED_P6x8Flo(0, 0, pitch, -3);
	OLED_P6x8Flo(0, 1, roll, -3);
	OLED_P6x8Flo(0, 2, yaw, -3);
	
	OLED_P6x8Int(0, 5, gyroX, 4);
	OLED_P6x8Int(0, 6, gyroY, 4);
	OLED_P6x8Int(0, 7, gyroZ, 4);
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

