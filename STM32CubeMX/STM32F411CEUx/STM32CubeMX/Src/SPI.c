#include "SPI.h"

uint8_t SPI_Init(SPI_TypeDef* SPIx, SPI_InitTypeDef* SPIx_Init){
	uint8_t APB_Prescaler_Value = 0;
	
	switch((uint32_t)SPIx){
		case (uint32_t)SPI1:
			RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
			APB_Prescaler_Value = 1 << (((RCC->CFGR & RCC_CFGR_PPRE2) >> 13) - 0x3);
			break;
		case (uint32_t)SPI2:
			RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
			APB_Prescaler_Value = 1 << (((RCC->CFGR & RCC_CFGR_PPRE1) >> 10) - 0x3);
			break;
		case (uint32_t)SPI3:
			RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;
			APB_Prescaler_Value = 1 << (((RCC->CFGR & RCC_CFGR_PPRE1) >> 10) - 0x3);
			break;
		default:
			SPIx_Init->InitStatus = 0x01;
			return 1;
	}
	
	SPIx_Init->SPI_Frequency = SystemCoreClock / APB_Prescaler_Value / (1 << ((SPIx_Init->Baudrate_Prescaler >> 3) + 1));
	
	SPIx->CR1 = 0x0000;
	
	if(SPIx_Init->LSB_First){
		SPIx->CR1 |= SPI_CR1_LSBFIRST;
	}
	
	if(SPIx_Init->Clock_Phase){
		SPIx->CR1 |= SPI_CR1_CPOL;
	}
	
	if(SPIx_Init->Clock_Polarity){
		SPIx->CR1 |= SPI_CR1_CPHA;
	}
	
	if(SPIx_Init->Half_Word_Mode){
		SPIx->CR1 |= SPI_CR1_DFF;
	}
	
	SPIx->CR1 &= ~(SPI_CR1_BR);
	SPIx->CR1 |= (SPIx_Init->Baudrate_Prescaler & SPI_CR1_BR);
	
	SPIx->CR2 |= SPI_CR2_TXDMAEN; // Включаем DMA для передачи
	
	SPIx->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_SPE | SPI_CR1_MSTR;
	
	return SPIx_Init->InitStatus;
}
//===============================================================================
void SPI_Transmit_Byte(SPI_TypeDef* SPIx, uint8_t data){
	uint32_t i = 0;
	uint8_t dummy_read = 0;
	
	dummy_read = SPIx->DR;
	dummy_read = SPIx->SR;
	
	while(!(SPIx->SR & SPI_SR_TXE));
	SPIx->DR = data;
	while((SPIx->SR & SPI_SR_BSY));

	while(!(SPIx->SR & SPI_SR_TXE));
	while(SPIx->SR & SPI_SR_BSY);
	dummy_read = SPIx->DR;
	dummy_read = SPIx->SR;
}

void SPI_Transmit(SPI_TypeDef* SPIx, uint8_t* data, uint32_t size){
	uint32_t i = 0;
	uint8_t dummy_read = 0;
	
	dummy_read = SPIx->DR;
	dummy_read = SPIx->SR;
	
	while(i < size){
		while(!(SPIx->SR & SPI_SR_TXE));
		SPIx->DR = data[i];
		while((SPIx->SR & SPI_SR_BSY));
		i++;
	}
	while(!(SPIx->SR & SPI_SR_TXE));
	while(SPIx->SR & SPI_SR_BSY);
	dummy_read = SPIx->DR;
	dummy_read = SPIx->SR;
}
//===============================================================================
uint8_t SPI_Receive_Byte(SPI_TypeDef* SPIx){
	uint8_t data;

	SPIx->DR = 0;
	while(!(SPIx->SR & SPI_SR_RXNE));
	data = SPIx->DR;

	return data;
}

void SPI_Receive(SPI_TypeDef* SPIx, uint8_t* data, uint32_t size){
	while(size){
		SPIx->DR = 0;
		while(!(SPIx->SR & SPI_SR_RXNE));
		*data++ = SPIx->DR;
		size--;
	}
}
//===============================================================================
void SPI_TransmitReceive(SPI_TypeDef* SPIx, uint8_t* data_Tx, uint8_t* data_Rx, uint32_t size){
	uint32_t i = 0;
  uint8_t dummy_read = 0;
    
  // Очищаем буфер приемника перед началом обмена, если там что-то было
  dummy_read = SPIx->DR;
  (void)dummy_read; // Убираем предупреждение компилятора о неиспользуемой переменной

  while(i < size){
		// 1. Ждем, пока освободится буфер передачи (TXE)
    while(!(SPIx->SR & SPI_SR_TXE));
        
    // 2. Записываем байт в регистр данных. 
    // Если указатель data_Tx передан как NULL, отправляем dummy-байт (0x00)
    if(data_Tx != NULL){
			SPIx->DR = data_Tx[i];
    } 
		else{
			SPIx->DR = 0x00; 
    }
        
		// 3. Ждем, пока придет ответный байт в буфер приема (RXNE)
    while(!(SPIx->SR & SPI_SR_RXNE));
        
    // 4. Считываем полученный байт.
    // Если указатель data_Rx равен NULL, просто сбрасываем байт в "пустоту"
    if(data_Rx != NULL) {
			data_Rx[i] = SPIx->DR;
    } 
		else {
			dummy_read = SPIx->DR;
    }    
    i++;
	}
    
  // Ожидаем окончания физической передачи последнего байта по шине
  while(SPIx->SR & SPI_SR_BSY);
}


