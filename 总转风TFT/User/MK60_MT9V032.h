/*!
 * @file       mt9v032.h
 * @brief      逐飞总转风摄像头实现
 * @author     llm
 * @注意：			 如果硬件修改了摄像头的引脚,应该将引脚配置中的宏重新配置,若配置正确即可使用
							 中断服务函数全部写在.c文件中,不建议加入自己程序
 */
#ifndef __MK60_MT9V032_H
#define __MK60_MT9V032_H

#include "headfile.h"

#define WIDTH   188//图像宽度   范围1-752     K60采集不允许超过188
#define HEIGHT  120//图像高度	范围1-480

#define BLACK		0x00
#define WHITE		0xff


//摄像头命令枚举
typedef enum
{
    INIT = 0,               //摄像头初始化命令
    AUTO_EXP,               //自动曝光命令
    EXP_TIME,               //曝光时间命令
    FPS,                    //摄像头帧率命令
    SET_COL,                //图像列命令
    SET_ROW,                //图像行命令
    LR_OFFSET,              //图像左右偏移命令
    UD_OFFSET,              //图像上下偏移命令
    GAIN,                   //图像偏移命令
    CONFIG_FINISH,          //非命令位，主要用来占位计数
    
    SET_EXP_TIME = 0XF0,    //单独设置曝光时间命令
    GET_STATUS,             //获取摄像头配置命令
    GET_VERSION,            //固件版本号命令
    
    SET_ADDR = 0XFE,        //寄存器地址命令
    SET_DATA                //寄存器数据命令
}CMD;

extern uint8 mt9v032_finish_flag;  //一场图像采集完成标志位


//////////////////////////引脚配置//////////////////////////

#define MT9V032_COF_UART    				uart3  													//配置摄像头所使用到的串口
#define MT9V032_COF_UART_INT				UART3_RX_TX_IRQHandler					//串口的中断函数
#define MT9V032_DMA_CH							DMA_CH0													//摄像头使用的DMA通道
#define MT9V032_DMA_CH_INT					DMA0_IRQHandler									//DMA通道的中断函数
#define MT9V032_PCLK                C18                            	//摄像头像素时钟
#define MT9V032_VSYNC               C6                            	//摄像头场同步信号
#define MT9V032_VSYNC_INT						PORTC_IRQHandler								//场同步信号的中断函数

#define MT9V032_DATAPORT 		C_IN_DATA(1)	                					//DMA数据口
//--------------------------------------------------------------------------------------------------
//        摄像头数据接口
//        C_IN_DATA(1)
//        第一个字母C代表着使用C口，可以选择A、B、C、D、E.分别对应A_IN_DATA，B_IN_DATA，C_IN_DATA，D_IN_DATA，E_IN_DATA
//        第二个括号里面的参数1代表着使用的是C8 - C15口,
//		  可以选择0、1、2、3，分别对应的是	该端口的X0-X7，X8-X15，X16-X23，X24-X31(这里x代表的是第一个字母，比如现在的就是C8-C15)
//--------------------------------------------------------------------------------------------------

void MT9V032_Init(void *imgaddr);
void GrayscaleScaledHalf(uint8 *grayscale, uint8 *grayscaleHalf, uint16 width, uint16 height);
void GrayscaleToBinary(uint8 *Grayscale, uint8 *Binary, uint8 Threshold, uint32 size);
void GrayscaleSend(UARTn uartn, unsigned char *grayscale, uint16 width, uint16 height);
int OSTUThreshold(uint8 *grayscale, uint8 *binary, uint32 size);
#endif
