/****************************************************************************
 * @�ļ�       menu.c
 * @����       ������
 * @����       �˵������ڵ�������
 * @ע��       �ó���ʹ���ⲿ��ʾ��(OLED)���������(��Ӧ��������ȷ��),
               �ⲿ�洢��(FLASH)��һ��������(�ɹر�)����ֲʱ��ע��
 * @���ʱ��   2019-12-17
****************************************************************************/
#include "menu.h"

/**************** �ڲ��ṹ��,ö���嶨�� ****************/

typedef struct
{
	int16 AdjustValue[4];
	int16 AdjustMax;
	int16 AdjustMin;
}AdjustDef;

typedef enum
{
	ButtonFlag_Void = 0x00,
	ButtonFlag_Up = 0x01,
	ButtonFlag_Down = 0x02,
	ButtonFlag_Left = 0x04,
	ButtonFlag_Right = 0x08,
	ButtonFlag_Udlr = ButtonFlag_Up | ButtonFlag_Down | ButtonFlag_Left | ButtonFlag_Right, //���������Ұ�������
	ButtonFlag_Confirm = 0x10,
	ButtonFlag_Pressed = ButtonFlag_Up | ButtonFlag_Down | ButtonFlag_Left | ButtonFlag_Right | ButtonFlag_Confirm, //���κΰ�������
	ButtonFlag_SerialPressed = 0x20, //����
}ButtonFlagDef;

/**** �˵����ڲ�����,�����ⲿ��Դ(GPIO,�ⲿ�洢��,�ⲿ��ʾ��).��������Ҫ����Ӳ��,���԰���Щ������д,ֻҪ��֤�������ܲ���,�˵�������Ȼ���� ****/

static void __showWarnPage(const char* str, uint8 isUpdateAll, int8 cursor);
static void __showHomePage(const Menu_Unit* menu, uint8 isUpdateAll, int8 cursor);
static void __showUintPage(const Menu_Unit* unit, int16* value, uint8 isUpdateAll, int8 cursor, uint8 confirm);
static void __changePages(void);
static void __setBuzzer(uint8 data);
static int8 __buttonGet(void);
static void __readDataFromStorage(int32* value, int sector);
static void __writeDataToStorage(int32* value, int sector);

//------------------------------------------------------------------------------------
//                                  * �˵���ʾԭ�� *                                  
//  ����ʾ����Ϊ1~ MENU_LINE_MAX ��(��0��Ϊ�����),ÿ����ʾ MENU_VARIABLE_PERLINE ������
//  ��ôһ����ʾ�����Է�Ϊ0~((MENU_LINE_MAX-1��*MENU_VARIABLE_PERLINE-1)��ŵ�λ��,ÿ��λ
//  ����һ�������͹�����ʾλ��.Ϊ��ֹ�ص�,���ͱ��������MENU_GAP_CURSOR_VARIABLE������.
//  __showWarnPage,__showHomePage,__showUintPage����һ��cursor����,�ò�������ָ�����ʾ
//  ��λ��.ͨ��MENU_SHOW_CURSOR��MENU_SHOW_ROW���Լ�����ù��λ������Ļ��ʵ�ʵ������,
//  �������������ֵ���ʾλ����ͨ�������ڲ˵��ṹ���ϵ�˳�����,��ͨ��MENU_SHOW_VARIABLE��
//  MENU_SHOW_ROW������ñ���λ������Ļ��ʵ�ʵ������.
//------------------------------------------------------------------------------------

#define MENU_VARIABLE_PERLINE           2 //ÿ�еı�����
#define MENU_GAP_CURSOR_VARIABLE        8 //������ʾ�͹����ʾ֮��ľ���
#define MENU_SHOW_CURSOR(x)             (OLED_X_MAX * ((x) % MENU_VARIABLE_PERLINE) / MENU_VARIABLE_PERLINE) //�����ʾ�ĺ�����λ��
#define MENU_SHOW_VARIABLE(x)           (OLED_X_MAX * ((x) % MENU_VARIABLE_PERLINE) / MENU_VARIABLE_PERLINE + MENU_GAP_CURSOR_VARIABLE) //������ʾ�ĺ�����λ��
#define MENU_SHOW_ROW(x)                ((x) / MENU_VARIABLE_PERLINE + 1) //�˵���ʾ��������
#define MENU_LINE_SAVE_BUTTON           MENU_LINE_MAX - 1 //"SAVE"�������ڵ�����
#define MENU_LINE_BACK_BUTTON           MENU_LINE_MAX //"BACK"�������ڵ�����
#define MENU_LINE_MAX                   7

//-------------------------------------------------------------------------
//  *�����ⲿ����ʾ���ĺ���
//  @����   ��ʾ�������
//  @����   str:�����ַ���
//          isUpdateAll:�Ƿ���Ҫ������ʾȫ����������
//                      0 = Ϊ��ֹ����,ֻ���¹��.  1 = ������ʾȫ����������
//          cursor:���λ��,��ϸ���ͼ�����*�˵���ʾԭ��*
//  @ʾ����ʾЧ��(��һ����Ҫ����):
//   +----------------------------+
//   |            Warn            |
//   | This is a warning example. |
//   | You have broken the rule   |
//   | that...                    |
//   |                            |
//   |  >  YES         NO         |
//   +----------------------------+
//-------------------------------------------------------------------------
static void __showWarnPage(const char* str, uint8 isUpdateAll, int8 cursor)
{
	static int8 cursorLast = 0;
	
	if(isUpdateAll)
	{
		OLED_P6x8Str((OLED_X_MAX - 1 - 6 * (sizeof("Warn") - 1)) / 2, 0, "Warn");//��ʾ����
		
		OLED_P6x8Str(MENU_SHOW_VARIABLE(0), 1, str);
		
		OLED_P6x8Str(MENU_SHOW_VARIABLE(0), MENU_LINE_MAX, " YES");
		OLED_P6x8Str(MENU_SHOW_VARIABLE(1), MENU_LINE_MAX, " NO");
	}
	
	OLED_P6x8Str(MENU_SHOW_CURSOR(cursorLast), MENU_SHOW_ROW(cursorLast), " "); //������
	OLED_P6x8Str(MENU_SHOW_CURSOR(cursor), MENU_SHOW_ROW(cursor), ">"); //��ʾ�¹��
	cursorLast = cursor;
}

