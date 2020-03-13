#include "config.h"
#include <STC15F2K60S2.H>
#include "stdio.h"
#include "type.h"
#include "intrins.h"

void Delay1us()		//@11.0592MHz
{
	//_nop_();
	//_nop_();
	//_nop_();
}

void Delay1ms()		//@11.0592MHz
{
	unsigned char i, j;

	_nop_();
	_nop_();
	_nop_();
	i = 11;
	j = 190;
	do
	{
		while (--j);
	} while (--i);
}


void Delay_us(unsigned long i)
{
	for(;i>0;i--)
	{
		Delay1us();
	}
}

void Delay_ms(unsigned long i)
{
	for(;i>0;i--)
	{
		Delay1ms();
	}
}
