#include <CMT2300drive.h>
#include "type.h"
#include "spi.c"
#include "stdio.h"

extern cmt2300aEasy radio;

unsigned char cmt2300aEasy_bGoTx(void)
{
unsigned char tmp, i;

INIT_TX:
 spi3Class_vSpi3Write(((unsigned short int)CMT23_MODE_CTL<<8)+MODE_GO_TX);		
 for(i=0; i<50; i++)
 	{
 	Delay_us(200);	
	tmp = (MODE_MASK_STA & spi3Class_bSpi3Read(CMT23_MODE_STA));	
	if(tmp==MODE_STA_TX)
		break;
	}
 if(i>=50)
 	{
 	
		cmt2300aEasy_bGoStandby();
 		goto INIT_TX;
 	}
 else
 	return(true);
}

/**********************************************************
**Name:     bGoRx
**Function: Entry Rx Mode
**Input:    none
**Output:   none
**********************************************************/

unsigned char cmt2300aEasy_vReadIngFlag1(void)
{
	return(spi3Class_bSpi3Read((unsigned char)(CMT23_INT_FLG>>8)));

}

unsigned char cmt2300aEasy_vReadIngFlag2(void)
{
	return(spi3Class_bSpi3Read((unsigned char)(CMT23_INT_CLR1	>>8)));

}


unsigned char cmt2300aEasy_bGoRx(void)
{
 unsigned char tmp, i;
 radio.RssiTrig = false;

 INIT_RX:
 spi3Class_vSpi3Write(((unsigned short int)CMT23_MODE_CTL<<8)+MODE_GO_RX);		
 for(i=0; i<50; i++)
 	{
 	Delay_us(200);	
	tmp = (MODE_MASK_STA & spi3Class_bSpi3Read(CMT23_MODE_STA));	
	if(tmp==MODE_STA_RX)
		break;
	}
 if(i>=50)
 	{
		cmt2300aEasy_bGoStandby();
 		goto INIT_RX;
 	}
 else
 	return(true);
}

/**********************************************************
**Name:     bGoSleep
**Function: Entry Sleep Mode
**Input:    none
**Output:   none
**********************************************************/
unsigned char cmt2300aEasy_bGoSleep(void)
{
 unsigned char tmp;
 
 spi3Class_vSpi3Write(((unsigned short int)CMT23_MODE_CTL<<8)+MODE_GO_SLEEP);	
 Delay_ms(100);		//enough?
 tmp = (MODE_MASK_STA & spi3Class_bSpi3Read(CMT23_MODE_STA));	
 if(tmp==MODE_STA_SLEEP)
 	return(true);
 else
 	return(false);
}

/**********************************************************
**Name:     bGoStandby
**Function: Entry Standby Mode
**Input:    none
**Output:   none
**********************************************************/
unsigned char cmt2300aEasy_bGoStandby(void)
{
 unsigned char tmp, i;	
 
 radio.RssiTrig = false;
 spi3Class_vSpi3Write(((unsigned short int)CMT23_MODE_CTL<<8)+MODE_GO_STBY);	
 for(i=0; i<50; i++)
 	{
 	Delay_us(400);	
	tmp = (MODE_MASK_STA & spi3Class_bSpi3Read(CMT23_MODE_STA));	
	if(tmp==MODE_STA_STBY)
		break;
	}
 if(i>=50)
 	return(false);
 else
 	return(true);
}

/**********************************************************
**Name:     vSoftReset
**Function: Software reset Chipset
**Input:    none
**Output:   none
**********************************************************/
void cmt2300aEasy_vSoftReset(void)
{
 spi3Class_vSpi3Write(((unsigned short int)CMT23_SOFTRST<<8)+0xFF); 
 Delay_us(1000);				//enough?
}

/**********************************************************
**Name:     bReadStatus
**Function: read chipset status
**Input:    none
**Output:   none
**********************************************************/
unsigned char cmt2300aEasy_bReadStatus(void)
{
 return(MODE_MASK_STA & spi3Class_bSpi3Read(CMT23_MODE_STA));		
}

/**********************************************************
**Name:     bReadRssi
**Function: Read Rssi
**Input:    true------dBm;
            false-----Code;
**Output:   none
**********************************************************/
unsigned char cmt2300aEasy_bReadRssi(unsigned char unit_dbm)
{
 if(unit_dbm)
 	return(spi3Class_bSpi3Read(CMT23_RSSI_DBM));
 else		
 	return(spi3Class_bSpi3Read(CMT23_RSSI_CODE));
}

