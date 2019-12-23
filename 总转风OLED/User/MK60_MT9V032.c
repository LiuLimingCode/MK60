/*!
 * @file       mt9v032.h
 * @brief      �����ת������ͷʵ��
 * @author     llm
 * @ע�⣺			 ���Ӳ���޸�������ͷ������,Ӧ�ý����������еĺ���������,��������ȷ����ʹ��
							 �жϷ�����ȫ��д��.c�ļ���,����������Լ�����
							 
							 ���Ĭ��MT9V032���߶��壺
								------------------------------------ 
									ģ��ܽ�            ��Ƭ���ܽ�
									SDA(51��RX)         C17
									SCL(51��TX)         C16
									���ж�              C6
									���ж�				δʹ�ã���˲���
									�����ж�            C18           
									���ݿ�              C8-C15 
								------------------------------------ 

								Ĭ�Ϸֱ�����            188*120
								Ĭ��FPS                 50֡
 */
#include "MK60_MT9V032.h"

static void get_config(void);

void   *mt9v032_img_buff;
uint8   receive[3];
uint8   receive_num = 0;
uint8   uart_receive_flag = 1;
uint8   mt9v032_finish_flag = 0;      //һ��ͼ��ɼ���ɱ�־λ

//��Ҫ���õ�����ͷ������
int16 MT9V032_CFG[CONFIG_FINISH][2]=
{
    {AUTO_EXP,          0},   //�Զ��ع�����      ��Χ1-63 0Ϊ�ر� ����Զ��ع⿪��  EXP_TIME�������õ����ݽ����Ϊ����ع�ʱ�䣬Ҳ�����Զ��ع�ʱ�������
                              //һ������ǲ���Ҫ����������ܣ���Ϊ�������ع���һ�㶼�ȽϾ��ȣ�����������߷ǳ������ȵ�������Գ������ø�ֵ������ͼ���ȶ���
    {EXP_TIME,          300}, //�ع�ʱ��          ����ͷ�յ�����Զ����������ع�ʱ�䣬������ù���������Ϊ�������������ع�ֵ
    {FPS,               30},  //ͼ��֡��          ����ͷ�յ�����Զ���������FPS���������������Ϊ������������FPS
    {SET_COL,           WIDTH}, //ͼ��������        ��Χ1-752     K60�ɼ���������188
    {SET_ROW,           HEIGHT}, //ͼ��������        ��Χ1-480
    {LR_OFFSET,         0},   //ͼ������ƫ����    ��ֵ ��ƫ��   ��ֵ ��ƫ��  ��Ϊ188 376 752ʱ�޷�����ƫ��    ����ͷ��ƫ�����ݺ���Զ��������ƫ�ƣ�������������ü�����������ƫ��
    {UD_OFFSET,         0},   //ͼ������ƫ����    ��ֵ ��ƫ��   ��ֵ ��ƫ��  ��Ϊ120 240 480ʱ�޷�����ƫ��    ����ͷ��ƫ�����ݺ���Զ��������ƫ�ƣ�������������ü�����������ƫ��
    {GAIN,              32},  //ͼ������          ��Χ16-64     ����������ع�ʱ��̶�������¸ı�ͼ�������̶�
    {INIT,              0}    //����ͷ��ʼ��ʼ��
};

//������ͷ�ڲ���ȡ������������
int16 GET_CFG[CONFIG_FINISH-1][2]=
{
    {AUTO_EXP,          0},   //�Զ��ع�����      
    {EXP_TIME,          0},   //�ع�ʱ��          
    {FPS,               0},   //ͼ��֡��          
    {SET_COL,           0},   //ͼ��������        
    {SET_ROW,           0},   //ͼ��������        
    {LR_OFFSET,         0},   //ͼ������ƫ����    
    {UD_OFFSET,         0},   //ͼ������ƫ����    
    {GAIN,              0},   //ͼ������          
};

/*!
*  @����       MV9V032����ͷ��ʼ������
*  @����       imgaddr   		ͼƬ�洢�ĵ�ַ
*  ʾ��:       MV9V032_Init (image);    //��ʼ��MV9V032����ͷ������imageΪͼƬ�洢��ַ
*/
static IRQn_Type PORTX_IRQn[] = { PORTA_IRQn, PORTB_IRQn, PORTC_IRQn, PORTD_IRQn, PORTE_IRQn, PORTF_IRQn };
static PORT_Type * PORTX[] = PORT_BASES;

