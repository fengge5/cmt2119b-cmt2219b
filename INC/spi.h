  #ifndef __SPI_H
    #define __SPI_H
  #include <STC15F2K60S2.H> 
	void Delay_us(unsigned long i);
	void Delay_ms(unsigned long i);
	
  #define SPI3_SPEED 1

   
  #ifndef	byte
          typedef unsigned char byte;
  #endif
          
  #ifndef word
          typedef unsigned int  word;
  #endif


  #define	SetCSB()	P33=1//GPIO_WriteHigh(GPIO_PORT_C,GPIO_PINS_CSB)
  #define	ClrCSB()	P33=0//GPIO_WriteLow(GPIO_PORT_C,GPIO_PINS_CSB)
          
  #define	SetFCSB()	P32=1//GPIO_WriteHigh(GPIO_PORT_C,GPIO_PINS_FCSB)
  #define	ClrFCSB()	P32=0//GPIO_WriteLow(GPIO_PORT_C,GPIO_PINS_FCSB)
          
  #define	SetSDCK()	P35=1//GPIO_WriteHigh(GPIO_PORT_C,GPIO_PINS_SDCK)
  #define	ClrSDCK()	P35=0//GPIO_WriteLow(GPIO_PORT_C,GPIO_PINS_SDCK)
          
  #define	SetSDIO()	P34=1//GPIO_WriteHigh(GPIO_PORT_C,GPIO_PINS_SDIO)
  #define	ClrSDIO()	P34=0//GPIO_WriteLow(GPIO_PORT_C,GPIO_PINS_SDIO)
          
  #define   InputSDIO()		//GPIO_Init(GPIO_PORT_C,GPIO_PINS_SDIO,GPIO_MODE_IN_FL_NO_IT)
  #define	OutputSDIO()	//GPIO_Init(GPIO_PORT_C,GPIO_PINS_SDIO,GPIO_MODE_OUT_PP_LOW_FAST)  
          
  #define	SDIO_H()	P34//GPIO_ReadInputPin(GPIO_PORT_C,GPIO_PINS_SDIO) 
  #define	SDIO_L()	P34//GPIO_ReadInputPin(GPIO_PORT_C,GPIO_PINS_SDIO) 

  //typedef struct 
	//{
		//void (*vSpi3Init)(void);				/** initialize software SPI-3 **/	
	 	//void (*vSpi3Write)(word dat);			/** SPI-3 send one word **/
		//byte (*bSpi3Read)(byte addr);			/** SPI-3 read one byte **/
		
		//void (*vSpi3WriteFIFO)(byte dat);		/** SPI-3 send one byte to FIFO **/
		//byte (*bSpi3ReadFIFO)(void);			/** SPI-3 read one byte from FIFO **/
		//void (*vSpi3BurstWriteFIFO)(byte ptr[], byte length);			/** SPI-3 burst send N byte to FIFO**/
		//void (*vSpi3BurstReadFIFO)(byte ptr[], byte length);			/** SPI-3 burst read N byte to FIFO**/

	    //void (*vSpi3WriteByte)(byte dat);		/** SPI-3 send one byte **/
	    //byte (*bSpi3ReadByte)(void);			/** SPI-3 read one byte **/

	//}spi3Class;


  #else
          #warning "spi.h have been defined!"
                             
  #endif 

