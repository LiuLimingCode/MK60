/*!
 * @file       mt9v032.h
 * @brief      逐飞总转风摄像头实现
 * @author     llm
 * @注意：			 如果硬件修改了摄像头的引脚,应该将引脚配置中的宏重新配置,若配置正确即可使用
							 中断服务函数全部写在.c文件中,不建议加入自己程序
							 
							 逐飞默认MT9V032接线定义：
								------------------------------------ 
									模块管脚            单片机管脚
									SDA(51的RX)         C17
									SCL(51的TX)         C16
									场中断              C6
									行中断				未使用，因此不接
									像素中断            C18           
									数据口              C8-C15 
								------------------------------------ 

								默认分辨率是            188*120
								默认FPS                 50帧
 */
#include "MK60_MT9V032.h"

static void get_config(void);

void   *mt9v032_img_buff;
uint8   receive[3];
uint8   receive_num = 0;
uint8   uart_receive_flag = 1;
uint8   mt9v032_finish_flag = 0;      //一场图像采集完成标志位

//需要配置到摄像头的数据
int16 MT9V032_CFG[CONFIG_FINISH][2]=
{
    {AUTO_EXP,          0},   //自动曝光设置      范围1-63 0为关闭 如果自动曝光开启  EXP_TIME命令设置的数据将会变为最大曝光时间，也就是自动曝光时间的上限
                              //一般情况是不需要开启这个功能，因为比赛场地光线一般都比较均匀，如果遇到光线非常不均匀的情况可以尝试设置该值，增加图像稳定性
    {EXP_TIME,          300}, //曝光时间          摄像头收到后会自动计算出最大曝光时间，如果设置过大则设置为计算出来的最大曝光值
    {FPS,               30},  //图像帧率          摄像头收到后会自动计算出最大FPS，如果过大则设置为计算出来的最大FPS
    {SET_COL,           WIDTH}, //图像列数量        范围1-752     K60采集不允许超过188
    {SET_ROW,           HEIGHT}, //图像行数量        范围1-480
    {LR_OFFSET,         0},   //图像左右偏移量    正值 右偏移   负值 左偏移  列为188 376 752时无法设置偏移    摄像头收偏移数据后会自动计算最大偏移，如果超出则设置计算出来的最大偏移
    {UD_OFFSET,         0},   //图像上下偏移量    正值 上偏移   负值 下偏移  行为120 240 480时无法设置偏移    摄像头收偏移数据后会自动计算最大偏移，如果超出则设置计算出来的最大偏移
    {GAIN,              32},  //图像增益          范围16-64     增益可以在曝光时间固定的情况下改变图像亮暗程度
    {INIT,              0}    //摄像头开始初始化
};

//从摄像头内部获取到的配置数据
int16 GET_CFG[CONFIG_FINISH-1][2]=
{
    {AUTO_EXP,          0},   //自动曝光设置      
    {EXP_TIME,          0},   //曝光时间          
    {FPS,               0},   //图像帧率          
    {SET_COL,           0},   //图像列数量        
    {SET_ROW,           0},   //图像行数量        
    {LR_OFFSET,         0},   //图像左右偏移量    
    {UD_OFFSET,         0},   //图像上下偏移量    
    {GAIN,              0},   //图像增益          
};

/*!
*  @描述       MV9V032摄像头初始化程序
*  @参数       imgaddr   		图片存储的地址
*  示例:       MV9V032_Init (image);    //初始化MV9V032摄像头并且以image为图片存储地址
*/
static IRQn_Type PORTX_IRQn[] = { PORTA_IRQn, PORTB_IRQn, PORTC_IRQn, PORTD_IRQn, PORTE_IRQn, PORTF_IRQn };
static PORT_Type * PORTX[] = PORT_BASES;

#define MT9V032_VSYNC_CHANNEL       (MT9V032_VSYNC & 0x1f)          //MT9V032_VSYNC对应引脚的引脚号 PTC6 --> 6
#define MT9V032_VSYNV_PORT					(MT9V032_VSYNC >> 5)						//MT9V032_VSYNC对应引脚的端口号 PTC6 --> PTC
#define MT9V032_INTERRUPT_NUNBERS   PORTX_IRQn[MT9V032_VSYNV_PORT]  //中断编号		PTC6 --> PORTC_IRQn
#define MT9V032_INTERRUPT_ISFR      PORTX[MT9V032_VSYNV_PORT]->ISFR //中断标志位 PTC6 --> PORTC->ISFR
void MT9V032_Init(void *imgaddr)
{
    //设置参数    具体请参看使用手册
    uint16 temp, i;
    uint8  send_buffer[4];
    
    mt9v032_img_buff = imgaddr;
    
    uart_init (MT9V032_COF_UART, 9600);	//初始换串口 配置摄像头    
    
    uart_rx_irq_en(MT9V032_COF_UART);
    
    //等待摄像头上电初始化成功
    systick_delay_ms(1000);
    uart_receive_flag = 0;
    
    //开始配置摄像头并重新初始化
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
    //等待摄像头初始化成功
    while(!uart_receive_flag);
    uart_receive_flag = 0;
    while((0xff != receive[1]) || (0xff != receive[2]));
    //以上部分对摄像头配置的数据全部都会保存在摄像头上51单片机的eeprom中
    //利用set_exposure_time函数单独配置的曝光数据不存储在eeprom中
    
    //获取配置便于查看配置是否正确
    get_config();

    //摄像头采集初始化
    DisableInterrupts;
    //DMA通道0初始化，PTC18触发源(默认上升沿)，源地址为C_IN_DATA(1)(PTC8-PTC15)，目的地址为：image，每次传输1Byte 传输完毕保持目的地址
    
    dma_portx2buff_init(MT9V032_DMA_CH, (void *)&MT9V032_DATAPORT, mt9v032_img_buff, MT9V032_PCLK, DMA_BYTE1, HEIGHT*WIDTH, DADDR_KEEPON);
    port_init(MT9V032_PCLK, ALT1 | DMA_FALLING | PULLDOWN);  	//PCLK  触发源设置
    DMA_DIS(MT9V032_DMA_CH);                                  //禁用DMA通道
    DMA_IRQ_CLEAN(MT9V032_DMA_CH);													  //清除通道传输中断标志位
    DMA_IRQ_EN(MT9V032_DMA_CH);                               //允许DMA通道中断
    DMA_EN(MT9V032_DMA_CH);  																	//使能DMA
	
	disable_irq(MT9V032_INTERRUPT_NUNBERS);                     //关闭PTC的中断
    //port_init(C7, ALT1 | IRQ_FALLING | PULLDOWN);      			//行中断
    
    port_init(MT9V032_VSYNC, ALT1 | IRQ_FALLING | PULLDOWN);    //场中断，下降沿触发中断、下拉
    set_irq_priority(MT9V032_INTERRUPT_NUNBERS,1);              // 中断优先级
    enable_irq (MT9V032_INTERRUPT_NUNBERS);
    EnableInterrupts;
}

