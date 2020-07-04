#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
#define  BOUND  9600

//测试
#define USART_REC_LEN  			200
#define  EN_USART1_RX 1


void MUSART_Init(void);
void usart_init(u32 bound);
void Mode_Init(void);
void uart_tx_bytes( u8* TxBuffer, u8 Length);
void USART_DMA_Config(void);
void DMAEN_Config(void);

//void uart_init(u32 bound);
//void Send_data(char USART_RX_BUF[], uint32_t Value);


#endif


