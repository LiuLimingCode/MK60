#include "headfile.h"

int liu = 0, asd = 10;
float dsa = 2;

#define BUZZ		D0
#define KC			B16
#define KR			C5
#define KL			A19
#define KD			B17
#define KU			B9

int main(void)
{
	get_clk();//上电后必须运行一次这个函数，获取各个频率信息
  
	OLED_Init();
	gpio_init(BUZZ, GPO, 0);
	gpio_init(KC, GPI, 1);
	gpio_init(KR, GPI, 1);
	gpio_init(KL, GPI, 1);
	gpio_init(KD, GPI, 1);
	gpio_init(KU, GPI, 1);
	
	Menu_Init();                                  //初始化菜单
	while(!Menu_Work()) systick_delay_ms(200);    //菜单每200ms工作一次，并根据是否按下“关闭菜单”选项后（函数返回0）结束死循环
}