#define MT9V032_VSYNC_CHANNEL       (MT9V032_VSYNC & 0x1f)          //MT9V032_VSYNC��Ӧ���ŵ����ź� PTC6 --> 6
#define MT9V032_VSYNV_PORT					(MT9V032_VSYNC >> 5)						//MT9V032_VSYNC��Ӧ���ŵĶ˿ں� PTC6 --> PTC
#define MT9V032_INTERRUPT_NUNBERS   PORTX_IRQn[MT9V032_VSYNV_PORT]  //�жϱ��		PTC6 --> PORTC_IRQn
#define MT9V032_INTERRUPT_ISFR      PORTX[MT9V032_VSYNV_PORT]->ISFR //�жϱ�־λ PTC6 --> PORTC->ISFR
void MT9V032_Init(void *imgaddr)
{
    //���ò���    ������ο�ʹ���ֲ�
    uint16 temp, i;
    uint8  send_buffer[4];
    
    mt9v032_img_buff = imgaddr;
    
    uart_init (MT9V032_COF_UART, 9600);	//��ʼ������ ��������ͷ    
    
    uart_rx_irq_en(MT9V032_COF_UART);
    
    //�ȴ�����ͷ�ϵ��ʼ���ɹ�
    systick_delay_ms(1000);
    uart_receive_flag = 0;
    
    //��ʼ��������ͷ�����³�ʼ��
    for(i=0; i<CONFIG_FINISH; i++)
    {
        send_buffer[0] = 0xA5;
        send_buffer[1] = MT9V032_CFG[i][0];
        temp = MT9V032_CFG[i][1];
        send_buffer[2] = temp>>8;
        send_buffer[3] = (uint8)temp;
        
        uart_putbuff(MT9V032_COF_UART,send_buffer,4);
        systick_delay_ms(2);
    }
    //�ȴ�����ͷ��ʼ���ɹ�
    while(!uart_receive_flag);
    uart_receive_flag = 0;
    while((0xff != receive[1]) || (0xff != receive[2]));
    //���ϲ��ֶ�����ͷ���õ�����ȫ�����ᱣ��������ͷ��51��Ƭ����eeprom��
    //����set_exposure_time�����������õ��ع����ݲ��洢��eeprom��
    
    //��ȡ���ñ��ڲ鿴�����Ƿ���ȷ
    get_config();

    //����ͷ�ɼ���ʼ��
    DisableInterrupts;
    //DMAͨ��0��ʼ����PTC18����Դ(Ĭ��������)��Դ��ַΪC_IN_DATA(1)(PTC8-PTC15)��Ŀ�ĵ�ַΪ��image��ÿ�δ���1Byte ������ϱ���Ŀ�ĵ�ַ
    
    dma_portx2buff_init(MT9V032_DMA_CH, (void *)&MT9V032_DATAPORT, mt9v032_img_buff, MT9V032_PCLK, DMA_BYTE1, HEIGHT*WIDTH, DADDR_KEEPON);
    port_init(MT9V032_PCLK, ALT1 | DMA_FALLING | PULLDOWN);  	//PCLK  ����Դ����
    DMA_DIS(MT9V032_DMA_CH);                                  //����DMAͨ��
    DMA_IRQ_CLEAN(MT9V032_DMA_CH);													  //���ͨ�������жϱ�־λ
    DMA_IRQ_EN(MT9V032_DMA_CH);                               //����DMAͨ���ж�
    DMA_EN(MT9V032_DMA_CH);  																	//ʹ��DMA
	
	disable_irq(MT9V032_INTERRUPT_NUNBERS);                     //�ر�PTC���ж�
    //port_init(C7, ALT1 | IRQ_FALLING | PULLDOWN);      			//���ж�
    
    port_init(MT9V032_VSYNC, ALT1 | IRQ_FALLING | PULLDOWN);    //���жϣ��½��ش����жϡ�����
    set_irq_priority(MT9V032_INTERRUPT_NUNBERS,1);              // �ж����ȼ�
    enable_irq (MT9V032_INTERRUPT_NUNBERS);
    EnableInterrupts;
}

