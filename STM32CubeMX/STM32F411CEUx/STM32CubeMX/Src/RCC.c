#include "RCC.h"

volatile uint32_t msTicks = 0;

void SysTick_Handler(void){
	msTicks++;
}

void delay_ms(uint32_t delay){
	uint32_t curTicks;
	
	curTicks = msTicks;
	while((msTicks - curTicks) < delay);
}

uint32_t get_ms(void){
	return msTicks;
}

void SystemCoreClockConfigure(RCC_InitTypeDef* RCC_Init){
	uint32_t OSC_Freq;
	if(RCC_Init->MCO1_Enable){
		RCC->CFGR &= ~RCC_CFGR_MCO1;
		RCC->CFGR &= ~RCC_CFGR_MCO1PRE;
		//RCC->CFGR |= RCC_CFGR_MCO1PRE;
	}
	
	if(RCC_Init->MCO1_Enable){
			
		RCC->AHB1ENR|=RCC_AHB1ENR_GPIOAEN; // enable clock for port A

		GPIOA->MODER &= ~GPIO_MODER_MODER8;
		GPIOA->MODER |= GPIO_MODER_MODER8_1;
		GPIOA->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR8);
		//GPIOA->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR8);
		
		
	}
	/*Enable clock source*/
	switch((uint32_t)RCC_Init->OscillatorType){
		case OSC_EXTERNAL:
			RCC->CR |= ((uint32_t)RCC_CR_HSEON);										// enable HSE
			while(!(RCC->CR & RCC_CR_HSERDY));											// wait till ready
		
			OSC_Freq = RCC_Init->ExternalOscillatorFreq;						// HSE frequency
			break;
		
		case OSC_INTERNAL:
			RCC->CR |= ((uint32_t)RCC_CR_HSION);										// enable HSI
			while(!(RCC->CR & RCC_CR_HSIRDY));											// wait till ready
		
			OSC_Freq = 16000000; 																		// HSI frequency
			break;
	}
		
	RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2);
	RCC->CFGR |= RCC_Init->AHB_Prescaler << 4;
	RCC->CFGR |= RCC_Init->APB1_Prescaler << 10;
	RCC->CFGR |= RCC_Init->APB2_Prescaler << 13;
	
	RCC_Init->HCLK_Clock = OSC_Freq / (1 << (RCC_Init->AHB_Prescaler - 0x7));
	RCC_Init->APB1_Clock  = RCC_Init->HCLK_Clock  / (1 << (RCC_Init->APB1_Prescaler - 0x3));
	RCC_Init->APB2_Clock = RCC_Init->HCLK_Clock  / (1 << (RCC_Init->APB2_Prescaler - 0x3));
	
	
	
	/*Set system clock source*/
	switch((uint32_t)RCC_Init->SystemClockSource){
		case HSE_CLK_SRC:
			RCC->CFGR &= ~RCC_CFGR_SW;
			RCC->CFGR = RCC_CFGR_SW_HSE;														// set system clock (HSI, HSE, PLL)
			while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSE);  // wait setting system clock
			break;
		
		case HSI_CLK_SRC:
			RCC->CFGR &= ~RCC_CFGR_SW;
			RCC->CFGR = RCC_CFGR_SW_HSI;														// set system clock (HSI, HSE, PLL)
			while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);  // wait setting system clock
			break;
		
		case PLL_CLK_SRC:
			RCC->CR &= ~RCC_CR_PLLON;
					
			RCC->PLLCFGR = 0x24000000; // Reset PLL configuration register
			
			/*PLL prescalers*/
			if((RCC_Init->PLL_M_Divider >= 2) && (RCC_Init->PLL_M_Divider <= 63)){
				RCC->PLLCFGR |= RCC_Init->PLL_M_Divider 		<< 0;
				RCC_Init->HCLK_Clock  =  RCC_Init->HCLK_Clock  / RCC_Init->PLL_M_Divider;
			}// out freq [0.95; 2.1] MHz
			
			if((RCC_Init->PLL_N_Multiplier >= 50) && (RCC_Init->PLL_N_Multiplier <= 432)){
				RCC->PLLCFGR |= RCC_Init->PLL_N_Multiplier 	<< 6;
				RCC_Init->HCLK_Clock  =  RCC_Init->HCLK_Clock  * RCC_Init->PLL_N_Multiplier;
			}// out freq [100; 432] MHz
			
			if((RCC_Init->PLL_P_Divider >= 0) && (RCC_Init->PLL_P_Divider <= 3)){
				RCC->PLLCFGR |= RCC_Init->PLL_P_Divider 		<< 16;
				RCC_Init->HCLK_Clock  =  RCC_Init->HCLK_Clock  / (2 * (RCC_Init->PLL_P_Divider + 1));
			}// out freq max 100 MHz
			
			if((RCC_Init->PLL_Q_Divider >= 2) && (RCC_Init->PLL_Q_Divider <= 15)){
				uint32_t Freq_After_PLLQ;
				Freq_After_PLLQ = OSC_Freq / RCC_Init->PLL_M_Divider * RCC_Init->PLL_N_Multiplier / RCC_Init->PLL_Q_Divider;
				if(Freq_After_PLLQ == 48000000){
					RCC_Init->PLLQ_Clock = Freq_After_PLLQ;
					RCC->PLLCFGR |= RCC_Init->PLL_Q_Divider 		<< 24;
				}
				else{
					RCC_Init->PLLQ_Clock = 32000000;
				}
			}// out freq must be 48 MHz
			
			/*PLL source clock*/
			if(RCC_Init->OscillatorType == OSC_EXTERNAL){
				RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE;
			}
			else if(RCC_Init->OscillatorType == OSC_INTERNAL){
				RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC_HSE;
			}
			
			RCC_Init->APB1_Clock = RCC_Init->HCLK_Clock  / (1 << (RCC_Init->APB1_Prescaler - 0x3));
			RCC_Init->APB2_Clock = RCC_Init->HCLK_Clock  / (1 << (RCC_Init->APB2_Prescaler - 0x3));
			
			FLASH->ACR = FLASH_ACR_PRFTEN;
			//====================================================================================================
			#if defined(STM32F401xE)
				if((RCC_Init->HCLK_Clock > 0) && (RCC_Init->HCLK_Clock <= 30000000))	
					FLASH->ACR |= FLASH_ACR_LATENCY_0WS;
				if((RCC_Init->HCLK_Clock > 30000000) && (RCC_Init->HCLK_Clock <= 60000000))	
					FLASH->ACR |= FLASH_ACR_LATENCY_1WS;
				if((RCC_Init->HCLK_Clock > 60000000) && (RCC_Init->HCLK_Clock <= 84000000))	
					FLASH->ACR |= FLASH_ACR_LATENCY_2WS;
			
			#elif defined(STM32F407xx)
				if((RCC_Init->HCLK_Clock > 0) && (RCC_Init->HCLK_Clock <= 30000000))	
					FLASH->ACR |= FLASH_ACR_LATENCY_0WS;
				if((RCC_Init->HCLK_Clock > 30000000) && (RCC_Init->HCLK_Clock <= 60000000))	
					FLASH->ACR |= FLASH_ACR_LATENCY_1WS;
				if((RCC_Init->HCLK_Clock > 60000000) && (RCC_Init->HCLK_Clock <= 90000000))	
					FLASH->ACR |= FLASH_ACR_LATENCY_2WS;
				if((RCC_Init->HCLK_Clock > 90000000) && (RCC_Init->HCLK_Clock <= 120000000))	
					FLASH->ACR |= FLASH_ACR_LATENCY_3WS;
				if((RCC_Init->HCLK_Clock > 120000000) && (RCC_Init->HCLK_Clock <= 150000000))	
					FLASH->ACR |= FLASH_ACR_LATENCY_4WS;
				if((RCC_Init->HCLK_Clock > 150000000) && (RCC_Init->HCLK_Clock <= 168000000))	
					FLASH->ACR |= FLASH_ACR_LATENCY_5WS;
			
			#elif defined(STM32F411xE)
				if((RCC_Init->HCLK_Clock > 0) && (RCC_Init->HCLK_Clock <= 30000000))	
					FLASH->ACR |= FLASH_ACR_LATENCY_0WS;
				if((RCC_Init->HCLK_Clock > 30000000) && (RCC_Init->HCLK_Clock <= 64000000))	
					FLASH->ACR |= FLASH_ACR_LATENCY_1WS;
				if((RCC_Init->HCLK_Clock > 64000000) && (RCC_Init->HCLK_Clock <= 90000000))	
					FLASH->ACR |= FLASH_ACR_LATENCY_2WS;
				if((RCC_Init->HCLK_Clock > 90000000) && (RCC_Init->HCLK_Clock <= 100000000))	
					FLASH->ACR |= FLASH_ACR_LATENCY_3WS;
			
			#endif
			//====================================================================================================
			uint32_t APB1_MAX_CLOCK, APB2_MAX_CLOCK;
			
			#if defined(STM32F401xE)
				APB1_MAX_CLOCK =  42000000;
				APB2_MAX_CLOCK =  84000000;
			#elif defined(STM32F411xE)
				APB1_MAX_CLOCK =  50000000;
				APB2_MAX_CLOCK = 100000000;
			#elif defined(STM32F407xx)
				APB1_MAX_CLOCK =  42000000;
				APB2_MAX_CLOCK =  84000000;
			#endif
			
			if(((RCC_Init->APB1_Clock <= APB1_MAX_CLOCK) && (RCC_Init->APB2_Clock <= APB2_MAX_CLOCK))){
				/*PLL on*/
				RCC->CR |= RCC_CR_PLLON;
				while(!(RCC->CR & RCC_CR_PLLRDY));
				
				if((RCC_Init->HCLK_Clock > 144000000))
					PWR->CR |= PWR_CR_VOS;
				else
					PWR->CR &= ~PWR_CR_VOS;
				
				/*Set PLL as system clock source*/
				RCC->CFGR &= ~RCC_CFGR_SW;
				RCC->CFGR |= RCC_CFGR_SW_PLL;
				while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
			
				
			}
			else{
				RCC_Init->InitStatus = 0x01;
				RCC_Init->HCLK_Clock = OSC_Freq / (1 << (RCC_Init->AHB_Prescaler - 0x7));
				RCC_Init->APB1_Clock  = RCC_Init->HCLK_Clock  / (1 << (RCC_Init->APB1_Prescaler - 0x3));
				RCC_Init->APB2_Clock = RCC_Init->HCLK_Clock  / (1 << (RCC_Init->APB2_Prescaler - 0x3));
			}
			break;
			
	}
	
	SystemCoreClockUpdate();
	SystemCoreClock = RCC_Init->HCLK_Clock;
	SysTick_Config(SystemCoreClock / 1000); 											// SysTick 1 msec interrupt
	
	
}