//-------------------------------------------------------------------------
//  *�����ⲿ����ʾ���ĺ���
//  @����   ��ʾ�˵�������
//  @����   menu:�˵��ṹ��ָ��
//          isUpdateAll:�Ƿ���Ҫ������ʾȫ����������
//                      0 = Ϊ��ֹ����,ֻ���¹��.  1 = ������ʾȫ����������
//          cursor:���λ��,��ϸ���ͼ�����*�˵���ʾԭ��*
//  @ʾ����ʾЧ��(��һ����Ҫ����):
//   +----------------------------+
//   |            Menu            |
//   |> CloseMenu     Variable_1  |
//   |  Variable_2    Variable_3  |
//   |  Variable_4    Variable_5  |
//   |  Variable_6    Variable_7  |
//   |  Variable_8    Variable_9  |
//   +----------------------------+
//-------------------------------------------------------------------------
static void __showHomePage(const Menu_Unit* menu, uint8 isUpdateAll, int8 cursor)
{
	static int8 cursorLast = 0;
	
	if(isUpdateAll)
	{
		uint8 temp;
		OLED_ClearScreen(OLED_BLACK);//����
		OLED_P6x8Str((OLED_X_MAX - 1 - 6 * (sizeof("Menu") - 1)) / 2, 0, "Menu");//��ʾ����
		OLED_P6x8Str(MENU_SHOW_VARIABLE(0), MENU_SHOW_ROW(0), "CloseMenu");
		
		for(temp = 0; temp < MENU_UNITS_NUM; ++temp)
		{
			if(menu[temp].UintTitle != 0)
			{
				OLED_P6x8Str(MENU_SHOW_VARIABLE(temp + 1), MENU_SHOW_ROW(temp + 1), (char*)menu[temp].UintTitle);
			}
		}
	}
	
	OLED_P6x8Str(MENU_SHOW_CURSOR(cursorLast), MENU_SHOW_ROW(cursorLast), " "); //������
	OLED_P6x8Str(MENU_SHOW_CURSOR(cursor), MENU_SHOW_ROW(cursor), ">"); //��ʾ�¹��
	cursorLast = cursor;
}

//-------------------------------------------------------------------------
//  *�����ⲿ����ʾ���ĺ���
//  @����   ��ʾ�˵���Ԫ����
//  @����   menu:�˵��ṹ��ָ��
//          value:����������ֵ
//          isUpdateAll:�Ƿ���Ҫ������ʾȫ����������
//                      0 = Ϊ��ֹ����,ֻ���¹��.  1 = ������ʾȫ����������
//          cursor:���λ��,��ϸ���ͼ�����*�˵���ʾԭ��*
//          confirm:��Ϊ1˵������ȷ�ϰ���,��Ҫ���Ĺ�����ʽ
//  @ʾ����ʾЧ��(��һ����Ҫ����):
//   +----------------------------+
//   |            Menu            |
//   |* Variable_1       value1   |
//   |  Variable_2       value2   |
//   |  Variable_3       value3   |
//   |  SAVE                      |
//   |  BACK                      |
//   +----------------------------+
//-------------------------------------------------------------------------
static void __showUintPage(const Menu_Unit* unit, int16* value, uint8 isUpdateAll, int8 cursor, uint8 confirm)
{
	static int8 cursorLast = 0;
	
	if(isUpdateAll)
	{
		uint8 number;
		OLED_ClearScreen(OLED_BLACK);//����
		for(number = 0; unit->UintTitle[number] != 0; ++number);//��ʾ����
		OLED_P6x8Str((OLED_X_MAX - 1 - 6 * number) / 2, 0, (char*)unit->UintTitle);
		
		OLED_P6x8Str(MENU_SHOW_VARIABLE(0), MENU_LINE_SAVE_BUTTON, "SAVE");
		OLED_P6x8Str(MENU_SHOW_VARIABLE(0), MENU_LINE_BACK_BUTTON, "BACK");
	}
	
	for(int temp = 0; temp < MENU_UNIT_VARIABLES_NUM; ++temp)
	{
		if(unit->VariableType[temp] == VariableType_Void) continue;
		OLED_P6x8Str(MENU_SHOW_VARIABLE(0), temp + 1, (char*)unit->VariableName[temp]);
		
		if(unit->VariableType[temp] == VariableType_Int8 || unit->VariableType[temp] == VariableType_Int16 || unit->VariableType[temp] == VariableType_Int32)
		{
			OLED_P6x8Int(OLED_X_MAX / 2, temp + 1, value[temp], -3);
		}
		else if(unit->VariableType[temp] == VariableType_Uint8 || unit->VariableType[temp] == VariableType_Uint16 || unit->VariableType[temp] == VariableType_Uint32)
		{
			OLED_P6x8Int(OLED_X_MAX / 2, temp + 1, value[temp], 4);
		}
		else if(unit->VariableType[temp] == VariableType_Float || unit->VariableType[temp] == VariableType_Double)
		{
			if(value[temp] >= 0)
			{
				OLED_P6x8Flo(OLED_X_MAX / 2, temp + 1, (double)value[temp] / 100.0 + 0.000001, -2);
			}
			else
			{
				OLED_P6x8Flo(OLED_X_MAX / 2, temp + 1, (double)value[temp] / 100.0 - 0.000001, -2);
			}
		}
	}
	
	OLED_P6x8Str(MENU_SHOW_CURSOR(cursorLast), MENU_SHOW_ROW(cursorLast), " ");
	if(confirm == 1)
		OLED_P6x8Str(MENU_SHOW_CURSOR(cursor), MENU_SHOW_ROW(cursor), "*");
	else OLED_P6x8Str(MENU_SHOW_CURSOR(cursor), MENU_SHOW_ROW(cursor), ">");
	cursorLast = cursor;
}