/*!
*  @����       �Ҷ�ͼ���ŵ�ԭ����һ��
*  @����			 grayscale			�Ҷ�ͼ���ݵ�ַ
*  @����       grayscaleHalf	�Ҷ�ͼ�������ݵ�ַ
*  @����       width					ԭͼ�Ŀ�
*  @����       height					ԭͼ�ĸ�
*  ʾ��:       GrayscaleScaledHalf (grayscale, grayscaleHalf, WIDTH, HEIGHT);
*  ע��:			 ��ԭͼ�ĳ����߿�Ϊ����ʱ,���ź�ĳ���Ϊ(x - 1) / 2,����һ��51*61��С��ͼƬ���ź�Ϊ25*30��С
*/
void GrayscaleScaledHalf(uint8 *grayscale, uint8 *grayscaleHalf, uint16 width, uint16 height)
{
	const uint16 widthHalf = width / 2;
	const uint16 heightHalf = height / 2;
	for(int temph = 0; temph < heightHalf; ++temph)
	{
		for(int tempw = 0; tempw < widthHalf; ++tempw)
		{
			grayscaleHalf[widthHalf * temph + tempw] = (grayscale[(2 * temph) * width + (2 * tempw)]  + 
																									grayscale[(2 * temph) * width + (2 * tempw + 1)] + 
																									grayscale[(2 * temph + 1) * width + (2 * tempw)] + 
																									grayscale[(2 * temph + 1) * width + (2 * tempw + 1)]) / 4;
		}
	}
}

/*!
*  @����       �Ҷ�ͼ��ֵ��ֵ��
*  @����			 grayscale	�Ҷ�ͼ���ݵ�ַ
*  @����       binary			��ֵ��ͼ���ݵ�ַ
*  @����       threshold	�趨��ֵ
*  @����       size				ͼ�����ݵĸ���
*  ʾ��:       GrayscaleToBinary (grayscale, binary, 128, WIDTH*HEIGHT);
*/
void GrayscaleToBinary(uint8 *grayscale, uint8 *binary, uint8 threshold, uint32 size)
{
	for(uint32 index = 0; index < size; ++index)
		binary[index] = (grayscale[index] > threshold) ? BLACK : WHITE;
}

/*!
*  @����       ���ɷ�
*  @����			 grayscale	�Ҷ�ͼ���ݵ�ַ
*  @����       binary			��ֵ��ͼ���ݵ�ַ
*  @����       size				ͼ�����ݵĸ���
*  @����			 �����������ֵ
*  ʾ��:       OSTUThreshold (grayscale, binary, WIDTH*HEIGHT);
*/
int OSTUThreshold(uint8 *grayscale, uint8 *binary, uint32 size)
{
    int Threshold = 0;
    int HistGram[256] = {0};
    int Y, Amount = 0;
    int PixelBack = 0, PixelFore = 0, PixelIntegralBack = 0, PixelIntegralFore = 0, PixelIntegral = 0;
    double OmegaBack, OmegaFore, MicroBack, MicroFore, SigmaB, Sigma;              // ��䷽��;
    int MinValue, MaxValue;

    for(int temp = 0; temp < size; ++temp)
        HistGram[grayscale[temp]]++;

    for (MinValue = 0; MinValue < 256 && HistGram[MinValue] == 0; MinValue++) ;
    for (MaxValue = 255; MaxValue > MinValue && HistGram[MaxValue] == 0; MaxValue--) ;

    for (Y = MinValue; Y <= MaxValue; Y++) Amount += HistGram[Y];        //  ��������

    PixelIntegral = 0;
    for (Y = MinValue; Y <= MaxValue; Y++) PixelIntegral += HistGram[Y] * Y;
    SigmaB = -1;
    for (Y = MinValue; Y < MaxValue; Y++)
    {
        PixelBack = PixelBack + HistGram[Y];
        PixelFore = Amount - PixelBack;
        OmegaBack = (float)(PixelBack) / Amount;
        OmegaFore = (float)(PixelFore) / Amount;
        PixelIntegralBack += HistGram[Y] * Y;
        PixelIntegralFore = PixelIntegral - PixelIntegralBack;
        MicroBack = (float)(PixelIntegralBack) / PixelBack;
        MicroFore = (float)(PixelIntegralFore) / PixelFore;
        Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore);
        if (Sigma > SigmaB)
        {
            SigmaB = Sigma;
            Threshold = Y;
        }
    }

    for(uint32 index = 0; index < size; ++index)
		binary[index] = (grayscale[index] > Threshold) ? BLACK : WHITE;
		
		return Threshold;
}

