
#ifndef _USART0_H
#define _USART0_H
/******************************************************************************/
#include "stm32f4xx.h"

/******************************************************************************/
#define USART0_BUFFER_SIZE0 256
/******************************************************************************/
extern uint8_t snd0_buff[USART0_BUFFER_SIZE0];
extern uint8_t rcv0_buff[USART0_BUFFER_SIZE0];
extern unsigned long rcv0_cntr;
extern unsigned long rcv0_flag;
/******************************************************************************/
void USART0_Init(unsigned long bound);
void USART0_SendDMA(uint32_t len);
/******************************************************************************/
void printf_DMA(char *fmt, ...);

#endif


