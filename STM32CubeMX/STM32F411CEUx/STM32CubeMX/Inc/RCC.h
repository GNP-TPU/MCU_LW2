//====================================================================================================
#ifndef RCC_H
#define RCC_H
//====================================================================================================
#define HSE_VALUE    ((uint32_t)25000000

#include <stm32f4xx.h>
#include <string.h>
#include <stdio.h>
//====================================================================================================
extern volatile uint32_t msTicks;
//====================================================================================================


#define OSC_EXTERNAL					0x01
#define OSC_INTERNAL					0x02

#define HSE_CLK_SRC						0x03
#define HSI_CLK_SRC						0x04
#define	PLL_CLK_SRC						0x05

#define AHB_PRESCALER_DIV1		0x7
#define AHB_PRESCALER_DIV2		0x8
#define AHB_PRESCALER_DIV4		0x9
#define AHB_PRESCALER_DIV8		0xA
#define AHB_PRESCALER_DIV16		0xB
#define AHB_PRESCALER_DIV64		0xC
#define AHB_PRESCALER_DIV128	0xD
#define AHB_PRESCALER_DIV256	0xE
#define AHB_PRESCALER_DIV512	0xF

#define APB_PRESCALER_DIV1		0x3
#define APB_PRESCALER_DIV2		0x4
#define APB_PRESCALER_DIV4		0x5
#define APB_PRESCALER_DIV8		0x6
#define APB_PRESCALER_DIV16		0x7

#define PLL_P_DIV2						0x0
#define	PLL_P_DIV4						0x1
#define	PLL_P_DIV6						0x2
#define	PLL_P_DIV8						0x3

#define PLL_Q_DIV2						0x2
#define PLL_Q_DIV3						0x3
#define PLL_Q_DIV4						0x4
#define PLL_Q_DIV5						0x5
#define PLL_Q_DIV6						0x6
#define PLL_Q_DIV7						0x7
#define PLL_Q_DIV8						0x8
#define PLL_Q_DIV9						0x9
#define PLL_Q_DIV10						0xA
#define PLL_Q_DIV11						0xB
#define PLL_Q_DIV12						0xC
#define PLL_Q_DIV13						0xD
#define PLL_Q_DIV14						0xE
#define PLL_Q_DIV15						0xF

#define HSI_MCO_SRC						0x0
#define LSE_MCO_SRC						0x1
#define HSE_MCO_SRC						0x2
#define PLL_MCO_SRC						0x3

#define MCO_DIV1							0x3
#define MCO_DIV2							0x4
#define MCO_DIV3							0x5
#define MCO_DIV4							0x6
#define MCO_DIV5							0x7
//====================================================================================================
typedef struct{
	uint8_t  	ManualCalculatePLL;
	
	uint32_t 	ExternalOscillatorFreq;
	
	uint8_t 	OscillatorType;				
	
	uint8_t 	SystemClockSource;
	
	uint8_t 	AHB_Prescaler;
	
	uint8_t 	APB1_Prescaler;
	uint8_t 	APB2_Prescaler;
	
	uint16_t 	PLL_M_Divider;								// PLLM = [2; 63]		| out freq [0.95; 2.1] MHz
	uint16_t 	PLL_N_Multiplier;							// PLLN = [50; 432]	| out freq [100 ; 432] MHz
	uint8_t 	PLL_P_Divider;								// PLLP = [0; 3]		| out freq max 100 MHz
	uint8_t		PLL_Q_Divider;								// PLLQ for USB 		| out freq must be 48 MHz
	
	uint8_t		MCO1_Enable;
	uint8_t		MCO1_Source;
	uint8_t		MCO1_Prescaler;
	
	uint32_t	HCLK_Clock;
	uint32_t	APB1_Clock;
	uint32_t	APB2_Clock;
	uint32_t 	PLLQ_Clock;
	
	uint8_t 	InitStatus;
	
}RCC_InitTypeDef;
//====================================================================================================
void SystemCoreClockConfigure(RCC_InitTypeDef*);
void delay_ms(uint32_t);
uint32_t get_ms(void);
//====================================================================================================
#endif
//====================================================================================================