/**********************************************************
GPIO & Interrupt CFG
**********************************************************/
/**********************************************************
**Name:     vGpioFuncCfg
**Function: GPIO Function config
**Input:    none
**Output:   none
**********************************************************/
void cmt2300aEasy_vGpioFuncCfg(unsigned char io_cfg)
{
 spi3Class_vSpi3Write(((unsigned short int)CMT23_IO_SEL<<8)+io_cfg);
}

/**********************************************************
**Name:     vIntSrcCfg
**Function: config interrupt source  
**Input:    int_1, int_2
**Output:   none
**********************************************************/
void cmt2300aEasy_vIntSrcCfg(unsigned char int_1, unsigned char int_2)
{
 unsigned char tmp;
 tmp = INT_MASK & spi3Class_bSpi3Read(CMT23_INT1_CTL);
 spi3Class_vSpi3Write(((unsigned short int)CMT23_INT1_CTL<<8)+(tmp|int_1));
 
 tmp = INT_MASK & spi3Class_bSpi3Read(CMT23_INT2_CTL);
 spi3Class_vSpi3Write(((unsigned short int)CMT23_INT2_CTL<<8)+(tmp|int_2));
}

/**********************************************************
**Name:     vEnableAntSwitch
**Function:  
**Input:    
**Output:   none
**********************************************************/
void cmt2300aEasy_vEnableAntSwitch(unsigned char mode)
{
 unsigned char tmp;
 tmp = spi3Class_bSpi3Read(CMT23_INT1_CTL);
 tmp&= 0x3F;
 switch(mode)
 	{
 	case 1:
 		tmp |= RF_SWT1_EN; break;		//GPO1=RxActive; GPO2=TxActive	
 	case 2:
 		tmp |= RF_SWT2_EN; break;		//GPO1=RxActive; GPO2=!RxActive
 	case 0:
 	default:
 		break;							//Disable	
 	}
 spi3Class_vSpi3Write(((unsigned short int)CMT23_INT1_CTL<<8)+tmp);
}


/**********************************************************
**Name:     vIntSrcEnable
**Function: enable interrupt source 
**Input:    en_int
**Output:   none
**********************************************************/
void cmt2300aEasy_vEnablePLLcheck(void)
{
  unsigned char tmp;
  tmp = spi3Class_bSpi3Read(CMT23_EN_CTL);
  tmp |= LD_STOP_EN; 	
  spi3Class_vSpi3Write(((unsigned short int)CMT23_EN_CTL<<8)+tmp);
}

/**********************************************************
**Name:     vEnablePLLcheck
**Function: enable PLLcheck 
**Input:    en_int
**Output:   none
**********************************************************/
void cmt2300aEasy_vIntSrcEnable(unsigned char en_int)
{
 spi3Class_vSpi3Write(((unsigned short int)CMT23_INT_EN<<8)+en_int);				
}


