//====================================================================================================
#ifndef UART_H
#define UART_H
//====================================================================================================
#include <stm32f4xx.h>

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
//====================================================================================================
#define MAX_TX_BUFFER_SIZE		100
#define MAX_RX_BUFFER_SIZE		1000
//====================================================================================================
typedef struct{
	USART_TypeDef* 	USART;
	
	uint32_t 				Baudrate;																// Set a baudrate

	bool		 			 	Tx_Enable;															// Enable Tx pin
	bool 			 			Rx_Enable;															// Enable Rx pin
	
	uint8_t        	Tx_IRq_Enable;													// Allow interrupt on receive
	uint8_t        	Rx_IRq_Enable;													// Allow interrupt on tramsmit
	
	char 						TxBuffer[MAX_TX_BUFFER_SIZE];						// Tx buffer
	uint32_t        TxBufferIndex;													// Tx buffer index
	uint32_t				TxBufferLength;
	
	char 						RxBuffer[MAX_RX_BUFFER_SIZE];						// Rx buffer
	uint32_t        RxBufferIndex;													// Rx buffer index 
	uint32_t				RxBufferLength;
	
	uint8_t 				InitStatus;
}USART_InitTypeDef;
//====================================================================================================
uint8_t USART_Init(USART_TypeDef*, USART_InitTypeDef*);
uint8_t USART_IRQ_Init(USART_TypeDef*, USART_InitTypeDef*);
uint8_t USART_DMA_Init(USART_TypeDef*, USART_InitTypeDef*);

void USART_SendChar(USART_TypeDef*, uint8_t);
void USART_SendString(USART_TypeDef*, char*);
void USART_SendArray(USART_TypeDef*, uint8_t*);
//====================================================================================================
#endif
//====================================================================================================