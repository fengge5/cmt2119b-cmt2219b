#include <STC15F2K60S2.H>
#include "type.h"
#include "CMT2300drive.c"
#include "stdio.h"
#include "TxConfig.h"

static unsigned char statetx = false;  //  false为RX  true为TX

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

//void Device_Init(void);
void CLK_Init(void);
void setup_Tx(void);
void setup_Rx(void);
void CMT2300_Init();
void loop_Tx(void);
void loop_Rx(void);

#define LEN 21

unsigned char str[LEN] = {'f','e','n','g','R','F',' ','R','F','M',' ','C','O','B','R','F','M','3','0','0','A'};
unsigned char getstr[LEN+1];
	
cmt2300aEasy radio;	
		



//-----------------------------------------
//define baudrate const
//BAUD = 65536 - FOSC/3/BAUDRATE/M (1T:M=1; 12T:M=12)
//NOTE: (FOSC/3/BAUDRATE) must be greater then 98, (RECOMMEND GREATER THEN 110)

//#define BAUD  0xF400                  // 1200bps @ 11.0592MHz
//#define BAUD  0xFA00                  // 2400bps @ 11.0592MHz
//#define BAUD  0xFD00                  // 4800bps @ 11.0592MHz
#define BAUD  0xFE80                  // 9600bps @ 11.0592MHz
//#define BAUD  0xFF40                  //19200bps @ 11.0592MHz
//#define BAUD  0xFFA0                    //38400bps @ 11.0592MHz

//#define BAUD  0xEC00                  // 1200bps @ 18.432MHz
//#define BAUD  0xF600                  // 2400bps @ 18.432MHz
//#define BAUD  0xFB00                  // 4800bps @ 18.432MHz
//#define BAUD  0xFD80                  // 9600bps @ 18.432MHz
//#define BAUD  0xFEC0                  //19200bps @ 18.432MHz
//#define BAUD    0xFF60                //38400bps @ 18.432MHz

//#define BAUD  0xE800                  // 1200bps @ 22.1184MHz
//#define BAUD  0xF400                  // 2400bps @ 22.1184MHz
//#define BAUD  0xFA00                  // 4800bps @ 22.1184MHz
//#define BAUD  0xFD00                  // 9600bps @ 22.1184MHz
//#define BAUD  0xFE80                  //19200bps @ 22.1184MHz
//#define BAUD  0xFF40                  //38400bps @ 22.1184MHz
//#define BAUD  0xFF80                  //57600bps @ 22.1184MHz

sbit RXB = P3^0;                        //define UART TX/RX port
sbit TXB = P3^1;

typedef bit BOOL;
typedef unsigned char BYTE;
typedef unsigned int WORD;

BYTE TBUF,RBUF;
BYTE TDAT,RDAT;
BYTE TCNT,RCNT;
BYTE TBIT,RBIT;
BOOL TING,RING;
BOOL TEND,REND;

void UART_INIT();

//加入以下代码,支持printf函数,可以在其他地方任意使用printf();     
char putchar (char c)   
{        
    while(!TEND);//wait send end
		TEND = 0;
		TBUF = c;
		TING = 1;      
    return 0;
}

//-----------------------------------------
//Timer interrupt routine for UART

void tm0() interrupt 1 using 1
{
    if (RING)
    {
        if (--RCNT == 0)
        {
            RCNT = 3;                   //reset send baudrate counter
            if (--RBIT == 0)
            {
                RBUF = RDAT;            //save the data to RBUF
                RING = 0;               //stop receive
                REND = 1;               //set receive completed flag
            }
            else
            {
                RDAT >>= 1;
                if (RXB) RDAT |= 0x80;  //shift RX data to RX buffer
            }
        }
    }
    else if (!RXB)
    {
        RING = 1;                       //set start receive flag
        RCNT = 4;                       //initial receive baudrate counter
        RBIT = 9;                       //initial receive bit number (8 data bits + 1 stop bit)
    }

    if (--TCNT == 0)
    {
        TCNT = 3;                       //reset send baudrate counter
        if (TING)                       //judge whether sending
        {
            if (TBIT == 0)
            {
                TXB = 0;                //send start bit
                TDAT = TBUF;            //load data from TBUF to TDAT
                TBIT = 9;               //initial send bit number (8 data bits + 1 stop bit)
            }
            else
            {
                TDAT >>= 1;             //shift data to CY
                if (--TBIT == 0)
                {
                    TXB = 1;
                    TING = 0;           //stop send
                    TEND = 1;           //set send completed flag
                }
                else
                {
                    TXB = CY;           //write CY to TX port
                }
            }
        }
    }
}

