//====================================================================================================
#ifndef GPIO_H
#define GPIO_H
//====================================================================================================
#include <stm32f4xx.h>
#include <string.h>
#include <stdio.h>
//====================================================================================================
#define GPIO_NUMBER 16
//====================================================================================================
#define GPIO_PIN_0                 		((uint16_t)0x0001)  /* Pin 0 selected    */
#define GPIO_PIN_1                 		((uint16_t)0x0002)  /* Pin 1 selected    */
#define GPIO_PIN_2                 		((uint16_t)0x0004)  /* Pin 2 selected    */
#define GPIO_PIN_3                 		((uint16_t)0x0008)  /* Pin 3 selected    */
#define GPIO_PIN_4                 		((uint16_t)0x0010)  /* Pin 4 selected    */
#define GPIO_PIN_5                	  ((uint16_t)0x0020)  /* Pin 5 selected    */
#define GPIO_PIN_6                 		((uint16_t)0x0040)  /* Pin 6 selected    */
#define GPIO_PIN_7                 		((uint16_t)0x0080)  /* Pin 7 selected    */
#define GPIO_PIN_8                 		((uint16_t)0x0100)  /* Pin 8 selected    */
#define GPIO_PIN_9                 		((uint16_t)0x0200)  /* Pin 9 selected    */
#define GPIO_PIN_10                		((uint16_t)0x0400)  /* Pin 10 selected   */
#define GPIO_PIN_11                		((uint16_t)0x0800)  /* Pin 11 selected   */
#define GPIO_PIN_12               		((uint16_t)0x1000)  /* Pin 12 selected   */
#define GPIO_PIN_13               		((uint16_t)0x2000)  /* Pin 13 selected   */
#define GPIO_PIN_14               		((uint16_t)0x4000)  /* Pin 14 selected   */
#define GPIO_PIN_15               		((uint16_t)0x8000)  /* Pin 15 selected   */
#define GPIO_PIN_All              	 	((uint16_t)0xFFFF)  /* All pins selected */
//====================================================================================================
// Mode
#define	MODE_INPUT           					0x00000000U           /*!< Input Mode                   */
#define MODE_OUTPUT          					0x00000001U           /*!< Output Mode                  */
#define MODE_AF              					0x00000002U           /*!< Alternate Function Mode      */
#define MODE_ANALOG          					0x00000003U           /*!< Analog Mode                  */
//====================================================================================================
// Type
#define	TYPE_PP              					0x00000000U           /*!< Push Pull Mode               */
#define TYPE_OD              					0x00000001U           /*!< Open Drain Mode              */
//====================================================================================================
// Speed
#define GPIO_SPEED_FREQ_LOW         	0x00000000U  /*!< IO works at 2 MHz, please refer to the product datasheet */
#define GPIO_SPEED_FREQ_MEDIUM      	0x00000001U  /*!< range 12,5 MHz to 50 MHz, please refer to the product datasheet */
#define GPIO_SPEED_FREQ_HIGH        	0x00000002U  /*!< range 25 MHz to 100 MHz, please refer to the product datasheet  */
#define GPIO_SPEED_FREQ_VERY_HIGH   	0x00000003U  /*!< range 50 MHz to 200 MHz, please refer to the product datasheet  */
//====================================================================================================
// Pull
#define PULL_FLOATING									0x00000000U						/*!< No pull-up, no pull-down     */
#define PULL_UP												0x00000001U						/*!< Pull-up				              */
#define PULL_DOWN											0x00000002U						/*!< Pull-down			              */
//====================================================================================================
// Alternate function
#define AF_AF0												0x0										//	System
#define AF_AF1												0x1										//	TIM1/TIM2
#define AF_AF2												0x2										//	TIM3..5
#define AF_AF3												0x3										//	TIM9..11
#define AF_AF4												0x4										//	I2C1..3
#define AF_AF5												0x5										//	SPI1..4
#define AF_AF6												0x6										//	SPI3..5
#define AF_AF7												0x7										// 	USART 1..2
#define AF_AF8												0x8										// 	USART 6	
#define AF_AF9												0x9										//  I2C2..3	
#define AF_AF10												0xA										//  OTG_FS	
#define AF_AF11												0xB										//  	
#define AF_AF12												0xC										//  SDIO	
#define AF_AF13												0xD										//  DCMI	
#define AF_AF14												0xE										//  	
#define AF_AF15												0xF										//  EVENTOUT

#define AF_SYSTEM											0x0										//	System

#define AF_TIM1												0x1										//	TIM1/TIM2
#define AF_TIM2												0x1										//	
	
#define AF_TIM3												0x2										//	TIM3..5
#define AF_TIM4												0x2										//	
#define AF_TIM5												0x2										//	

#define AF_TIM9												0x3										//	TIM9..11
#define AF_TIM10											0x3										//	
#define AF_TIM11											0x3										//	

#define AF_I2C1												0x4										//	I2C1..3
#define AF_I2C2												0x4										//	
#define AF_I2C3												0x4										//	

#define AF_SPI1												0x5										//	SPI1..4
#define AF_SPI2												0x5										//	
#define AF_SPI3												0x5										//	
#define AF_SPI4												0x5										//	

#define AF_USART1											0x7										// 	USART 1..2
#define AF_USART2											0x7										// 	

#define AF_USART6											0x8										// 	USART 6	

#define AF_USB_OTG_FS									0xA										//  OTG_FS	

#define AF_SDIO												0xC										//  SDIO	

#define AF_DCMI												0xD										//  DCMI	

#define AF_EVENTOUT										0xF										//  EVENTOUT
//====================================================================================================
typedef struct{
	uint32_t Pin;
  uint32_t Mode;
  uint32_t Type;
  uint32_t Speed;
	uint32_t Pull;
  uint32_t Alternate;
}GPIO_InitTypeDef;
//====================================================================================================
void GPIO_Init(GPIO_TypeDef*, GPIO_InitTypeDef*);

void GPIO_Pin_High(GPIO_TypeDef*, uint16_t);
void GPIO_Pin_Low (GPIO_TypeDef*, uint16_t);

uint8_t GPIO_Pin_Read(GPIO_TypeDef*, uint16_t);
//====================================================================================================
#endif
//====================================================================================================