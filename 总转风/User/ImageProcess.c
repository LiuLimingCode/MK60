/*!
 * @文件       ImageProcess.c
 * @功能       包含图像的处理函数,因为摄像组的程序传承比较困难,故希望每届摄像组队员能够
 *             总结有用的图像处理算法到这个文件中.以这个文件为基础传承摄像头图像处理程序
 */

#include "ImageProcess.h"
#include <math.h>

//----------------------------------------------------------------------------------------
//  @描述       灰度图阈值二值化
//  @参数       grayscale       灰度图数据地址
//  @参数       binary          二值化图数据地址
//  @参数       threshold       设定阈值
//  @参数       size            图像数据的个数
//  @示例       GrayscaleToBinary (grayscale, binary, 128, COL*ROW);
//----------------------------------------------------------------------------------------
void GrayscaleToBinary(uint8 *grayscale, uint8 *binary, uint8 threshold, uint32 size)
{
	for(uint32 index = 0; index < size; ++index)
	binary[index] = (grayscale[index] > threshold) ? IMAGE_BLACK : IMAGE_WHITE;
}

//----------------------------------------------------------------------------------------
//  @描述       使用大律法进行灰度图二值化
//  @参数       grayscale	      灰度图数据地址
//  @参数       col			        灰度图的列数
//  @参数       row	            灰度图的行数
//  @示例       OSTUThreshold (grayscale, COL, ROW);
//  @注意       计算阈值的一定要是原图像
//----------------------------------------------------------------------------------------
uint8 OSTUThreshold(uint8 *grayscale, uint16 col, uint16 row)
{
	#define GrayScale 256
	uint16 width = col;
	uint16 height = row;
	int pixelCount[GrayScale];
	float pixelPro[GrayScale];
	int i, j, pixelSum = width * height/4;
	uint8 threshold = 0;
	uint8* data = grayscale;  //指向像素数据的指针
	for (i = 0; i < GrayScale; i++)
	{
		pixelCount[i] = 0;
		pixelPro[i] = 0;
	}
	
	uint32 gray_sum=0;
	//统计灰度级中每个像素在整幅图像中的个数  
	for (i = 0; i < height; i+=2)
	{
		for (j = 0; j < width; j+=2)
		{
			pixelCount[(int)data[i * width + j]]++;  //将当前的点的像素值作为计数数组的下标
			gray_sum+=(int)data[i * width + j];	   //灰度值总和
		}
	}
	
	//计算每个像素值的点在整幅图像中的比例  
  
	for (i = 0; i < GrayScale; i++)
	{
		pixelPro[i] = (float)pixelCount[i] / pixelSum;
	}

	//遍历灰度级[0,255]  
	float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
	
	w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
	for (j = 0; j < GrayScale; j++)         
	{
		w0 += pixelPro[j];  //背景部分每个灰度值的像素点所占比例之和   即背景部分的比例
		u0tmp += j * pixelPro[j];  //背景部分 每个灰度值的点的比例 *灰度值 
	   
		w1=1-w0;
		u1tmp=gray_sum/pixelSum-u0tmp;

		u0 = u0tmp / w0;              //背景平均灰度
		u1 = u1tmp / w1;              //前景平均灰度
		u = u0tmp + u1tmp;            //全局平均灰度
		deltaTmp = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);
		if (deltaTmp > deltaMax)
		{
			deltaMax = deltaTmp;
			threshold = j;
		}
		if (deltaTmp < deltaMax)
		{
			break;
		}
	 }

    return threshold;
}

//----------------------------------------------------------------------------------------
//  @描述       总钻风摄像头图像发送至上位机查看图像
//  @参数       uartn           数据发送的串口号
//  @参数       sendImage       发送的图像
//  @参数       size            图像大小
//  @注意       调用该函数前请先初始化相应的串口,该发送函数适用于逐飞上位机
//----------------------------------------------------------------------------------------
void GrayscaleSend(UARTn uartn, uint8 *sendImage, uint32 size)
{
	uart_putchar(uartn,0x00);uart_putchar(uartn,0xff);uart_putchar(uartn,0x01);uart_putchar(uartn,0x01);//发送命令
	uart_putbuff(uartn, (uint8_t *)sendImage, size);  //发送图像
}
