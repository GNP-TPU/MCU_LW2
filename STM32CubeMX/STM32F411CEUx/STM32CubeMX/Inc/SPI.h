//====================================================================================================
#ifndef SPI_H
#define SPI_H
//====================================================================================================
#include <stm32f4xx.h>

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
//====================================================================================================
#define SPI_MAX_ATTEMPTS						100000
//====================================================================================================
#define SPI_BAUDRATE_DIV2						0x0000
#define SPI_BAUDRATE_DIV4						0x0008
#define SPI_BAUDRATE_DIV8						0x0010
#define SPI_BAUDRATE_DIV16					0x0018
#define SPI_BAUDRATE_DIV32					0x0020
#define SPI_BAUDRATE_DIV64					0x0028
#define SPI_BAUDRATE_DIV128					0x0030
#define SPI_BAUDRATE_DIV256					0x0038
//====================================================================================================
typedef struct{
	SPI_TypeDef* 		SPI;
	
	uint16_t				Baudrate_Prescaler;									// baudrate control
	
	uint8_t 				Clock_Phase;
	uint8_t 				Clock_Polarity;
	
	bool						LSB_First;
	bool						Master;
	
	bool						Half_Word_Mode;
	
	uint32_t 				SPI_Frequency;
	
	uint8_t					InitStatus;
}SPI_InitTypeDef;
//====================================================================================================
uint8_t SPI_Init(SPI_TypeDef*, SPI_InitTypeDef*);

void SPI_Transmit_Byte(SPI_TypeDef* SPIx, uint8_t data);
void SPI_Transmit(SPI_TypeDef*, uint8_t*, uint32_t);

uint8_t SPI_Receive_Byte(SPI_TypeDef* SPIx);
void SPI_Receive(SPI_TypeDef*, uint8_t*, uint32_t);

void SPI_TransmitReceive(SPI_TypeDef*, uint8_t*, uint8_t*, uint32_t);
//====================================================================================================
#endif
//====================================================================================================