/**********************************************************
**Name:     vIntSrcFlagClr
**Function: clear flag
**Input:    none
**Output:   equ CMT23_INT_EN
**********************************************************/
unsigned char cmt2300aEasy_bIntSrcFlagClr(void)
{
 unsigned char tmp;
 unsigned char int_clr2 = 0;
 unsigned char int_clr1 = 0;
 unsigned char flg = 0;
 
 tmp = spi3Class_bSpi3Read(CMT23_INT_FLG);
 if(tmp&LBD_STATUS_FLAG)		//LBD_FLG_Active
 	int_clr2 |= LBD_CLR;
 
 if(tmp&PREAMBLE_PASS_FLAG)		//Preamble Active
 	{
 	int_clr2 |= PREAMBLE_PASS_CLR;
 	flg |= PREAMBLE_PASS_EN;
	}
 if(tmp&SYNC_PASS_FLAG)			//Sync Active
 	{
 	int_clr2 |= SYNC_PASS_CLR;		
 	flg |= SYNC_PASS_EN;		
 	}
 if(tmp&NODE_PASS_FLAG)			//Node Addr Active
 	{
 	int_clr2 |= NODE_PASS_CLR;	
 	flg |= NODE_PASS_EN;
 	}
 if(tmp&CRC_PASS_FLAG)			//Crc Pass Active
 	{
 	int_clr2 |= CRC_PASS_CLR;
 	flg |= CRC_PASS_EN;
 	}
 if(tmp&RX_DONE_FLAG)			//Rx Done Active
 	{
 	int_clr2 |= RX_DONE_CLR;
 	flg |= PKT_DONE_EN;
 	}
 	
 if(tmp&COLLISION_ERR_FLAG)		//这两个都必须通过RX_DONE清除
 	int_clr2 |= RX_DONE_CLR;
 if(tmp&DC_ERR_FLAG)
 	int_clr2 |= RX_DONE_CLR;
 	
 spi3Class_vSpi3Write(((unsigned short int)CMT23_INT_CLR2<<8)+int_clr2);	//Clear flag
 
 
 tmp = spi3Class_bSpi3Read(CMT23_INT_CLR1);
 if(tmp&TX_DONE_FLAG)
 	{
 	int_clr1 |= TX_DONE_CLR;
 	flg |= TX_DONE_EN;
 	}	
 if(tmp&SLEEP_TIMEOUT_FLAG)
 	{
 	int_clr1 |= SLEEP_TIMEOUT_CLR;
 	flg |= SLEEP_TMO_EN;
 	} 
 if(tmp&RX_TIMEOUT_FLAG)
 	{
 	int_clr1 |= RX_TIMEOUT_CLR;
 	flg |= RX_TMO_EN;
 	}	
 spi3Class_vSpi3Write(((unsigned short int)CMT23_INT_CLR1<<8)+int_clr1);	//Clear flag 
 
 return(flg);
}

/**********************************************************
**Name:     bClearFIFO
**Function: clear FIFO buffer
**Input:    none
**Output:   FIFO state
**********************************************************/
unsigned char cmt2300aEasy_vClearFIFO(void)
{
 unsigned char tmp;	
 tmp = spi3Class_bSpi3Read(CMT23_FIFO_FLG);
 spi3Class_vSpi3Write(((unsigned short int)CMT23_FIFO_CLR<<8)+FIFO_CLR_RX+FIFO_CLR_TX);
 return(tmp);
}

void cmt2300aEasy_vEnableWrFifo(void)
{
 unsigned char tmp;
 tmp = spi3Class_bSpi3Read(CMT23_FIFO_CTL);
 tmp |= (SPI_FIFO_RD_WR_SEL|FIFO_RX_TX_SEL);
 spi3Class_vSpi3Write(((unsigned short int)CMT23_FIFO_CTL<<8)+tmp);
}

void cmt2300aEasy_vEnableRdFifo(void)
{
 unsigned char tmp;
 tmp = spi3Class_bSpi3Read(CMT23_FIFO_CTL);
 tmp &= (~(SPI_FIFO_RD_WR_SEL|FIFO_RX_TX_SEL));
 spi3Class_vSpi3Write(((unsigned short int)CMT23_FIFO_CTL<<8)+tmp);
}

/**********************************************************
CFG
**********************************************************/
/**********************************************************
**Name:     vInit
**Function: Init. CMT2300A
**Input:    none
**Output:   none
**********************************************************/
void cmt2300aEasy_vInit(void)
{
 //unsigned char i;
 unsigned char tmp;
 unsigned char tmp1;
 //unsigned short int len;
 spi3Class_vSpi3Init();
 GPO3In();

 cmt2300aEasy_vSoftReset();
 Delay_ms(20);
tmp1 = cmt2300aEasy_bGoStandby();
if(tmp1 == false)
{
	printf("tmp1=%d while(1)\n",(u16)tmp1);
	while(1);
}
 //	
 tmp = spi3Class_bSpi3Read(CMT23_MODE_STA);
 tmp|= EEP_CPY_DIS;
 tmp&= (~RSTN_IN_EN);			//Disable RstPin	
 spi3Class_vSpi3Write(((unsigned short int)CMT23_MODE_STA<<8)+tmp);

 cmt2300aEasy_bIntSrcFlagClr();
 
}

void cmt2300aEasy_vCfgBank(unsigned short int cfg[], unsigned char length)
{
 unsigned char i;
 
 if(length!=0)
 	{	
 	for(i=0; i<length; i++)	
 		spi3Class_vSpi3Write(cfg[i]);
 	}	
}


