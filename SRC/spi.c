
/**********************************************************
3-wire spi
**********************************************************/

#include "spi.h"


/**********************************************************
**Name: 	vSpi3Init
**Func: 	Init Spi-3 Config
**Note: 	
**********************************************************/
void spi3Class_vSpi3Init(void)
{
//	GPIO_Init(GPIO_PORT_C, (GPIO_Pin_TypeDef)GPIO_PINS, GPIO_MODE_OUT_PP_LOW_FAST);;	//SPI-3 for output	
	SetCSB();
	SetFCSB();
	SetSDIO();
	ClrSDCK();
}

/**********************************************************
**Name: 	vSpi3WriteByte
**Func: 	SPI-3 send one byte
**Input:
**Output:  
**********************************************************/
static void vSpi3WriteByte(byte dat)
{

 	byte bitcnt;	
 
	SetFCSB();				//FCSB = 1;
 
 	OutputSDIO();			//SDA output mode
 	OutputSDIO();			//SDA output mode
 	SetSDIO();				//    output 1
 
 	ClrSDCK();				
 	ClrCSB();

 	for(bitcnt=8; bitcnt!=0; bitcnt--)
 		{
		ClrSDCK();	
		//Delay_us(SPI3_SPEED);
 		if(dat&0x80)
 			SetSDIO();
 		else
 			ClrSDIO();
		SetSDCK();
 		dat <<= 1; 		
 		//Delay_us(SPI3_SPEED);
 		}
 	ClrSDCK();		
 	SetSDIO();
}

/**********************************************************
**Name: 	bSpi3ReadByte
**Func: 	SPI-3 read one byte
**Input:
**Output:  
**********************************************************/
static byte bSpi3ReadByte(void)
{
	byte RdPara = 0;
 	byte bitcnt;
  
 	ClrCSB(); 
 	InputSDIO();			
  	InputSDIO();		
 	for(bitcnt=8; bitcnt!=0; bitcnt--)
 		{
 		ClrSDCK();
 		RdPara <<= 1;
 		//Delay_us(SPI3_SPEED);
 		SetSDCK();
 		//Delay_us(SPI3_SPEED);
 		if(SDIO_H())
 			RdPara |= 0x01;
 		else
 			RdPara |= 0x00;
 		} 
 	ClrSDCK();
 	OutputSDIO();
	OutputSDIO();
 	SetSDIO();
 	SetCSB();			
 	return(RdPara);	
}

/**********************************************************
**Name:	 	vSpi3Write
**Func: 	SPI Write One word
**Input: 	Write word
**Output:	none
**********************************************************/
void spi3Class_vSpi3Write(word dat)
{
 	vSpi3WriteByte((byte)(dat>>8)&0x7F);
 	vSpi3WriteByte((byte)dat);
 	SetCSB();
}

/**********************************************************
**Name:	 	bSpi3Read
**Func: 	SPI-3 Read One byte
**Input: 	readout addresss
**Output:	readout byte
**********************************************************/
byte spi3Class_bSpi3Read(byte addr)
{
  	vSpi3WriteByte(addr|0x80);
 	return(bSpi3ReadByte());
}

/**********************************************************
**Name:	 	vSpi3WriteFIFO
**Func: 	SPI-3 send one byte to FIFO
**Input: 	one byte buffer
**Output:	none
**********************************************************/
void spi3Class_vSpi3WriteFIFO(byte dat)
{
 	byte bitcnt;	
 
 	SetCSB();	
	OutputSDIO();	
	ClrSDCK();
 	ClrFCSB();			//FCSB = 0
	for(bitcnt=8; bitcnt!=0; bitcnt--)
 		{
 		ClrSDCK();
 		
 		if(dat&0x80)
			SetSDIO();		
		else
			ClrSDIO();
		Delay_us(SPI3_SPEED);
		SetSDCK();
		Delay_us(SPI3_SPEED);
 		dat <<= 1;
 		}
 	ClrSDCK();	
 	Delay_us(SPI3_SPEED);		//Time-Critical
 	Delay_us(SPI3_SPEED);		//Time-Critical
 	SetFCSB();
	SetSDIO();
 	Delay_us(SPI3_SPEED);		//Time-Critical
 	Delay_us(SPI3_SPEED);		//Time-Critical
}

/**********************************************************
**Name:	 	bSpi3ReadFIFO
**Func: 	SPI-3 read one byte to FIFO
**Input: 	none
**Output:	one byte buffer
**********************************************************/
byte spi3Class_bSpi3ReadFIFO(void)
{
	byte RdPara;
 	byte bitcnt;	
 	
 	SetCSB();
	InputSDIO();
 	ClrSDCK();
	ClrFCSB();
		
 	for(bitcnt=8; bitcnt!=0; bitcnt--)
 		{
 		ClrSDCK();
 		RdPara <<= 1;
 		Delay_us(SPI3_SPEED);
		SetSDCK();
		Delay_us(SPI3_SPEED);
 		if(SDIO_H())
 			RdPara |= 0x01;		//NRZ MSB
 		else
 		 	RdPara |= 0x00;		//NRZ MSB
 		}
 	
 	ClrSDCK();
 	Delay_us(SPI3_SPEED);		//Time-Critical
 	Delay_us(SPI3_SPEED);		//Time-Critical
 	SetFCSB();
	OutputSDIO();
	SetSDIO();
 	Delay_us(SPI3_SPEED);		//Time-Critical
 	Delay_us(SPI3_SPEED);		//Time-Critical
 	return(RdPara);
}

/**********************************************************
**Name:	 	vSpi3BurstWriteFIFO
**Func: 	burst wirte N byte to FIFO
**Input: 	array length & head pointer
**Output:	none
**********************************************************/
void spi3Class_vSpi3BurstWriteFIFO(byte ptr[], byte length)
{
 	byte i;
 	if(length!=0x00)
	 	{
 		for(i=0;i<length;i++)
 			spi3Class_vSpi3WriteFIFO(ptr[i]);
 		}
 	return;
}

/**********************************************************
**Name:	 	vSpiBurstRead
**Func: 	burst wirte N byte to FIFO
**Input: 	array length  & head pointer
**Output:	none
**********************************************************/
void spi3Class_vSpi3BurstReadFIFO(byte ptr[], byte length)
{
	byte i;
 	if(length!=0)
 		{
 		for(i=0;i<length;i++)
 			ptr[i] = spi3Class_bSpi3ReadFIFO();
 		}	
 	return;
}
  
  
  
  
  
  

