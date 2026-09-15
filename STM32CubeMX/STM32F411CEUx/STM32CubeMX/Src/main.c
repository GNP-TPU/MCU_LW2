//====================================================================================================
#include "main.h"
//====================================================================================================
void RCC_Configure(void);
void GPIO_Configure(void);
void SPI_Configure(void);
void USART_Configure(void);
//====================================================================================================
USART_InitTypeDef				USART_PC;
SPI_InitTypeDef 				SPI_W25Q;


void __SPI_Select(bool select){
	if(select) GPIO_Pin_Low(GPIOA, GPIO_PIN_4);
	else GPIO_Pin_High(GPIOA, GPIO_PIN_4);
}

void __SPI_Write(uint8_t byte){
	SPI_Transmit_Byte(SPI1, byte);
}

uint8_t __SPI_Read(void){
	return SPI_Receive_Byte(SPI1);
}

W25Q_t MyFlash = {
	.W25Q_Select 	= __SPI_Select,
	.W25Q_SPI_Write = __SPI_Write,
	.W25Q_SPI_Read 	= __SPI_Read
};

extern volatile uint8_t  msc_write_request;
extern volatile uint32_t msc_write_lba;  

extern volatile uint8_t  msc_read_request;
extern volatile uint32_t msc_requested_lba;

extern volatile uint8_t  msc_scsi_cmd;


extern volatile uint32_t msc_remaining_bytes;
extern __ALIGN4 uint8_t msc_sector_buffer[1024];

__ALIGN4 static uint8_t flash_cache_buffer[4096]; 
int32_t current_cached_sector_addr = -1; // -1 означает, что кэш пуст

/*
W25Q MyFlash(
	__SPI_Select,
	__SPI_Write,
	__SPI_Read
);
*/

// Обработка стирания сектора 1 раз
void Flush_Flash_Cache(void) {
    if (current_cached_sector_addr == -1) return;

    // ERASE: Стираем один раз физический 4 КБ сектор
    W25Q_SectorErase(&MyFlash, current_cached_sector_addr);
    while(W25Q_IsBusy(&MyFlash));

    // WRITE: Записываем весь 4 КБ кэш обратно (16 страниц по 256 байт)
    for (uint32_t i = 0; i < 16; i++) {
        uint32_t page_address = current_cached_sector_addr + (i * 256);
        W25Q_PageProgram(&MyFlash, &flash_cache_buffer[i * 256], page_address, 256);
        while(W25Q_IsBusy(&MyFlash));
    }

    // Помечаем кэш как пустой
    current_cached_sector_addr = -1;
}