/******************************************************************************
**函数名称：bGetMessage
**函数功能：接收一包数据
**输入参数：无
**输出参数：非0——接收成功
**          0——接收失败
******************************************************************************/
unsigned char cmt2300aEasy_bGetMessage(unsigned char msg[])
{
 unsigned char i;	
 
 cmt2300aEasy_vEnableRdFifo();	
 if(radio.FixedPktLength)
 	{
  	spi3Class_vSpi3BurstReadFIFO(msg, radio.PayloadLength);
	i = radio.PayloadLength;
	}
 else
 	{
	i = spi3Class_bSpi3ReadFIFO();	
 	spi3Class_vSpi3BurstReadFIFO(msg, i);
 	}
 return(i);
}

unsigned char cmt2300aEasy_bGetMessageByFlag(unsigned char msg[])
{
 unsigned char tmp;
 unsigned char tmp1;
 unsigned char rev = 0;
 tmp = spi3Class_bSpi3Read(CMT23_INT_FLG);
 if((tmp&SYNC_PASS_FLAG)&&(!radio.RssiTrig))
 	{
 	radio.PktRssi = cmt2300aEasy_bReadRssi(false);
 	radio.RssiTrig = true;
 	}
 
 tmp1 = spi3Class_bSpi3Read(CMT23_CRC_CTL);
 cmt2300aEasy_vEnableRdFifo();	 
 if(tmp1&CRC_ENABLE)		//Enable CrcCheck
 	{
 	if(tmp&CRC_PASS_FLAG)
 		{
 		if(radio.FixedPktLength)
 			{
  			spi3Class_vSpi3BurstReadFIFO(msg, radio.PayloadLength);
			rev = radio.PayloadLength;
			}
 		else
 			{	
			rev = spi3Class_bSpi3ReadFIFO();	
 			spi3Class_vSpi3BurstReadFIFO(msg, rev);
 			}
 		radio.RssiTrig = false;
 		}
 	}
 else
 	{
	if(tmp&RX_DONE_FLAG) 		
		{
 		if(radio.FixedPktLength)
 			{
  			spi3Class_vSpi3BurstReadFIFO(msg, radio.PayloadLength);
			rev = radio.PayloadLength;
			}
 		else
 			{	
			rev = spi3Class_bSpi3ReadFIFO();	
 			spi3Class_vSpi3BurstReadFIFO(msg, rev);
 			}	
 		radio.RssiTrig = false;		
		}
 	}
 
 if(tmp&COLLISION_ERR_FLAG)			//错误处理
	rev = 0xFF;
 return(rev);
}

/******************************************************************************
**函数名称：bSendMessage
**函数功能：发射一包数据
**输入参数：无
**输出参数：
**          
******************************************************************************/
unsigned char cmt2300aEasy_bSendMessage(unsigned char msg[], unsigned char length)
{
 //mode1
 /*cmt2300aEasy_vSetTxPayloadLength(length);
 cmt2300aEasy_bGoStandby();
 cmt2300aEasy_vEnableWrFifo();	
 spi3Class_vSpi3BurstWriteFIFO(msg, length);
 cmt2300aEasy_bGoTx();*/
 
 //mode2
 cmt2300aEasy_bIntSrcFlagClr();  //清中断
 cmt2300aEasy_vSetTxPayloadLength(length);
 cmt2300aEasy_bGoTx();
 cmt2300aEasy_vEnableWrFifo();	
 spi3Class_vSpi3BurstWriteFIFO(msg, length);
 return(true);
}

void cmt2300aEasy_vSetTxPayloadLength(unsigned short int length)
{
 unsigned char tmp;	
 unsigned char len;
 cmt2300aEasy_bGoStandby();
 tmp = spi3Class_bSpi3Read(CMT23_PKT_CTRL1);
 tmp&= 0x8F;
 
 if(length!=0)
 	{
 	if(radio.FixedPktLength)
		len = length-1;
 	else
		len = length;
	}
 else
 	len = 0;
 
 tmp|= (((unsigned char)(len>>8)&0x07)<<4);
 spi3Class_vSpi3Write(((unsigned short int)CMT23_PKT_CTRL1<<8)+tmp);
 spi3Class_vSpi3Write(((unsigned short int)CMT23_PKT_LEN<<8)+(unsigned char)len);	//Payload length
 //bGoSleep();
}
