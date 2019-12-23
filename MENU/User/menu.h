/****************************************************************************
 * @文件       menu.h
 * @作者       刘力铭
 * @功能       菜单，用于调整参数
 * @注意       该程序使用基于OLED，五个按键（对应上下左右确认），FLASH读写
               和一个蜂鸣器（可关闭），移植时请确保初始化
 * @完成时间   2018-4-25
****************************************************************************/
#ifndef __MENU_H
#define __MENU_H

#include "headfile.h"
#include "MK60_oled.h"

#define BUTTON_UP                       B9
#define BUTTON_DOWN											B17
#define BUTTON_LEFT											A19
#define BUTTON_RIGHT	         	      	C5
#define BUTTON_CONFIRM	                B16
#define BUZZER                          D0

#define BUZZER_EN                       1                                       //给这个宏 写0关闭蜂鸣器 写1开启蜂鸣器

#define MENU_MAX                        10                                      //菜单的个数
#define SUBMENU_MAX			4                                       //每个菜单能储存待调整的变量数
#define MENU_SHOW(x)		        ((x == 0)? 0: OLED_X_MAX/2)
#define  FLASH_SAVE_RAW_MENU		(FLASH_SECTOR_NUM - 150)
#define  FLASH_SAVE_PRO_MENU		(FLASH_SECTOR_NUM - 100)

typedef struct
{
	const char* Title;                                                      //该菜单的名字
	void* Sub_Value[SUBMENU_MAX];                                           //该菜单储存待调整的变量数值
	int8 Sub_Type[SUBMENU_MAX];                                             //该菜单储存待调整的变量数类型
	const char* Sub_Title[SUBMENU_MAX];					//该菜单储存待调整的变量数名字
	int8 Sub_Value_Num;
}Menu_TypeDef;

void Menu_Init(void);                                                           //初始化菜单
uint8 Menu_Work(void);                                                          //菜单工作
//示范写法
//DisableInterrupts();                          //关闭所有中断，防止菜单调节过程中出现中断
//Menu_Init();                                  //初始化菜单
//while(!Menu_Work()) systick_delay_ms(200);    //菜单每200ms工作一次，并根据是否按下“关闭菜单”选项后（函数返回0）结束死循环
//EnableInterrupts();
#endif
