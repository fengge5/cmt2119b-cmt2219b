#include <STC15F2K60S2.H>
#include "type.h"
#include "CMT2300drive.c"
#include "stdio.h"
#include "simulateUart.h"
#include "TxConfig.h"
#include "RxConfig.h"

static unsigned char statetx = true;  //  false为RX  true为TX

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

//void Device_Init(void);
void CLK_Init(void);
void setup_Tx(void);
void setup_Rx(void);
void CMT2119B_TxInit();
void CMT2219B_RxInit();
void loop_Tx(void);
void loop_Rx(void);

#define LEN 21

code unsigned char str[LEN] = {"feng rf test!!!!feng!"};
unsigned char getstr[LEN+1];
	
cmt2300aEasy radio;	
		


void main(void)
{
	P3M1&=~(1<1);
	P3M0|=(1<<1);
	P3M1&=~(1<5);
	P3M0|=(1<<5);
	UART_INIT();
	EA = 1;  
	printf("start0!\r\n");
  
  if(false == statetx )
	{
		CMT2219B_RxInit();
		setup_Rx();
		while(1)
		{
				loop_Rx();
		}
	}
  else
	{	
		CMT2119B_TxInit();
		setup_Tx(); 
		while (1)
		{
			 loop_Tx();
		}
	}
  
}



/**
  * @brief Device Initialize configuration
  * @param None
  * @retval None
  */

void CMT2119B_TxInit()
{
	/**********基础设置初始化一次即可*******/
	radio.FixedPktLength    = false;				
	radio.PayloadLength     = LEN;	
	cmt2300aEasy_vInit();

	cmt2300aEasy_vCfgBank(CMTBank, 12);
	cmt2300aEasy_vCfgBank(SystemBank, 12);
	cmt2300aEasy_vCfgBank(FrequencyBank, 8);
	cmt2300aEasy_vCfgBank(DataRateBank, 24);
	cmt2300aEasy_vCfgBank(BasebandBank, 29);
	cmt2300aEasy_vCfgBank(TXBank, 11);

	cmt2300aEasy_vEnablePLLcheck();
	cmt2300aEasy_bGoSleep();  				//让配置生效
	/**************************************/
}

void CMT2219B_RxInit()
{
	/**********基础设置初始化一次即可*******/
	radio.FixedPktLength    = false;				
	radio.PayloadLength     = LEN;	
	cmt2300aEasy_vInit();

		cmt2300aEasy_vCfgBank(CMT_Bank, sizeof(CMT_Bank)/sizeof(unsigned short int));
		cmt2300aEasy_vCfgBank(System_Bank, sizeof(System_Bank)/sizeof(unsigned short int));
		cmt2300aEasy_vCfgBank(Frequency_Bank, sizeof(Frequency_Bank)/sizeof(unsigned short int));
		cmt2300aEasy_vCfgBank(Data_Rate_Bank, sizeof(Data_Rate_Bank)/sizeof(unsigned short int));
		cmt2300aEasy_vCfgBank(Baseband_Bank, sizeof(Baseband_Bank)/sizeof(unsigned short int));
		cmt2300aEasy_vCfgBank(Reserve_Bank, sizeof(Reserve_Bank)/sizeof(unsigned short int));
		cmt2300aEasy_vCfgBank(LBD_Bank, sizeof(LBD_Bank)/sizeof(unsigned short int));

	cmt2300aEasy_vEnablePLLcheck();
	cmt2300aEasy_bGoSleep();  				//让配置生效
	/**************************************/
}

void setup_Tx(void)
{

	if(cmt2300aEasy_bGoStandby())//;   //进入配置模式
	{
		printf("bGoStandby ok!\r\n");
	}
	cmt2300aEasy_vEnableAntSwitch(0);  //设置天线切换_IO口切换
	cmt2300aEasy_vGpioFuncCfg(GPIO1_INT2+GPIO2_INT2+GPIO3_INT2); //IO口的映射
	cmt2300aEasy_vIntSrcCfg(INT_FIFO_NMTY_TX, INT_TX_DONE);    //IO口中断的映射
	cmt2300aEasy_vIntSrcEnable(TX_DONE_EN);           //中断使能        
	
	cmt2300aEasy_vClearFIFO();  //清除FIFO
	if(cmt2300aEasy_bGoSleep())//;    //进入睡眠，让配置生效
	{
		printf("bGoSleep ok!\r\n");;
	}
 
}

void setup_Rx(void)
{
	cmt2300aEasy_bGoStandby();   //进入配置模式
	cmt2300aEasy_vEnableAntSwitch(0); //为 1 时 GPIO1 和 GPIO2 不可用
	cmt2300aEasy_vGpioFuncCfg(GPIO1_INT1+GPIO2_Dout+GPIO3_DCLK);  //IO口的功能映射

	//cmt2300aEasy_vIntSrcCfg(INT_RSSI_VALID, INT_CRC_PASS);   //GPO3映射成CRC_pass中断，此处如果要用该中断，RFPDK需要配置CRC
	cmt2300aEasy_vIntSrcCfg(INT_PKT_DONE, INT_PKT_DONE);  //GPO3映射成PKT_DONE中断 //IO口中断的映射
	cmt2300aEasy_vIntSrcEnable(PKT_DONE_EN /*+ CRC_PASS_EN*/);          //中断使能 

	cmt2300aEasy_vClearFIFO();
	cmt2300aEasy_bGoSleep();           //让配置生效
	cmt2300aEasy_bGoRx();              //for Rx
}


void loop_Tx()
{
	printf("send!\r\n");
	cmt2300aEasy_bSendMessage(str, LEN);
	while(GPO3_L());   // 判断GPIO中断 为低等 为高运行下面代码
	cmt2300aEasy_bIntSrcFlagClr();
	cmt2300aEasy_vClearFIFO(); 
	Delay_ms(20);
}

unsigned char tmp;
void loop_Rx()
{
	
	if(GPO1_H())
	{
		cmt2300aEasy_bGoStandby();
		tmp = cmt2300aEasy_bGetMessage(getstr);  //仿真到此能看到getstr收到的数据包
		printf("recv=%s\r\n",getstr);
		cmt2300aEasy_bIntSrcFlagClr();
		cmt2300aEasy_vClearFIFO(); 
		cmt2300aEasy_bGoRx();
	}else
	{
		printf("nothing!\r\n");
		Delay_ms(1000);
	}
}