void USB_MSC_Background_Process(void) {
	// Логика чтения
    if (msc_read_request) {
        W25Q_FastRead(&MyFlash, msc_sector_buffer, msc_requested_lba * STORAGE_SECTOR_SIZE, STORAGE_SECTOR_SIZE);
        msc_read_request = 0;
        uint32_t chunk = (msc_remaining_bytes > 64) ? 64 : msc_remaining_bytes;
        msc_remaining_bytes -= chunk;
        USB_EP_Tx(1, msc_sector_buffer, chunk); 
    }
	// Логика записи
    if (msc_write_request) {
        // Находим физический адрес начала 4 КБ сектора флешки (округляем вниз до 4096)
		uint32_t flash_sector_address = (msc_write_lba * STORAGE_SECTOR_SIZE) & 0xFFFFF000;
		
		// Вычисляем смещение (индекс) внутри 4 КБ кэша, куда запишутся новые 512 байт
		uint32_t cache_offset = (msc_write_lba % 8) * STORAGE_SECTOR_SIZE;

		if (current_cached_sector_addr != -1 && current_cached_sector_addr != flash_sector_address) {
            // Сброс кэша
            Flush_Flash_Cache(); 
        }

		if (current_cached_sector_addr == -1) {
            W25Q_FastRead(&MyFlash, flash_cache_buffer, flash_sector_address, 4096);
            current_cached_sector_addr = flash_sector_address;
        }

		// Копируем новые 512 байт от ПК поверх старых данных в буфере кэша
		memcpy(&flash_cache_buffer[cache_offset], msc_sector_buffer, STORAGE_SECTOR_SIZE);

        // Сбрасываем флаг запроса
        msc_write_request = 0;

        if (msc_remaining_bytes == 0) {
			// ПК закончил передачу. Принудительно сбрасываем накопленный кэш на физическую флешку
            Flush_Flash_Cache();
            // Если все секторы от хоста приняты и записаны — закрываем команду
            msc_scsi_cmd = 0;
            MSC_Send_CSW(0);  // Отправляем хосту статус успешного завершения записи WRITE_10
            
            // Перевзводим точку на ожидание новой команды CBW
            EP1_OUT->DOEPTSIZ = (1U << USB_OTG_DOEPTSIZ_PKTCNT_Pos) | (64 << USB_OTG_DOEPTSIZ_XFRSIZ_Pos);
            EP1_OUT->DOEPCTL |= USB_OTG_DOEPCTL_EPENA | USB_OTG_DOEPCTL_CNAK;
        } 
        else {
            // Если хост хочет записать еще секторы в рамках текущей сессии
            msc_write_lba++; // Сдвигаем адрес на следующий сектор
            
            // Перевзводим точку OUT на прием следующего пакета данных секторов
            EP1_OUT->DOEPTSIZ = (1U << USB_OTG_DOEPTSIZ_PKTCNT_Pos) | (64 << USB_OTG_DOEPTSIZ_XFRSIZ_Pos);
            EP1_OUT->DOEPCTL |= USB_OTG_DOEPCTL_EPENA | USB_OTG_DOEPCTL_CNAK;
        }
    }
}



int main(void){
	RCC_Configure();
	GPIO_Configure();
	SPI_Configure();
	USART_Configure();
	
	char hex_str[200];

	uint32_t val = W25Q_ReadID(&MyFlash);
	sprintf(hex_str, "0x%06X", val);
	USART_SendString(USART1, hex_str);
	delay_ms(1000);

	W25Q_ChipErase(&MyFlash);
	uint32_t erase_start_ms = get_ms();

	sprintf(hex_str, "Началась полная очистка W25Q, подождите...");
	USART_SendString(USART1, hex_str);

	while(W25Q_IsBusy(&MyFlash)){
	}

	uint32_t erase_stop_ms = get_ms();

	sprintf(hex_str, "W25Q очищена, время очистки: %u ms", erase_stop_ms - erase_start_ms);
	USART_SendString(USART1, hex_str);

	sprintf(hex_str, "Выполняется тестовая запись в 0 сектор, 2 страницы. Подождите...");
	USART_SendString(USART1, hex_str);

	uint8_t tx_buffer[256];

	for(uint16_t i = 0; i < 256; i++){
		tx_buffer[i] = i;
	}

	uint32_t page_address = 0x00000000;

	erase_start_ms = get_ms();
	for (uint32_t i = 0; i < 2; i++) {
        page_address = i * 256;
        W25Q_PageProgram(&MyFlash, tx_buffer, page_address, 256);
        while(W25Q_IsBusy(&MyFlash));
    }
	erase_stop_ms = get_ms();

	sprintf(hex_str, "Запись выполнена, время записи двух страниц: %u ms", erase_stop_ms - erase_start_ms);
	USART_SendString(USART1, hex_str);

	sprintf(hex_str, "Проверка корректности записи первой страницы.");
	USART_SendString(USART1, hex_str);

	

	bool correct_flag = true;
	for(uint8_t page = 0; page < 2; page++){
		uint8_t rx_buffer[256];
		W25Q_FastRead(&MyFlash, rx_buffer, page * 256, 256);
		for(uint16_t i = 0; i < 256; i++){
			if(rx_buffer[i] != tx_buffer[i]){
				correct_flag = false;
				sprintf(hex_str, "Ошибка данных на странице %d, смещение 0x%02X", page, i);
				USART_SendString(USART1, hex_str);
				break;
			}
		}	
	}

	if(correct_flag){
		sprintf(hex_str, "Все записи в странице совпали с отправленными");
		USART_SendString(USART1, hex_str);
	}
	

	// PageProgram(&MyFlash, FlashBuf, 0x00000000, 64);

	//USB_Core_Init();
	
	while(1){
		

		//USB_MSC_Background_Process();
		/*
		char hex_str[100];

		uint32_t val = MyFlash.ReadID();

		sprintf(hex_str, "0x%06X", val);
		USART_SendString(USART1, hex_str);
		delay_ms(1000);

		bool busy_flag = MyFlash.IsBusy();
		sprintf(hex_str, "Chip Business: %01X", busy_flag);
		USART_SendString(USART1, hex_str);
		delay_ms(1000);

		

		/*
		sprintf(hex_str, "Erasing Chip");
		USART_SendString(USART1, hex_str);
		MyFlash.ChipErase();
		while(MyFlash.IsBusy()){
			sprintf(hex_str, ".");
			USART_SendString(USART1, hex_str);
		}
		delay_ms(10000);
		*/
	}

}