//-------------------------------------------------------------------------
//  *�����ⲿ����ʾ����GPIO�ĺ���
//  @����   ���ڽ����л�ʱ,�������ҵȴ������ſ�
//-------------------------------------------------------------------------
static void __changePages(void)
{
	while(gpio_get(MENU_BUTTON_CONFIRM) == 0);
	OLED_ClearScreen(OLED_BLACK);
}

//-------------------------------------------------------------------------
//  *����GPIO�ĺ���
//  @����   ���÷�����
//  @����   data:����ֵ
//-------------------------------------------------------------------------
static void __setBuzzer(uint8 data)
{
#if MENU_ENABLE_BUZZER == 1 //���������������,��ô�������������Ϊ�պ���
	gpio_set(MENU_BUZZER, data);
#endif
}

//-------------------------------------------------------------------------
//  *����GPIO�ĺ���
//  @����   ��ⰴ��
//  @����   ����ֵ
//-------------------------------------------------------------------------
static int8 __buttonGet(void)
{
	int8 return_flag = ButtonFlag_Void;
	
	if(gpio_get(MENU_BUTTON_UP) == 0)
		return_flag |= ButtonFlag_Up;
	if(gpio_get(MENU_BUTTON_DOWN) == 0)
		return_flag |= ButtonFlag_Down;
	if(gpio_get(MENU_BUTTON_LEFT) == 0)
		return_flag |= ButtonFlag_Left;
	if(gpio_get(MENU_BUTTON_RIGHT) == 0)
		return_flag |= ButtonFlag_Right;
	if(gpio_get(MENU_BUTTON_CONFIRM) == 0)
		return_flag |= ButtonFlag_Confirm;
	
	return return_flag;
}

//-------------------------------------------------------------------------------
//                               * �˵��Ĵ���ԭ�� *                               
//  �˽�һ�£�
//  ��Ȼ�˵��Ŀ��Կ������в�ͬ���͵ı���,��ʵ��,�ڲ˵��޸�����,��������,��ȡ���ݵĹ���
//  ��,���ǰѱ���ת��Ϊint16(��short int)��������.��Ϊ�ڲ˵����ԵĹ�����,���α������
//  ����ΧΪ0~9999��-999~999,�������ĵ�����ΧΪ-99.99~99.99;���ǰѸ�������100���Ϊ
//  ���α���.��ô���б�������������ΧΪ-9999~9999�õ�����Χ������һ��15λ�Ķ����ƴ�
//  ��.�ڴ洢ʱ,�˵���int16�ı���ȡ�����õ�15λ,�����ϵ�int32���������ڱ���.
//
//  �˽�һ�£�
//  �˵���Ҫ�洢��������,�ֱ𴢴��ڲ�ͬ�ĵط�,�Ҷ���Ҫռ��MENU_STORAGE_SIZE�ռ�.
//  1.һ���ǲ˵��ڳ�ʼ����ʱ��,�Ὣ���еı���ֵȫ�����ϲ���ѹ�����浽�洢����,��Щ��
//  ��Ϊ"Raw����",��˼��ԭ����.ÿһ��ϵͳ����������,�˵���Ѹñ�������ݺ͵�ǰ�����в�
//  ���ṹ����������Ա�,�ɴ����ж��û��Ƿ��ڳ����϶Բ˵��еı����������޸�.
//  2.��һ���������ڲ˵��ڹ�����ʱ��,ʹ�ò˵����û����ò˵��޸��˱�����ֵ��SAVE,�˵���
//  ����Щ����ֵͬ��ѹ�����浽�洢����,��Щ����Ϊ"Pro����",��˼�Ǵ���������.��һ��ϵ
//  ͳ����������,���û�û�жԲ˵��еı��������޸�,��ôĬ��ʹ�ø�����.
//
//  ���Ҫ��ֲ�˵��ĳ���Ҫ����ֻ����ΰ���32λ�ı����洢���洢����
//  ��__writeDataToStorage������__readDataFromStorage��������sector����,���ݸò���
//  Ϊ0����1ʱ,������Ҫʵ�ִӴ洢���ж�Ӧ����/д��"Raw����"����"Pro����".
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------
//  *�����ⲿ�洢���ĺ���
//  @����   �ѱ���ѹ���󱣴浽��������
//  @����   value:һ���ⲿ������int32[MENU_STORE_NUM]����,����������
//          sector:��ͬ�����ݴ洢�ڲ�ͬ�Ĵ洢�� 1 = Pro����, 0 = Raw����,��ϸ
//          ���ͼ�����*�˵��Ĵ���ԭ��*
//-------------------------------------------------------------------------
static void __writeDataToStorage(int32* value, int sector)
{
	int save = (sector == 0 ? MENU_STORAGE_RAWDATA : MENU_STORAGE_PRODATA);
	FLASH_EraseSector(save);
	
	for(int temp = 0; temp < MENU_STORE_NUM; ++temp)
	{ 
		FLASH_WriteSector(save, MENU_STORE_BIT * temp, value[temp]);
	}
}

//-------------------------------------------------------------------------
//  *�����ⲿ�洢���ĺ���
//  @����   �Ӵ������ж��������ѹ������
//  @����   value:һ���ⲿ������int32[MENU_STORE_NUM]����,������������
//          sector:��ͬ�����ݴ洢�ڲ�ͬ�Ĵ洢�� 1 = Pro����, 0 = Raw����,��ϸ
//          ���ͼ�����*�˵��Ĵ���ԭ��*
//-------------------------------------------------------------------------
static void __readDataFromStorage(int32* value, int sector)
{
	int save = (sector == 0 ? MENU_STORAGE_RAWDATA : MENU_STORAGE_PRODATA);
	
	for(int temp = 0; temp < MENU_STORE_NUM; ++temp)
	{ 
		value[temp] = flash_read(save, MENU_STORE_BIT * temp, int32);
	}
}

/**************** �˵����ڲ��㷨����,�������ⲿ��Դ,���������㷨��������� ****************/

