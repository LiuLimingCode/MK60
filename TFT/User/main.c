#include "headfile.h"

int main(void)
{
	get_clk();//上电后必须运行一次这个函数，获取各个频率信息
  
	gpio_init(A14, GPO, 0);
	
	TFT_Init();
	TFT_ClearScreen(TFT_GBP454_YELLOW);
	
	while(1);
}

