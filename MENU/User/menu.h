/****************************************************************************
 * @�ļ�       menu.h
 * @����       ������
 * @����       �˵������ڵ�������
 * @ע��       �ó���ʹ�û���OLED�������������Ӧ��������ȷ�ϣ���FLASH��д
               ��һ�����������ɹرգ�����ֲʱ��ȷ����ʼ��
 * @���ʱ��   2018-4-25
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

#define BUZZER_EN                       1                                       //������� д0�رշ����� д1����������

#define MENU_MAX                        10                                      //�˵��ĸ���
#define SUBMENU_MAX			4                                       //ÿ���˵��ܴ���������ı�����
#define MENU_SHOW(x)		        ((x == 0)? 0: OLED_X_MAX/2)
#define  FLASH_SAVE_RAW_MENU		(FLASH_SECTOR_NUM - 150)
#define  FLASH_SAVE_PRO_MENU		(FLASH_SECTOR_NUM - 100)

typedef struct
{
	const char* Title;                                                      //�ò˵�������
	void* Sub_Value[SUBMENU_MAX];                                           //�ò˵�����������ı�����ֵ
	int8 Sub_Type[SUBMENU_MAX];                                             //�ò˵�����������ı���������
	const char* Sub_Title[SUBMENU_MAX];					//�ò˵�����������ı���������
	int8 Sub_Value_Num;
}Menu_TypeDef;

void Menu_Init(void);                                                           //��ʼ���˵�
uint8 Menu_Work(void);                                                          //�˵�����
//ʾ��д��
//DisableInterrupts();                          //�ر������жϣ���ֹ�˵����ڹ����г����ж�
//Menu_Init();                                  //��ʼ���˵�
//while(!Menu_Work()) systick_delay_ms(200);    //�˵�ÿ200ms����һ�Σ��������Ƿ��¡��رղ˵���ѡ��󣨺�������0��������ѭ��
//EnableInterrupts();
#endif