static int menuAbs(int temp) { return temp > 0 ? temp : -temp; }
static uint8 buttonGet(void);
static void compressVariable(int16* srcVariable, int32* desVariable);
static void decompressVariable(int32* srcVariable, int16* desVariable);
static void readAllVariablesFormStorage(int16* value, int sector);
static int16 readSingleVariableFromAddress(void* addr, VariableTypeDef type);
static void writeAllVariablesToStorage(int16* value, int sector);
static void writeAllVariablesToAddress(Menu_Unit* menu, int16* value);
static void writeSingleVariableToAddress(void* addr, VariableTypeDef type, int16 value);
static void collectVariablesValue(Menu_Unit* menu, int16* value);
static uint8 checkIfVariablesChanged(Menu_Unit* menu);
static void coverVariablesToAddressOrStorage(Menu_Unit* menu, uint8 direction);
static uint8 homePageHandle(uint8 buttonFlag);
static uint8 warnPageHandle(uint8 buttonFlag);
static uint8 unitPageHandle(uint8 buttonFlag);

//-------------------------------------------------------------------------
//  @����   ��ⰴ�����ж�����
//  @����   ����ֵ
//-------------------------------------------------------------------------
static uint8 buttonGet(void)
{
	static uint8 buttonFlagLast = ButtonFlag_Void;
	static uint16 serialPressed = 0;
	uint8 buttonFlag = __buttonGet();
	
	if((buttonFlag & ButtonFlag_Udlr) != ButtonFlag_Void && (buttonFlag & ButtonFlag_Udlr) == (buttonFlagLast & ButtonFlag_Udlr))
	{
		serialPressed++;
		if(serialPressed >= 5) buttonFlag |= ButtonFlag_SerialPressed;
	}
	else serialPressed = 0;
	
	buttonFlagLast = buttonFlag;
	return buttonFlag;
}

//-------------------------------------------------------------------------
//  @����   ��MENU_VARIABLE_BITλ������ѹ��ΪMENU_STORE_BITλ������
//  @����   srcVariable:һ���ⲿ������int16[MENU_VARIABLES_NUM]����,����ѹ����
//          desVariable:һ���ⲿ������int32[MENU_STORE_NUM]����,��������
//-------------------------------------------------------------------------
static void compressVariable(int16 srcVariable[MENU_VARIABLES_NUM], int32 desVariable[MENU_STORE_NUM])
{
	//��һλΪ����λ����������Ϊ����λ
	int32 signMask = 0x01 << (MENU_VARIABLE_BIT - 1);	//signMaskΪ����λ���� 
	int32 valueMask = 0;	//valueMaskΪ����λ���� 
	int16 offset = 0, index = 0;
	
	for(int16 temp = 0; temp < MENU_VARIABLE_BIT - 1; ++temp) 
	{
		valueMask <<= 1;
		valueMask |= 0x01; 
	}
	int32 wholeMask = valueMask | signMask;
	
	for(int16 temp = 0; temp < MENU_VARIABLES_NUM; ++temp) 
	{
		int32 compressedValue = menuAbs(srcVariable[temp]) & valueMask; //ȡ������λ 
		if(srcVariable[temp] < 0)  compressedValue |= signMask; //ȡ������λ 
		
		if(offset + MENU_VARIABLE_BIT < MENU_STORE_BIT) //���û����� 
		{
			desVariable[index] |= compressedValue << offset; //�� MENU_VARIABLE_BIT λ������ѹ���ŵ�32λ�ı����� 
			offset += MENU_VARIABLE_BIT;
		}
		else if(offset + MENU_VARIABLE_BIT == MENU_STORE_BIT)
		{
			desVariable[index] |= compressedValue << offset; //�� MENU_VARIABLE_BIT λ������ѹ���ŵ�32λ�ı����� 
			offset = 0;
			++index;
		}
		else //���������� MENU_VARIABLE_BIT λ�����ָ��ֱ�洢�ڲ�ͬ��32λ������ surplus|remains
		{
			int16 surplus = offset + MENU_VARIABLE_BIT - MENU_STORE_BIT; //����surplus��λ��
			int16 remains = MENU_VARIABLE_BIT - surplus;                 //����remains��λ��
			int32 wholeRemainsMask = (uint32)wholeMask >> surplus;       //����remains��������
			int32 wholeSurplusMask = (uint32)wholeMask >> remains;       //����surplus��������
			int32 compressedRemainsValue = compressedValue & wholeRemainsMask; //����remains��������
			int32 compressedSurplusValue = ((uint32)compressedValue >> remains) & wholeSurplusMask; //����surplus��������
			
			desVariable[index] |= 	compressedRemainsValue << offset; //��remains���ַŵ�32λ�ı�����
			desVariable[++index] |= compressedSurplusValue; //��surplus���ַŵ���һ��32λ�ı�����
			offset = surplus;
		} 
	}
}

//-------------------------------------------------------------------------
//  @����   ��MENU_STORE_BITλ�����ݽ�ѹΪMENU_VARIABLE_BITλ������
//  @����   srcVariable:һ���ⲿ������int32[MENU_STORE_NUM]����,����ѹ����
//          desVariable:һ���ⲿ������int16[MENU_VARIABLES_NUM]����,��������
//-------------------------------------------------------------------------
static void decompressVariable(int32 srcVariable[MENU_STORE_NUM], int16 desVariable[MENU_VARIABLES_NUM])
{
	//��һλΪ����λ����������Ϊ����λ
	int32 signMask = 0x01 << (MENU_VARIABLE_BIT - 1);	//signMaskΪ����λ���� 
	int32 valueMask = 0;	//valueMaskΪ����λ���� 
	int16 offset = 0, index = 0;
	
	for(int16 temp = 0; temp < MENU_VARIABLE_BIT - 1; ++temp) 
	{
		valueMask <<= 1;
		valueMask |= 0x01; 
	}
	int32 wholeMask = valueMask | signMask;
	
	for(int16 temp = 0; temp < MENU_VARIABLES_NUM; ++temp)
	{
		int32 decompressedValue = 0;
		
		if(offset + MENU_VARIABLE_BIT < MENU_STORE_BIT) //���û����� 
		{
			decompressedValue = ((uint32)srcVariable[index] >> offset) & wholeMask; //��32λ�ı�����ѹ�ŵ� MENU_VARIABLE_BIT λ�������� 
			offset += MENU_VARIABLE_BIT;
		}
		else if(offset + MENU_VARIABLE_BIT == MENU_STORE_BIT)
		{
			decompressedValue = ((uint32)srcVariable[index] >> offset) & wholeMask; //��32λ�ı�����ѹ�ŵ� MENU_VARIABLE_BIT λ��������
			offset = 0;
			++index;
		}
		else //���������ѷֱ�洢�ڲ�ͬ��32λ�������ϵ� MENU_VARIABLE_BIT λ���� surplus|remains
		{
			int16 surplus = offset + MENU_VARIABLE_BIT - MENU_STORE_BIT; //����surplus��λ��
			int16 remains = MENU_VARIABLE_BIT - surplus;                 //����remains��λ��
			int32 wholeRemainsMask = (uint32)wholeMask >> surplus;       //����remains��������
			int32 wholeSurplusMask = (uint32)wholeMask >> remains;       //����surplus��������
			
			int32 decompressedRemainsValue = ((uint32)srcVariable[index] >> offset) & wholeRemainsMask; //��remains���ִ�32λ�ı���ȡ�� 
			int32 decompressedSurplusValue =  srcVariable[++index] & wholeSurplusMask; //��surplus���ִ���һ��32λ�ı���ȡ�� 
			offset = surplus;
			decompressedValue = decompressedSurplusValue << remains | decompressedRemainsValue; //������� 
		}
		
		desVariable[temp] = decompressedValue & valueMask; 
		if(decompressedValue & signMask) desVariable[temp] *= -1;
	}
}

