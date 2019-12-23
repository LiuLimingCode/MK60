#include "headfile.h"

int main(void)
{
	get_clk();//上电后必须运行一次这个函数，获取各个频率信息
  
	OLED_Init();
	OLED_ClearScreen(0xff);
	
	while(1);
}

