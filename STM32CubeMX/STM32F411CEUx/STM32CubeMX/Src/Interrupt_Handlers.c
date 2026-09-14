#include "Interrupt_Handlers.h"



void USART1_IRQHandler(void){
	if(USART1->SR & USART_SR_RXNE){
		uint8_t byte = USART1->DR;
	}
	if(USART1->SR & USART_SR_ORE){
		uint8_t byte_error = USART1->DR;							
	}
}