//-------------------------------------------------------------------------
//  @����   ���ڴ��ַ�ж���һ������
//  @����   addr:�����ĵ�ַ
//          type:����������
//  @����   �����ݵ�ֵ
//-------------------------------------------------------------------------
static int16 readSingleVariableFromAddress(void* addr, VariableTypeDef type)
{
	int16 value = 0;
	
	if(type == VariableType_Int8)
		value = *((int8*)addr);
	else if(type == VariableType_Uint8)
		value = *((uint8*)addr);
	else if(type == VariableType_Int16)
		value = *((int16*)addr);
	else if(type == VariableType_Uint16)
		value = *((uint16*)addr);
	else if(type == VariableType_Int32)
		value = *((int32*)addr);
	else if(type == VariableType_Uint32)
		value = *((uint32*)addr);
	else if(type == VariableType_Float)
	{
		float temp = *((float*)addr);
		if(temp >= 0) temp += 0.000001f;
		else temp -= 0.000001f;
		value = (int16)(temp * 100);
	}
	else if(type == VariableType_Double)
	{
		float temp = *((double*)addr);
		if(temp >= 0) temp += 0.000001f;
		else temp -= 0.000001f;
		value = (int16)(temp * 100);
	}
	
	return value;
}

//-------------------------------------------------------------------------
//  @����   �Ӵ洢���ж������б�ѹ�������ݲ���ѹ
//  @����   value:һ���ⲿ������int16[MENU_VARIABLES_NUM]����,������������
//          sector:��ͬ�����ݴ洢�ڲ�ͬ�Ĵ洢�� 1 = Pro����, 0 = Raw����,��ϸ
//          ���ͼ�����*�˵��Ĵ���ԭ��*
//-------------------------------------------------------------------------
static void readAllVariablesFormStorage(int16* value, int sector)
{
	int32 compressedValue[MENU_STORE_NUM] = {0};
	__readDataFromStorage(compressedValue, sector);
	decompressVariable(compressedValue, value);
}

//-------------------------------------------------------------------------
//  @����   ���������ݱ��浽��Ӧ�Ĳ˵������ڴ��ַ��
//  @����   addr:�����ĵ�ַ
//          type:����������
//          value:���洢������
//-------------------------------------------------------------------------
static void writeSingleVariableToAddress(void* addr, VariableTypeDef type, int16 value)
{
	if(type == VariableType_Int8)
		*((int8*)addr) = value;
	else if(type == VariableType_Uint8)
		*((uint8*)addr) = value;
	else if(type == VariableType_Int16)
		*((int16*)addr) = value;
	else if(type == VariableType_Uint16)
		*((uint16*)addr) = value;
	else if(type == VariableType_Int32)
		*((int32*)addr) = value;
	else if(type == VariableType_Uint32)
		*((uint32*)addr) = value;
	else if(type == VariableType_Float)
		*((float*)addr) = (float)value / 100.0f;
	else if(type == VariableType_Double)
		*((double*)addr) = (double)value / 100.0;
}

//-------------------------------------------------------------------------
//  @����		�������е�ȫ�����ݱ��浽��Ӧ�Ĳ˵������ڴ��ַ��
//  @����		menu:�˵��ṹ��ָ��
//          value:һ���ⲿ������int16[MENU_VARIABLES_NUM]����,���洢������
//-------------------------------------------------------------------------
static void writeAllVariablesToAddress(Menu_Unit* menu, int16* value)
{
	int index = 0;
	for(int alpha = 0; alpha < MENU_UNITS_NUM; ++alpha)
	{
		if(menu[alpha].UintTitle == 0) continue;
		for(int beta = 0; (beta < MENU_UNIT_VARIABLES_NUM); ++beta)
		{
			if(menu[alpha].VariableType[beta] != VariableType_Void)
			{
				writeSingleVariableToAddress(menu[alpha].VariableAddr[beta], menu[alpha].VariableType[beta], value[index]);
				index++;
			}
		}
	}
}

//-------------------------------------------------------------------------
//  @����		�������е�ȫ�����ݾ���ѹ����洢���洢����
//  @����		value:һ���ⲿ������int16[MENU_VARIABLES_NUM]����,���洢������
//          sector:��ͬ�����ݴ洢�ڲ�ͬ�Ĵ洢�� 1 = Pro����, 0 = Raw����,��ϸ
//          ���ͼ�����*�˵��Ĵ���ԭ��*
//-------------------------------------------------------------------------
static void writeAllVariablesToStorage(int16* value, int sector)
{
	int32 compressedValue[MENU_STORE_NUM] = {0};
	compressVariable(value, compressedValue);
	__writeDataToStorage(compressedValue, sector);
}

