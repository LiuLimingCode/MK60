#include "headfile.h"

uint8 image[HEIGHT][WIDTH];
uint8 imageUse[HEIGHT][WIDTH];
uint8 imageBinary[HEIGHT / 2][WIDTH / 2];
uint8 imageHalf[HEIGHT / 2][WIDTH / 2];

int main(void)
{
	get_clk();//上电后必须运行一次这个函数，获取各个频率信息
    
	OLED_Init();
	uart_init(uart4, 115200);
	
	MT9V032_Init(image);
	
	while(1)
	{
		if(mt9v032_finish_flag)
		{
			mt9v032_finish_flag = 0;
			for(int a = 0; a < WIDTH; ++ a)
			{
				for(int b = 0; b < HEIGHT; ++b)
					imageUse[b][a] = image[b][a];
			}
			GrayscaleScaledHalf((uint8 *)imageUse, (uint8 *)imageHalf, WIDTH, HEIGHT);
			OSTUThreshold((uint8 *)imageHalf, (uint8 *)imageBinary, WIDTH / 2 * HEIGHT / 2);
			OLED_PrintBinary(WIDTH / 2, HEIGHT / 2, (uint8 *)imageBinary);
		}
	}
}

