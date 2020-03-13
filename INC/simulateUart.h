#ifndef __SIMULATEUART_H
#define __SIMULATEUART_H
#include <STC15F2K60S2.H>
#define UART_RX P3^0
#define UART_TX P3^1
void UART_INIT();
#else
	#warning "simulateUart.h have been defined!"
#endif