//-------------------------------------------------------------------------
//  @����		���˵������б�����ֵȫ�����ϵ�һ��������
//  @����		menu:�˵��ṹ��ָ��
//          value:һ���ⲿ������int16[MENU_VARIABLES_NUM]����,������������
//-------------------------------------------------------------------------
static void collectVariablesValue(Menu_Unit* menu, int16* value)
{
	int index = 0;
	for(int alpha = 0; alpha < MENU_UNITS_NUM; ++alpha)
	{
		if(menu[alpha].UintTitle == 0) continue;
		for(int beta = 0; (beta < MENU_UNIT_VARIABLES_NUM); ++beta)
		{
			if(menu[alpha].VariableType[beta] != VariableType_Void)
			{
				value[index] = readSingleVariableFromAddress(menu[alpha].VariableAddr[beta], menu[alpha].VariableType[beta]);
				index++;
			}
		}
	}
}

//-------------------------------------------------------------------------
//  @����		���˵��ĸ���������ֵ��û����Ϊ��̷����ı�
//  @����		menu:�˵��ṹ��ָ��
//  @����		1 = �б仯, 0 = �ޱ仯
//-------------------------------------------------------------------------
static uint8 checkIfVariablesChanged(Menu_Unit* menu)
{
	uint8 ifChanged = 0;
	int16 copyVariableValue[MENU_VARIABLES_NUM] = {0};
	int16 storedRawValue[MENU_VARIABLES_NUM] = {0};
	
	collectVariablesValue(menu, copyVariableValue);
	readAllVariablesFormStorage(storedRawValue, 0);
	
	//�Աȱ�����ֵ,����Ƿ�ı�
	for(int temp = 0; temp < MENU_VARIABLES_NUM; ++temp)
	{ 
		if(copyVariableValue[temp] != storedRawValue[temp])
		{
			ifChanged = 1;
			break;
		}
	}
	
	return ifChanged;
}

//-------------------------------------------------------------------------
//  @����   ��checkIfVariablesChanged����ֵΪ1ʱ,��������ֵ���ǵ��ڴ��ַ��
//          �߸��ǵ�������
//  @����   menu:�˵��ṹ��ָ��
//          direction:1 = ��������ֵ���ǵ�������. 0 = ��������ֵ���ǵ��ڴ��ַ
//-------------------------------------------------------------------------
static void coverVariablesToAddressOrStorage(Menu_Unit* menu, uint8 direction)
{
	int16 storedProValue[MENU_VARIABLES_NUM] = {0}, copyVariableValue[MENU_VARIABLES_NUM] = {0};
	readAllVariablesFormStorage(storedProValue, 1);
	collectVariablesValue(menu, copyVariableValue);
	
	if(direction == 0)
	{
		writeAllVariablesToAddress(menu, storedProValue);
	}
	else
	{
		writeAllVariablesToStorage(copyVariableValue, 0);
		writeAllVariablesToStorage(copyVariableValue, 1);
	}
}

/**************** �˵����ⲿ����,ȫ�ֱ����Լ���Ҫ�Ľ��洦���� ****************/

//�˵��õ���ȫ�ֱ���
static int8 homePageCursor = 0, unitPageCursor = 0;
static uint8(*pageHandle)(uint8);
static Menu_Unit* menuPoint;
static AdjustDef Adjust[VariableType_SupportNum] = {
//����ͨ�������ýṹ��ĸ��������ﵽֱ�ӵ����˵�ÿ�ΰ��������Ҽ�������ݵ��޸�ֵ

// ��   ��   ��   ��   ���ֵ   ��Сֵ
	 0,   0,   0,   0,       0,      0,   //void
	 5,  -5,  -1,   1,     127,   -128,		//int8
	 5,  -5,  -1,   1,     255,   -000,		//uint8
	 5,  -5,  -1,   1,     999,   -999,		//int16
	 5,  -5,  -1,   1,    9999,  -0000,		//uint16
	 5,  -5,  -1,   1,     999,   -999,		//int32
	 5,  -5,  -1,   1,    9999,  -0000,		//uint32
	10, -10,  -1,   1,    9999,  -9999,		//float(ʵ�ʵ���ֵ���100)
	10, -10,  -1,   1,    9999,  -9999,		//double(ʵ�ʵ���ֵ���100)
};

//-------------------------------------------------------------------------
//  @����   �˵���ʼ������
//  @����   menu:�˵���ʼ���ṹ��
//  @ע��   �ڵ��øú���ǰ,��ȷ���ⲿ��Դ(GPIO,�ⲿ�洢��,�ⲿ��ʾ��)�ĳ�ʼ��
//-------------------------------------------------------------------------
void Menu_Init(Menu menu)
{
	uint8 ifChanged = 0; 
	menuPoint = menu;
	
	ifChanged = checkIfVariablesChanged(menu);
		
#if MENU_ENABLE_WARNPAGE //���ENABLE����ҳ
	if(ifChanged == 0) //�������û�жԱ���ֵ�����޸�,��Ĭ��ʹ�ñ����ڴ洢���е�ֵ��Ϊ��������ֵ
	{
		coverVariablesToAddressOrStorage(menu, ifChanged);
		pageHandle = &homePageHandle;
	}
	else
	{
		pageHandle = &warnPageHandle;
	}
#else //���DISABLE����ҳ
	coverVariablesToAddressOrStorage(menu, ifChanged);
	pageHandle = &warnPageHandle; //��仰û����,��Ҫ��Ϊ������warnPageHandleû�ñ����õľ���
	pageHandle = &homePageHandle;
#endif
}