/*!
*  @����       ���ͻҶ�ͼ���ݵ���λ��
*  @����       uartn   		���͵�UART��
*  @����			 grayscale	�Ҷ�ͼ���ݵ�ַ
*  @����       width			�Ҷ�ͼ�Ŀ�
*  @����       height			�Ҷ�ͼ�ĸ�
*  ע��:       ����һ��ͨ��Э��,Э��Ĺ涨:ÿ�η������ݿ�ʼ��Ҫ����������ʼλ(0x4C),�ٷ���ͼƬ�Ŀ�͸�,
							 Ȼ����ͼ������,���Ƿ���������ÿ����������ʼλ(0x4C)�ͽ���λ(0x55)������λ(0x49)ʱ,��
							 ǰ�����һ������λ,����0x49,0x4C��0x49,0x55��0x49,0x49�滻����
							 ���磺 ����һ��2*3����Ϊ0x4C,0x00,0x49,0x55,0x55,0x10��ͼ��
							 ���͸�ʽΪ��0x4C,0x4C,  0x02,0x03,  0x49,0x4C,  0x00,  0x49,0x49,  0x49,0x55,  0x49,0x55,  0x10,  0x55,0x55
*  ʾ��:       GrayscaleSend (uart4, image, 188, 120);    //��uart4����һ��188*120�ĻҶ�ͼimage
*/
void GrayscaleSend(UARTn uartn, unsigned char *grayscale, uint16 width, uint16 height)
{
	#define CMD_START		0x4C
	#define CMD_DATA		0x49
	#define CMD_END			0x55
	
	uint8 cmds[] = {CMD_START, CMD_START};
	uint8 cmde[] = {CMD_END, CMD_END};
	
	uart_putbuff(uartn, cmds, sizeof(cmds));
	
	if(width == CMD_START || width == CMD_DATA || width == CMD_END)
	{
		uart_putchar(uartn, CMD_DATA);
	}
	uart_putchar(uartn, width);
	if(height == CMD_START || height == CMD_DATA || height == CMD_END)
	{
		uart_putchar(uartn, CMD_DATA);
	}
	uart_putchar(uartn, height);
	
	for(int temp = 0; temp < width * height; ++temp)
	{
		if(grayscale[temp] == CMD_START || grayscale[temp] == CMD_DATA || grayscale[temp] == CMD_END)
		{
			uart_putchar(uartn, CMD_DATA);
		}
		
		uart_putchar(uartn, grayscale[temp]);
	}
	
	uart_putbuff(uartn, cmde, sizeof(cmde));
}

/////////////////////////�жϷ��������ڲ�����////////////////////////////////
//-------------------------------------------------------------------------------------------------------------------
//  @brief      ��ȡ����ͷ�ڲ�������Ϣ
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:				���øú���ǰ���ȳ�ʼ��uart3
//-------------------------------------------------------------------------------------------------------------------
static void get_config(void)
{
	uint16 temp, i;
    uint8  send_buffer[4];
    
    for(i=0; i<CONFIG_FINISH-1; i++)
    {
        send_buffer[0] = 0xA5;
        send_buffer[1] = GET_STATUS;
        temp = GET_CFG[i][0];
        send_buffer[2] = temp>>8;
        send_buffer[3] = (uint8)temp;
        
        uart_putbuff(MT9V032_COF_UART,send_buffer,4);
        
        //�ȴ����ܻش�����
        while(!uart_receive_flag);
        uart_receive_flag = 0;
        
        GET_CFG[i][1] = receive[1]<<8 | receive[2];
        
    }
}

void MT9V032_VSYNC_INT(void)
{
	//����жϱ�־��һ�ַ���ֱ�Ӳ����Ĵ�����ÿһλ��Ӧһ������
	MT9V032_INTERRUPT_ISFR = 0xffffffff;
	//ʹ�����Ǳ�д�ĺ궨����������жϵ�����
	//PORTC_FLAG_CLR(C1);
	MT9V032_INTERRUPT_ISFR = 1 << MT9V032_VSYNC_CHANNEL;
	dma_repeat(MT9V032_DMA_CH, (void *)&MT9V032_DATAPORT, mt9v032_img_buff, WIDTH*HEIGHT);
}

void MT9V032_DMA_CH_INT(void)
{
	DMA_IRQ_CLEAN(MT9V032_DMA_CH);
  mt9v032_finish_flag = 1;
}

static UART_Type * UARTX[] = UART_BASES;

void MT9V032_COF_UART_INT(void)
{
    if(UARTX[MT9V032_COF_UART]->S1 & UART_S1_RDRF_MASK)                                     //�������ݼĴ�����
    {
        uart_getchar(MT9V032_COF_UART, &receive[receive_num]);
				receive_num++;
				
				if(1==receive_num && 0XA5!=receive[0])  receive_num = 0;
				if(3 == receive_num)
				{
						receive_num = 0;
						uart_receive_flag = 1;
				}
    }
}