//-----------------------------------------
//initial UART module variable

void UART_INIT()
{
    TING = 0;
    RING = 0;
    TEND = 1;
    REND = 0;
    TCNT = 0;
    RCNT = 0;
	
		TMOD = 0x00;                        //timer0 in 16-bit auto reload mode
    AUXR = 0x80;                        //timer0 working at 1T mode
    TL0 = BAUD;
    TH0 = BAUD>>8;                      //initial timer0 and set reload value
    TR0 = 1;                            //tiemr0 start running
    ET0 = 1;                            //enable timer0 interrupt
    PT0 = 1;                            //improve timer0 interrupt priority
    EA = 1;  
}	


void main(void)
{
	P3M1&=~(1<1);
	P3M0|=(1<<1);
	P3M1&=~(1<5);
	P3M0|=(1<<5);
	UART_INIT();//UartInit();
	printf("start0!\r\n");
  CMT2300_Init();
	printf("1!\r\n");
  if(false == statetx )
	{
		printf("2!\r\n");
		setup_Rx();
		while(1)
		{
				loop_Rx();
		}
	}
  else
	{
		
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

void CMT2300_Init()
{
	/**********基础设置初始化一次即可*******/
	printf("4!\r\n");
	radio.FixedPktLength    = false;				
	radio.PayloadLength     = LEN;	
	cmt2300aEasy_vInit();
	printf("5!\r\n");
	cmt2300aEasy_vCfgBank(CMTBank, 12);
	cmt2300aEasy_vCfgBank(SystemBank, 12);
	cmt2300aEasy_vCfgBank(FrequencyBank, 8);
	cmt2300aEasy_vCfgBank(DataRateBank, 24);
	cmt2300aEasy_vCfgBank(BasebandBank, 29);
	cmt2300aEasy_vCfgBank(TXBank, 11);
	printf("6!\r\n");
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
printf("7!\r\n");
	cmt2300aEasy_bGoStandby();   //进入配置模式
	cmt2300aEasy_vEnableAntSwitch(0); //为 1 时 GPIO1 和 GPIO2 不可用
	cmt2300aEasy_vGpioFuncCfg(GPIO1_INT2+GPIO2_Dout+GPIO3_INT2);  //IO口的功能映射
printf("8!\r\n");
	//cmt2300aEasy_vIntSrcCfg(INT_RSSI_VALID, INT_CRC_PASS);   //GPO3映射成CRC_pass中断，此处如果要用该中断，RFPDK需要配置CRC
	cmt2300aEasy_vIntSrcCfg(INT_FIFO_Wunsigned_char_RX, INT_PKT_DONE);  //GPO3映射成PKT_DONE中断 //IO口中断的映射
	cmt2300aEasy_vIntSrcEnable(PKT_DONE_EN + CRC_PASS_EN);          //中断使能 
printf("9!\r\n");	
	cmt2300aEasy_vClearFIFO();
	printf("10!\r\n");
	cmt2300aEasy_bGoSleep();           //让配置生效
	printf("11!\r\n");
	cmt2300aEasy_bGoRx();              //for Rx
printf("12!\r\n");
}


void loop_Tx()
{
	printf("send!\r\n");
	cmt2300aEasy_bSendMessage(str, LEN);
	while(GPO3_L());   // 判断GPIO中断 为低等 为高运行下面代码
	//cmt2300aEasy_bIntSrcFlagClr();
	cmt2300aEasy_vClearFIFO(); 
	Delay_ms(2);
}

void loop_Rx()
{
	unsigned char tmp;
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
		Delay_ms(200);
	}
}