//-------------------------------------------------------------------------
//  @����   �˵�������
//  @����   buttonFlag:����ֵ
//-------------------------------------------------------------------------
static uint8 homePageHandle(uint8 buttonFlag)
{
	static uint8 isInited = 0;
	uint8 returnFlag = 0;
	
	if(isInited == 0) //�����ʼ��,ֻ�ڽ���ý���ʱִ��һ��,���˳����������
	{
		isInited = 1;
		__showHomePage(menuPoint, 1, 0);
	}
	
	if(buttonFlag & ButtonFlag_Udlr) //�ڸý�����,�������Ұ������ƹ���ƶ�
	{
		if(buttonFlag & ButtonFlag_Up) homePageCursor -= MENU_VARIABLE_PERLINE;
		if(buttonFlag & ButtonFlag_Down) homePageCursor += MENU_VARIABLE_PERLINE;
		if(buttonFlag & ButtonFlag_Left) homePageCursor -= 1;
		if(buttonFlag & ButtonFlag_Right) homePageCursor += 1;
		
		if(homePageCursor < 0) homePageCursor += (MENU_UNITS_NUM + 1);
		if(homePageCursor >= (MENU_UNITS_NUM + 1)) homePageCursor -= (MENU_UNITS_NUM + 1);
	}
	
	if(buttonFlag & ButtonFlag_Confirm)
	{
		if(homePageCursor != 0)
		{
			if(menuPoint[homePageCursor - 1].UintTitle != 0) //����˵���Ԫ����
			{
				isInited = 0;
				pageHandle = &unitPageHandle;
			}
		}
		else if(homePageCursor == 0) //�����CloseMenuѡ��,���˳��˵�
		{
			returnFlag = 1;
		}
	}
	
	__showHomePage(menuPoint, 0, homePageCursor); //���¹��
	if(returnFlag) __changePages();
	return returnFlag;
}

//-------------------------------------------------------------------------
//  @����   �˵���Ԫ����
//  @����   buttonFlag:����ֵ
//  @ע��   �Ƚϸ���,������Ļ�,����Ҫÿ�仰������
//-------------------------------------------------------------------------
static uint8 unitPageHandle(uint8 buttonFlag)
{
	static uint8 confirmFlag = 0;
	static uint8 isInited = 0;
	static int16 tempVariablesValue[MENU_UNIT_VARIABLES_NUM] = {0};
	static Menu_Unit* currentMenuUint;
	
	if(isInited == 0) //�����ʼ��,ֻ�ڽ���ý���ʱִ��һ��,���˳����������
	{
		isInited = 1;
		unitPageCursor = -1;
		currentMenuUint = &menuPoint[homePageCursor - 1]; //��ǰ�Ĳ˵���Ԫ��ָ��
		
		for(int temp = 0; temp < MENU_UNIT_VARIABLES_NUM; ++temp)
		{
			tempVariablesValue[temp] = readSingleVariableFromAddress(currentMenuUint->VariableAddr[temp], currentMenuUint->VariableType[temp]);
			if(currentMenuUint->VariableType[temp] != VariableType_Void && unitPageCursor == -1) //�ù��ָ���һ������
				unitPageCursor = temp * MENU_VARIABLE_PERLINE;
		}
		if(unitPageCursor == -1)
		{
			unitPageCursor = ((MENU_LINE_SAVE_BUTTON > MENU_LINE_BACK_BUTTON ? MENU_LINE_BACK_BUTTON : MENU_LINE_SAVE_BUTTON) - 1) * MENU_VARIABLE_PERLINE; //����Ҳ�������,��ָ��"BACK"����"SAVE"
		}
		__showUintPage(currentMenuUint, tempVariablesValue, 1, 0, 0); //��ʾ�˵���Ԫ����
	}
	
	if(buttonFlag & ButtonFlag_Udlr) //������������ұ�����
	{
		if(confirmFlag == 0)
		{ //���û�н����������״̬,����ƹ���ƶ�,���ڸý�������ܲ��������ƶ�,�������if���ӱȽϸ���,������û��ϵ
			if(buttonFlag & ButtonFlag_Up || buttonFlag & ButtonFlag_Left)
			{
				unitPageCursor -= MENU_VARIABLE_PERLINE;
				if(unitPageCursor < 0) unitPageCursor += MENU_LINE_MAX * MENU_VARIABLE_PERLINE;
				if(unitPageCursor >= MENU_LINE_MAX * MENU_VARIABLE_PERLINE) unitPageCursor -= MENU_LINE_MAX * MENU_VARIABLE_PERLINE;
				
				while(unitPageCursor / MENU_VARIABLE_PERLINE >= MENU_UNIT_VARIABLES_NUM
					&& unitPageCursor / MENU_VARIABLE_PERLINE < (MENU_LINE_BACK_BUTTON > MENU_LINE_SAVE_BUTTON ? MENU_LINE_SAVE_BUTTON : MENU_LINE_BACK_BUTTON) - 1)
					unitPageCursor -= MENU_VARIABLE_PERLINE;
				while(unitPageCursor / MENU_VARIABLE_PERLINE < MENU_UNIT_VARIABLES_NUM
					&& unitPageCursor / MENU_VARIABLE_PERLINE >= 0
					&& currentMenuUint->VariableType[unitPageCursor / 2] == VariableType_Void)
					unitPageCursor -= MENU_VARIABLE_PERLINE;
			}
			if(buttonFlag & ButtonFlag_Down || buttonFlag & ButtonFlag_Right)
			{
				unitPageCursor += MENU_VARIABLE_PERLINE;
				if(unitPageCursor < 0) unitPageCursor += MENU_LINE_MAX * MENU_VARIABLE_PERLINE;
				if(unitPageCursor >= MENU_LINE_MAX * MENU_VARIABLE_PERLINE) unitPageCursor -= MENU_LINE_MAX * MENU_VARIABLE_PERLINE;
				
				while(unitPageCursor / MENU_VARIABLE_PERLINE < MENU_UNIT_VARIABLES_NUM
					&& unitPageCursor / MENU_VARIABLE_PERLINE >= 0
					&& currentMenuUint->VariableType[unitPageCursor / 2] == VariableType_Void)
					unitPageCursor += MENU_VARIABLE_PERLINE;
				while(unitPageCursor / MENU_VARIABLE_PERLINE >= MENU_UNIT_VARIABLES_NUM
					&& unitPageCursor / MENU_VARIABLE_PERLINE < (MENU_LINE_BACK_BUTTON > MENU_LINE_SAVE_BUTTON ? MENU_LINE_SAVE_BUTTON : MENU_LINE_BACK_BUTTON) - 1)
					unitPageCursor += MENU_VARIABLE_PERLINE;
			}
			if(unitPageCursor < 0) unitPageCursor += MENU_LINE_MAX * MENU_VARIABLE_PERLINE;
			if(unitPageCursor >= MENU_LINE_MAX * MENU_VARIABLE_PERLINE) unitPageCursor -= MENU_LINE_MAX * MENU_VARIABLE_PERLINE;
		}
		else //����,������ֵ
		{
			for(int index = 0, temp = buttonFlag; index < 4; ++index) //�������Ұ�������,����һ��forѭ����д,�Ƚ�����
			{
				if(temp & 0x01) //������ֵ��tempVariablesValue��
				{
					int serialPressed = 1;
					if(buttonFlag & ButtonFlag_SerialPressed) serialPressed = 4; //����ж�Ϊ����,�򵥴ε�������ֵ����
					VariableTypeDef type = currentMenuUint->VariableType[unitPageCursor / MENU_VARIABLE_PERLINE];
					int16 adjust = Adjust[type].AdjustValue[index]; //���ݵ�ǰ����������,��Adjust�ṹ���л�ȡ���ε�������ֵ
					tempVariablesValue[unitPageCursor / MENU_VARIABLE_PERLINE] += adjust * serialPressed;
					if(tempVariablesValue[unitPageCursor / MENU_VARIABLE_PERLINE] >= Adjust[type].AdjustMax) tempVariablesValue[unitPageCursor / MENU_VARIABLE_PERLINE] = Adjust[type].AdjustMax;
					else if(tempVariablesValue[unitPageCursor / MENU_VARIABLE_PERLINE] <= Adjust[type].AdjustMin) tempVariablesValue[unitPageCursor / MENU_VARIABLE_PERLINE] = Adjust[type].AdjustMin;
				}
				temp >>= 1;
			}
		}
	}
	
	if(buttonFlag & ButtonFlag_Confirm) //�������ȷ������
	{
		if(unitPageCursor == (MENU_LINE_BACK_BUTTON - 1) * MENU_VARIABLE_PERLINE || unitPageCursor == (MENU_LINE_SAVE_BUTTON - 1) * MENU_VARIABLE_PERLINE) //������µ���SAVE����BACK��ʱ��
		{
			if(unitPageCursor == (MENU_LINE_SAVE_BUTTON - 1) * MENU_VARIABLE_PERLINE) //������ָ��SAVE
			{
				for(int temp = 0; temp < MENU_UNIT_VARIABLES_NUM; ++temp) //��tempVariablesValue���浽������ַ��
				{
					writeSingleVariableToAddress(currentMenuUint->VariableAddr[temp], currentMenuUint->VariableType[temp], tempVariablesValue[temp]);
				}
				int16 copyVariableValue[MENU_VARIABLES_NUM] = {0};
				collectVariablesValue(menuPoint, copyVariableValue);
				writeAllVariablesToStorage(copyVariableValue, 1); //������ֵ���浽��������
			}
			isInited = 0;
			pageHandle = &homePageHandle; //�˳��ý���,ת���˵�������
		}
		else if(unitPageCursor < MENU_UNIT_VARIABLES_NUM * MENU_VARIABLE_PERLINE) //������ָ�����
		{
			confirmFlag ^= 1;
		}
	}
	
	__showUintPage(currentMenuUint, tempVariablesValue, 0, unitPageCursor, confirmFlag); //���¹��
	return 0;
}

