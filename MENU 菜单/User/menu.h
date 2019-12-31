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

/***************** ��Ҫ�궨�� *****************/
#define MENU_ENABLE_WARNPAGE    1           //����� д0�رվ������ д1����
#define MENU_ENABLE_BUZZER      0           //����� д0�رշ����� д1����������
#define MENU_UNITS_NUM          13          //�˵���Ԫ��������
#define MENU_UNIT_VARIABLES_NUM 4           //ÿ���˵���Ԫ���ܿ��Ƶı�����������
#define MENU_VARIABLE_BIT       15          //ÿ�������ڴ洢���д洢��λ��(���޸�)
#define MENU_VARIABLES_NUM      (MENU_STORAGE_SIZE/MENU_VARIABLE_BIT) //�˵������Դ洢�ı�����

/***************** ��Ҫ�궨�� *****************/
#define MENU_STORAGE_PRODATA    (FLASH_SECTOR_NUM - 10)                  //�˵����������ڴ洢���д洢��λ��(���޸�)
#define MENU_STORAGE_RAWDATA    (FLASH_SECTOR_NUM - 11)                  //�˵�ԭʼ�����ڴ洢���д洢��λ��(���޸�)
#define MENU_STORAGE_SIZE       (SECTOR_SIZE)                            //�洢���Ĵ洢��λ��(���޸�)
#define MENU_STORE_BIT          32                                       //�˵��ı���ѹ�����ڴ洢���洢��ռ��λ��
#define MENU_STORE_NUM          (MENU_STORAGE_SIZE/MENU_STORE_BIT)       //�˵��ı���ѹ�����ڴ洢���д洢������

/****************** ����ѡ�� ******************/
#define MENU_BUTTON_UP          B16         //�ϰ���
#define MENU_BUTTON_DOWN        A19         //�°���
#define MENU_BUTTON_LEFT        B0          //�󰴼�
#define MENU_BUTTON_RIGHT       B9          //�Ұ���
#define MENU_BUTTON_CONFIRM     B1          //ȷ�ϰ���
#if MENU_ENABLE_BUZZER
#define MENU_BUZZER             D0          //������,��MENU_ENABLE_BUZZERΪ0,���Բ�����
#endif

//------------------------------------------------------------------------------------
//                              * �˵��������ֵ������ *                              
//  �˵�ʹ���ⲿ�洢����������,���Ե��û��ı����ֵ�������,���޸ĵ������ڵ�Ƭ����������Ȼ��
//  ���޸ĵ�ֵ.��������û�ͨ����̸ı��˲˵���ĳ����������ֵ��������/ɾ���˵��е�ĳ��������
//  �˵���������һ�޸Ĳ���������Ӧ,�÷�Ӧ����MENU_ENABLE_WARNPAGE��ͬ����ͬ,��Ϊ1,��˵�
//  �������������ѯ���û�,�����û��Ĳ�ͬѡ��ʹ���µı���ֵ����ʹ�ñ�����FLASH�еı���ֵ; ��
//  Ϊ0,��˵�Ĭ��ֱ�����µı���ֵ����ԭֵ���Ҹ��Ǵ洢��,����ѯ��.������ϸ��Ϣ���menu.c�е�
//  * �˵��Ĵ���ԭ�� *
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
	const char*       UintTitle;                             //��ʾ������
	void*             VariableAddr[MENU_UNIT_VARIABLES_NUM]; //�������ݵ�ַ
	VariableTypeDef   VariableType[MENU_UNIT_VARIABLES_NUM]; //������������
	const char*       VariableName[MENU_UNIT_VARIABLES_NUM]; //������ʾ����
}Menu_Unit;

typedef Menu_Unit Menu[MENU_UNITS_NUM]; //�û���Menu�����Լ��Ĳ˵��ṹ��,�﷨: Menu myMenu; //�ȼ���Menu_Unit[MENU_UNIT_VARIABLES_NUM] myMenu;

void Menu_Init(Menu menu); //�ڵ��øú���ǰ,��ȷ���ⲿ��Դ(GPIO,�ⲿ�洢��,�ⲿ��ʾ��)�ĳ�ʼ��
uint8 Menu_Work(void); //����ÿ��200msִ��һ�θú���,��:while(!Menu_Work()) systick_delay_ms(200);

#endif