//====================================================================================================
void RCC_Configure(void){
	RCC_InitTypeDef 	RCC_InitStruct;
	
	RCC_InitStruct.ManualCalculatePLL				= false;
	
	RCC_InitStruct.OscillatorType					= OSC_EXTERNAL;	
	RCC_InitStruct.SystemClockSource				= PLL_CLK_SRC;
	
	RCC_InitStruct.ExternalOscillatorFreq 	= 25000000;
	
	RCC_InitStruct.AHB_Prescaler						= AHB_PRESCALER_DIV1;
	RCC_InitStruct.APB1_Prescaler						= APB_PRESCALER_DIV2;
	RCC_InitStruct.APB2_Prescaler						= APB_PRESCALER_DIV1;
		
	RCC_InitStruct.PLL_M_Divider						= 25;
	RCC_InitStruct.PLL_N_Multiplier					= 192;
	RCC_InitStruct.PLL_P_Divider						=	PLL_P_DIV2;
	RCC_InitStruct.PLL_Q_Divider						=	4;
	
	SystemCoreClockConfigure(&RCC_InitStruct);
}

void GPIO_Configure(void){
	GPIO_InitTypeDef GPIO_InitStruct;

	// SPI 
	GPIO_InitStruct.Pin 		= GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;									
	GPIO_InitStruct.Mode 		= MODE_AF;
	GPIO_InitStruct.Type 		= TYPE_PP;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate	= AF_SPI1;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	// SPI CS
	GPIO_InitStruct.Pin 			= GPIO_PIN_4;
	GPIO_InitStruct.Mode 			= MODE_OUTPUT;
	GPIO_InitStruct.Type 			= TYPE_PP;
	GPIO_InitStruct.Speed 			= GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	// UART
	GPIO_InitStruct.Pin 			= GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_InitStruct.Mode 			= MODE_AF;
	GPIO_InitStruct.Type 			= TYPE_PP;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate	= AF_USART1;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	// USB (PA11(DM), PA12(DP))
	
	GPIO_InitStruct.Pin 			= GPIO_PIN_11 | GPIO_PIN_12;	
	GPIO_InitStruct.Mode 			= MODE_AF;
	GPIO_InitStruct.Type 			= TYPE_PP;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate	= AF_USB_OTG_FS;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	
}

void SPI_Configure(void){
	SPI_W25Q.Baudrate_Prescaler = SPI_BAUDRATE_DIV2;
	SPI_Init(SPI1, &SPI_W25Q);
}

void USART_Configure(void){
	USART_PC.Baudrate = 115200;
	
	USART_PC.Rx_Enable = true;
	USART_PC.Tx_Enable = true;
	
	USART_PC.Rx_IRq_Enable = true;
	
	USART_Init(USART1, &USART_PC);
}
//====================================================================================================