/*!
*  @描述       灰度图缩放到原来的一般
*  @参数			 grayscale			灰度图数据地址
*  @参数       grayscaleHalf	灰度图缩放数据地址
*  @参数       width					原图的宽
*  @参数       height					原图的高
*  示例:       GrayscaleScaledHalf (grayscale, grayscaleHalf, WIDTH, HEIGHT);
*  注意:			 当原图的长或者宽为奇数时,缩放后的长宽为(x - 1) / 2,例如一个51*61大小的图片缩放后为25*30大小
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
*  @描述       灰度图阈值二值化
*  @参数			 grayscale	灰度图数据地址
*  @参数       binary			二值化图数据地址
*  @参数       threshold	设定阈值
*  @参数       size				图像数据的个数
*  示例:       GrayscaleToBinary (grayscale, binary, 128, WIDTH*HEIGHT);
*/
void GrayscaleToBinary(uint8 *grayscale, uint8 *binary, uint8 threshold, uint32 size)
{
	for(uint32 index = 0; index < size; ++index)
		binary[index] = (grayscale[index] > threshold) ? BLACK : WHITE;
}

/*!
*  @描述       大律法
*  @参数			 grayscale	灰度图数据地址
*  @参数       binary			二值化图数据地址
*  @参数       size				图像数据的个数
*  @返回			 计算出来的阈值
*  示例:       OSTUThreshold (grayscale, binary, WIDTH*HEIGHT);
*/
int OSTUThreshold(uint8 *grayscale, uint8 *binary, uint32 size)
{
    int Threshold = 0;
    int HistGram[256] = {0};
    int Y, Amount = 0;
    int PixelBack = 0, PixelFore = 0, PixelIntegralBack = 0, PixelIntegralFore = 0, PixelIntegral = 0;
    double OmegaBack, OmegaFore, MicroBack, MicroFore, SigmaB, Sigma;              // 类间方差;
    int MinValue, MaxValue;

    for(int temp = 0; temp < size; ++temp)
        HistGram[grayscale[temp]]++;

    for (MinValue = 0; MinValue < 256 && HistGram[MinValue] == 0; MinValue++) ;
    for (MaxValue = 255; MaxValue > MinValue && HistGram[MaxValue] == 0; MaxValue--) ;

    for (Y = MinValue; Y <= MaxValue; Y++) Amount += HistGram[Y];        //  像素总数

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
*  @描述       发送灰度图数据到上位机
*  @参数       uartn   		发送的UART号
*  @参数			 grayscale	灰度图数据地址
*  @参数       width			灰度图的宽
*  @参数       height			灰度图的高
*  注意:       这是一个通信协议,协议的规定:每次发送数据开始都要发送两个起始位(0x4C),再发送图片的宽和高,
							 然后发送图像数据,但是发送数据中每当遇到和起始位(0x4C)和结束位(0x55)和数据位(0x49)时,在
							 前面加上一个数据位,即用0x49,0x4C和0x49,0x55和0x49,0x49替换他们
							 例如： 发送一个2*3数据为0x4C,0x00,0x49,0x55,0x55,0x10的图像
							 发送格式为：0x4C,0x4C,  0x02,0x03,  0x49,0x4C,  0x00,  0x49,0x49,  0x49,0x55,  0x49,0x55,  0x10,  0x55,0x55
*  示例:       GrayscaleSend (uart4, image, 188, 120);    //用uart4发送一个188*120的灰度图image
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

/////////////////////////中断服务程序和内部函数////////////////////////////////
//-------------------------------------------------------------------------------------------------------------------
//  @brief      获取摄像头内部配置信息
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:				调用该函数前请先初始化uart3
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
        
        //等待接受回传数据
        while(!uart_receive_flag);
        uart_receive_flag = 0;
        
        GET_CFG[i][1] = receive[1]<<8 | receive[2];
        
    }
}

void MT9V032_VSYNC_INT(void)
{
	//清除中断标志第一种方法直接操作寄存器，每一位对应一个引脚
	MT9V032_INTERRUPT_ISFR = 0xffffffff;
	//使用我们编写的宏定义清除发生中断的引脚
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
    if(UARTX[MT9V032_COF_UART]->S1 & UART_S1_RDRF_MASK)                                     //接收数据寄存器满
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
