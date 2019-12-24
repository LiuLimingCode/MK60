#include "headfile.h"

uint8 imageUse[ROW][COL];
uint8 imageBinary[ROW][COL];

int main(void)
{
	get_clk();//上电后必须运行一次这个函数，获取各个频率信息
    
	OLED_Init();
    camera_init();
	
	while(1)
	{
		if(mt9v032_finish_flag)
		{
			mt9v032_finish_flag = 0;
			for(int a = 0; a < COL; ++ a)
			{
				for(int b = 0; b < ROW; ++b)
					imageUse[b][a] = image[b][a];
			}
			uint8 Threshold = OSTUThreshold((uint8 *)imageUse, COL, ROW);
			GrayscaleToBinary((uint8 *)imageUse, (uint8 *)imageBinary, Threshold, COL*ROW);
			OLED_PrintBinary(COL, ROW, (uint8 *)imageBinary);
		}
	}
}

