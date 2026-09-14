#include "GPIO.h"

void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIOx_Init){
	switch((uint32_t)GPIOx){
		case (uint32_t)GPIOA:
			RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
			break;
		case (uint32_t)GPIOB:
			RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
			break;
		case (uint32_t)GPIOC:
			RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
			break;
		case (uint32_t)GPIOD:
			RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
			break;
		case (uint32_t)GPIOE:
			RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
			break;
	}
	
	uint32_t io_position = 0;
	uint32_t io_current  = 0;
	
	for(uint32_t position = 0; position < GPIO_NUMBER; position++){
		io_position = 0x01U << position;
		io_current  = (uint32_t)(GPIOx_Init->Pin) & io_position;
		
		if(io_current == io_position){
			GPIOx->MODER 		&= ~(GPIO_MODER_MODE0 << (position * 2));
			GPIOx->OTYPER 	&= ~(GPIO_OTYPER_OT0 << position);
			GPIOx->OSPEEDR 	&= ~(GPIO_OSPEEDER_OSPEEDR0 << (position * 2));
			GPIOx->PUPDR 		&= ~(GPIO_PUPDR_PUPDR0 << (position * 2));
			
			GPIOx->AFR[position >> 3U] &= ~(0xFU << ((uint32_t)(position & 0x07U) * 4U));
			
			switch(GPIOx_Init->Mode){
				case MODE_INPUT:{
					GPIOx->MODER 		|=  (GPIOx_Init->Mode  	<< (position * 2));
					GPIOx->PUPDR 		|=  (GPIOx_Init->Pull  	<< (position * 2));
					break;
				}
				case MODE_OUTPUT:{
					GPIOx->MODER 		|=  (GPIOx_Init->Mode  	<< (position * 2));
					GPIOx->OTYPER 	|=  (GPIOx_Init->Type  	<< (position));
					GPIOx->OSPEEDR 	|=  (GPIOx_Init->Speed 	<< (position * 2));
					GPIOx->PUPDR 		|=  (GPIOx_Init->Pull  	<< (position * 2));
					break;
				}
				case MODE_AF:{
					GPIOx->MODER 		|=  (GPIOx_Init->Mode  	<< (position * 2));
					GPIOx->OTYPER 	|=  (GPIOx_Init->Type  	<< (position));
					GPIOx->OSPEEDR 	|=  (GPIOx_Init->Speed 	<< (position * 2));
					GPIOx->PUPDR 		|=  (GPIOx_Init->Pull  	<< (position * 2));
					
					GPIOx->AFR[position >> 3U] |=  ((uint32_t)(GPIOx_Init->Alternate) << (((uint32_t)position & 0x07U) * 4U));
					break;
				}
				case MODE_ANALOG:{
					GPIOx->MODER 		|=  (GPIOx_Init->Mode  	<< (position * 2));
					break;
				}
			}
		}
	}
}
	
void GPIO_Pin_High(GPIO_TypeDef* GPIOx, uint16_t Pin){
	GPIOx->BSRR = (uint32_t)Pin;
}

void GPIO_Pin_Low(GPIO_TypeDef* GPIOx, uint16_t Pin){
	GPIOx->BSRR = (uint32_t)Pin << 16;
}

uint8_t GPIO_Pin_Read(GPIO_TypeDef* GPIOx, uint16_t Pin){
	if(GPIOx->IDR & Pin){
		return 1;
	}
	else{
		return 0;
	}
}
