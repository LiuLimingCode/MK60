#include "headfile.h"

uint8 Image[HEIGHT][WIDTH];
uint8 Image_Binary[HEIGHT][WIDTH];

int main(void)
{
	get_clk();//上电后必须运行一次这个函数，获取各个频率信息
  
	gpio_init(A14, GPO, 0);
	
	TFT_Init();
	
	MT9V032_Init(Image);
	
	gpio_init(B23, GPI, 1);
	
	while(1)
	{
		if(mt9v032_finish_flag)
		{          
			mt9v032_finish_flag = 0;
			GrayscaleToBinary((uint8 *)Image, (uint8 *)Image_Binary, 0x30, WIDTH * HEIGHT);
			
			if(gpio_get(B23) == 1)
				TFT_PrintGrayImage(0, 0, WIDTH, HEIGHT,(const unsigned char *)Image, 1);
			else
				TFT_PrintGrayImage(0, 0, WIDTH, HEIGHT,(const unsigned char *)Image_Binary, 1);
		}
	}
}