//-------------------------------------------------------------------------
//  @����   �������
//  @����   buttonFlag:����ֵ
//-------------------------------------------------------------------------
static uint8 warnPageHandle(uint8 buttonFlag)
{
	static uint8 isInited = 0;
	static int8 warmPageCursor = (MENU_LINE_MAX - 1) * 2;
	
	if(isInited == 0) //�����ʼ��,ֻ�ڽ���ý���ʱִ��һ��,���˳����������
	{
		isInited = 1;
		warmPageCursor = (MENU_LINE_MAX - 1) * 2;
		__showWarnPage("Some variables have been changed. Would  you like to use the  new value?", 1, warmPageCursor); //��ʾ�ý���
	}
	
	if(buttonFlag & ButtonFlag_Udlr) //�ڸý�����,�������Ұ������ƹ���ƶ�
	{
		if(buttonFlag & ButtonFlag_Up) warmPageCursor -= 1;
		if(buttonFlag & ButtonFlag_Down) warmPageCursor += 1;
		if(buttonFlag & ButtonFlag_Left) warmPageCursor -= 1;
		if(buttonFlag & ButtonFlag_Right) warmPageCursor += 1;
		
		if(warmPageCursor < (MENU_LINE_MAX - 1) * 2) warmPageCursor += 2;
		if(warmPageCursor >= MENU_LINE_MAX * 2) warmPageCursor -= 2;
	}
	
	if(buttonFlag & ButtonFlag_Confirm) //�������"YES"����"NO"
	{
		coverVariablesToAddressOrStorage(menuPoint, (warmPageCursor + 1) % 2);
		pageHandle = &homePageHandle;
		__changePages();
	}
	
	__showWarnPage("", 0, warmPageCursor); //���¹��
	return 0;
}

//-------------------------------------------------------------------------
//  @����   �˵���������
//  @����   0:�˵�û�а��¡�CloseMenu��ѡ�� 1:�˵����¡�CloseMenu��ѡ��
//  @ע��   ����ÿ��200msִ��һ�θú���,��:while(!Menu_Work()) systick_delay_ms(200);
//-------------------------------------------------------------------------
uint8 Menu_Work(void)
{
	int8 buttonFlag = 0, returnFlag = 0;
	
	buttonFlag = buttonGet(); //��ⰴ��
	
	//pageHandleΪ����ָ��,����ָ��warnPageHandle,homePageHandle,unitPageHandle,ָ��ͬ�ĺ���,����ʾ��ͬ�Ľ���
	returnFlag = pageHandle(buttonFlag);

	if(buttonFlag != ButtonFlag_Void && !returnFlag) __setBuzzer(1);
	else __setBuzzer(0);
	return returnFlag;
}
