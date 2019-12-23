/****************************************************************************
 * @�ļ�       menu.h
 * @����       ������
 * @����       �˵������ڵ�������
 * @ע��       �ó���ʹ���ⲿ��ʾ��(OLED)���������(��Ӧ��������ȷ��),
               �ⲿ�洢��(FLASH)��һ��������(�ɹر�)����ֲʱ��ע��
 * @ʱ��       2019-12-17
****************************************************************************/
#ifndef __MENU_H
#define __MENU_H
 
#include "headfile.h"
#include "MK60_oled.h"

//��Ҫ��
#define ENABLE_WARNPAGE					1           //����� д0�رվ������ д1����
#define ENABLE_BUZZER           1           //����� д0�رշ����� д1����������
#define MENU_UNITS_MAX					13          //�˵���Ԫ��������
#define UNIT_VARIABLES_MAX			4           //ÿ���˵���Ԫ���ܿ��Ƶı�����������
#define MENU_BUTTON_UP					B16         //�ϰ���
#define MENU_BUTTON_DOWN				A19         //�°���
#define MENU_BUTTON_LEFT				B0          //�󰴼�
#define MENU_BUTTON_RIGHT				B9          //�Ұ���
#define MENU_BUTTON_CONFIRM			B1          //ȷ�ϰ���
#if ENABLE_BUZZER
#define MENU_BUZZER							D0          //������,��ENABLE_BUZZERΪ0,���Բ�����
#endif
#define VARIABLE_STORE_NUM 		 (FLASH_SECTOR_SIZE/VARIABLE_STORE_BIT) 	//�˵������Դ洢�ı�����
//��Ҫ��
#define FLASH_SAVE_PRO_MENU		 (FLASH_SECTOR_NUM - 100)									//�˵�������FLASH�д洢��λ��
#define FLASH_SAVE_RAW_MENU		 (FLASH_SECTOR_NUM - 150)									//�˵�������FLASH�д洢��λ��
#define FLASH_SECTOR_SIZE			 SECTOR_SIZE															//FLASH�Ĵ洢���ֽ���
#define VARIABLE_STORE_BIT		 15																				//ÿ��������FLASH�д洢��λ��
#define FLASH_STORE_BIT				 32																				//�˵��ı���ѹ������FLASH�洢��ռ��λ��
#define FLASH_STORE_NUM				 (FLASH_SECTOR_SIZE/FLASH_STORE_BIT)	  	//�˵��ı���ѹ������FLASH�д洢�ı�����

//------------------------------------------------------------------------------------
//                              * �˵��������ֵ������ *                              
//  �˵�ʹ���ⲿ�洢����������,���Ե��û��ı����ֵ�������,���޸ĵ������ڵ�Ƭ����������Ȼ��
//  ���޸ĵ�ֵ.��������û�ͨ����̸ı��˲˵���ÿһ��������ֵ����������/ɾ���˲˵��е�ĳ����
//  �˵���������һ�޸Ĳ���������Ӧ,�÷�Ӧ����ENABLE_WARNPAGE��ͬ����ͬ,��Ϊ1,��˵�������
//  �������ѯ���û�,�����û��Ĳ�ͬѡ��ʹ���µı���ֵ����ʹ�ñ�����FLASH�еı���ֵ; ��Ϊ0,��
//  �˵�Ĭ��ֱ�����µı���ֵ����ԭֵ���Ҹ��Ǵ洢��,����ѯ��.
//------------------------------------------------------------------------------------

typedef enum
{
	VariableType_Void,       //��Ч����
	VariableType_Int8,       //�з��� ���� 8λ
	VariableType_Uint8,      //�޷��� ���� 8λ
	VariableType_Int16,      //�з��� ���� 16λ
	VariableType_Uint16,     //�޷��� ���� 16λ
	VariableType_Int32,      //�з��� ���� 32λ
	VariableType_Uint32,     //�޷��� ���� 32λ
	VariableType_Float,      //������
	VariableType_Double,     //˫���ȸ�����
	VariableType_SupportNum, //������ʹ��
}VariableTypeDef;

typedef struct
{
	const char* 			UintTitle;												//��ʾ������
	void* 						VariableAddr[UNIT_VARIABLES_MAX];	//�������ݵ�ַ
	VariableTypeDef 	VariableType[UNIT_VARIABLES_MAX];	//������������
	const char* 			VariableName[UNIT_VARIABLES_MAX];	//������ʾ����
}Menu_Unit;

typedef Menu_Unit Menu[MENU_UNITS_MAX]; //�û���Menu�����Լ��Ĳ˵��ṹ��,�﷨: Menu myMenu; //�ȼ���Menu_Unit[MENU_UNITS_MAX] myMenu;

void Menu_Init(Menu menu); //�ڵ��øú���ǰ,��ȷ���ⲿ��Դ(GPIO,�ⲿ�洢��,�ⲿ��ʾ��)�ĳ�ʼ��
uint8 Menu_Work(void); //����ÿ��200msִ��һ�θú���,��:while(!Menu_Work()) systick_delay_ms(200);

#endif
