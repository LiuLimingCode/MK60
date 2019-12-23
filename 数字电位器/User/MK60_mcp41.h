/*!
 * @file       mcp41.h
 * @brief      mcp41数字电位器函数实现
 * @author     llm
 */
#ifndef __MK60_MCP41_H
#define __MK60_MCP41_H

#include "headfile.h"

#define MCP41_SCK		E2
#define MCP41_SDA		E1

#define SPI_MCP41_SCKL	gpio_set(MCP41_SCK, 0)
#define SPI_MCP41_SCKH	gpio_set(MCP41_SCK, 1)
#define SPI_MCP41_SDAL	gpio_set(MCP41_SDA, 0)
#define SPI_MCP41_SDAH	gpio_set(MCP41_SDA, 1)

#define CS0		C0
#define CS1		C0
#define CS2		C0
#define CS3		C0
#define CS4		C0
#define CS5		0xff
#define CS6		0xff
#define CS7		0xff

#define P0		ADC0_SE8
#define P1		ADC0_SE8
#define P2		ADC0_SE8
#define P3		ADC0_SE8
#define P4		ADC0_SE8
#define P5		0xff
#define P6		0xff
#define P7		0xff

void MCP41_Init(void);
void MCP41_SetCmd(uint8 Set_Num ,uint8 data);
uint8 MCP41_SetValue(uint8 Set_Num, uint8 Ask_Value);

#endif
