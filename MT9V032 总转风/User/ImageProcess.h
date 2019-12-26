/*!
 * @文件       ImageProcess.c
 * @功能       包含图像的处理函数,因为摄像组的程序传承比较困难,故希望每届摄像组队员能够
 *             总结有用的图像处理算法到这个文件中.以这个文件为基础传承摄像头图像处理程序
 */
 
#ifndef __IMAGEPROCESS_H
#define __IMAGEPROCESS_H

#include "headfile.h"

/***************** 重要宏定义 *****************/
#define IMAGE_BLACK     0xff
#define IMAGE_WHITE     0x00

/****************** 可调用函数 ******************/
void GrayscaleToBinary(uint8 *grayscale, uint8 *binary, uint8 threshold, uint32 size);
uint8 OSTUThreshold(uint8 *image, uint16 col, uint16 row);
void GrayscaleSend(UARTn uartn, uint8 *sendImage, uint32 size);

#endif
