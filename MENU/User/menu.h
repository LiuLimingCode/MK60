/****************************************************************************
 * @文件       menu.h
 * @作者       刘力铭
 * @功能       菜单，用于调整参数
 * @注意       该程序使用外部显示器(OLED)，五个按键(对应上下左右确认),
               外部存储器(FLASH)和一个蜂鸣器(可关闭)，移植时请注意
 * @时间       2019-12-17
****************************************************************************/
#ifndef __MENU_H
#define __MENU_H
 
#include "headfile.h"
#include "MK60_oled.h"

//主要宏
#define ENABLE_WARNPAGE					1           //这个宏 写0关闭警告界面 写1开启
#define ENABLE_BUZZER           1           //这个宏 写0关闭蜂鸣器 写1开启蜂鸣器
#define MENU_UNITS_MAX					13          //菜单单元的最大个数
#define UNIT_VARIABLES_MAX			4           //每个菜单单元中能控制的变量的最大个数
#define MENU_BUTTON_UP					B16         //上按键
#define MENU_BUTTON_DOWN				A19         //下按键
#define MENU_BUTTON_LEFT				B0          //左按键
#define MENU_BUTTON_RIGHT				B9          //右按键
#define MENU_BUTTON_CONFIRM			B1          //确认按键
#if ENABLE_BUZZER
#define MENU_BUZZER							D0          //蜂鸣器,若ENABLE_BUZZER为0,可以不设置
#endif
#define VARIABLE_STORE_NUM 		 (FLASH_SECTOR_SIZE/VARIABLE_STORE_BIT) 	//菜单最大可以存储的变量数
//次要宏
#define FLASH_SAVE_PRO_MENU		 (FLASH_SECTOR_NUM - 100)									//菜单数据在FLASH中存储的位置
#define FLASH_SAVE_RAW_MENU		 (FLASH_SECTOR_NUM - 150)									//菜单数据在FLASH中存储的位置
#define FLASH_SECTOR_SIZE			 SECTOR_SIZE															//FLASH的存储总字节数
#define VARIABLE_STORE_BIT		 15																				//每个变量在FLASH中存储的位数
#define FLASH_STORE_BIT				 32																				//菜单的变量压缩后在FLASH存储所占的位数
#define FLASH_STORE_NUM				 (FLASH_SECTOR_SIZE/FLASH_STORE_BIT)	  	//菜单的变量压缩后在FLASH中存储的变量数

//------------------------------------------------------------------------------------
//                              * 菜单保存变量值的问题 *                              
//  菜单使用外部存储器保存数据,所以当用户改变变量值并保存后,该修改的数据在单片机重启后依然保
//  持修改的值.但是如果用户通过编程改变了菜单中每一个变量的值或者增加了/删除了菜单中的某变量
//  菜单会察觉到这一修改并且做出反应,该反应根据ENABLE_WARNPAGE不同而不同,若为1,则菜单会跳出
//  警告界面询问用户,根据用户的不同选择使用新的变量值或者使用保存在FLASH中的变量值; 若为0,则
//  菜单默认直接用新的变量值覆盖原值并且覆盖存储器,不再询问.
//------------------------------------------------------------------------------------

typedef enum
{
	VariableType_Void,       //无效类型
	VariableType_Int8,       //有符号 整型 8位
	VariableType_Uint8,      //无符号 整型 8位
	VariableType_Int16,      //有符号 整型 16位
	VariableType_Uint16,     //无符号 整型 16位
	VariableType_Int32,      //有符号 整型 32位
	VariableType_Uint32,     //无符号 整型 32位
	VariableType_Float,      //浮点数
	VariableType_Double,     //双精度浮点数
	VariableType_SupportNum, //不允许使用
}VariableTypeDef;

typedef struct
{
	const char* 			UintTitle;												//显示的名字
	void* 						VariableAddr[UNIT_VARIABLES_MAX];	//变量数据地址
	VariableTypeDef 	VariableType[UNIT_VARIABLES_MAX];	//变量数据类型
	const char* 			VariableName[UNIT_VARIABLES_MAX];	//变量显示名字
}Menu_Unit;

typedef Menu_Unit Menu[MENU_UNITS_MAX]; //用户用Menu定义自己的菜单结构体,语法: Menu myMenu; //等价于Menu_Unit[MENU_UNITS_MAX] myMenu;

void Menu_Init(Menu menu); //在调用该函数前,请确保外部资源(GPIO,外部存储器,外部显示器)的初始化
uint8 Menu_Work(void); //建议每隔200ms执行一次该函数,如:while(!Menu_Work()) systick_delay_ms(200);

#endif
