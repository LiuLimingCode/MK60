/*!
 * @�ļ�       ImageProcess.c
 * @����       ����ͼ��Ĵ�����,��Ϊ������ĳ��򴫳бȽ�����,��ϣ��ÿ���������Ա�ܹ�
 *             �ܽ����õ�ͼ�����㷨������ļ���.������ļ�Ϊ������������ͷͼ�������
 */

#include "ImageProcess.h"
#include <math.h>

//----------------------------------------------------------------------------------------
//  @����       �Ҷ�ͼ��ֵ��ֵ��
//  @����       grayscale       �Ҷ�ͼ���ݵ�ַ
//  @����       binary          ��ֵ��ͼ���ݵ�ַ
//  @����       threshold       �趨��ֵ
//  @����       size            ͼ�����ݵĸ���
//  @ʾ��       GrayscaleToBinary (grayscale, binary, 128, COL*ROW);
//----------------------------------------------------------------------------------------
void GrayscaleToBinary(uint8 *grayscale, uint8 *binary, uint8 threshold, uint32 size)
{
	for(uint32 index = 0; index < size; ++index)
	binary[index] = (grayscale[index] > threshold) ? IMAGE_BLACK : IMAGE_WHITE;
}

//----------------------------------------------------------------------------------------
//  @����       ʹ�ô��ɷ����лҶ�ͼ��ֵ��
//  @����       grayscale	      �Ҷ�ͼ���ݵ�ַ
//  @����       col			        �Ҷ�ͼ������
//  @����       row	            �Ҷ�ͼ������
//  @ʾ��       OSTUThreshold (grayscale, COL, ROW);
//  @ע��       ������ֵ��һ��Ҫ��ԭͼ��
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
	uint8* data = grayscale;  //ָ���������ݵ�ָ��
	for (i = 0; i < GrayScale; i++)
	{
		pixelCount[i] = 0;
		pixelPro[i] = 0;
	}
	
	uint32 gray_sum=0;
	//ͳ�ƻҶȼ���ÿ������������ͼ���еĸ���  
	for (i = 0; i < height; i+=2)
	{
		for (j = 0; j < width; j+=2)
		{
			pixelCount[(int)data[i * width + j]]++;  //����ǰ�ĵ������ֵ��Ϊ����������±�
			gray_sum+=(int)data[i * width + j];	   //�Ҷ�ֵ�ܺ�
		}
	}
	
	//����ÿ������ֵ�ĵ�������ͼ���еı���  
  
	for (i = 0; i < GrayScale; i++)
	{
		pixelPro[i] = (float)pixelCount[i] / pixelSum;
	}

	//�����Ҷȼ�[0,255]  
	float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
	
	w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
	for (j = 0; j < GrayScale; j++)         
	{
		w0 += pixelPro[j];  //��������ÿ���Ҷ�ֵ�����ص���ռ����֮��   ���������ֵı���
		u0tmp += j * pixelPro[j];  //�������� ÿ���Ҷ�ֵ�ĵ�ı��� *�Ҷ�ֵ 
	   
		w1=1-w0;
		u1tmp=gray_sum/pixelSum-u0tmp;

		u0 = u0tmp / w0;              //����ƽ���Ҷ�
		u1 = u1tmp / w1;              //ǰ��ƽ���Ҷ�
		u = u0tmp + u1tmp;            //ȫ��ƽ���Ҷ�
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
//  @����       ���������ͷͼ��������λ���鿴ͼ��
//  @����       uartn           ���ݷ��͵Ĵ��ں�
//  @����       sendImage       ���͵�ͼ��
//  @����       size            ͼ���С
//  @ע��       ���øú���ǰ���ȳ�ʼ����Ӧ�Ĵ���,�÷��ͺ��������������λ��
//----------------------------------------------------------------------------------------
void GrayscaleSend(UARTn uartn, uint8 *sendImage, uint32 size)
{
	uart_putchar(uartn,0x00);uart_putchar(uartn,0xff);uart_putchar(uartn,0x01);uart_putchar(uartn,0x01);//��������
	uart_putbuff(uartn, (uint8_t *)sendImage, size);  //����ͼ��
}
