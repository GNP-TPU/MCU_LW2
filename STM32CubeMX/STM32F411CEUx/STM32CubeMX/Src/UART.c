#include "UART.h"

uint8_t USART_Init(USART_TypeDef* USARTx, USART_InitTypeDef* USARTx_Struct){	
	double USART_DIV_Calc = 0;
	uint8_t APB_Prescaler_Value = 0;
	
	USARTx_Struct->InitStatus = 0;
	
	switch((uint32_t)USARTx){
		case (uint32_t)USART1:
			RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
			APB_Prescaler_Value = 1 << (((RCC->CFGR & RCC_CFGR_PPRE2) >> 13) - 0x3);
			break;
		case (uint32_t)USART2:
			RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
			APB_Prescaler_Value = 1 << (((RCC->CFGR & RCC_CFGR_PPRE1) >> 10) - 0x3);
			break;
		case (uint32_t)USART6:
			RCC->APB2ENR |= RCC_APB2ENR_USART6EN;
			APB_Prescaler_Value = 1 << (((RCC->CFGR & RCC_CFGR_PPRE2) >> 13) - 0x3);
			break;
		default:
			USARTx_Struct->InitStatus = 0x01;
			return 1;
	}
	
	USARTx->CR1 = 0;												// clear USART control register 1
	USARTx->CR1 |= USART_CR1_UE;						// USART enable
	USARTx->CR1 &= ~USART_CR1_M;						// word length
	
	USART_DIV_Calc = (double)(SystemCoreClock / APB_Prescaler_Value);
	USART_DIV_Calc = USART_DIV_Calc /((double)(8 * 2 * (uint32_t)USARTx_Struct->Baudrate));
	USART_DIV_Calc = USART_DIV_Calc * 16 + 0.5;
	USARTx->BRR = (uint16_t)USART_DIV_Calc;
					 
	
	if(USARTx_Struct->Tx_Enable)
		USARTx->CR1 |= USART_CR1_TE;					// Tx enable
	
	if(USARTx_Struct->Rx_Enable)
		USARTx->CR1 |= USART_CR1_RE;					// Rx enable
	
	if(USARTx_Struct->Rx_IRq_Enable || USARTx_Struct->Tx_IRq_Enable){ 
		if(USARTx_Struct->Rx_IRq_Enable)
			USARTx->CR1 |= USART_CR1_RXNEIE;		// Rx buffer not empty interrupt enable
		
		if(USARTx_Struct->Tx_IRq_Enable)
			USARTx->CR1 |= USART_CR1_TXEIE;			// Tx buffer empty interrupt enable
		
		if(USARTx == USART1){
			NVIC_EnableIRQ(USART1_IRQn);
			NVIC_SetPriority(USART1_IRQn, 1);
		}
		if(USARTx == USART2){
			NVIC_EnableIRQ(USART2_IRQn);
			NVIC_SetPriority(USART2_IRQn, 2);
		}
		if(USARTx == USART6){
			NVIC_EnableIRQ(USART6_IRQn);
			NVIC_SetPriority(USART6_IRQn, 3);
		}
	}
	
	return USARTx_Struct->InitStatus;
}

void USART_SendChar(USART_TypeDef* USARTx, uint8_t ch){
	while(!(USARTx->SR & USART_SR_TXE));
	USARTx->DR = ch;
}

void USART_SendString(USART_TypeDef* USARTx, char* str){
	while(*str)
		USART_SendChar(USARTx, *str++);
	USART_SendChar(USARTx, '\r');
	USART_SendChar(USARTx, '\n');
}

void USART_SendArray(USART_TypeDef* USARTx, uint8_t* array){
	uint32_t array_size = sizeof(array);
	for(uint32_t i = 0; i < array_size; i++)
		USART_SendChar(USARTx, array[i]);
}
